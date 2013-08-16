// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <claire/common/threading/Thread.h>

#include <assert.h>

#include <boost/bind.hpp>
#include <boost/atomic.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <claire/common/base/Exception.h>
#include <claire/common/logging/Logging.h>
#include <claire/common/threading/ThisThread.h>

namespace claire {
namespace detail {

boost::atomic<int> num_created_;

struct ThreadData
{
    boost::function<void()> func_;
    boost::weak_ptr<pid_t> weak_tid_;
    const std::string name_;

    ThreadData(const boost::function<void()>& func,
               const boost::shared_ptr<pid_t>& tid,
               const std::string& name)
        : func_(func),
          weak_tid_(tid),
          name_(name)
    { }

    void RunInThread()
    {
        auto tid = claire::ThisThread::tid();

        auto ptid = weak_tid_.lock();
        if (ptid)
        {
            *ptid = tid;
            ptid.reset();
        }

        claire::ThisThread::set_thread_name(name_.c_str());
        try
        {
            func_();
            claire::ThisThread::set_thread_name("finished");
        }
        catch (const Exception& ex)
        {
            claire::ThisThread::set_thread_name("crashed");
            fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            fprintf(stderr, "stack trace: %s\n", ex.StackTrace());
            abort();
        }
        catch (const std::exception& ex)
        {
            claire::ThisThread::set_thread_name("crashed");
            fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            abort();
        }
        catch (...)
        {
            claire::ThisThread::set_thread_name("crashed");
            fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
            throw; // rethrow
        }
    }
};

void* StartThread(void *obj)
{
    auto data = static_cast<ThreadData*>(obj);
    data->RunInThread();
    delete data;
    return NULL;
}

} // namespace detail
} // namespace claire

using namespace claire;

Thread::Thread(const ThreadMain& func, const std::string& nameArg)
    : started_(false),
      joined_(false),
      pthread_id_(0),
      tid_(new pid_t(0)),
      func_(func),
      name_(nameArg)
{
    detail::num_created_++;
}

Thread::~Thread()
{
    if (started_ && !joined_)
    {
        pthread_detach(pthread_id_);
    }
}

void Thread::Start()
{
    assert(!started_);
    started_ = true;

    auto data = new detail::ThreadData(func_, tid_, name_);
    if (pthread_create(&pthread_id_, NULL, &detail::StartThread, data))
    {
        started_ = false;
        delete data; // use scopeguard FIXME
        LOG(FATAL) << "pthread_create failed";
    }
}

int Thread::Join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthread_id_, NULL);
}

int Thread::num_created()
{
    return detail::num_created_;
}

