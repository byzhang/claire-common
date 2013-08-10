// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_STATS_STATS_H
#define _CLAIRE_COMMON_STATS_STATS_H

#include <vector>

#include <boost/noncopyable.hpp>

#include <claire/common/stats/Metric.h>
#include <claire/common/stats/Counter.h>
#include <claire/common/threading/Mutex.h>

namespace claire {

class Stats : boost::noncopyable
{
public:
    static Stats* default_instance();

    void AddMetric(Metric* metric)
    {
        MutexLock lock(mutex_);
        metrics_.push_back(metric);
    }

    std::vector<Metric*> GetAllMetrics() const
    {
        MutexLock lock(mutex_);
        return metrics_;
    }

    void AddCounter(Counter* counter)
    {
        MutexLock lock(mutex_);
        counters_.push_back(counter);
    }

    std::vector<Counter*> GetAllCounters() const
    {
        MutexLock lock(mutex_);
        return counters_;
    }

private:
    mutable Mutex mutex_;
    std::vector<Metric*> metrics_;
    std::vector<Counter*> counters_;
};

class MetricRegisterer : boost::noncopyable
{
public:
    MetricRegisterer(Metric* metric)
    {
        ::claire::Stats::default_instance()->AddMetric(metric);
    }
};

class CounterRegisterer : boost::noncopyable
{
public:
    CounterRegisterer(Counter* counter)
    {
        ::claire::Stats::default_instance()->AddCounter(counter);
    }
};

std::vector<Metric*> GetAllMetrics()
{
    Stats::default_instance()->GetAllMetrics();
}

std::vector<Counter*> GetAllCounters()
{
    Stats::default_instance()->GetAllCounters();
}

#define DEFINE_METRIC(name) \
    namespace claire { \
    static ::claire::Metric METRIC_##name(#name); \
    ::claire::MetricRegisterer cm_##name(&METRIC_##name); \
    using ::claire::METRIC_##name; \
    }
}

#define DEFINE_COUNTER(name) \
    namespace claire { \
    static ::claire::Counter COUNTER_##name(#name); \
    ::claire::CounterRegisterer cc_##name(&COUNTER_##name); \
    using ::claire::COUNTER_##name; \
    }

#endif // _CLAIRE_COMMON_STATS_STATS_H
