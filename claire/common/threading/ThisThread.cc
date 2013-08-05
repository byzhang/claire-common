// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <claire/common/threading/ThisThread.h>

#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include <boost/type_traits/is_same.hpp>

namespace claire {
namespace detail {

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

void afterfork()
{
    claire::ThisThread::t_cached_tid = 0;
    claire::ThisThread::t_thread_name = "main";
    ThisThread::tid();
}

class ThreadNameInitializer
{
public:
    ThreadNameInitializer()
    {
        claire::ThisThread::t_thread_name = "main";
        ThisThread::tid();
        pthread_atfork(NULL, NULL, &afterfork);
    }
};

ThreadNameInitializer init;
} // namespace detail

namespace ThisThread {

__thread int t_cached_tid = 0;
__thread char t_tid_string[32];
__thread const char* t_thread_name = "unknown";

static_assert(boost::is_same<int, pid_t>::value, "pid_t should equal int");

void CacheTid()
{
    if (t_cached_tid == 0)
    {
        t_cached_tid = detail::gettid();
        int n = snprintf(t_tid_string, sizeof t_tid_string, "%5d ", t_cached_tid);
        assert(n == 6);
        (void) n;
    }
}

bool IsMainThread()
{
    return tid() == ::getpid();
}

} // namespace ThisThread
} // namespace claire

