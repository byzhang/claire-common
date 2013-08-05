// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_THREADING_CONDITION_H_
#define _CLAIRE_COMMON_THREADING_CONDITION_H_

#include <pthread.h>

#include <boost/noncopyable.hpp>

#include <claire/common/threading/Mutex.h>
#include <claire/common/logging/Logging.h>

namespace claire {

class Condition : boost::noncopyable
{
public:
    explicit Condition(Mutex& mutex)
        : mutex_(mutex)
    {
        auto ret = pthread_cond_init(&cond_, NULL);
        CHECK_ERR(ret) << "pthread_cond_init failed";
    }

    ~Condition()
    {
        auto ret = pthread_cond_destroy(&cond_);
        DCHECK_ERR(ret) << "pthread_cond_destroy failed";
    }

    void Wait()
    {
        auto ret = pthread_cond_wait(&cond_, &(mutex_.mutex_));
        DCHECK_ERR(ret) << "pthread_cond_wait failed";
    }

    // return true if timeout, otherwise return false
    bool WaitForSeconds(int seconds)
    {
        struct timespec abstime;
        ::clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += seconds;

        Mutex::UnAssignGuard ng(mutex_);
        return ETIMEDOUT == pthread_cond_timedwait(&cond_, &(mutex_.mutex_), &abstime);
    }

    void Notify()
    {
        auto ret = pthread_cond_signal(&cond_);
        DCHECK_ERR(ret) << "pthread_cond_signal failed";
    }

    void NotifyAll()
    {
        auto ret = pthread_cond_broadcast(&cond_);
        DCHECK_ERR(ret) << "pthread_cond_broadcast failed";
    }

private:
    Mutex& mutex_;
    pthread_cond_t cond_;
};

} // namespace claire

#endif // _CLAIRE_COMMON_THREADING_CONDITION_H_
