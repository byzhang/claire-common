// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_STATS_METRIC_H_
#define _CLAIRE_COMMON_STATS_METRIC_H_

#include <boost/noncopyable.hpp>

#include <claire/common/stats/Histogram.h>
#include <claire/common/threading/Mutex.h>

namespace claire {

class Metric : boost::noncopyable
{
public:
    Metric(const char* nameArg)
        : name_(nameArg),
          histogram_(0.01, 10000)
    { }

    std::string name() const
    {
        return name_;
    }

    void Clear()
    {
        MutexLock lock(mutex_);
        histogram_.Clear();
    }

    void Add(int64_t v)
    {
        MutexLock lock(mutex_);
        histogram_.Add(v);
    }

    int64_t min() const
    {
        MutexLock lock(mutex_);
        return histogram_.min();
    }

    int64_t max() const
    {
        MutexLock lock(mutex_);
        return histogram_.max();
    }

    int64_t median() const
    {
        MutexLock lock(mutex_);
        double q = 50.00;
        auto v = histogram_.GetQuantiles(&q, 1);
        return v[0];
    }

private:
    const std::string name_;
    mutable Mutex mutex_;
    Histogram histogram_;
};

}

#endif // _CLAIRE_COMMON_STATS_METRIC_H_
