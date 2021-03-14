// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/command_line.h"

#include <Windows.h>
#include <shellapi.h>


namespace utl {

    // static
    void CommandLine::initialize() {
        int argc;
        LPWSTR* argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
        if (!argv) {
            return;
        }
        initialize(argc, reinterpret_cast<char16_t**>(argv));

        ::LocalFree(argv);
    }

}