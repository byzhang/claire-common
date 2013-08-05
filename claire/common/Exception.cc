// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <claire/common/Exception.h>

#include <stdlib.h>
#include <execinfo.h>

#undef CLAIRE_DEMANGLE
#if defined(__GNUG__) && __GNUG__ >= 4
#include <cxxabi.h>
#include <string.h>
#define CLAIRE_DEMANGLE 1
#endif

namespace claire {
namespace detail {

#ifdef CLAIRE_DEMANGLE

std::string demangle(const char* name)
{
    int status;
    size_t len = 0;

    // malloc() memory for the demangled type name
    char* demangled = abi::__cxa_demangle(name, NULL, &len, &status);
    if (status != 0)
    {
        return name;
    }
    // len is the length of the buffer (including NUL terminator and maybe
    // other junk)

    std::string result(demangled, strlen(demangled));
    free(demangled);

    return result;
}

#else

std::string demangle(const char* name)
{
    return name;
}

#endif // CLAIRE_DEMANGLE

#undef CLAIRE_DEMANGLE

} // namespace detail
} // namespace claire

using namespace claire;

void Exception::FillStackTrace()
{
    void* buffer[200];
    int nptrs = ::backtrace(buffer, sizeof(buffer)/sizeof(buffer[0]));
    char** strings = ::backtrace_symbols(buffer, nptrs);
    if (strings)
    {
        for (int i = 2; i < nptrs; ++i)
        {
            stack_.append(detail::demangle(strings[i]));
            stack_.push_back('\n');
        }
        free(strings);
    }
}

