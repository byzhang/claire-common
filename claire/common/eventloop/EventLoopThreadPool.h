#ifndef _CLAIRE_COMMON_EVENTLOOP_EVENTLOOPTHREADPOOL_H_
#define _CLAIRE_COMMON_EVENTLOOP_EVENTLOOPTHREADPOOL_H_

#include <vector>

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <claire/common/logging/Logging.h>

namespace claire {

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : boost::noncopyable
{
public:
    typedef boost::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThreadPool(EventLoop* base_loop)
        : base_loop_(base_loop),
          started_(false),
          num_threads_(0),
          next_(0)
    {
        CHECK(baseLoop_ != NULL);
    }

    ~EventLoopThreadPool()
    { }

    void SetThreadNum(int num_threads)
    {
        num_threads_ = num_threads;
    }

    void Start(const ThreadInitCallback& cb = ThreadInitCallback());

    EventLoop *GetNextLoop();

private:
    EventLoop* base_loop_;
    bool started_;
    int num_threads_;
    int next_;

    boost::ptr_vector<EventLoopThread> threads_;
    std::vector<EventLoop*> loops_;
};

} // namespace claire

#endif // _CLAIRE_COMMON_EVENTLOOP_EVENTLOOPTHREADPOOL_H_
