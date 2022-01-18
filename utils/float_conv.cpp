// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "float_conv.h"

#include <string>

#include "utils/float_conv_internal.hpp"


namespace utl {

namespace internal {

    template <typename FTy, typename Cy>
    bool ftos(
        FTy val, Cy* buf, size_t* len,
        int precision, int fmt = FF_NOR, int round = FR_NEAR)
    {
        if (fmt & FF_HEX2) {
            fmt |= FF_SCI;
        }

        if ((fmt & FF_HEX) || (fmt & FF_HEX2)) {
            return ftos_base<FTy, uint_fast64_t, Cy, 16>(val, fmt, round, precision, buf, len);
        } else {
            return ftos_base<FTy, uint_fast64_t, Cy, 10>(val, fmt, round, precision, buf, len);
        }
    }

    template <typename FTy, typename Cy>
    void ftos(
        FTy val, std::basic_string<Cy>* out,
        int precision, int fmt = FF_NOR, int round = FR_NEAR)
    {
        static_assert(
            std::is_floating_point<FTy>::value, "FTy must be a floating type!");

        if (precision < 0) {
            precision = 0;
        }

        if (fmt & FF_HEX2) {
            fmt |= FF_SCI;
        }

        std::basic_string<Cy> result;
        if ((fmt & FF_HEX) || (fmt & FF_HEX2)) {
            ftos_base<FTy, uint_fast64_t, Cy, 16>(val, fmt, round, precision, &result);
        } else {
            ftos_base<FTy, uint_fast64_t, Cy, 10>(val, fmt, round, precision, &result);
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
            ret = stof_base<FTy, uint_fast64_t, Cy, 16>(str, len, fmt, round, out, &_p);
        } else {
            ret = stof_base<FTy, uint_fast64_t, Cy, 10>(str, len, fmt, round, out, &_p);
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

}

    bool ftos(
        float val, char* buf, size_t* len,
        int precision, int fmt, int round)
    {
        return internal::ftos(val, buf, len, precision, fmt, round);
    }

    bool ftos(
        double val, char* buf, size_t* len,
        int precision, int fmt, int round)
    {
        return internal::ftos(val, buf, len, precision, fmt, round);
    }

    bool ftos(
        long double val, char* buf, size_t* len,
        int precision, int fmt, int round)
    {
        return internal::ftos(val, buf, len, precision, fmt, round);
    }

    bool ftos(
        float val, char16_t* buf, size_t* len,
        int precision, int fmt, int round)
    {
        return internal::ftos(val, buf, len, precision, fmt, round);
    }

    bool ftos(
        double val, char16_t* buf, size_t* len,
        int precision, int fmt, int round)
    {
        return internal::ftos(val, buf, len, precision, fmt, round);
    }

    bool ftos(
        long double val, char16_t* buf, size_t* len,
        int precision, int fmt, int round)
    {
        return internal::ftos(val, buf, len, precision, fmt, round);
    }

    bool ftos(
        float val, char32_t* buf, size_t* len,
        int precision, int fmt, int round)
    {
        return internal::ftos(val, buf, len, precision, fmt, round);
    }

    bool ftos(
        double val, char32_t* buf, size_t* len,
        int precision, int fmt, int round)
    {
        return internal::ftos(val, buf, len, precision, fmt, round);
    }

    bool ftos(
        long double val, char32_t* buf, size_t* len,
        int precision, int fmt, int round)
    {
        return internal::ftos(val, buf, len, precision, fmt, round);
    }

    void ftos(
        float val, std::string* out,
        int precision, int fmt, int round)
    {
        internal::ftos(val, out, precision, fmt, round);
    }

    void ftos(
        double val, std::string* out,
        int precision, int fmt, int round)
    {
        internal::ftos(val, out, precision, fmt, round);
    }

    void ftos(
        long double val, std::string* out,
        int precision, int fmt, int round)
    {
        internal::ftos(val, out, precision, fmt, round);
    }

    void ftos(
        float val, std::u16string* out,
        int precision, int fmt, int round)
    {
        internal::ftos(val, out, precision, fmt, round);
    }

    void ftos(
        double val, std::u16string* out,
        int precision, int fmt, int round)
    {
        internal::ftos(val, out, precision, fmt, round);
    }

    void ftos(
        long double val, std::u16string* out,
        int precision, int fmt, int round)
    {
        internal::ftos(val, out, precision, fmt, round);
    }

    void ftos(
        float val, std::u32string* out,
        int precision, int fmt, int round)
    {
        internal::ftos(val, out, precision, fmt, round);
    }

    void ftos(
        double val, std::u32string* out,
        int precision, int fmt, int round)
    {
        internal::ftos(val, out, precision, fmt, round);
    }

    void ftos(
        long double val, std::u32string* out,
        int precision, int fmt, int round)
    {
        internal::ftos(val, out, precision, fmt, round);
    }

    int stof(
        const char* str, size_t len, float* out,
        int fmt, int round, const char** n)
    {
        return internal::stof(str, len, out, fmt, round, n);
    }

    int stof(
        const char* str, size_t len, double* out,
        int fmt, int round, const char** n)
    {
        return internal::stof(str, len, out, fmt, round, n);
    }

    int stof(
        const char* str, size_t len, long double* out,
        int fmt, int round, const char** n)
    {
        return internal::stof(str, len, out, fmt, round, n);
    }

    int stof(
        const char16_t* str, size_t len, float* out,
        int fmt, int round, const char16_t** n)
    {
        return internal::stof(str, len, out, fmt, round, n);
    }

    int stof(
        const char16_t* str, size_t len, double* out,
        int fmt, int round, const char16_t** n)
    {
        return internal::stof(str, len, out, fmt, round, n);
    }

    int stof(
        const char16_t* str, size_t len, long double* out,
        int fmt, int round, const char16_t** n)
    {
        return internal::stof(str, len, out, fmt, round, n);
    }

    int stof(
        const char32_t* str, size_t len, float* out,
        int fmt, int round, const char32_t** n)
    {
        return internal::stof(str, len, out, fmt, round, n);
    }

    int stof(
        const char32_t* str, size_t len, double* out,
        int fmt, int round, const char32_t** n)
    {
        return internal::stof(str, len, out, fmt, round, n);
    }

    int stof(
        const char32_t* str, size_t len, long double* out,
        int fmt, int round, const char32_t** n)
    {
        return internal::stof(str, len, out, fmt, round, n);
    }

    int stof(
        const std::string_view& str, float* out,
        int fmt, int round)
    {
        return internal::stof(str.data(), str.size(), out, fmt, round);
    }

    int stof(
        const std::string_view& str, double* out,
        int fmt, int round)
    {
        return internal::stof(str.data(), str.size(), out, fmt, round);
    }

    int stof(
        const std::string_view& str, long double* out,
        int fmt, int round)
    {
        return internal::stof(str.data(), str.size(), out, fmt, round);
    }

    int stof(
        const std::u16string_view& str, float* out,
        int fmt, int round)
    {
        return internal::stof(str.data(), str.size(), out, fmt, round);
    }

    int stof(
        const std::u16string_view& str, double* out,
        int fmt, int round)
    {
        return internal::stof(str.data(), str.size(), out, fmt, round);
    }

    int stof(
        const std::u16string_view& str, long double* out,
        int fmt, int round)
    {
        return internal::stof(str.data(), str.size(), out, fmt, round);
    }

    int stof(
        const std::u32string_view& str, float* out,
        int fmt, int round)
    {
        return internal::stof(str.data(), str.size(), out, fmt, round);
    }

    int stof(
        const std::u32string_view& str, double* out,
        int fmt, int round)
    {
        return internal::stof(str.data(), str.size(), out, fmt, round);
    }

    int stof(
        const std::u32string_view& str, long double* out,
        int fmt, int round)
    {
        return internal::stof(str.data(), str.size(), out, fmt, round);
    }

}