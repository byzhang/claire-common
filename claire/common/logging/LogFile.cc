// Copyright (c) 2013 The Claire Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include <claire/common/logging/LogFile.h>

#include <assert.h>
#include <string.h>
#include <unistd.h>

#include <gflags/gflags.h>

#include <claire/common/file/FileUtil.h>
#include <claire/common/time/Timestamp.h>
#include <claire/common/threading/Mutex.h>
#include <claire/common/system/ProcessInfo.h>

DEFINE_bool(drop_log_memory, true, "Drop in-memory buffers of log contents. "
            "Logs can grow very quickly and they are rarely read before they "
            "need to be evicted from memory. Instead, drop them from memory "
            "as soon as they are flushed to disk.");

using namespace claire;

const static int kCheckTimeRoll = 1024;
const static int kSecondsPerDay = 60*60*24;

LogFile::LogFile(bool thread_safe)
    : basename_(::google::ProgramInvocationShortName()),
      count_(0),
      mutex_(thread_safe ? new Mutex : NULL),
      last_period_(0),
      last_roll_(0),
      last_flush_(0)
{
    assert(basename_.find('/') == std::string::npos);
    RollFile();
}

LogFile::~LogFile()
{ }

void LogFile::Append(const char* msg, size_t len)
{
    if(mutex_)
    {
        MutexLock lock(*mutex_);
        AppendUnLocked(msg, len);
    }
    else
    {
        AppendUnLocked(msg, len);
    }
}

void LogFile::Flush()
{
    if (mutex_)
    {
        MutexLock lock(*mutex_);
        file_->Flush();
        DropFileMemory();

    }
    else
    {
        file_->Flush();
        DropFileMemory();
    }
}

void LogFile::AppendUnLocked(const char* msg, size_t len)
{
    file_->Append(msg, len);

    if (file_->WrittenBytes() > static_cast<uint32_t>(FLAGS_max_log_size)*1024*1024)
    {
        RollFile();
    }
    else
    {
        if (count_ > kCheckTimeRoll)
        {
            count_ = 0;
            auto now = Timestamp::Now().SecondsSinceEpoch();
            auto this_period = now / kSecondsPerDay * kSecondsPerDay;
            if (this_period != last_period_)
            {
                RollFile();
            }
            else if (now - last_flush_ > FLAGS_logbufsecs)
            {
                last_flush_ = now;
                file_->Flush();
            }
        }
        else
        {
            ++count_;
        }
    }
}

void LogFile::RollFile()
{
    auto now = Timestamp::Now().SecondsSinceEpoch();
    auto filename = GetLogFileName();
    auto start = now / kSecondsPerDay * kSecondsPerDay;

    if (now > last_roll_)
    {
        last_roll_ = now;
        last_flush_ = now;
        last_period_ = start;

        file_.reset(new FileUtil::AppendableFile(filename));

        std::string linkpath;
        auto slash = ::strrchr(filename.c_str(), '/');
        if (slash)
        {
            linkpath = std::string(filename, slash-filename.c_str()+1);
        }
        linkpath += basename_ + ".log";

        if (FileUtil::FileExists(linkpath))
        {
            std::string name;
            uint64_t len;
            if ((FileUtil::ReadLink(linkpath, &name) == FileUtil::kOk)
                && (FileUtil::GetFileSize(name, &len) == FileUtil::kOk)
                && (len == 0))
            {
                FileUtil::DeleteFile(name);
            }

            FileUtil::DeleteFile(linkpath);
        }

        auto linkdest = slash ? (slash+1) : filename;
        FileUtil::SymLink(linkdest, linkpath);
    }
}

void LogFile::DropFileMemory()
{
    auto length = file_->WrittenBytes();
    if (FLAGS_drop_log_memory && static_cast<int>(length) >= ::getpagesize())
    {
        // don't evict the most recent page
        length &= ~(::getpagesize() - 1);
        ::posix_fadvise(file_->fd(), 0, length, POSIX_FADV_DONTNEED);
    }
}

std::string LogFile::GetLogFileName() const
{
    auto filename = FLAGS_log_dir;

    if (!filename.empty() && *filename.rbegin() != '/')
    {
        filename += "/";
    }

    filename += basename_;
    filename += ".";
    filename += Timestamp::Now().ToFormattedString();
    filename += ".";
    filename += ProcessInfo::hostname();
    filename += ".";
    filename += ProcessInfo::GetPidString();
    filename += ".";
    filename += ProcessInfo::username();
    filename += ".log";

    return filename;
}
