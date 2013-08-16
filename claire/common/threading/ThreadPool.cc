// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <claire/common/threading/ThreadPool.h>

#include <boost/bind.hpp>

#include <claire/common/base/Exception.h>
#include <claire/common/logging/Logging.h>

using namespace claire;

void ThreadPool::Start(int num_threads)
{
    if (running_)
    {
        return ;
    }
    running_ = true;

    threads_.reserve(num_threads);
    for (int i = 0;i < num_threads;i++)
    {
        char buf[32];
        snprintf(buf, sizeof buf, "%d", i);
        threads_.push_back(new claire::Thread(
            boost::bind(&ThreadPool::RunInThread, this), name_+buf));
        threads_[i].Start();
    }
}

void ThreadPool::Stop()
{
    if (!running_)
    {
        return;
    }

    running_ = false;
    {
        MutexLock lock(mutex_);
        cond_.NotifyAll();
    }

    for_each(threads_.begin(), threads_.end(),
             boost::bind(&claire::Thread::Join, _1));

}

void ThreadPool::Run(const Task& task)
{
    CHECK(!threads_.empty());

    {
        MutexLock lock(mutex_);
        queue_.push_back(task);
        cond_.Notify();
    }
}

ThreadPool::Task ThreadPool::Take()
{
    MutexLock lock(mutex_);
    while (queue_.empty() && running_)
    {
        cond_.Wait();
    }

    Task task;
    if (!queue_.empty())
    {
        task = queue_.front();
        queue_.pop_front();
    }

    return task;
}

void ThreadPool::RunInThread()
{
    try
    {
        while (running_)
        {
            Task task(Take());
            if (task)
            {
                task();
            }
        }
    }
    catch (const Exception& ex)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        fprintf(stderr, "stack trace: %s\n", ex.StackTrace());
        abort();
    }
    catch (const std::exception& ex)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    }
    catch (...)
    {
        fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
        throw; // rethrow
    }
}
