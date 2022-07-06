// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_ENDIAN_HPP_
#define UTILS_ENDIAN_HPP_

#include <climits>
#include <type_traits>

#include "utils/platform_utils.h"

#define _utl_ff_at(bpos) (Ty(static_cast<unsigned char>(~0u)) << CHAR_BIT * (bpos))


namespace utl {
namespace internal {

    template <typename Ty>
    Ty swap_ui2b(Ty v) {
        return (v << CHAR_BIT) | (v >> CHAR_BIT);
    }

    template <typename Ty>
    Ty swap_ui4b(Ty v) {
        return (v << (CHAR_BIT * 3)) |
            ((v << CHAR_BIT) & _utl_ff_at(2)) |
            (v >> (CHAR_BIT * 3)) |
            ((v >> CHAR_BIT) & _utl_ff_at(1));
    }

    template <typename Ty>
    Ty swap_ui8b(Ty v) {
        return (v << (CHAR_BIT * 7)) |
            ((v << (CHAR_BIT * 5)) & _utl_ff_at(6)) |
            ((v << (CHAR_BIT * 3)) & _utl_ff_at(5)) |
            ((v << CHAR_BIT) & _utl_ff_at(4)) |
            (v >> (CHAR_BIT * 7)) |
            ((v >> (CHAR_BIT * 5)) & _utl_ff_at(1)) |
            ((v >> (CHAR_BIT * 3)) & _utl_ff_at(2)) |
            ((v >> CHAR_BIT) & _utl_ff_at(3));
    }

    template <typename Ty>
    Ty swap_f(Ty v) {
        Ty out;
        auto src = reinterpret_cast<unsigned char*>(&v) + (sizeof(Ty) - 1);
        auto dst = reinterpret_cast<unsigned char*>(&out);
        for (size_t i = 0; i < sizeof(Ty); ++i) {
            *dst++ = *src--;
        }
        return out;
    }

}

    template <typename Ty>
    Ty swapBytes(Ty val) {
        static_assert(sizeof(Ty) > 1, "unavailable type!");
        static_assert(
            std::is_floating_point<Ty>::value || std::is_integral<Ty>::value,
            "unavailable type!");

        if constexpr (std::is_floating_point<Ty>::value) {
            return internal::swap_f(val);
        } else if constexpr (sizeof(Ty) == 2) {
            if constexpr (std::is_unsigned<Ty>::value) {
                return internal::swap_ui2b(val);
            } else {
                typedef typename std::make_unsigned<Ty>::type UTy;
                auto tmp = internal::swap_ui2b(UTy(val));
                return reinterpret_cast<Ty&>(tmp);
            }
        } else if constexpr (sizeof(Ty) == 4) {
            if constexpr (std::is_unsigned<Ty>::value) {
                return internal::swap_ui4b(val);
            } else {
                typedef typename std::make_unsigned<Ty>::type UTy;
                auto tmp = internal::swap_ui4b(UTy(val));
                return reinterpret_cast<Ty&>(tmp);
            }
        } else if constexpr (sizeof(Ty) == 8) {
            if constexpr (std::is_unsigned<Ty>::value) {
                return internal::swap_ui8b(val);
            } else {
                typedef typename std::make_unsigned<Ty>::type UTy;
                auto tmp = internal::swap_ui8b(UTy(val));
                return reinterpret_cast<Ty&>(tmp);
            }
        }

        Ty out;
        auto src = reinterpret_cast<unsigned char*>(&val);
        auto dst = reinterpret_cast<unsigned char*>(&out) + sizeof(Ty);
        for (size_t i = 0; i < sizeof(Ty); ++i) {
            *dst-- = *src++;
        }

        return out;
    }


    template <typename Ty>
    Ty fromToBE(Ty val) {
#ifdef ENDIAN_LE
        return swapBytes(val);
#else
        return val;
#endif
    }

    template <typename Ty>
    Ty fromToLE(Ty val) {
#ifdef ENDIAN_LE
        return val;
#else
        return swapBytes(val);
#endif
    }

}

#endif  // UTILS_ENDIAN_HPP_
