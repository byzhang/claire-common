// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_TIME_TIMESTAMP_H_
#define _CLAIRE_COMMON_TIME_TIMESTAMP_H_

#include <boost/operators.hpp>

namespace claire
{

class Timestamp : boost::less_than_comparable<Timestamp>
{
public:
    Timestamp()
        : microseconds_since_epoch_(0)
    {}

    explicit Timestamp(int64_t tm)
        : microseconds_since_epoch_(tm)
    {}

    void swap(Timestamp& other)
    {
        std::swap(microseconds_since_epoch_, other.microseconds_since_epoch_);
    }

    std::string ToString() const;
    std::string ToFormattedString() const;

    bool Valid() const
    {
        return microseconds_since_epoch_ > 0;
    }

    time_t SecondsSinceEpoch() const
    {
        return static_cast<time_t>(microseconds_since_epoch_/kMicroSecondsPerSecond);
    }

    int64_t MicroSecondsSinceEpoch() const
    {
        return microseconds_since_epoch_;
    }

    static Timestamp Now();
    static Timestamp Invalid()
    {
        return Timestamp();
    }

    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t microseconds_since_epoch_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
    return lhs.MicroSecondsSinceEpoch() < rhs.MicroSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
{
    return lhs.MicroSecondsSinceEpoch() == rhs.MicroSecondsSinceEpoch();
}

/// Gets time difference of two timestamps, result in microSeconds
inline int64_t timeDifference(Timestamp high, Timestamp low)
{
    return (high.MicroSecondsSinceEpoch() - low.MicroSecondsSinceEpoch());
}

/// @return timestamp + microSeconds
inline Timestamp addTime(Timestamp timestamp, int64_t microSeconds)
{
    return Timestamp(timestamp.MicroSecondsSinceEpoch() + microSeconds);
}

} // namespace claire

#endif // _CLAIRE_COMMON_TIME_TIMESTAMP_H_
