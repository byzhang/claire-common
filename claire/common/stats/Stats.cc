// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <claire/common/stats/Stats.h>
#include <claire/common/threading/Singleton.h>

using namespace claire;

Stats* Stats::default_instance()
{
    return &(Singleton<Stats>::instance());
}
