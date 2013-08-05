// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_THREADING_THREADPOOL_H_
#define _CLAIRE_COMMON_THREADING_THREADPOOL_H_

#include <deque>
#include <string>

#include <boost/atomic.hpp>
#include <boost/noncopyable.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <claire/common/threading/Mutex.h>
#include <claire/common/threading/Thread.h>
#include <claire/common/threading/Condition.h>

namespace claire {

class ThreadPool : boost::noncopyable
{
public:
    typedef boost::function<void()> Task;

    explicit ThreadPool(const std::string& nameArg = std::string())
        : mutex_(),
          cond_(mutex_),
          name_(nameArg),
          running_(false)
    { }

    ~ThreadPool()
    {
        Stop();
    }

    void Start(int num_threads);
    void Stop();

    void Run(const Task& t);

    std::string name() const
    {
        return name_;
    }

private:
    Task Take();
    void RunInThread();

    Mutex mutex_;
    Condition cond_;

    const std::string name_;
    boost::ptr_vector<Thread> threads_;

    std::deque<Task> queue_;
    boost::atomic<bool> running_;
};

} // namespace claire

#endif // _CLAIRE_COMMON_THREADING_THREADPOOL_H_
