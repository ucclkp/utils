// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_FLOAT_CONV_H_
#define UTILS_FLOAT_CONV_H_

#include <string>

#include "utils/float_conv_internal.hpp"


namespace utl {

    template <typename FTy, typename Cy>
    bool ftos(
        FTy val, Cy* buf, size_t* len,
        int precision, int fmt = FF_NOR, int round = FR_NEAR)
    {
        if (fmt & FF_HEX2) {
            fmt |= FF_SCI;
        }

        if ((fmt & FF_HEX) || (fmt & FF_HEX2)) {
            return internal::ftos_base<FTy, uint_fast64_t, Cy, 16>(val, fmt, round, precision, buf, len);
        } else {
            return internal::ftos_base<FTy, uint_fast64_t, Cy, 10>(val, fmt, round, precision, buf, len);
        }
    }

    template <typename FTy, typename Cy>
    void ftos(
        FTy val, std::basic_string<Cy>* out,
        int precision, int fmt = FF_NOR, int round = FR_NEAR)
    {
        if (precision < 0) {
            precision = 0;
        }

        if (fmt & FF_HEX2) {
            fmt |= FF_SCI;
        }

        std::basic_string<Cy> result;
        if ((fmt & FF_HEX) || (fmt & FF_HEX2)) {
            internal::ftos_base<FTy, uint_fast64_t, Cy, 16>(val, fmt, round, precision, &result);
        } else {
            internal::ftos_base<FTy, uint_fast64_t, Cy, 10>(val, fmt, round, precision, &result);
        }

        *out = std::move(result);
    }

    template <typename FTy, typename Cy>
    int stof(
        const Cy* str, size_t len, FTy* out,
        int fmt = FF_NOR, int round = FR_NEAR, const Cy** n = nullptr)
    {
        if (!len || !str) {
            if (n) *n = str;
            return UCR_FAILED;
        }

        if (fmt & FF_HEX2) {
            fmt |= FF_SCI;
        }

        int ret;
        const Cy* _p;
        if ((fmt & FF_HEX) || (fmt & FF_HEX2)) {
            ret = internal::stof_base<FTy, uint_fast64_t, Cy, 16>(str, len, fmt, round, out, &_p);
        } else {
            ret = internal::stof_base<FTy, uint_fast64_t, Cy, 10>(str, len, fmt, round, out, &_p);
        }

        if (ret != UCR_OK) {
            if (n) *n = str;
            return ret;
        }
        if (!n && _p != str + len) {
            return UCR_FAILED;
        }

        if (n) *n = _p;
        return UCR_OK;
    }

    template <typename FTy, typename Cy>
    int stof(
        const std::basic_string<Cy>& str, FTy* out,
        int fmt = FF_NOR, int round = FR_NEAR)
    {
        return stof(str.data(), str.size(), out, fmt, round);
    }

}

#endif  // UTILS_FLOAT_CONV_H_