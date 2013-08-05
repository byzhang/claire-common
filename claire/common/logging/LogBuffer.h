// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_LOGGING_LOGBUFFER_H_
#define _CLAIRE_COMMON_LOGGING_LOGBUFFER_H_

#include <stdlib.h>
#include <string.h>

#include <string>

#include <boost/noncopyable.hpp>

namespace claire {

const int kSmallBuffer = 4096;
const int kLargeBuffer = 4096*1000;

template<int SIZE>
class LogBuffer : boost::noncopyable
{
public:
    LogBuffer()
        : cur_(data_)
    {
        set_cookie(CookieStart);
    }

    ~LogBuffer()
    {
        set_cookie(CookieEnd);
    }

    void Append(const char* msg, size_t len)
    {
        // FIXME: use stack vector!!!
        if (static_cast<size_t>(avail()) > len)
        {
            memcpy(cur_, msg, len);
            cur_ += len;
        }
        else
        {
            if (avail() > 0)
            {
                memcpy(cur_, msg, avail());
                cur_ += avail();
            }
        }
    }

    const char* data() const
    {
        return data_;
    }

    size_t length() const
    {
        return cur_ - data_;
    }

    // write to data_ directly
    char* current()
    {
        return cur_;
    }

    size_t avail() const
    {
        return end() - cur_;
    }

    void Add(size_t len)
    {
        cur_ += len;
    }

    void Reset()
    {
        cur_ = data_;
    }

    void bzero()
    {
        ::bzero(data_, sizeof data_);
    }

    // for used by GDB
    const char* DebugString();

    void set_cookie(void (*cookie)())
    {
        cookie_ = cookie;
    }

    // for used by unit test
    std::string ToString() const
    {
        return std::string(data_, length());
    }

private:
    const char* end() const
    {
        return data_ + sizeof data_;
    }

    // Must be outline function for cookies.
    static void CookieStart();
    static void CookieEnd();

    void (*cookie_)();

    char data_[SIZE];
    char* cur_;
};

} // namespace claire

#endif // _CLAIRE_COMMON_LOGGING_LOGBUFFER_H_
