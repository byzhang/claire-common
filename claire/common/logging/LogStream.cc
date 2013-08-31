// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <claire/common/logging/LogStream.h>

#include <limits>

const static size_t kMaxNumericSize = 32;

static_assert(static_cast<int>(kMaxNumericSize - 10) > std::numeric_limits<double>::digits10, 
              "double limits over kMaxNumericSize");
static_assert(static_cast<int>(kMaxNumericSize - 10) > std::numeric_limits<long double>::digits10,
              "long limits over kMaxNumericSize");
static_assert(static_cast<int>(kMaxNumericSize - 10) > std::numeric_limits<long>::digits10,
              "long limits over kMaxNumericSize");
static_assert(static_cast<int>(kMaxNumericSize - 10) > std::numeric_limits<long long>::digits10,
              "long long limits over kMaxNumericSize");

using namespace claire;

template<typename T>
void LogStream::FormatInteger(T v)
{
    if (buffer_.avail() >= kMaxNumericSize)
    {
        auto len = IntToBuffer(buffer_.current(), v);
        buffer_.Add(len);
    }
}

LogStream& LogStream::operator<<(short v)
{
    *this<<static_cast<int>(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
    *this<<static_cast<unsigned int>(v);
    return *this;
}

LogStream& LogStream::operator<<(int v)
{
    FormatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
    FormatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long v)
{
    FormatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
    FormatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long long v)
{
    FormatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
    FormatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(double v)
{
    if (buffer_.avail() >= kMaxNumericSize)
    {
        char buf[32];
        snprintf(buf, sizeof buf, "%.12g", v);
        Append(buf, strlen(buf));
    }
    return *this;
}

LogStream& LogStream::operator<<(const void* p)
{
    auto v = reinterpret_cast<uintptr_t>(p);
    if (buffer_.avail() >= kMaxNumericSize)
    {
        auto buf = buffer_.current();
        buf[0] = '0';
        buf[1] = 'x';
        auto len = HexToBuffer(buf+2, v);
        buffer_.Add(len+2);
    }

    return *this;
}
