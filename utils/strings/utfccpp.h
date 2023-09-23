// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STRINGS_UTFCCPP_H_
#define UTILS_STRINGS_UTFCCPP_H_

#include <string>

#include "utils/strings/utfcc.h"


namespace utl {

    int utf8_substr(const std::string_view& sv, size_t* sub_len, int flags);
    int utf16_substr(const std::u16string_view& sv, size_t* sub_len, int flags);
    int utf32_substr(const std::u32string_view& sv, size_t* sub_len, int flags);
    int wchar_substr(const std::wstring_view& sv, size_t* sub_len, int flags);

    int utf8_to_utf16(const std::string_view& sv, char16_t* buf, size_t* buf_len, int flags);
    int utf8_to_utf16(const char* src, size_t len, std::u16string* dst, int flags);
    int utf8_to_utf16(const std::string_view& sv, std::u16string* dst, int flags);

    int utf8_to_utf32(const std::string_view& sv, char32_t* buf, size_t* buf_len, int flags);
    int utf8_to_utf32(const char* src, size_t len, std::u32string* dst, int flags);
    int utf8_to_utf32(const std::string_view& sv, std::u32string* dst, int flags);

    int utf16_to_utf8(const std::u16string_view& sv, char* buf, size_t* buf_len, int flags);
    int utf16_to_utf8(const char16_t* src, size_t len, std::string* dst, int flags);
    int utf16_to_utf8(const std::u16string_view& sv, std::string* dst, int flags);

    int utf16_to_utf32(const std::u16string_view& sv, char32_t* buf, size_t* buf_len, int flags);
    int utf16_to_utf32(const char16_t* src, size_t len, std::u32string* dst, int flags);
    int utf16_to_utf32(const std::u16string_view& sv, std::u32string* dst, int flags);

    int utf32_to_utf8(const std::u32string_view& sv, char* buf, size_t* buf_len, int flags);
    int utf32_to_utf8(const char32_t* src, size_t len, std::string* dst, int flags);
    int utf32_to_utf8(const std::u32string_view& sv, std::string* dst, int flags);

    int utf32_to_utf16(const std::u32string_view& sv, char16_t* buf, size_t* buf_len, int flags);
    int utf32_to_utf16(const char32_t* src, size_t len, std::u16string* dst, int flags);
    int utf32_to_utf16(const std::u32string_view& sv, std::u16string* dst, int flags);

    int wchar_to_utf8(const std::wstring_view& sv, char* buf, size_t* buf_len, int flags);
    int wchar_to_utf8(const wchar_t* src, size_t len, std::string* dst, int flags);
    int wchar_to_utf8(const std::wstring_view& sv, std::string* dst, int flags);

    int wchar_to_utf16(const std::wstring_view& sv, char16_t* buf, size_t* buf_len, int flags);
    int wchar_to_utf16(const wchar_t* src, size_t len, std::u16string* dst, int flags);
    int wchar_to_utf16(const std::wstring_view& sv, std::u16string* dst, int flags);

    int wchar_to_utf32(const std::wstring_view& sv, char32_t* buf, size_t* buf_len, int flags);
    int wchar_to_utf32(const wchar_t* src, size_t len, std::u32string* dst, int flags);
    int wchar_to_utf32(const std::wstring_view& sv, std::u32string* dst, int flags);

    int utf8_to_wchar(const std::string_view& sv, wchar_t* buf, size_t* buf_len, int flags);
    int utf8_to_wchar(const char* src, size_t len, std::wstring* dst, int flags);
    int utf8_to_wchar(const std::string_view& sv, std::wstring* dst, int flags);

    int utf16_to_wchar(const std::u16string_view& sv, wchar_t* buf, size_t* buf_len, int flags);
    int utf16_to_wchar(const char16_t* src, size_t len, std::wstring* dst, int flags);
    int utf16_to_wchar(const std::u16string_view& sv, std::wstring* dst, int flags);

    int utf32_to_wchar(const std::u32string_view& sv, wchar_t* buf, size_t* buf_len, int flags);
    int utf32_to_wchar(const char32_t* src, size_t len, std::wstring* dst, int flags);
    int utf32_to_wchar(const std::u32string_view& sv, std::wstring* dst, int flags);

}

#endif  // UTILS_STRINGS_UTFCCPP_H_