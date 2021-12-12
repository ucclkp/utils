// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_LOG_H_
#define UTILS_LOG_H_

#include <cassert>
#include <filesystem>
#include <string>
#include <sstream>
#include <signal.h>

#include "utils/define_utils.hpp"

#ifdef _MSC_VER
#define DEBUG_BREAK  __debugbreak()
#else
#define DEBUG_BREAK  (void)raise(SIGTRAP)
#endif

#define NEED_EARLY_BREAK(level)  \
    (level == ::utl::Log::FATAL || level == ::utl::Log::ERR)

#define EARLY_BREAK(level)  \
    (((level == ::utl::Log::FATAL || level == ::utl::Log::ERR) &&  \
        ::utl::Log::isDebuggerPresent()) ? DEBUG_BREAK : (void)0)

#define LOG_STREAM(level) \
    ::utl::Log(__WFILE__, __LINE__, level).stream()

#define EARLY_BREAK_STREAM(level, condition) \
    !(condition) ? (void) 0 : EARLY_BREAK(level), ::utl::LogVoidify() & (LOG_STREAM(level))

#ifndef NDEBUG
#define IS_DEBUG_ON  true
#else
#define IS_DEBUG_ON  false
#endif

#define LOG(level)         EARLY_BREAK_STREAM(level, true)
#define DLOG(level)        EARLY_BREAK_STREAM(level, IS_DEBUG_ON)
#define DBREAK(condition)  EARLY_BREAK_STREAM(::utl::Log::ERR, IS_DEBUG_ON && !(condition))

#define THROW  \
    assert(false); LOG_STREAM(::utl::Log::FATAL)
#define THROW_IF(condition)  \
    !(condition) ? (void) 0 : assert(false), ::utl::LogVoidify() & (LOG_STREAM(::utl::Log::FATAL))


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

        enum VTFormat {
            VT_DEFAULT,
            VT_UNDERLINE,
            VT_NO_UNDERLINE,
            VT_NEGATIVE,
            VT_POSITIVE,
            VT_FG_BLACK,
            VT_FG_WHITE,
            VT_FG_RED,
            VT_FG_GREEN,
            VT_FG_YELLOW,
            VT_FG_BLUE,
            VT_FG_DEFAULT,
            VT_BG_BLACK,
            VT_BG_WHITE,
            VT_BG_RED,
            VT_BG_GREEN,
            VT_BG_YELLOW,
            VT_BG_BLUE,
            VT_BG_DEFAULT,
        };

        struct Params {
            unsigned int target = STANDARD;
            bool short_file_name = true;
            bool enable_vt = false;
            std::u16string file_name;
            std::filesystem::path log_path;
        };

        static void debugBreak();
        static bool isDebuggerPresent();

        static bool openConsole();
        static bool closeConsole();
        static bool writeConsole(const std::string& msg);
        static void modifyTarget(unsigned int* target);
        static void setVTEnabled(bool enabled);
        static void getDefaultLogPath(std::filesystem::path* path);

        Log(const wchar_t* file_name, int line_number, Severity level);
        ~Log();

        std::ostringstream& stream();

        static void logMessage(Severity level, const std::string& msg);
        static const char* vt_format(VTFormat f);

    private:
        static void outputDebugString(const std::string& msg);

        static bool is_vt_enabled_;

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
