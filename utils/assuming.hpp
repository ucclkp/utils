// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_ASSUMING_HPP_
#define UTILS_ASSUMING_HPP_

#include <cassert>
#include <numeric>


namespace utl {

    inline void assuming() {
        assert(
            (static_cast<signed char>(-1) &
                static_cast<unsigned char>(~0u)) == static_cast<unsigned char>(~0u));
        assert(CHAR_BIT == 8);
        assert('A' == 65);
        assert(std::numeric_limits<float>::radix == 2);
        assert(std::numeric_limits<float>::is_iec559);
        assert(std::numeric_limits<double>::radix == 2);
        assert(std::numeric_limits<double>::is_iec559);
        assert(std::numeric_limits<long double>::radix == 2);
        assert(std::numeric_limits<long double>::is_iec559);
        assert(sizeof(long double) <= sizeof(uint_fast64_t));
    }

}

#endif  // UTILS_ASSUMING_HPP_