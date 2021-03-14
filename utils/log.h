// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_LOG_H_
#define UTILS_LOG_H_

#include <filesystem>
#include <string>
#include <sstream>


#define TO_WIDE_STRING(x) L##x
#define _TO_WIDE_STRING(x) TO_WIDE_STRING(x)
#define __WFILE__ _TO_WIDE_STRING(__FILE__)


#define LOG_STREAM(level) \
    ::utl::Log(__WFILE__, __LINE__, level).stream()

#define VOIDABLE_STREAM(stream, condition) \
    !(condition) ? (void) 0 : ::utl::LogVoidify() & (stream)

#define IS_LOG_ON   true
#define IS_CHECK_ON true

#ifndef NDEBUG

#define IS_DLOG_ON   true
#define IS_DCHECK_ON true

#else

#define IS_DLOG_ON   false
#define IS_DCHECK_ON false

#endif

#define LOG(level) \
    VOIDABLE_STREAM(LOG_STREAM(level), IS_LOG_ON)

#define CHECK(condition) \
    VOIDABLE_STREAM(LOG_STREAM(::utl::Log::FATAL), IS_CHECK_ON && !(condition))

#define DLOG(level) \
    VOIDABLE_STREAM(LOG_STREAM(level), IS_DLOG_ON)

#define DCHECK(condition) \
    VOIDABLE_STREAM(LOG_STREAM(::utl::Log::FATAL), IS_DCHECK_ON && !(condition))


namespace utl {

    class Log {
    public:
        enum Severity {
            INFO,
            WARNING,
            ERR,
            FATAL
        };

        enum OutputTarget {
            CONSOLE =  1 << 0,
            FILE =     1 << 1,
            DEBUGGER = 1 << 2,
            STANDARD = 1 << 3,
        };

        struct Params {
            unsigned int target;
            bool short_file_name;
            std::u16string file_name;
            std::filesystem::path log_path;
        };

        static void debugBreak();
        static void debugBreakIfInDebugger();

        static bool openConsole();
        static bool closeConsole();
        static bool writeConsole(const std::string& msg);
        static void modifyTarget(unsigned int* target);
        static void getDefaultLogPath(std::filesystem::path* path);

        Log(const wchar_t* file_name, int line_number, Severity level);
        ~Log();

        std::ostringstream& stream();

    private:
        static void outputDebugString(const std::string& msg);

        Severity level_;
        int line_number_;
        std::u16string file_name_;
        std::ostringstream stream_;
    };

    class LogVoidify {
    public:
        LogVoidify() = default;
        void operator&(std::ostream& s) {}
    };


    void InitLogging(const Log::Params& params);
    void UninitLogging();

}

using Log = utl::Log;


#endif  // UTILS_LOG_H_
