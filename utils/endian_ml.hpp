// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_ENDIAN_ML_HPP_
#define UTILS_ENDIAN_ML_HPP_

#include "utils/endian_internal.hpp"
#include "utils/platform_utils.h"


namespace utl {

    template <int N, typename Ty>
    Ty swapBytesMLB2L(Ty val) {
        static_assert(
            (sizeof(Ty) == 4 && (N == 2 || N == 3)) ||
            (sizeof(Ty) == 8 && (N == 4 || N == 5 || N == 6 || N == 7)),
            "unavailable Ty or N!");

        if constexpr (sizeof(Ty) == 4) {
            if constexpr (N == 2) {
                if constexpr (std::is_unsigned<Ty>::value) {
                    return internal::swap_ui4b_b2l_ml2u(val);
                } else {
                    return internal::swap_i4b_b2l_ml2s(val);
                }
            } else if constexpr (N == 3) {
                if constexpr (std::is_unsigned<Ty>::value) {
                    return internal::swap_ui4b_b2l_ml3u(val);
                } else {
                    return internal::swap_i4b_b2l_ml3s(val);
                }
            }
        } else if constexpr (sizeof(Ty) == 8) {
            if constexpr (N == 4) {
                if constexpr (std::is_unsigned<Ty>::value) {
                    return internal::swap_ui8b_b2l_ml4u(val);
                } else {
                    return internal::swap_i8b_b2l_ml4s(val);
                }
            } else if constexpr (N == 5) {
                if constexpr (std::is_unsigned<Ty>::value) {
                    return internal::swap_ui8b_b2l_ml5u(val);
                } else {
                    return internal::swap_i8b_b2l_ml5s(val);
                }
            } else if constexpr (N == 6) {
                if constexpr (std::is_unsigned<Ty>::value) {
                    return internal::swap_ui8b_b2l_ml6u(val);
                } else {
                    return internal::swap_i8b_b2l_ml6s(val);
                }
            } else if constexpr (N == 7) {
                if constexpr (std::is_unsigned<Ty>::value) {
                    return internal::swap_ui8b_b2l_ml7u(val);
                } else {
                    return internal::swap_i8b_b2l_ml7s(val);
                }
            }
        }
        return 0;
    }

    template <int N, typename Ty>
    Ty swapBytesMLL2B(Ty val) {
        static_assert(
            (sizeof(Ty) == 4 && (N == 2 || N == 3)) ||
            (sizeof(Ty) == 8 && (N == 4 || N == 5 || N == 6 || N == 7)),
            "unavailable Ty or N!");

        if constexpr (sizeof(Ty) == 4) {
            if constexpr (N == 2) {
                if constexpr (std::is_unsigned<Ty>::value) {
                    return internal::swap_ui4b_l2b_ml2u(val);
                } else {
                    return internal::swap_i4b_l2b_ml2s(val);
                }
            } else if constexpr (N == 3) {
                if constexpr (std::is_unsigned<Ty>::value) {
                    return internal::swap_ui4b_l2b_ml3u(val);
                } else {
                    return internal::swap_i4b_l2b_ml3s(val);
                }
            }
        } else if constexpr (sizeof(Ty) == 8) {
            if constexpr (N == 4) {
                if constexpr (std::is_unsigned<Ty>::value) {
                    return internal::swap_ui8b_l2b_ml4u(val);
                } else {
                    return internal::swap_i8b_l2b_ml4s(val);
                }
            } else if constexpr (N == 5) {
                if constexpr (std::is_unsigned<Ty>::value) {
                    return internal::swap_ui8b_l2b_ml5u(val);
                } else {
                    return internal::swap_i8b_l2b_ml5s(val);
                }
            } else if constexpr (N == 6) {
                if constexpr (std::is_unsigned<Ty>::value) {
                    return internal::swap_ui8b_l2b_ml6u(val);
                } else {
                    return internal::swap_i8b_l2b_ml6s(val);
                }
            } else if constexpr (N == 7) {
                if constexpr (std::is_unsigned<Ty>::value) {
                    return internal::swap_ui8b_l2b_ml7u(val);
                } else {
                    return internal::swap_i8b_l2b_ml7s(val);
                }
            }
        }

        return 0;
    }


    template <size_t N, typename Ty>
    Ty MLToLE(Ty val) {
#ifdef ENDIAN_LE
        static_assert(std::is_integral<Ty>::value, "Ty must be a integral type!");
        static_assert(sizeof(Ty) > N, "The size of Ty must be greater than N!");
        if constexpr (std::is_unsigned<Ty>::value) {
            return val & ((Ty(1u) << (N * CHAR_BIT)) - 1);
        } else {
            typedef typename std::make_unsigned<Ty>::type UTy;
            auto tmp = UTy(val) & ((UTy(1u) << (N * CHAR_BIT)) - 1);
            return reinterpret_cast<Ty&>(tmp);
        }
#else
        return swapBytesMLB2L<N>(val);
#endif
    }

    template <size_t N, typename Ty>
    Ty MLToBE(Ty val) {
#ifdef ENDIAN_LE
        return swapBytesMLL2B<N>(val);
#else
        static_assert(std::is_integral<Ty>::value, "Ty must be a integral type!");
        static_assert(sizeof(Ty) > N, "The size of Ty must be greater than N!");
        if constexpr (std::is_unsigned<Ty>::value) {
            return val >> ((sizeof(Ty) - N) * CHAR_BIT);
        } else {
            typedef std::make_unsigned<Ty>::type UTy;
            auto tmp = UTy(val) >> ((sizeof(Ty) - N) * CHAR_BIT);
            return reinterpret_cast<Ty&>(tmp);
        }
#endif
    }

    template <size_t N, typename Ty>
    Ty fromMLLE(Ty val) {
#ifdef ENDIAN_LE
        static_assert(std::is_integral<Ty>::value, "Ty must be a integral type!");
        static_assert(sizeof(Ty) > N, "The size of Ty must be greater than N!");
        if constexpr (std::is_unsigned<Ty>::value) {
            return val & ((Ty(1u) << (N * CHAR_BIT)) - 1);
        } else {
            typedef typename std::make_unsigned<Ty>::type UTy;
            auto tmp = UTy(val) & ((UTy(1u) << (N * CHAR_BIT)) - 1);
            return reinterpret_cast<Ty&>(tmp);
        }
#else
        return swapBytesMLL2B<N>(val);
#endif
    }

    template <size_t N, typename Ty>
    Ty fromMLBE(Ty val) {
#ifdef ENDIAN_LE
        return swapBytesMLB2L<N>(val);
#else
        static_assert(std::is_integral<Ty>::value, "Ty must be a integral type!");
        static_assert(sizeof(Ty) > N, "The size of Ty must be greater than N!");
        if constexpr (std::is_unsigned<Ty>::value) {
            return val >> ((sizeof(Ty) - N) * CHAR_BIT);
        } else {
            typedef std::make_unsigned<Ty>::type UTy;
            auto tmp = UTy(val) >> ((sizeof(Ty) - N) * CHAR_BIT);
            return reinterpret_cast<Ty&>(tmp);
        }
#endif
    }

    template <size_t N, typename Ty>
    Ty toMLLE(Ty val) {
#ifdef ENDIAN_LE
        static_assert(std::is_integral<Ty>::value, "Ty must be a integral type!");
        static_assert(sizeof(Ty) > N, "The size of Ty must be greater than N!");
        if constexpr (std::is_unsigned<Ty>::value) {
            return val & ((Ty(1u) << (N * CHAR_BIT)) - 1);
        } else {
            typedef typename std::make_unsigned<Ty>::type UTy;
            auto tmp = UTy(val) & ((UTy(1u) << (N * CHAR_BIT)) - 1);
            return reinterpret_cast<Ty&>(tmp);
        }
#else
        return swapBytesMLB2L<N>(val);
#endif
    }

    template <size_t N, typename T>
    T toMLBE(T val) {
#ifdef ENDIAN_LE
        return swapBytesMLL2B<N>(val);
#else
        static_assert(std::is_integral<T>::value, "T must be a integral type!");
        static_assert(sizeof(T) > N, "The size of T must be greater than N!");
        if constexpr (std::is_unsigned<Ty>::value) {
            return val >> ((sizeof(T) - N) * CHAR_BIT);
        } else {
            typedef std::make_unsigned<Ty>::type UTy;
            auto tmp = UTy(val) >> ((sizeof(T) - N) * CHAR_BIT);
            return reinterpret_cast<Ty&>(tmp);
        }
#endif
    }

}

#endif  // UTILS_ENDIAN_ML_HPP_
