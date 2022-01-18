// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_USPRINTF_H_
#define UTILS_USPRINTF_H_

#include <string>


namespace utl {

    int usprintf(char* buf, size_t* len, const char* format, ...);
    std::string usprintf(const char* format, ...);

    int usprintf(char16_t* buf, size_t* len, const char16_t* format, ...);
    std::u16string usprintf(const char16_t* format, ...);

    int usprintf(char32_t* buf, size_t* len, const char32_t* format, ...);
    std::u32string usprintf(const char32_t* format, ...);

}

#endif  // UTILS_USPRINTF_H_