// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_ENDIAN_HPP_
#define UTILS_ENDIAN_HPP_

#include <cstdint>

#include "utils/platform_utils.h"
#include "utils/type_utils.hpp"


namespace utl {

    /**
     * swapBytes 的通用版本。
     * 如果给定的类型不是下面几种特例指定的类型的话应直接报错。
     */
    template <typename T>
    T swapBytes(T val) {
        static_assert(sat_stub<T>::value, "unavailable type!");
        return 0;
    }

    template <>
    inline int16_t swapBytes(int16_t val) {
        return (val << 8) | ((val >> 8) & 0xFF);
    }

    template <>
    inline uint16_t swapBytes(uint16_t val) {
        return (val << 8) | (val >> 8);
    }

    template <>
    inline int32_t swapBytes(int32_t val) {
        return (val << 24) | ((val << 8) & 0x00FF0000) |
            ((val >> 24) & 0xFF) | ((val >> 8) & 0x0000FF00);
    }

    template <>
    inline uint32_t swapBytes(uint32_t val) {
        return (val << 24) | ((val << 8) & 0x00FF0000) |
            (val >> 24) | ((val >> 8) & 0x0000FF00);
    }

    template <>
    inline int64_t swapBytes(int64_t val) {
        return (val << 56) | ((val << 40) & 0xFF000000000000) |
            ((val << 24) & 0xFF0000000000) | ((val << 8) & 0xFF00000000) |
            (val >> 56 & 0xFF) | ((val >> 40) & 0xFF00) |
            ((val >> 24) & 0xFF0000) | ((val >> 8) & 0xFF000000);
    }

    template <>
    inline uint64_t swapBytes(uint64_t val) {
        return (val << 56) | ((val << 40) & 0xFF000000000000) |
            ((val << 24) & 0xFF0000000000) | ((val << 8) & 0xFF00000000) |
            (val >> 56) | ((val >> 40) & 0xFF00) |
            ((val >> 24) & 0xFF0000) | ((val >> 8) & 0xFF000000);
    }

    template <>
    inline char16_t swapBytes(char16_t val) {
        return static_cast<char16_t>(
            swapBytes(static_cast<int16_t>(val)));
    }

    template <>
    inline char32_t swapBytes(char32_t val) {
        return static_cast<char32_t>(
            swapBytes(static_cast<int32_t>(val)));
    }

    template <>
    inline float swapBytes(float val) {
        if constexpr (sizeof(float) == 4) {
            union {
                uint32_t u32i;
                float f32;
            } u;
            u.f32 = val;
            u.u32i = swapBytes(u.u32i);
            return u.f32;
        }

        if constexpr (sizeof(float) == 8) {
            union {
                uint64_t u64i;
                float f64;
            } u;
            u.f64 = val;
            u.u64i = swapBytes(u.u64i);
            return u.f64;
        }

        static_assert(
            sizeof(float) == 4 || sizeof(float) == 8,
            "unavailable type!");
        return 0;
    }

    template <>
    inline double swapBytes(double val) {
        if constexpr (sizeof(double) == 4) {
            union {
                uint32_t u32i;
                double f32;
            } u;
            u.f32 = val;
            u.u32i = swapBytes(u.u32i);
            return u.f32;
        }

        if constexpr (sizeof(double) == 8) {
            union {
                uint64_t u64i;
                double f64;
            } u;
            u.f64 = val;
            u.u64i = swapBytes(u.u64i);
            return u.f64;
        }

        static_assert(
            sizeof(double) == 4 || sizeof(double) == 8,
            "unavailable type!");
        return 0;
    }


    template <typename T>
    T fromToBE(T val) {
#ifdef ENDIAN_LE
        return swapBytes(val);
#else
        return val;
#endif
    }

    template <typename T>
    T fromToLE(T val) {
#ifdef ENDIAN_LE
        return val;
#else
        return swapBytes(val);
#endif
    }

}

#endif  // UTILS_ENDIAN_HPP_