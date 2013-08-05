// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef _CLAIRE_COMMON_LOGGING_LOGFILE_H_
#define _CLAIRE_COMMON_LOGGING_LOGFILE_H_

#include <time.h>

#include <string>

#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <claire/common/file/FileUtil.h>
#include <claire/common/threading/Mutex.h>

namespace claire {

class LogFile : boost::noncopyable
{
public:
    LogFile(bool thread_safe);
    ~LogFile();

    void Append(const char* msg, size_t len);
    void Flush();

private:
    void AppendUnLocked(const char* msg, size_t len);
    void DropFileMemory();

    std::string GetLogFileName()  const;
    void RollFile();

    const std::string basename_;
    int count_;

    boost::scoped_ptr<Mutex> mutex_;
    boost::scoped_ptr<FileUtil::WritableFile> file_;

    time_t last_period_;
    time_t last_roll_;
    time_t last_flush_;
};

} // namespace claire

#endif // _CLAIRE_COMMON_LOGGING_LOGFILE_H_
