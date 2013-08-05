// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_THREADING_MUTEX_H_
#define _CLAIRE_COMMON_THREADING_MUTEX_H_

#include <pthread.h>

#include <boost/noncopyable.hpp>

#include <claire/common/logging/Logging.h>
#include <claire/common/threading/ThisThread.h>

namespace claire {

class Mutex : boost::noncopyable
{
public:
    Mutex()
        : holder_(0)
    {
        auto ret = pthread_mutex_init(&mutex_, NULL);
        CHECK_ERR(ret) << "pthread_mutex_init failed";
    }

    ~Mutex()
    {
        auto ret = pthread_mutex_destroy(&mutex_);
        DCHECK_ERR(ret) << "pthread_mutex_destroy failed";
    }

    void Lock()
    {
        auto ret = pthread_mutex_lock(&mutex_);
        DCHECK_ERR(ret) << "pthread_mutex_lock failed";
        holder_ = ThisThread::tid();
    }

    void Unlock()
    {
        holder_ = 0;
        auto ret = pthread_mutex_unlock(&mutex_);
        DCHECK_ERR(ret) << "pthread_mutex_unlock failed";
    }

    bool IsHeldByThisThread() const
    {
        return holder_ == ThisThread::tid();
    }

    void AssertHeld() const
    {
        assert(IsHeldByThisThread());
    }

private:
    friend class Condition;

    class UnAssignGuard
    {
    public:
        UnAssignGuard(Mutex& owner)
            : owner_(owner)
        {
            owner_.UnAssignHolder();
        }

        ~UnAssignGuard()
        {
            owner_.AssignHolder();
        }

    private:
        Mutex& owner_;
    };

    void AssignHolder()
    {
        holder_ = ThisThread::tid();
    }

    void UnAssignHolder()
    {
        holder_ = 0;
    }

    pid_t holder_;
    pthread_mutex_t mutex_;
};

class MutexLock : boost::noncopyable
{
public:
    MutexLock(Mutex& mutex)
        : mutex_(mutex)
    {
        mutex_.Lock();
    }

    ~MutexLock()
    {
        mutex_.Unlock();
    }

private:
    Mutex& mutex_;
};

} // namespace claire

#endif // _CLAIRE_COMMON_THREADING_MUTEX_H_
