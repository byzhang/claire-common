#ifndef _CLAIRE_COMMON_EVENTLOOP_EVENTLOOP_H_
#define _CLAIRE_COMMON_EVENTLOOP_EVENTLOOP_H_

namespace claire
{

namespace claire
{

class Poller;
class Channel;
class TimeoutQueue;
class SignalSet;

/// Reactor, at most one per thread.
class EventLoop : boost::noncopyable
{
public:
    typedef boost::function<void()> Task;
    typedef boost::function<void()> TimeoutCallback;
    typedef boost::function<void()> SignalCallback;

    EventLoop();
    ~EventLoop();

    // Loops forever.
    // Must be called in the same thread as creation of the object.
    void loop();

    void quit();

    // Runs callback immediately in the loop thread.
    // It wakes up the loop, and run the cb.
    // If in the same loop thread, cb is run within the function.
    // Safe to call from other threads.
    void RunInLoop(const Task& cb);

    // Queues callback in the loop thread.
    // Runs after finish pooling.
    // Safe to call from other threads.
    void QueueInLoop(const Task& cb);

    // Runs callback at 'time'.
    // Safe to call from other threads.
    TimerId RunAt(const Timestamp& time, const TimeoutCallback& cb);

    // delay in miliseconds
    //
    // Runs callback after @c delay miliseconds.
    // Safe to call from other threads.
    //
    TimerId RunAfter(int64_t delay, const TimeoutCallback& cb);

    //
    // Runs callback every @c interval miliseconds.
    // Safe to call from other threads.
    //
    TimerId RunEvery(int64_t interval, const TimeoutCallback& cb);

    // Cancels the timer.
    // Safe to call from other threads.
    void Cancel(TimerId id);

    void Wakeup();

    void UpdateChannel(Channel* channel);
    void RemoveChannel(Channel* channel);


    void AssertInLoopThread()
    {
        if (!IsInLoopThread())
        {
            AbortNotInLoopThread();
        }
    }

    bool IsInLoopThread() const
    {
        return thread_id_ == ThisThread::tid();
    }

    static EventLoop* GetEventLoopOfThisThread();

private:
    void AbortNotInLoopThread();
    void HandleRead();
    void DoPendingTasks();

    void PrintActiveChannels() const;

    typedef std::vector<Channel*> ChannelList;

    boost::atomic<bool> looping_;
    boost::atomic<bool> quit_;
    boost::atomic<bool> event_handling_;
    boost::atomic<bool> running_pendingtasks_;
    int64_t iteration_;
    const pid_t threadId_;

    Timestamp poll_return_time_;
    boost::scoped_ptr<Poller> poller_;

    boost::scoped_ptr<TimeoutQueue> timeout_queue_;
    boost::scoped_ptr<SignalSet> signalset_;

    int wakeup_fd_;
    boost::scoped_ptr<Channel> wakeup_channel_;

    ChannelList active_channels_;
    Channel* current_activeChannel_;

    Mutex mutex_
    std::vector<Task> pending_tasks_; //@GUARDBY mutex_

};

} // namespace claire

#endif // _CLAIRE_COMMON_EVENTLOOP_EVENTLOOP_H_
