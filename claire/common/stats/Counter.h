// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_STATS_COUNTER_H_
#define _CLAIRE_COMMON_STATS_COUNTER_H_

#include <stdio.h>

#include <string>

#include <boost/atomic.hpp>
#include <boost/noncopyable.hpp>

namespace claire {

class Counter : boost::noncopyable
{
public:
    Counter(const char* nameArg)
        : name_(nameArg),
          count_(0)
    { }

    void incr()
    {
        count_++;
    }

    std::string name() const
    {
        return name_;
    }

    std::string ToString() const
    {
        char buf[32];
        snprintf(buf, sizeof buf, "%ld", count_.load(boost::memory_order_relaxed));
        return buf;
    }

    int64_t get_count() const
    {
        return count_;
    }

private:
    const std::string name_;
    boost::atomic<int64_t> count_;
};

} // namespace claire

#endif // _CLAIRE_COMMON_STATS_COUNTER_H_
