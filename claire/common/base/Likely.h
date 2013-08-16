// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_BASE_LIKELY_H_
#define _CLAIRE_COMMON_BASE_LIKELY_H_

#undef LIKELY
#undef UNLIKELY

#if __GNUC_MINOR__ >= 6
#pragma GCC diagnostic push
#else
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

#define LIKELY(x)   (__builtin_expect(!!(x), 0))
#define UNLIKELY(x) (__builtin_expect((x), 0))

#if __GNUC_MINOR__ >= 6
#pragma GCC diagnostic pop
#else
#pragma GCC diagnostic error "-Wconversion"
#pragma GCC diagnostic error "-Wold-style-cast"
#endif

#endif // _CLAIRE_COMMON_BASE_LIKELY_H_
