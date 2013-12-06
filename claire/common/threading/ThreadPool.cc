// Copyright (c) 2013 The claire-common Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <claire/common/threading/ThreadPool.h>

#include <boost/bind.hpp>

#include <claire/common/base/Exception.h>
#include <claire/common/logging/Logging.h>

namespace claire {

ThreadPool::ThreadPool(const std::string& name)
    : mutex_(),
      not_empty_(mutex_),
      not_full_(mutex_),
      name_(name),
      max_queue_size_(0),
      running_(false)
{}

ThreadPool::~ThreadPool()
{
    if (running_)
    {
        Stop();
    }
}

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
        char id[32];
        snprintf(id, sizeof id, "%d", i);
        threads_.push_back(new claire::Thread(
            boost::bind(&ThreadPool::RunInThread, this), name_+id));
        threads_[i].Start();
    }
}

void ThreadPool::Stop()
{
    if (!running_)
    {
        return ;
    }
    running_ = false;

    {
        MutexLock lock(mutex_);
        not_empty_.NotifyAll();
    }

    for_each(threads_.begin(),
             threads_.end(),
             boost::bind(&Thread::Join, _1));

}

void ThreadPool::Run(const Task& task)
{
    if (threads_.empty())
    {
        task();
    }
    else
    {
        MutexLock lock(mutex_);
        while (IsFull())
        {
            not_full_.Wait();
        }

        queue_.push_back(task);
        not_empty_.Notify();
    }
}

void ThreadPool::Run(Task&& task)
{
    if (threads_.empty())
    {
        task();
    }
    else
    {
        MutexLock lock(mutex_);
        while (IsFull())
        {
            not_full_.Wait();
        }

        queue_.push_back(std::move(task));
        not_empty_.Notify();
    }
}

ThreadPool::Task ThreadPool::Take()
{
    MutexLock lock(mutex_);
    while (queue_.empty() && running_)
    {
        not_empty_.Wait();
    }

    Task task;
    if (!queue_.empty())
    {
        task = queue_.front();
        queue_.pop_front();
        if (max_queue_size_ > 0)
        {
            not_full_.Notify();
        }
    }
    return task;
}

bool ThreadPool::IsFull() const
{
    mutex_.AssertLocked();
    return max_queue_size_ > 0 && queue_.size() >= max_queue_size_;
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
    catch (const Exception& e)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", e.what());
        fprintf(stderr, "stack trace: %s\n", e.stack_trace());
        abort();
    }
    catch(const std::exception& e)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", e.what());
        abort();
    }
    catch (...)
    {
        fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
        throw; // rethrow
    }
}

} // namespace claire
