// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_STRING_STRINGUTIL_H_
#define _CLAIRE_COMMON_STRING_STRINGUTIL_H_

#include <stdint.h>
#include <string.h>

namespace claire {

template<typename T>
size_t IntToBuffer(char buf[], T value);
size_t HexToBuffer(char buf[], uintptr_t value);

size_t HexString(char buff[], size_t size,
                 const char *str, size_t len);

} // namespace claire

#endif // _CLAIRE_COMMON_STRING_STRINGUTIL_H_
