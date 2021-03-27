// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/log.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


#include "utils/convert.h"
#include "utils/files/file_utils.h"
#include "utils/number.hpp"


namespace utl {

    bool is_alloc_console_ = false;
    HANDLE console_output_handle_ = INVALID_HANDLE_VALUE;


    // static
    void Log::debugBreak() {
        __debugbreak();
    }

    // static
    void Log::debugBreakIfInDebugger() {
        if (::IsDebuggerPresent() != 0) {
            __debugbreak();
        }
    }

    // static
    bool Log::openConsole() {
        if (is_alloc_console_) {
            return true;
        }

        if (::AllocConsole() == 0) {
            return false;
        }

        is_alloc_console_ = true;
        console_output_handle_ = ::GetStdHandle(STD_OUTPUT_HANDLE);
        return true;
    }

    // static
    bool Log::closeConsole() {
        if (!is_alloc_console_) {
            return true;
        }

        if (::FreeConsole() == 0) {
            return false;
        }

        is_alloc_console_ = false;
        return true;
    }

    // static
    bool Log::writeConsole(const std::string& msg) {
        if (console_output_handle_ == INVALID_HANDLE_VALUE) {
            return false;
        }

        BOOL ret = ::WriteConsoleW(
            console_output_handle_, msg.data(), num_cast<DWORD>(msg.length()), nullptr, nullptr);
        return ret != 0;
    }

    // static
    void Log::modifyTarget(unsigned int* target) {
    }

    // static
    void Log::getDefaultLogPath(std::filesystem::path* path) {
        *path = getExecFileName(true);
    }

    // static
    void Log::outputDebugString(const std::string& msg) {
        ::OutputDebugStringW(
            reinterpret_cast<LPCWSTR>(UTF8ToUTF16(msg).c_str()));
    }

}
