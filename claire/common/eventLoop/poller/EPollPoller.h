#ifndef _CLAIRE_COMMON_EVENTLOOP_POLLER_EPOLLPOLLER_H_
#define _CLAIRE_COMMON_EVENTLOOP_POLLER_EPOLLPOLLER_H_

#include <claire/common/eventloop/poller/Poller.h>

#include <set>
#include <vector>

struct epoll_event;

namespace claire {

class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop* loop);
    virtual ~EPollPoller();

    virtual void poll(const boost::chrono::milliseconds timeout, ChannelList* actives);

    virtual void UpdateChannel(Channel* channel);
    virtual void RemoveChannel(Channel* channel);

    void ReArmAll();
    
private:
    void Update(int operation, Channel* channel);

    int epollfd_;

    struct ChannleInfo
    {
        Channel* channel;
        uint32_t egen;
        int mask;
    };

    std::vector<struct epoll_event> events_;
    std::vector<ChannelInfo> channels_;
    std::set<int> eperms_;

    static const int kMaxWaitTime = 59.743;
};

} // namespace claire

#endif // _CLAIRE_COMMON_EVENTLOOP_POLLER_EPOLLPOLLER_H_