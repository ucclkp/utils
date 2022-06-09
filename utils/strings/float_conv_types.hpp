// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STRINGS_FLOAT_CONV_TYPES_H_
#define UTILS_STRINGS_FLOAT_CONV_TYPES_H_

#include "utils/strings/conv_ret_code.hpp"

/**
 * 测试用。
 * 用于模拟非规格化浮点数禁用的情况。
 */
#define FORCE_NO_DENORM  false


namespace utl {

    enum UFFormat : unsigned int {
        UFF_FIX  = 1u << 0u,
        UFF_SCI  = 1u << 1u,
        UFF_HEX  = 1u << 2u,
        UFF_HEX2 = 1u << 3u,
        UFF_UPP  = 1u << 4u,
        UFF_DIG  = 1u << 5u,
        UFF_EXA  = 1u << 6u,
        UFF_NTZ  = 1u << 7u,
        UFF_ENZ  = 1u << 8u,
    };

    enum UFRound {
        UFR_NEAR = 0,
        UFR_CEIL,
        UFR_FLOOR,
        UFR_ZERO,
    };

}

#endif  // UTILS_STRINGS_FLOAT_CONV_TYPES_H_
