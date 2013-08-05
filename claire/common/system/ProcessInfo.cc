// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <claire/common/system/ProcessInfo.h>

#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <pwd.h>
#include <stdio.h> // snprintf
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

using namespace claire;

pid_t ProcessInfo::pid()
{
    return ::getpid();
}

std::string ProcessInfo::GetPidString()
{
    char buf[32];
    snprintf(buf, sizeof buf, "%d", pid());
    return buf;
}

std::string ProcessInfo::username()
{
    struct passwd pwd;
    struct passwd* result = NULL;
    char buf[8192];
    const char* name = "unknownuser";

    ::getpwuid_r(::getuid(), &pwd, buf, sizeof buf, &result);
    if (result)
    {
        name = pwd.pw_name;
    }
    return name;
}

std::string ProcessInfo::hostname()
{
    char buf[64] = "unknownhost";
    buf[sizeof(buf)-1] = '\0';
    ::gethostname(buf, sizeof buf);
    return buf;
}

