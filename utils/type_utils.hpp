// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_TYPE_UTILS_HPP_
#define UTILS_TYPE_UTILS_HPP_

#include <type_traits>


namespace utl {

    // 可用于防止编译器直接触发 static_assert
    template <typename Ty>
    class sat_stub : public std::false_type {};

}

#endif  // UTILS_TYPE_UTILS_HPP_