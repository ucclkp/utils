// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STRINGS_FLOAT_CONV_TYPES_H_
#define UTILS_STRINGS_FLOAT_CONV_TYPES_H_

#include "utils/strings/sc_ret_code.h"

/**
 * 测试用。
 * 用于模拟非规格化浮点数禁用的情况。
 */
#define FORCE_NO_DENORM  false


namespace utl {

    enum FCFormat {
        FCF_FIX  = 1 << 0,
        FCF_SCI  = 1 << 1,
        FCF_HEX  = 1 << 2,
        FCF_HEX2 = 1 << 3,
        FCF_UPP  = 1 << 4,
        FCF_DIG  = 1 << 5,
        FCF_EXA  = 1 << 6,
        FCF_NTZ  = 1 << 7,
        FCF_ENZ  = 1 << 8,
    };

    enum FCRound {
        FCR_NEAR = 0,
        FCR_CEIL,
        FCR_FLOOR,
        FCR_ZERO,
    };

}

#endif  // UTILS_STRINGS_FLOAT_CONV_TYPES_H_
