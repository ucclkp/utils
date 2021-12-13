// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "int_conv.h"


namespace utl {
namespace internal {

    template <typename Ty>
    int decd_slow(Ty v, int base) {
        int r = 0;
        while (v) {
            v /= base;
            ++r;
        }
        return r;
    }

    template <typename Ty>
    int decd(Ty v, int base) {
        static_assert(
            std::is_unsigned<Ty>::value,
            "Ty must be unsigned type!");

        if (v < 10) return 1;
        if (v < 100) return 2;
        if (v < 1000) return 3;
        if (v < 10000) return 4;
        if (v < 100000) return 5;
        if (v < 1000000) return 6;
        if (v < 10000000) return 7;
        if (v < 100000000) return 8;
        if (v < 1000000000) return 9;
        if (v < 10000000000) return 10;
        if (v < 100000000000) return 11;
        if (v < 1000000000000) return 12;
        if (v < 10000000000000) return 13;
        if (v < 100000000000000) return 14;
        if (v < 1000000000000000) return 15;
        if (v < 10000000000000000) return 16;
        if (v < 100000000000000000) return 17;
        if (v < 1000000000000000000) return 18;
        if (v < 10000000000000000000) return 19;
        return decd_slow(v / 10000000000000000000, base) + 19;
    }

}
}
