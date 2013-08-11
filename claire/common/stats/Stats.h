// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_STATS_STATS_H
#define _CLAIRE_COMMON_STATS_STATS_H

#include <vector>

#include <boost/noncopyable.hpp>

#include <claire/common/stats/Gauge.h>
#include <claire/common/stats/Metric.h>
#include <claire/common/stats/Counter.h>
#include <claire/common/threading/Mutex.h>

namespace claire {

class Stats : boost::noncopyable
{
public:
    static Stats* default_instance();

    void AddGauge(Gauge* gauge)
    {
        MutexLock lock(mutex_);
        gauges_.push_back(gauge);
    }

    std::vector<Gauge*> GetAllGauges() const
    {
        MutexLock lock(mutex_);
        std::vector<Gauge*> v(gauges_.begin(), gauges_.end());
        return v;
    }

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
    std::ptr_vector<Gauge> gauges_;
};

class GaugeRegisterer : boost::noncopyable
{
public:
    template<typename T>
    GaugeRegisterer(const T* v)
    {
        Gauge* gauge = new GaugeBindVariable(v);
        ::claire::Stats::default_instance()->AddGauge(gauge);
    }

    template<typename T>
    GaugeRegisterer(const boost::function<T()>& func)
    {
        Gauge* gauge = new GaugeBindFunction(func);
        ::claire::Stats::default_instance()->AddGauge(gauge);
    }
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

inline std::vector<<Gauge*> GetAllGauges()
{
    return Stats::default_instance()->GetAllGauges();
}

inline std::vector<Metric*> GetAllMetrics()
{
    return Stats::default_instance()->GetAllMetrics();
}

inline std::vector<Counter*> GetAllCounters()
{
    return Stats::default_instance()->GetAllCounters();
}

} // namespace claire

#define DEFINE_GAUGE(name, value) \
    namespace claire { \
    ::claire::GaugeRegisterer cg_##name(name, value); \
    }

#define DEFINE_METRIC(name) \
    namespace claire { \
    static ::claire::Metric METRIC_##name(#name); \
    ::claire::MetricRegisterer cm_##name(&METRIC_##name); \
    using ::claire::METRIC_##name; \
    }

#define DEFINE_COUNTER(name) \
    namespace claire { \
    static ::claire::Counter COUNTER_##name(#name); \
    ::claire::CounterRegisterer cc_##name(&COUNTER_##name); \
    using ::claire::COUNTER_##name; \
    }

#endif // _CLAIRE_COMMON_STATS_STATS_H
