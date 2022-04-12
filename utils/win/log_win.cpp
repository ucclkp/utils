// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/log.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "utils/files/file_utils.h"
#include "utils/numbers.hpp"
#include "utils/strings/string_utils.hpp"


namespace {

    bool enableVT(HANDLE console, bool enabled) {
        if (enabled) {
            DWORD mode;
            if (::GetConsoleMode(console, &mode) != 0) {
                mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                if (::SetConsoleMode(console, mode) != 0) {
                    return true;
                }
            }
        } else {
            DWORD mode;
            if (::GetConsoleMode(console, &mode) != 0) {
                mode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                if (::SetConsoleMode(console, mode) != 0) {
                    return true;
                }
            }
        }
        return false;
    }

}

namespace utl {

    bool is_alloc_console_ = false;
    HANDLE console_output_handle_ = INVALID_HANDLE_VALUE;


    // static
    void Log::debugBreak() {
        __debugbreak();
    }

    // static
    bool Log::isDebuggerPresent() {
        return ::IsDebuggerPresent() != 0;
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
        if (console_output_handle_) {
            enableVT(console_output_handle_, true);
        }

        return true;
    }

    // static
    bool Log::setUTF8Console() {
        return ::SetConsoleOutputCP(CP_UTF8) != 0;
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
    bool Log::writeConsole(const std::string_view& msg) {
        if (console_output_handle_ == INVALID_HANDLE_VALUE) {
            return false;
        }

        auto u16_msg = UTF8ToUTF16(msg);
        std::wstring w_msg(u16_msg.begin(), u16_msg.end());

        BOOL ret = ::WriteConsoleW(
            console_output_handle_,
            w_msg.data(), num_cast<DWORD>(w_msg.length()),
            nullptr, nullptr);
        return ret != 0;
    }

    // static
    void Log::modifyTarget(unsigned int* target) {
    }

    // static
    void Log::setVTEnabled(bool enabled) {
        if (enabled == is_vt_enabled_) {
            return;
        }

        is_vt_enabled_ = enabled;
        if (console_output_handle_) {
            enableVT(console_output_handle_, enabled);
        }
    }

    // static
    void Log::getDefaultLogPath(std::filesystem::path* path) {
        *path = getExecFileName(true);
    }

    // static
    void Log::outputDebugString(const std::string_view& msg) {
        auto u16_msg = UTF8ToUTF16(msg);
        std::wstring w_msg(u16_msg.begin(), u16_msg.end());
        ::OutputDebugStringW(w_msg.c_str());
    }

}
