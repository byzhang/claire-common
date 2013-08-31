#include <claire/common/eventloop/EventLoopThreadPool.h>

#include <claire/common/eventloop/EventLoop.h>
#include <claire/common/eventloop/EventLoopThread.h>

using namespace claire;

void EventLoopThreadPool::Start(const ThreadInitCallback& cb)
{
    base_loop_->AssertInLoopThread();
    if (started_)
    {
        return ;
    }
    started_ = true;

    for (int i = 0;i < num_threads_; ++i)
    {
        threads_.push_back(new EventLoopThread(cb));
        loops_.push_back(threads.back().StartLoop());
    }

    if (num_threads_ == 0 && cb)
    {
        cb(base_loop_);
    }
}

EventLoop* EventLoopThreadPool::GetNextLoop()
{
    base_loop_->AssertInLoopThread();

    if (loops_.empty())
    {
        return base_loop_;
    }
    else
    {
        // round-robin
        auto loop = loops_[next_++];
        if (next_ >= static_cast<int>(loops_.size()))
        {
            next_ = 0;
        }

        return loop;
    }
}

