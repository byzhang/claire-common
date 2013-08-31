#ifndef _CLAIRE_COMMON_EVENTLOOP_EVENTLOOPTREAD_H_
#define _CLAIRE_COMMON_EVENTLOOP_EVENTLOOPTREAD_H_

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <claire/common/threading/Thread.h>
#include <claire/common/threading/Mutex.h>
#include <claire/common/threading/Condition.h>

namespace claire {

class EventLoop;

class EventLoopThread : boost::noncopyable
{
public:
    typedef boost::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback());
    ~EventLoopThread();

    EventLoop* StartLoop();

private:
    void ThreadMain();

    EventLoop* loop_;
    Thread thread_;
    Mutex mutex_;
    Condition cond_;
    ThreadInitCallback callback_;
};

} // namespace claire

#endif // _CLAIRE_COMMON_EVENTLOOP_EVENTLOOPTREAD_H_
