// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STRINGS_STRING_UTILS_TYPES_HPP_
#define UTILS_STRINGS_STRING_UTILS_TYPES_HPP_

#define u8c(c) reinterpret_cast<char>(u8##c)
#define u8p(s) reinterpret_cast<const char*>(u8##s)
#define u8r(r) reinterpret_cast<const char&>(u8##r)


namespace utl {

    enum TrimFlags {
        TRF_NONE   = 0,
        TRF_START  = 1,
        TRF_MIDDLE = 2,
        TRF_END    = 4,
        TRF_SE     = TRF_START | TRF_END,
        TRF_ALL    = TRF_START | TRF_MIDDLE | TRF_END,
    };

}


#endif  // UTILS_STRINGS_STRING_UTILS_TYPES_HPP_