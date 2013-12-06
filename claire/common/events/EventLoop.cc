// Copyright (c) 2013 The claire-common Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <claire/common/events/EventLoop.h>

#include <sys/eventfd.h>
#include <sys/timerfd.h>

#include <algorithm>

#include <boost/bind.hpp>

#include <claire/common/events/Poller.h>
#include <claire/common/events/Channel.h>
#include <claire/common/events/poller/EPollPoller.h>
#include <claire/common/time/TimeoutQueue.h>
#include <claire/common/logging/Logging.h>
#include <claire/common/threading/ThisThread.h>

namespace claire {

namespace {

__thread EventLoop * tLoopInThisThread = NULL;
const int64_t kPollTimeMs = 10;

struct Cmp
{
    bool operator()(const Channel* lhs, const Channel* rhs)
    {
        return static_cast<int>(lhs->priority()) > static_cast<int>(rhs->priority());
    }
};

int CreateEventfd()
{
    int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd < 0)
    {
        PLOG(FATAL) << "Failed in eventfd";
    }

    return fd;
}

int CreateTimerfd()
{
    int fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (fd < 0)
    {
        PLOG(FATAL) << "Failed in timerfd_create ";
    }

    return fd;
}

struct timespec HowMuchTimeFromNow(int64_t when, int64_t now)
{
    auto delta = when - now;
    if (delta < 100)
    {
        delta = 100;
    }

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(delta / 1000000);
    ts.tv_nsec = static_cast<long>((delta % Timestamp::kMicroSecondsPerSecond) * 1000); // FIXME
    return ts;
}

void ReadTimerfd(int fd)
{
    uint64_t howmany;
    ssize_t n = ::read(fd, &howmany, sizeof howmany);
    if (n != sizeof howmany)
    {
        PLOG(ERROR) << "ResetTimerfd reads " << n << " bytes instead of 8";
    }
}

void ResetTimerfd(int fd, int64_t expiration)
{
    // wake up loop by timerfd_settime()
    struct itimerspec spec_new;
    struct itimerspec spec_old;
    ::bzero(&spec_new, sizeof spec_new);
    ::bzero(&spec_old, sizeof spec_old);

    spec_new.it_value = HowMuchTimeFromNow(expiration,
                                           Timestamp::Now().MicroSecondsSinceEpoch());
    int ret = ::timerfd_settime(fd, 0, &spec_new, &spec_old);
    if (ret)
    {
        PLOG(ERROR) << "timerfd_settime failed";
    }
}

} // namespace

EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      tid_(ThisThread::tid()),
      poller_(new EPollPoller(this)), // FIXME
      timeouts_(new TimeoutQueue()), // since 2.6.28
      wakeup_fd_(CreateEventfd()),
      wakeup_channel_(new Channel(this, wakeup_fd_)),
      timer_fd_(CreateTimerfd()),
      timer_channel_(new Channel(this, timer_fd_)),
      current_active_channel_(NULL)
{
    LOG(DEBUG) << "EventLoop create " << this << "in thread " << tid_;
    if (tLoopInThisThread)
    {
        LOG(FATAL) << "Another EventLoop " << tLoopInThisThread
                   << "already exist in this thread" << tid_;
    }
    else
    {
        tLoopInThisThread = this;
    }

    wakeup_channel_->set_read_callback(
        boost::bind(&EventLoop::OnWakeup, this));
    wakeup_channel_->EnableReading();

    timer_channel_->set_read_callback(
        boost::bind(&EventLoop::OnTimer, this));
    timer_channel_->EnableReading();
    timer_channel_->set_priority(Channel::Priority::kHigh); // FIXME
}

EventLoop::~EventLoop()
{
    LOG(DEBUG) << "EventLoop " << this << " of thread " << tid_
               << " destructs in thread " << ThisThread::tid();
    ::close(timer_fd_);
    ::close(wakeup_fd_);

    DCHECK(tLoopInThisThread == this);
    tLoopInThisThread = NULL;
}

void EventLoop::loop()
{
    DCHECK(!looping_);
    AssertInLoopThread();

    looping_ = true;
    LOG(INFO) << "EventLoop " << this << " start looping";

    while(!quit_)
    {
        active_channels_.clear();
        poller_->poll(kPollTimeMs, &active_channels_);

        Cmp cmp;
        sort(active_channels_.begin(), active_channels_.end(), cmp); // FIXME

        for (auto it = active_channels_.begin(); it != active_channels_.end(); ++it)
        {
            current_active_channel_ = *it;
            (*it)->OnEvent();
        }
        current_active_channel_ = NULL;

        RunPendingTasks();
    }

    LOG(INFO) << "EventLoop " << this << " stop looping";
    looping_ = false;
}

void EventLoop::quit()
{
    quit_ = true; // FIXME
    if (!IsInLoopThread())
    {
        Wakeup();
    }
}

void EventLoop::Run(const Task& task)
{
    if (IsInLoopThread())
    {
        task();
    }
    else
    {
        Post(task);
    }
}

void EventLoop::Post(const Task& task)
{
    {
        MutexLock lock(mutex_);
        pending_tasks_.push_back(task);
    }

    if (!IsInLoopThread() || !current_active_channel_) // FIXME
    {
        Wakeup();
    }
}

void EventLoop::Run(Task&& task)
{
    if (IsInLoopThread())
    {
        task();
    }
    else
    {
        Post(std::move(task));
    }
}

void EventLoop::Post(Task&& task)
{
    {
        MutexLock lock(mutex_);
        pending_tasks_.push_back(std::move(task)); // emplace_back
    }

    if (!IsInLoopThread() || !current_active_channel_) // FIXME
    {
        Wakeup();
    }
}

TimerId EventLoop::RunAt(const Timestamp& time, const TimeoutCallback& callback)
{
    AssertInLoopThread();
    auto now = Timestamp::Now().MicroSecondsSinceEpoch();
    bool reset = timeouts_->NextExpiration() > time.MicroSecondsSinceEpoch();
    auto id = timeouts_->Add(now, time.MicroSecondsSinceEpoch(), callback);
    if (reset)
    {
        ResetTimerfd(timer_fd_, time.MicroSecondsSinceEpoch());
    }
    LOG(TRACE) << "EventLoop " << this << " RunAt return id " << id;
    return id;
}

TimerId EventLoop::RunAfter(int delay_in_milliseconds, const TimeoutCallback& callback)
{
    return RunAt(AddTime(Timestamp::Now(), delay_in_milliseconds*1000), callback);
}

TimerId EventLoop::RunEvery(int interval_in_milliseconds, const TimeoutCallback& callback)
{
    AssertInLoopThread();
    auto now = Timestamp::Now().MicroSecondsSinceEpoch();
    bool reset = timeouts_->NextExpiration() > (now +interval_in_milliseconds*1000);
    auto id = timeouts_->AddRepeating(now, interval_in_milliseconds*1000, callback);
    if (reset)
    {
        ResetTimerfd(timer_fd_, timeouts_->NextExpiration());
    }
    return id;
}

void EventLoop::Cancel(TimerId id)
{
    AssertInLoopThread();
    LOG(TRACE) << "EventLoop " << this << " Cancel " << id.get();
    timeouts_->Cancel(id.get());
}

void EventLoop::UpdateChannel(Channel* channel)
{
    DCHECK(channel->OwnerLoop() == this);
    AssertInLoopThread();
    poller_->UpdateChannel(channel);
}

void EventLoop::RemoveChannel(Channel* channel)
{
    DCHECK(channel->OwnerLoop() == this);
    AssertInLoopThread();

    if (current_active_channel_)
    {
        DCHECK(current_active_channel_ == channel ||
            std::find(active_channels_.begin(), active_channels_.end(), channel) == active_channels_.end())
            << "Can not remove when processing channel";
    }
    poller_->RemoveChannel(channel);
}

void EventLoop::AbortNotInLoopThread() const
{
    LOG(FATAL) << "EventLoop::AbortNotInLoopThread - EventLoop " << this
               << " was created in thread " << tid_
               << ", current thread id is " <<  ThisThread::tid();
}

void EventLoop::Wakeup()
{
    uint64_t one = 1;
    ssize_t n =  ::write(wakeup_fd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        LOG(ERROR) << "EventLoop::Wakeup writes " << n << " bytes instead of 8";
    }
}

void EventLoop::OnWakeup()
{
    uint64_t one = 1;
    ssize_t n = ::read(wakeup_fd_, &one, sizeof(one));
    if (n != sizeof(one))
    {
        LOG(ERROR) << "EventLoop::OnWakeup reads " << n << " bytes instead of 8";
    }
}

void EventLoop::OnTimer()
{
    ReadTimerfd(timer_fd_);
    ResetTimerfd(timer_fd_, timeouts_->Run(Timestamp::Now().MicroSecondsSinceEpoch()));
}

void EventLoop::RunPendingTasks()
{
    std::vector<Task> tasks;
    {
        MutexLock lock(mutex_);
        tasks.swap(pending_tasks_);
    }

    for (auto it = tasks.begin(); it != tasks.end(); ++it)
    {
        (*it)();
    }
}

bool EventLoop::IsInLoopThread() const
{
    return tid_ == ThisThread::tid();
}

void EventLoop::AssertInLoopThread() const
{
    assert(IsInLoopThread());
}
} // namespace claire
