// Copyright (c) 2013 The claire-common Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <assert.h>

#include <claire/common/threading/Mutex.h>
#include <claire/common/threading/ThisThread.h>
#include <claire/common/logging/Logging.h>

namespace claire {

Mutex::Mutex()
    : holder_(0)
{
    DCHECK_ERR(pthread_mutex_init(&mutex_, NULL));
}

Mutex::~Mutex()
{
    DCHECK_ERR(pthread_mutex_destroy(&mutex_));
}

void Mutex::Lock()
{
    //PROFILE_MUTEX_START_LOCK();
    DCHECK_ERR(pthread_mutex_lock(&mutex_));
    AssignHolder();
    //PROFILE_MUTEX_LOCKED();
}

void Mutex::Unlock()
{
    //PROFILE_MUTEX_START_UNLOCK();
    UnAssignHolder();
    DCHECK_ERR(pthread_mutex_unlock(&mutex_));
    //PROFILE_MUTEX_UNLOCKED();
}

bool Mutex::IsLockedByThisThread() const
{
    return holder_ == ThisThread::tid();
}

void Mutex::AssertLocked() const
{
    assert(IsLockedByThisThread());
}

void Mutex::AssignHolder()
{
    holder_ = ThisThread::tid();
}

void Mutex::UnAssignHolder()
{
    holder_ = 0;
}

} // namespace claire
