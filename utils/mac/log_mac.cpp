// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include <iostream>

#include "utils/log.h"

#include <signal.h>
#include <sys/sysctl.h>
#include <unistd.h>


namespace {

    bool IsDebuggerPresent() {
        int data[4];
        data[0] = CTL_KERN;
        data[1] = KERN_PROC;
        data[2] = KERN_PROC_PID;
        data[3] = getpid();

        kinfo_proc info;
        info.kp_proc.p_flag = 0;

        size_t size = sizeof(info);

        sysctl(data, sizeof(data) / sizeof(*data), &info, &size, nullptr, 0);

        return (info.kp_proc.p_flag & P_TRACED) != 0;
    }

}

namespace utl {

    bool is_alloc_console_ = false;


    // static
    void Log::debugBreak() {
        raise(SIGTRAP);
    }

    // static
    bool Log::isDebuggerPresent() {
        return IsDebuggerPresent();
    }

    // static
    bool Log::openConsole() {
        if (is_alloc_console_) {
            return true;
        }

        is_alloc_console_ = true;
        return true;
    }

    // static
    bool Log::closeConsole() {
        if (!is_alloc_console_) {
            return true;
        }

        is_alloc_console_ = false;
        return true;
    }

    // static
    bool Log::writeConsole(const std::string_view& msg) {
        return false;
    }

    // static
    void Log::modifyTarget(unsigned int* target) {
        *target &= ~FILE;
        *target &= ~CONSOLE;

        if ((*target & DEBUGGER) && (*target & STANDARD)) {
            *target &= ~DEBUGGER;
        }
    }

    // static
    void Log::setVTEnabled(bool enabled) {
    }

    // static
    void Log::getDefaultLogPath(std::filesystem::path* path) {
        //log_params.log_path = utl::getExecFileName(true)
        //    .parent_path().parent_path().parent_path();
    }

    // static
    void Log::outputDebugString(const std::string_view& msg) {
        std::cout << msg;
    }

}
