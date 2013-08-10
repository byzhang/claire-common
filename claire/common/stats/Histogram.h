// Code taken from LevelDB
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_STATS_HISTOGRAM_H_
#define _CLAIRE_COMMON_STATS_HISTOGRAM_H_

#include <string>

namespace claire {

class Histogram
{
public:
    void Clear();
    void Add(double value);

    std::string ToString() const;

    double get_min() const
    {
        return min_;
    }

    double get_max() const
    {
        return max_;
    }

    double get_sum() const
    {
        return sum_;
    }

    double Average() const
    {
        if (sum_ == 0) return 0.0;
        return sum_ / num_;
    }

    double Median() const
    {
        return Percentile(50.0);
    }

private:
    double min_;
    double max_;
    double num_;
    double sum_;
    double sum_squares_;

    static const int kNumBuckets = 154;
    static const double kBucketLimit[kNumBuckets];
    double buckets_[kNumBuckets];

    double Percentile(double p) const;
    double StandardDeviation() const;
};

} // namespace claire

#endif // _CLAIRE_COMMON_STATS_HISTOGRAM_H_
