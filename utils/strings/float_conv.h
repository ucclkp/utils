// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STRINGS_FLOAT_CONV_H_
#define UTILS_STRINGS_FLOAT_CONV_H_

#include <string>

#include "float_conv_types.hpp"


namespace utl {

    bool ftos(
        float val, char* buf, size_t* len,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    bool ftos(
        double val, char* buf, size_t* len,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    bool ftos(
        long double val, char* buf, size_t* len,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);

    bool ftos(
        float val, char16_t* buf, size_t* len,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    bool ftos(
        double val, char16_t* buf, size_t* len,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    bool ftos(
        long double val, char16_t* buf, size_t* len,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);

    bool ftos(
        float val, char32_t* buf, size_t* len,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    bool ftos(
        double val, char32_t* buf, size_t* len,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    bool ftos(
        long double val, char32_t* buf, size_t* len,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);

    bool ftos(
        float val, wchar_t* buf, size_t* len,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    bool ftos(
        double val, wchar_t* buf, size_t* len,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    bool ftos(
        long double val, wchar_t* buf, size_t* len,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);

    void ftos(
        float val, std::string* out,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    void ftos(
        double val, std::string* out,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    void ftos(
        long double val, std::string* out,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);

    void ftos(
        float val, std::u16string* out,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    void ftos(
        double val, std::u16string* out,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    void ftos(
        long double val, std::u16string* out,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);

    void ftos(
        float val, std::u32string* out,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    void ftos(
        double val, std::u32string* out,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    void ftos(
        long double val, std::u32string* out,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);

    void ftos(
        float val, std::wstring* out,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    void ftos(
        double val, std::wstring* out,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    void ftos(
        long double val, std::wstring* out,
        int precision, int fmt = FCF_FIX, int round = FCR_NEAR);

    std::string ftos8(
        float val, int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    std::string ftos8(
        double val, int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    std::string ftos8(
        long double val, int precision, int fmt = FCF_FIX, int round = FCR_NEAR);

    std::u16string ftos16(
        float val, int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    std::u16string ftos16(
        double val, int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    std::u16string ftos16(
        long double val, int precision, int fmt = FCF_FIX, int round = FCR_NEAR);

    std::u32string ftos32(
        float val, int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    std::u32string ftos32(
        double val, int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    std::u32string ftos32(
        long double val, int precision, int fmt = FCF_FIX, int round = FCR_NEAR);

    std::wstring ftosw(
        float val, int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    std::wstring ftosw(
        double val, int precision, int fmt = FCF_FIX, int round = FCR_NEAR);
    std::wstring ftosw(
        long double val, int precision, int fmt = FCF_FIX, int round = FCR_NEAR);

    int stof(
        const char* str, size_t len, float* out,
        int fmt = FCF_FIX, int round = FCR_NEAR, const char** n = nullptr);
    int stof(
        const char* str, size_t len, double* out,
        int fmt = FCF_FIX, int round = FCR_NEAR, const char** n = nullptr);
    int stof(
        const char* str, size_t len, long double* out,
        int fmt = FCF_FIX, int round = FCR_NEAR, const char** n = nullptr);

    int stof(
        const char16_t* str, size_t len, float* out,
        int fmt = FCF_FIX, int round = FCR_NEAR, const char16_t** n = nullptr);
    int stof(
        const char16_t* str, size_t len, double* out,
        int fmt = FCF_FIX, int round = FCR_NEAR, const char16_t** n = nullptr);
    int stof(
        const char16_t* str, size_t len, long double* out,
        int fmt = FCF_FIX, int round = FCR_NEAR, const char16_t** n = nullptr);

    int stof(
        const char32_t* str, size_t len, float* out,
        int fmt = FCF_FIX, int round = FCR_NEAR, const char32_t** n = nullptr);
    int stof(
        const char32_t* str, size_t len, double* out,
        int fmt = FCF_FIX, int round = FCR_NEAR, const char32_t** n = nullptr);
    int stof(
        const char32_t* str, size_t len, long double* out,
        int fmt = FCF_FIX, int round = FCR_NEAR, const char32_t** n = nullptr);

    int stof(
        const wchar_t* str, size_t len, float* out,
        int fmt = FCF_FIX, int round = FCR_NEAR, const wchar_t** n = nullptr);
    int stof(
        const wchar_t* str, size_t len, double* out,
        int fmt = FCF_FIX, int round = FCR_NEAR, const wchar_t** n = nullptr);
    int stof(
        const wchar_t* str, size_t len, long double* out,
        int fmt = FCF_FIX, int round = FCR_NEAR, const wchar_t** n = nullptr);

    int stof(
        const std::string_view& str, float* out,
        int fmt = FCF_FIX, int round = FCR_NEAR);
    int stof(
        const std::string_view& str, double* out,
        int fmt = FCF_FIX, int round = FCR_NEAR);
    int stof(
        const std::string_view& str, long double* out,
        int fmt = FCF_FIX, int round = FCR_NEAR);

    int stof(
        const std::u16string_view& str, float* out,
        int fmt = FCF_FIX, int round = FCR_NEAR);
    int stof(
        const std::u16string_view& str, double* out,
        int fmt = FCF_FIX, int round = FCR_NEAR);
    int stof(
        const std::u16string_view& str, long double* out,
        int fmt = FCF_FIX, int round = FCR_NEAR);

    int stof(
        const std::u32string_view& str, float* out,
        int fmt = FCF_FIX, int round = FCR_NEAR);
    int stof(
        const std::u32string_view& str, double* out,
        int fmt = FCF_FIX, int round = FCR_NEAR);
    int stof(
        const std::u32string_view& str, long double* out,
        int fmt = FCF_FIX, int round = FCR_NEAR);

    int stof(
        const std::wstring_view& str, float* out,
        int fmt = FCF_FIX, int round = FCR_NEAR);
    int stof(
        const std::wstring_view& str, double* out,
        int fmt = FCF_FIX, int round = FCR_NEAR);
    int stof(
        const std::wstring_view& str, long double* out,
        int fmt = FCF_FIX, int round = FCR_NEAR);

}

#endif  // UTILS_STRINGS_FLOAT_CONV_H_