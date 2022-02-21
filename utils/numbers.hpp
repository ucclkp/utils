// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_NUMBERS_HPP_
#define UTILS_NUMBERS_HPP_

#include <assert.h>
#include <cmath>
#include <limits>
#include <type_traits>


namespace utl {

    template <typename R, typename Ty>
    R num_cast(Ty val) {
        static_assert(
            std::is_arithmetic<R>::value &&
            std::is_arithmetic<Ty>::value, "Ty and R must be number types!");

        if constexpr (
            std::numeric_limits<R>::is_integer &&
            std::numeric_limits<Ty>::is_integer)
        {
            if constexpr (!std::numeric_limits<R>::is_signed) {
                // int/uint -> uint
                assert(
                    val >= 0 &&
                    (std::numeric_limits<R>::max)() >= typename std::make_unsigned<Ty>::type(val));
            } else {
                if constexpr (std::numeric_limits<Ty>::is_signed) {
                    // int -> int
                    assert(
                        (std::numeric_limits<R>::max)() >= val &&
                        (std::numeric_limits<R>::min)() <= val);
                } else {
                    // uint -> int
                    assert(typename std::make_unsigned<R>::type((std::numeric_limits<R>::max)()) >= val);
                }
            }
        } else if constexpr (
            std::numeric_limits<R>::is_integer &&
            !std::numeric_limits<Ty>::is_integer)
        {
            // float -> int/uint
            assert(
                (std::numeric_limits<R>::max)() >= val &&
                (std::numeric_limits<R>::min)() <= val);

            assert(std::abs(val - R(val)) <= 1);
        } else if constexpr (
            !std::numeric_limits<R>::is_integer &&
            std::numeric_limits<Ty>::is_integer)
        {
            // int/uint -> float
            assert(
                (std::numeric_limits<R>::max)() >= val &&
                (std::numeric_limits<R>::min)() <= val);

            assert(
                val >= Ty(R(val)) ? val - Ty(R(val)) <= 1 : Ty(R(val)) - val <= 1);
        } else {
            assert(
                (std::numeric_limits<R>::max)() >= val &&
                (std::numeric_limits<R>::min)() <= val);
        }

        return R(val);
    }

    template <typename R, typename Ty>
    bool can_num_cast(Ty val) {
        static_assert(
            std::is_arithmetic<R>::value &&
            std::is_arithmetic<Ty>::value, "Ty and R must be number types!");

        if constexpr (
            std::numeric_limits<R>::is_integer &&
            std::numeric_limits<Ty>::is_integer)
        {
            if constexpr (!std::numeric_limits<R>::is_signed) {
                // int/uint -> uint
                return (
                    val >= 0 &&
                    (std::numeric_limits<R>::max)() >= std::make_unsigned<Ty>::type(val));
            } else {
                if constexpr (std::numeric_limits<Ty>::is_signed) {
                    // int -> int
                    return (
                        (std::numeric_limits<R>::max)() >= val &&
                        (std::numeric_limits<R>::min)() <= val);
                } else {
                    // uint -> int
                    return (std::make_unsigned<R>::type((std::numeric_limits<R>::max)()) >= val);
                }
            }
        } else if constexpr (
            std::numeric_limits<R>::is_integer &&
            !std::numeric_limits<Ty>::is_integer)
        {
            // float -> int/uint
            return (
                (std::numeric_limits<R>::max)() >= val &&
                (std::numeric_limits<R>::min)() <= val &&
                std::abs(val - R(val)) <= 1);
        } else if constexpr (
            !std::numeric_limits<R>::is_integer &&
            std::numeric_limits<Ty>::is_integer)
        {
            // int/uint -> float
            return (
                (std::numeric_limits<R>::max)() >= val &&
                (std::numeric_limits<R>::min)() <= val &&
                (val >= Ty(R(val)) ? val - Ty(R(val)) <= 1 : Ty(R(val)) - val <= 1));
        } else {
            return (
                (std::numeric_limits<R>::max)() >= val &&
                (std::numeric_limits<R>::min)() <= val);
        }
    }

    template <typename Ty>
    bool is_num_equal(Ty v1, Ty v2) {
        static_assert(std::is_arithmetic<Ty>::value, "Ty must be a number type!");
        if constexpr (std::numeric_limits<Ty>::is_integer) {
            return v1 == v2;
        } else {
            return std::fabs(v1 - v2) <= std::numeric_limits<Ty>::epsilon() * std::fabs(v1 + v2)
                || std::fabs(v1 - v2) < (std::numeric_limits<Ty>::min)();
        }
    }

    template <typename Ty>
    bool is_num_zero(Ty val) {
        return is_num_equal(val, Ty(0));
    }

    template <typename Ret, typename Base, typename Exp>
    Ret powui(Base base, Exp exp) {
        static_assert(std::is_unsigned<Ret>::value &&
            std::is_unsigned<Base>::value &&
            std::is_unsigned<Exp>::value);

        Ret result = 1u;
        while (exp) {
            if (exp & 1u) {
                result *= base;
            }
            exp >>= 1u;
            result *= result;
        }
        return result;
    }

}

#endif  // UTILS_NUMBERS_HPP_
