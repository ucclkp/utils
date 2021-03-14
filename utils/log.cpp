// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/log.h"

#include <fstream>
#include <iostream>

#include "utils/convert.h"
#include "utils/files/file_utils.h"


namespace utl {

    namespace fs = std::filesystem;

    Log::Params log_params_;
    std::ofstream log_file_stream_;

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
            DCHECK(!log_file_stream_.fail());
        }
        if (log_params_.target & Log::OutputTarget::CONSOLE) {
            Log::openConsole();
        }
    }

    void UninitLogging() {
        if (log_params_.target & Log::OutputTarget::CONSOLE) {
            Log::closeConsole();
        }
        if (log_params_.target & Log::OutputTarget::FILE) {
            log_file_stream_.close();
        }
    }

    Log::Log(const wchar_t* file_name, int line_number, Severity level)
        : level_(level),
          line_number_(line_number),
          file_name_(WideToUTF16(file_name)) {
    }

    Log::~Log() {
        if (log_params_.short_file_name) {
            file_name_ = fs::path(file_name_).filename().u16string();
        }

        std::string msg;
        msg.append(UTF16ToUTF8(file_name_))
            .append(u8"(")
            .append(std::to_string(line_number_))
            .append(u8"): ")
            .append(stream_.str())
            .append(u8"\n");

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

        switch (level_) {
        case INFO:
            break;
        case WARNING:
            break;
        case ERR:
            debugBreakIfInDebugger();
            break;
        case FATAL:
            debugBreak();
            break;
        }
    }

    std::ostringstream& Log::stream() {
        return stream_;
    }

}
