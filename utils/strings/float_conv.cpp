// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "float_conv.h"

#include <string>

#include "utils/strings/float_conv_internal.hpp"


namespace utl {

namespace internal {

    template <typename FTy, typename Cy>
    bool ftos(
        FTy val, Cy* buf, size_t* len,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR)
    {
        static_assert(
            std::is_floating_point<FTy>::value, "FTy must be a floating type!");

        if (fmt & FCF_HEX2) {
            fmt |= FCF_SCI;
            return ftos_hex2_base(val, fmt, round, precision, buf, len);
        } else if (fmt & FCF_HEX) {
            return ftos_base<FTy, uint_fast64_t, Cy, 16>(val, fmt, round, precision, buf, len);
        } else {
            return ftos_base<FTy, uint_fast64_t, Cy, 10>(val, fmt, round, precision, buf, len);
        }
    }

    template <typename FTy, typename Cy>
    void ftos(
        FTy val, std::basic_string<Cy>* out,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR)
    {
        static_assert(
            std::is_floating_point<FTy>::value, "FTy must be a floating type!");

        if (precision < 0) {
            precision = 0;
        }

        std::basic_string<Cy> result;
        if (fmt & FCF_HEX2) {
            fmt |= FCF_SCI;
            ftos_hex2_base(val, fmt, round, precision, &result);
        } else if (fmt & FCF_HEX) {
            ftos_base<FTy, uint_fast64_t, Cy, 16>(val, fmt, round, precision, &result);
        } else {
            ftos_base<FTy, uint_fast64_t, Cy, 10>(val, fmt, round, precision, &result);
        }

        *out = std::move(result);
    }

    template <typename FTy, typename Cy>
    int stof(
        const Cy* str, size_t len, FTy* out,
        int fmt = FCF_FIX, int round = FCR_NEAR, const Cy** n = nullptr)
    {
        static_assert(
            std::is_floating_point<FTy>::value, "FTy must be a floating type!");

        if (!len || !str) {
            if (n) *n = str;
            return SCR_FAIL;
        }

        int ret;
        const Cy* _p;
        if (fmt & FCF_HEX2) {
            fmt |= FCF_SCI;
            ret = stof_hex2_base(str, len, fmt, round, out, &_p);
        } else if (fmt & FCF_HEX) {
            ret = stof_base<FTy, uint_fast64_t, Cy, 16>(str, len, fmt, round, out, &_p);
        } else {
            ret = stof_base<FTy, uint_fast64_t, Cy, 10>(str, len, fmt, round, out, &_p);
        }

        if (ret != SCR_OK) {
            if (n) {
                if (ret == SCR_OF) {
                    *n = str + len;
                } else {
                    *n = str;
                }
            }
            return ret;
        }
        if (!n && _p != str + len) {
            return SCR_FAIL;
        }

        if (n) *n = _p;
        return SCR_OK;
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

    bool ftos(
        float val, wchar_t* buf, size_t* len,
        int precision, int fmt, int round)
    {
        return internal::ftos(val, buf, len, precision, fmt, round);
    }

    bool ftos(
        double val, wchar_t* buf, size_t* len,
        int precision, int fmt, int round)
    {
        return internal::ftos(val, buf, len, precision, fmt, round);
    }

    bool ftos(
        long double val, wchar_t* buf, size_t* len,
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

    void ftos(
        float val, std::wstring* out,
        int precision, int fmt, int round)
    {
        internal::ftos(val, out, precision, fmt, round);
    }

    void ftos(
        double val, std::wstring* out,
        int precision, int fmt, int round)
    {
        internal::ftos(val, out, precision, fmt, round);
    }

    void ftos(
        long double val, std::wstring* out,
        int precision, int fmt, int round)
    {
        internal::ftos(val, out, precision, fmt, round);
    }

    std::string ftos8(
        float val, int precision, int fmt, int round)
    {
        std::string out;
        internal::ftos(val, &out, precision, fmt, round);
        return out;
    }

    std::string ftos8(
        double val, int precision, int fmt, int round)
    {
        std::string out;
        internal::ftos(val, &out, precision, fmt, round);
        return out;
    }

    std::string ftos8(
        long double val, int precision, int fmt, int round)
    {
        std::string out;
        internal::ftos(val, &out, precision, fmt, round);
        return out;
    }

    std::u16string ftos16(
        float val, int precision, int fmt, int round)
    {
        std::u16string out;
        internal::ftos(val, &out, precision, fmt, round);
        return out;
    }

    std::u16string ftos16(
        double val, int precision, int fmt, int round)
    {
        std::u16string out;
        internal::ftos(val, &out, precision, fmt, round);
        return out;
    }

    std::u16string ftos16(
        long double val, int precision, int fmt, int round)
    {
        std::u16string out;
        internal::ftos(val, &out, precision, fmt, round);
        return out;
    }

    std::u32string ftos32(
        float val, int precision, int fmt, int round)
    {
        std::u32string out;
        internal::ftos(val, &out, precision, fmt, round);
        return out;
    }

    std::u32string ftos32(
        double val, int precision, int fmt, int round)
    {
        std::u32string out;
        internal::ftos(val, &out, precision, fmt, round);
        return out;
    }

    std::u32string ftos32(
        long double val, int precision, int fmt, int round)
    {
        std::u32string out;
        internal::ftos(val, &out, precision, fmt, round);
        return out;
    }

    std::wstring ftosw(
        float val, int precision, int fmt, int round)
    {
        std::wstring out;
        internal::ftos(val, &out, precision, fmt, round);
        return out;
    }

    std::wstring ftosw(
        double val, int precision, int fmt, int round)
    {
        std::wstring out;
        internal::ftos(val, &out, precision, fmt, round);
        return out;
    }

    std::wstring ftosw(
        long double val, int precision, int fmt, int round)
    {
        std::wstring out;
        internal::ftos(val, &out, precision, fmt, round);
        return out;
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
        const wchar_t* str, size_t len, float* out,
        int fmt, int round, const wchar_t** n)
    {
        return internal::stof(str, len, out, fmt, round, n);
    }

    int stof(
        const wchar_t* str, size_t len, double* out,
        int fmt, int round, const wchar_t** n)
    {
        return internal::stof(str, len, out, fmt, round, n);
    }

    int stof(
        const wchar_t* str, size_t len, long double* out,
        int fmt, int round, const wchar_t** n)
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

    int stof(
        const std::wstring_view& str, float* out,
        int fmt, int round)
    {
        return internal::stof(str.data(), str.size(), out, fmt, round);
    }

    int stof(
        const std::wstring_view& str, double* out,
        int fmt, int round)
    {
        return internal::stof(str.data(), str.size(), out, fmt, round);
    }

    int stof(
        const std::wstring_view& str, long double* out,
        int fmt, int round)
    {
        return internal::stof(str.data(), str.size(), out, fmt, round);
    }

}