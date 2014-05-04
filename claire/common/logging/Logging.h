// Copyright (c) 2013 The claire-common Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

// Copyright (c) 1999, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// This file contains #include information about logging-related stuff.
// Pretty much everybody needs to #include this file so that they can
// log various happenings.

#ifndef _CLAIRE_COMMON_LOGGING_LOGGING_H
#define _CLAIRE_COMMON_LOGGING_LOGGING_H

#include <glog/logging.h>

#define COMPACT_GOOGLE_LOG_TRACE COMPACT_GOOGLE_LOG_INFO
#define LOG_TO_STRING_TRACE LOG_TO_STRING_INFO
#define COMPACT_GOOGLE_LOG_DEBUG COMPACT_GOOGLE_LOG_INFO
#define LOG_TO_STRING_DEBUG LOG_TO_STRING_INFO

// Plus some debug-logging macros that get compiled to nothing for production

#ifndef NDEBUG

// debug-only checking.  not executed in NDEBUG mode.
#define DCHECK_ERR(invocation) CHECK_ERR(invocation)

#else  // NDEBUG

#define DCHECK_ERR(invocation) \
    auto ret = (invocation); \
    while (false) CHECK_ERR(ret)

#endif  // NDEBUG

#define NOTREACHED() DCHECK(false)

#define HEXDUMP(severity) \
    LOG(severity) << claire::LogFormat::kHex

namespace claire {
const char * strerror_tl(int saved_errno);
}  // namespace claire
#endif // _CLAIRE_COMMON_LOGGING_LOGGING_H
