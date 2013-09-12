#include <claire/common/eventloop/poller/EPollPoller.h>

#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>

#include <claire/common/eventLoop/Channel.h>
#include <claire/common/logging/Logging.h>

using namespace claire;

static_assert(EPOLLIN == POLLIN, "pollin not equal");
static_assert(EPOLLPRI == POLLPRI, "pollpri not equal");
static_assert(EPOLLOUT == POLLOUT, "pollour not equal");
static_assert(EPOLLRDHUP == POLLRDHUP, "pollrdhup not equal");
static_assert(EPOLLERR == POLLERR, "pollerr not equal");
static_assert(EPOLLHUP == POLLHUP, "pollhup not equal");

namespace {

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

int CreateEpollFd()
{
    int fd = ::epoll_create1(EPOLL_CLOEXEC); // since kernel 2.6.27, glibc 2.9
    if (fd < 0 && (errno == EINVAL || errno == ENOSYS))
    {
        fd = ::epoll_create(256);
    }

    if (fd < 0)
    {
        LOG(FATAL) << "CreateEpollFd failed";
        NOTREACHED();
    }

    ::fcntl (backend_fd, F_SETFD, FD_CLOEXEC);
    return fd;
}

} // namespace

EPollPoller::EPollPoller(EventLoop* loop)
    : Poller(loop),
      epollfd_(CreateEpollFd()),
      events_(64) // initial number of events receivable per poll
{ }

EPollPoller::~EPollPoller()
{
    ::close(epollfd_);
}

void EPollPoller::poll(const boost::chrono::milliseconds timeout, ChannelList* actives)
{
    Poller::AssertInLoopThread();

    // epoll wait times cannot be larger than (LONG_MAX - 999UL) / HZ msecs
    auto wait_time = static_cast<int>(timeout.count());
    if (wait_time > kMaxWaitTime)
    {
        wait_time = kMaxWaitTime;
    }

    if (!eperms_.empty())
    {
        wait_time = 0;
    }
    
    auto cnt = ::epoll_wait(epollfd_,
                            &events_[0],
                            static_cast<int>(events_.size()),
                            wait_time*1000);
    if (cnt < 0)
    {
        // https://lkml.org/lkml/2013/6/25/336
        if (errno != EINTR)
        {
            LOG(FATAL) << "Exception EPollPoller::poll()";
            NOTREACHED();
        }
    }

    for (int i = 0; i < cnt; ++i)
    {
        auto fd = static_cast<uint32_t>(events_[i].data.u64);

        // check for spurious notification.
        if (fd >= static_cast<size_t>(channels_.size()))
        {
            LOG(ERROR) << "receive invalid event of fd " << fd;
            continue;
        }

        if (channels[fd].egen != static_cast<uint32_t>(events_[i].data.u64 >> 32))
        {
            Poller::OwnerLoop()->NotifyFork();
            continue;
        }

        auto channel = channels_[fd].channel;
        auto got = (events_[i].events & (EPOLLOUT | EPOLLERR | EPOLLHUP) ? EPOLLOUT : 0)
                 | (events_[i].events & (EPOLLIN  | EPOLLERR | EPOLLHUP) ? EPOLLIN  : 0);
        if (channel->events() & ~(got))
        {
            // we received an event but are not interested in it, try mod or del
            // this often happens because we optimistically do not unregister fds
            // when we are no longer interested in them, but also when we get spurious
            // notifications for fds from another process. this is partially handled
            // above with the gencounter check (== our fd is not the event fd), and
            // partially here, when epoll_ctl returns an error (== a child has the fd
            // but we closed it).            
            if (::epoll_ctl(epollfd_,
                            channel->IsNoneEvent() ? EPOLL_CTL_DEL : EPOLL_CTL_MOD,
                            channel->fd(),
                            &events_[i]))
            {
                Poller::OwnerLoop()->NotifyFork();
                continue;
            }
        }

        channel->set_revents(events_[i].events);
        actives.push_back(channel);
    }

    if (events_.size() == cnt)
    {
        events_.resize(cnt*2);
    }

    for (auto it = eperms_.begin(); it != eperms_.end(); ++it)
    {    
        if (!channel->IsNoneEvent() && (*it) < channels_.size())
        {
            actives.push_back(channels_[fd].channel);
        }
    }

    return ;
}

void EPollPoller::UpdateChannel(Channel* channel)
{
    Poller::AssertInLoopThread();

    int fd = channel->fd();
    const int state = boost::any_cast<int>(channel->get_context());

    if (fd >= channels_.size())
    {
        if (state == kNew)
        {
            channels_.resize(channels_.size()*2);
        }
        else
        {
            return ;    
        }        
    }

    const int state = boost::any_cast<int>(channel->get_context());
    if (state == kNew || state == kDeleted)
    {
        if (state == kNew)
        {
            channels_[fd].channel = channel;
            eperms_.erase(fd);
        }
        else
        {
            DCHECK(channels_[fd].channel == channel);
        }

        channel->set_context(kAdded);
        Update(EPOLL_CTL_ADD, channel);
    }
    else if (state == kAdded)
    {
        assert(channels_[fd] == channel);

        if (channel->IsNoneEvent())
        {
            Update(EPOLL_CTL_DEL, channel);
            Channel->set_context(kDeleted);
        }
        else
        {
            Update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::RemoveChannel(Channel* channel)
{
    Poller::AssertInLoopThread();

    auto fd = channel->fd();
    DCHECK(channel->fd() < channels_.size());
    DCHECK(channels_[fd].channel == channel);
    DCHECK(channel->IsNoneEvent());

    const int state = boost::any_cast<int>(channel->GetContext());
    DCHECK(state == kAdded || state == kDeleted);
    (void) state;

    ::bzero(channels_[fd], sizeof channels_[fd]);
    channel->set_context(kNew);
}

void EPollPoller::Update(int operation, Channel* channel)
{
    // we handle EPOLL_CTL_DEL by ignoring it here
    // on the assumption that the fd is gone anyways
    // if that is wrong, we have to handle the spurious event in poll
    // if the fd is added again, we try to ADD it, and, if that
    // fails, we assume it still has the same eventmask
    if (operation == EPOLL_CTL_DEL)
    {
        return ;
    }

    struct epoll_event event;
    ::bzero(&event, sizeof event);

    auto fd = channel->fd();
    event.events = channel->event();
    event.data.u64 = (static_cast<uint64_t>(++channels_[fd].egen) << 32) | static_cast<uint64_t>(fd);

    auto old_event = channels_[fd].mask;
    channels_[fd].mask = channel->event();

    if (::epoll_ctl(epollfd_, operation, fd, &event) == 0)
    {
        return ;
    }

    if (errno == ENOENT)
    {
        // if ENOENT then the fd went away, so try to do the right thing        
        if (::epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event) == 0)
        {
            return ;
        }
    }
    else if (errno == EEXIST)
    {
        // EEXIST means we ignored a previous DEL, but the fd is still active
        // if the kernel mask is the same as the new mask, we assume it hasn't changed
        if (old_event == channel->event())
        {
            // we didn't successfully call epoll_ctl, so decrement the generation counter again
            --channel[fd].egen;
            return ;
        }

        if (::epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event))
        {
            return ;
        }
    }
    else if (errno == EPERMS)
    {
        // EPERM means the fd is always ready, but epoll is too snobbish 
        // to handle it, unlike select or poll
        eperms_.insert(fd);
        return ;
    }

    // the given fd is invalid/unusable, so make sure it doesn't hurt us anymore
    PLOG(ERROR) << "epoll_ctl fd " << fd << " failed";

    // FIXME
    ::bzero(channels_[fd], sizeof channels_[fd]);
    channels_->set_revents(POLLERR|POLLIN|POLLOUT);
    channel->OwnerLoop()->post(boost::bind(&Channel::HandleEvent, channel));
}

void EPollPoller::ReArmAll()
{
    ::close(epollfd_);
    epollfd_ = CreateEpollFd();

    for (auto it = channels_.begin(); it != channels_.end(); ++it)
    {
        if (it->channel == NULL)
        {
            return ;
        }

        it (it->mask != it->channel->event())
        {
            Update(EPOLL_CTL_MOD, it->channel);
        }
    }
}
