// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// This is an internal header file, you should not include this.

#ifndef _CLAIRE_COMMON_EVENTLOOP_TIMEOUTQUEUE_H_
#define _CLAIRE_COMMON_EVENTLOOP_TIMEOUTQUEUE_H_

#include <claire/net/Channel.h>
#include <claire/net/TimerId.h>
#include <claire/base/Timestamp.h>

#include <boost/atomic.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <stdint.h>

namespace claire {

class EventLoop;

class TimeoutQueue : boost::noncopyable
{
public:
    typedef boost::function<void()> TimeoutCallback;

    TimeoutQueue(EventLoop* loop);
    ~TimeoutQueue();

    /// Add a repeating timeout event that will fire every "interval" time units
    /// if interval < 0, then do not repeat
    /// (it will first fire when run*() is called with a time value >= expiration
    ///
    /// Thread safe
    TimerId Add(const TimeoutCallback& callback,
                Timestamp expiration,
                int64_t interval = -1);

    /// Erase a given timeout event
    /// Thread safe
    void Cancel(TimerId id);

private:
    /// Process all events that are due at times <= "now" by calling their
    /// callbacks.
    ///
    /// Callbacks are allowed to call back into the queue and add / erase events;
    /// they might create more events that are already due.  In this case,
    /// runOnce(true) will only go through the queue once, and return a "next
    /// expiration" time in the past or present (<= now); else runOnce(false)
    /// will process the queue again, until there are no events already due.
    /// Note that it is then possible for run Loop to never return if
    /// callbacks re-add themselves to the queue (or if you have repeating
    /// callbacks with an interval of 0).
    ///
    /// Return the time that the next event will be due (same as
    /// nextExpiration(), below)
    Timestamp RunOnce(bool runOnce);

    void AddInLoop(TimerId id,
                   Timestamp expiration,
                   int64_t interval,
                   const TimeoutCallback& callback);
    void CancelInLoop(TimerId id);

    /// Return the time that the next event will be due.
    Timestamp NextExpiration() const;

    void HandleRead();

    struct Event
    {
        TimerId id;
        Timestamp expiration;
        int64_t interval;
        TimeoutCallback callback;
    };

    typedef boost::multi_index_container<
        Event,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<boost::multi_index::member<
                Event, TimerId, &Event::id
            >>,
            boost::multi_index::ordered_non_unique<boost::multi_index::member<
                Event, Timestamp, &Event::expiration
            >>
        >
    > Set;

    enum
    {
        kById = 0,
        kByExpiration = 1
    };

    EventLoop* loop_;
    const int timerfd_;
    Channel timerfd_channel_;

    Set timeouts_;
    boost::atomic<int64_t> next_;
};

} // namespace claire

#endif // _CLAIRE_NET_TIMEOUTQUEUE_H_

