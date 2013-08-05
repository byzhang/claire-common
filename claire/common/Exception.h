// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_EXCEPTION_H_
#define _CLAIRE_COMMON_EXCEPTION_H_

#include <string>
#include <exception>

namespace claire {

class Exception : public std::exception
{
public:
    explicit Exception(const char* msg)
        : message_(msg)
    {
        FillStackTrace();
    }

    explicit Exception(const std::string& msg)
        : message_(msg)
    {
        FillStackTrace();
    }

    virtual ~Exception() noexcept
    { }

    virtual const char* what() const noexcept
    {
        return message_.c_str();
    }

    const char* StackTrace() const noexcept
    {
        return stack_.c_str();
    }

private:
    void FillStackTrace();

    std::string message_;
    std::string stack_;
};

} // namespace claire

#endif // _CLAIRE_COMMON_EXCEPTION_H_
