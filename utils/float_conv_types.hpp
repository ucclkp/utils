// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_FLOAT_CONV_TYPES_H_
#define UTILS_FLOAT_CONV_TYPES_H_

#include "utils/conv_types.hpp"


namespace utl {

    enum FFormat : unsigned int {
        FF_NOR  = 1u << 0u,
        FF_SCI  = 1u << 1u,
        FF_HEX  = 1u << 2u,
        FF_HEX2 = 1u << 3u,
        FF_UPP  = 1u << 4u,
        FF_DIG  = 1u << 5u,
        FF_EXA  = 1u << 6u,
        FF_NTZ  = 1u << 7u,
        FF_ENZ  = 1u << 8u,
    };

    enum FRound {
        FR_NEAR = 0,
        FR_CEIL,
        FR_FLOOR,
        FR_ZERO,
    };

}

#endif  // UTILS_FLOAT_CONV_TYPES_H_