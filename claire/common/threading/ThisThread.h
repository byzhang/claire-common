// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_THREADING_THISTHREAD_H_
#define _CLAIRE_COMMON_THREADING_THISTHREAD_H_

#include <pthread.h>

namespace claire {
namespace ThisThread {

extern __thread int t_cached_tid;
extern __thread char t_tid_string[32];
extern __thread const char* t_thread_name;

void CacheTid();
bool IsMainThread();

inline int tid()
{
    if (t_cached_tid == 0)
    {
        CacheTid();
    }

    return t_cached_tid;
}

inline const char* TidString()
{
    return t_tid_string;
}

inline const char* name()
{
    return t_thread_name;
}

inline void set_thread_name(const char* name)
{
    t_thread_name = name;
}

} // namespace ThisThread
} // namespace claire

#endif // _CLAIRE_COMMON_THREADING_THISTHREAD_H_
