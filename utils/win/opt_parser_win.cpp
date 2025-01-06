// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/opt_parser.hpp"

#define NOMINMAX
#include <Windows.h>


char* __wtou8(const wchar_t* src) {
    int ret = ::WideCharToMultiByte(
        CP_UTF8,
        0,
        src, (int)std::wcslen(src),
        nullptr, 0,
        nullptr, nullptr);
    if (ret <= 0) {
        return {};
    }

    char* buf = new char[ret + 1];
    ret = ::WideCharToMultiByte(
        CP_UTF8,
        0,
        src, (int)std::wcslen(src),
        buf, ret,
        nullptr, nullptr);
    if (ret <= 0) {
        delete[] buf;
        return {};
    }

    buf[ret] = '\0';
    return buf;
}

namespace utl {

    int opt_parser::parse_win() {
        int argc;
        LPWSTR* argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
        if (!argv) {
            return rc_par_arg;
        }

        int ret = 0;

        if (argc > 0) {
            std::vector<char*> ss(argc);
            for (int i = 0; i < argc; ++i) {
                wchar_t* arg = argv[i];
                if (arg) {
                    ss[i] = __wtou8(arg);
                } else {
                    ss[i] = new char[1];
                    ss[i][0] = '\0';
                }
            }

            ret = parse(argc, ss.data());
            for (auto c : ss) {
                delete[] c;
            }
        }

        ::LocalFree(argv);
        return ret;
    }

}