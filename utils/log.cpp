// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/log.h"

#include <cassert>
#include <fstream>
#include <iostream>

#include "utils/files/file_utils.h"
#include "utils/string_utils.hpp"


namespace utl {

    namespace fs = std::filesystem;

    Log::Params log_params_;
    std::ofstream log_file_stream_;

    // static
    bool Log::is_vt_enabled_ = false;

    void InitLogging(const Log::Params& params) {
        log_params_ = params;
        if (log_params_.file_name.empty()) {
            log_params_.file_name = u"Debug.log";
        }

        Log::modifyTarget(&log_params_.target);

        if (log_params_.target & Log::OutputTarget::FILE) {
            if (log_params_.log_path.empty()) {
                Log::getDefaultLogPath(&log_params_.log_path);
            }
            log_file_stream_.open(log_params_.log_path / log_params_.file_name, std::ios::out | std::ios::app);
            assert(!log_file_stream_.fail());
        }
        if (log_params_.target & Log::OutputTarget::CONSOLE) {
            Log::openConsole();
        }

        Log::setVTEnabled(params.enable_vt);
    }

    void UninitLogging() {
        if (log_params_.target & Log::OutputTarget::CONSOLE) {
            Log::closeConsole();
        }
        if (log_params_.target & Log::OutputTarget::FILE) {
            log_file_stream_.close();
        }
    }

    Log::Log(
        const wchar_t* file_name, int line_number, Severity level)
        : level_(level),
          line_number_(line_number),
          file_name_(WideToUTF16(file_name)),
          is_str_(false),
          need_new_line_(true) {}

    Log::Log(
        const wchar_t* file_name, int line_number, Severity level,
        const std::string& msg, bool new_line)
        : level_(level),
          line_number_(line_number),
          file_name_(WideToUTF16(file_name)),
          is_str_(true),
          need_new_line_(new_line),
          str_(msg) {}

    Log::~Log() {
        if (log_params_.short_file_name) {
            file_name_ = fs::path(file_name_).filename().u16string();
        }

        std::string msg;
        msg.append(UTF16ToUTF8(file_name_))
            .append(u8p("("))
            .append(std::to_string(line_number_))
            .append(u8p("): "))
            .append(is_str_ ? str_ : stream_.str());
        if (need_new_line_) {
            msg.append(u8p("\n"));
        }
        logMessage(level_, msg);
    }

    std::ostringstream& Log::stream() {
        return stream_;
    }

    // static
    void Log::logMessage(Severity level, const std::string& msg) {
        if (log_params_.target & DEBUGGER) {
            outputDebugString(msg);
        }
        if (log_params_.target & FILE) {
            if (log_file_stream_.is_open()) {
                log_file_stream_ << msg << std::flush;
            }
        }
        if (log_params_.target & CONSOLE) {
            writeConsole(msg);
        }
        if (log_params_.target & STANDARD) {
            std::cout << msg;
        }

        switch (level) {
        case INFO:
            break;
        case WARNING:
            break;
        case ERR:
            break;
        case FATAL:
            throw std::runtime_error(msg);
        }
    }

    // static
    const char* Log::vt_format(VTFormat f) {
        if (!is_vt_enabled_) {
            return "";
        }

        switch (f) {
        case VT_DEFAULT:      return "\x1b[m";
        case VT_UNDERLINE:    return "\x1b[4m";
        case VT_NO_UNDERLINE: return "\x1b[24m";
        case VT_NEGATIVE:     return "\x1b[7m";
        case VT_POSITIVE:     return "\x1b[27m";
        case VT_FG_BLACK:     return "\x1b[30m";
        case VT_FG_WHITE:     return "\x1b[37m";
        case VT_FG_RED:       return "\x1b[31m";
        case VT_FG_GREEN:     return "\x1b[32m";
        case VT_FG_YELLOW:    return "\x1b[33m";
        case VT_FG_BLUE:      return "\x1b[34m";
        case VT_FG_DEFAULT:   return "\x1b[39m";
        case VT_BG_BLACK:     return "\x1b[40m";
        case VT_BG_WHITE:     return "\x1b[47m";
        case VT_BG_RED:       return "\x1b[41m";
        case VT_BG_GREEN:     return "\x1b[42m";
        case VT_BG_YELLOW:    return "\x1b[43m";
        case VT_BG_BLUE:      return "\x1b[44m";
        case VT_BG_DEFAULT:   return "\x1b[49m";

        default:
            return "";
        }

    }

}
