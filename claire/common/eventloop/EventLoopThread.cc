#include <claire/common/eventloop/EventLoopThread.h>

using namespace claire;

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb)
    : loop_(NULL),
      thread_(boost::bind(&EventLoopThread::ThreadMain, this)),
      mutex_(),
      cond_(mutex_),
      callback_(cb)
{ }

EventLoopThread::~EventLoopThread()
{
    loop_->quit();
    thread_.Join();
}

EventLoop * EventLoopThread::StartLoop()
{
    assert(!thread_.started());
    thread_.start();

    {
        MutexLock lock(mutex_);
        while(loop_ == NULL)
        {
            cond_.Wait();
        }
    }

    return loop_;
}

void EventLoopThread::ThreadMain()
{
    EventLoop loop;
    if (callback_)
    {
        callback_(&loop);
    }

    {
        MutexLock lock(mutex_);
        loop_ = &loop;
        cond_.Notify();
    }

    loop.loop();
}

