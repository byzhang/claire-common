// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_THREADING_COUNTDOWNLATCH_H_
#define _CLAIRE_COMMON_THREADING_COUNTDOWNLATCH_H_

#include <boost/noncopyable.hpp>

#include <claire/common/threading/Mutex.h>
#include <claire/common/threading/Condition.h>


namespace claire {

class CountDownLatch : boost::noncopyable
{
public:
    CountDownLatch(int count)
        : mutex_(),
          cond_(mutex_),
          count_(count)
    { }

    ~CountDownLatch()
    { }

    void Wait()
    {
        MutexLock lock(mutex_);
        while (count_ > 0)
        {
            cond_.Wait();
        }
    }

    void CountDown()
    {
        MutexLock lock(mutex_);
        --count_;
        if (count_ == 0)
        {
            cond_.NotifyAll();
        }
    }

    int get_count() const
    {
        MutexLock lock(mutex_);
        return count_;
    }

private:
    mutable Mutex mutex_;
    Condition cond_;
    int count_;
};

} // namespace claire

#endif // _CLAIRE_COMMON_THREADING_COUNTDOWNLATCH_H_
