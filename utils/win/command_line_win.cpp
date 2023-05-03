// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/command_line.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
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

        if (argc > 0) {
            char16_t** c_strs = new char16_t*[argc];
            for (int i = 0; i < argc; ++i) {
                auto w_str = argv[i];
                if (w_str) {
                    auto len = std::wcslen(w_str);
                    char16_t* c_str = new char16_t[len + 1];
                    for (size_t n = 0; n < len; ++n) {
                        c_str[n] = w_str[n];
                    }
                    c_str[len] = u'\0';
                    c_strs[i] = c_str;
                } else {
                    c_strs[i] = nullptr;
                }
            }

            initialize(argc, c_strs);

            for (int i = 0; i < argc; ++i) {
                delete[] c_strs[i];
            }
            delete[] c_strs;
        }

        ::LocalFree(argv);
    }

}