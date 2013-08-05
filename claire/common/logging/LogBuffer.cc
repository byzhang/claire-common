// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <claire/common/logging/LogBuffer.h>

using namespace claire;

template<int SIZE>
const char* LogBuffer<SIZE>::DebugString()
{
    *cur_ = '\0';
    return data_;
}

template<int SIZE>
void LogBuffer<SIZE>::CookieStart()
{ }

template<int SIZE>
void LogBuffer<SIZE>::CookieEnd()
{ }

template class LogBuffer<kSmallBuffer>;
template class LogBuffer<kLargeBuffer>;
