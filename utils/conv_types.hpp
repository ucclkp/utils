// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_CONV_TYPES_HPP_
#define UTILS_CONV_TYPES_HPP_


namespace utl {

    enum ConvRetCode {
        UCR_OK = 0,
        UCR_BUFFER,
        UCR_FAILED,
        UCR_OVERFLOWED,
    };

}

#endif  // UTILS_CONV_TYPES_HPP_