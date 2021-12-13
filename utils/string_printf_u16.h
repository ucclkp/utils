// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STRING_PRINTF_U16_H_
#define UTILS_STRING_PRINTF_U16_H_

#include <string>


namespace utl {

    std::u16string stringPrintf_u16(const char16_t* format, ...);

}

#endif  // UTILS_STRING_PRINTF_U16_H_