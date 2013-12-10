// Copyright (c) 2013 The claire-common Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

//
// Copyright 2013 Facebook, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#define __STDC_LIMIT_MACROS
#include <claire/common/time/TimeoutQueue.h>

#include <vector>
#include <algorithm>

#include <boost/bind.hpp>

#include <claire/common/logging/Logging.h>

namespace claire {

TimeoutQueue::Id TimeoutQueue::Add(int64_t expiration, const Callback& callback)
{
    auto id = next_++;
    timeouts_.insert({id, expiration, -1, callback});
    return id;
}

TimeoutQueue::Id TimeoutQueue::Add(int64_t expiration, Callback&& callback)
{
    auto id = next_++;
    timeouts_.insert({id, expiration, -1, std::move(callback)});
    return id;
}

TimeoutQueue::Id TimeoutQueue::AddRepeating(int64_t now, int64_t interval, const Callback& callback)
{
    auto id = next_++;
    timeouts_.insert({id, now + interval, interval, callback});
    return id;
}

TimeoutQueue::Id TimeoutQueue::AddRepeating(int64_t now, int64_t interval, Callback&& callback)
{
    auto id = next_++;
    timeouts_.insert({id, now + interval, interval, std::move(callback)});
    return id;
}

bool TimeoutQueue::Cancel(Id id)
{
    return timeouts_.get<kById>().erase(id);
}

int64_t TimeoutQueue::NextExpiration() const
{
    return (timeouts_.empty() ? std::numeric_limits<int64_t>::max() :
        timeouts_.get<kByExpiration>().begin()->expiration);
}

int64_t TimeoutQueue::Run(int64_t now)
{
    std::vector<Event> expired;
    auto& events = timeouts_.get<kByExpiration>();
    auto end = events.upper_bound(now);
    std::move(events.begin(), end, std::back_inserter(expired));
    events.erase(events.begin(), end);

    for (auto it = expired.begin(); it != expired.end(); ++it)
    {
        auto& event = *it;
        // Reinsert if repeating, do this before executing callbacks
        // so the callbacks have a chance to call erase
        if (event.repeat_interval >= 0)
        {
            timeouts_.insert({event.id,
                              now + event.repeat_interval,
                              event.repeat_interval,
                              event.callback});
        }
    }

    // if delete expire timer in callback, no realy action
    // if delete repeat in callback, it is safe
    for (auto it = expired.begin(); it != expired.end(); ++it)
    {
        (*it).callback();
    }
    return NextExpiration();
}

} // namespace claire
