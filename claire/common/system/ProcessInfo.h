// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_SYSTEM_PROCESSINFO_H_
#define _CLAIRE_COMMON_SYSTEM_PROCESSINFO_H_

#include <unistd.h>

#include <string>

namespace claire {
namespace ProcessInfo {

pid_t pid();
std::string GetPidString();

std::string username();
std::string hostname();

} // namespace ProcessInfo
} // namespace claire

#endif // _CLAIRE_COMMON_SYSTEM_PROCESSINFO_H_
