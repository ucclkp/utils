// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STRINGS_UNICODE_CONV_H_
#define UTILS_STRINGS_UNICODE_CONV_H_

#include <string>

#include "utils/strings/conv_ret_code.hpp"

#define IS_HIGH_SURROGATES(c16)  ((c16) >= 0xD800 && (c16) <= 0xDBFF)
#define IS_LOW_SURROGATES(c16)   ((c16) >= 0xDC00 && (c16) <= 0xDFFF)
#define IS_SURROGATES(c16)       ((c16) >= 0xD800 && (c16) <= 0xDFFF)


namespace utl {

    bool utf8_substr(const char* src, size_t len, size_t* sub_len);
    bool utf8_substr(const std::string_view& sv, size_t* sub_len);

    bool utf16_substr(const char16_t* src, size_t len, size_t* sub_len);
    bool utf16_substr(const std::u16string_view& sv, size_t* sub_len);

    void utf32_substr(const char32_t* src, size_t len, size_t* sub_len);
    void utf32_substr(const std::u32string_view& sv, size_t* sub_len);

    bool wchar_substr(const wchar_t* src, size_t len, size_t* sub_len);
    bool wchar_substr(const std::wstring_view& sv, size_t* sub_len);

    bool utf8_to_utf16(char ch, char16_t* out);
    int utf8_to_utf16(const char* src, size_t len, char16_t* buf, size_t* buf_len);
    int utf8_to_utf16(const std::string_view& sv, char16_t* buf, size_t* buf_len);
    bool utf8_to_utf16(const char* src, size_t len, std::u16string* dst);
    bool utf8_to_utf16(const std::string_view& sv, std::u16string* dst);

    bool utf8_to_utf32(char ch, char32_t* out);
    int utf8_to_utf32(const char* src, size_t len, char32_t* buf, size_t* buf_len);
    int utf8_to_utf32(const std::string_view& sv, char32_t* buf, size_t* buf_len);
    bool utf8_to_utf32(const char* src, size_t len, std::u32string* dst);
    bool utf8_to_utf32(const std::string_view& sv, std::u32string* dst);

    bool utf16_to_utf8(char16_t ch, char* dst, size_t* len);
    int utf16_to_utf8(const char16_t* src, size_t len, char* buf, size_t* buf_len);
    int utf16_to_utf8(const std::u16string_view& sv, char* buf, size_t* buf_len);
    bool utf16_to_utf8(const char16_t* src, size_t len, std::string* dst);
    bool utf16_to_utf8(const std::u16string_view& sv, std::string* dst);

    bool utf16_to_utf32(char16_t ch, char32_t* out);
    int utf16_to_utf32(const char16_t* src, size_t len, char32_t* buf, size_t* buf_len);
    int utf16_to_utf32(const std::u16string_view& sv, char32_t* buf, size_t* buf_len);
    bool utf16_to_utf32(const char16_t* src, size_t len, std::u32string* dst);
    bool utf16_to_utf32(const std::u16string_view& sv, std::u32string* dst);

    bool utf32_to_utf8(char32_t ch, char* dst, size_t* len);
    bool utf32_to_utf8(const char32_t* src, size_t len, char* buf, size_t* buf_len);
    bool utf32_to_utf8(const std::u32string_view& sv, char* buf, size_t* buf_len);
    void utf32_to_utf8(const char32_t* src, size_t len, std::string* dst);
    void utf32_to_utf8(const std::u32string_view& sv, std::string* dst);

    bool utf32_to_utf16(char32_t ch, char16_t* dst, size_t* len);
    bool utf32_to_utf16(const char32_t* src, size_t len, char16_t* buf, size_t* buf_len);
    bool utf32_to_utf16(const std::u32string_view& sv, char16_t* buf, size_t* buf_len);
    void utf32_to_utf16(const char32_t* src, size_t len, std::u16string* dst);
    void utf32_to_utf16(const std::u32string_view& sv, std::u16string* dst);

    int wchar_to_utf8(wchar_t ch, char* dst, size_t* len);
    int wchar_to_utf8(const wchar_t* src, size_t len, char* buf, size_t* buf_len);
    int wchar_to_utf8(const std::wstring_view& sv, char* buf, size_t* buf_len);
    bool wchar_to_utf8(const wchar_t* src, size_t len, std::string* dst);
    bool wchar_to_utf8(const std::wstring_view& sv, std::string* dst);

    int wchar_to_utf16(wchar_t ch, char16_t* dst, size_t* len);
    int wchar_to_utf16(const wchar_t* src, size_t len, char16_t* buf, size_t* buf_len);
    int wchar_to_utf16(const std::wstring_view& sv, char16_t* buf, size_t* buf_len);
    bool wchar_to_utf16(const wchar_t* src, size_t len, std::u16string* dst);
    bool wchar_to_utf16(const std::wstring_view& sv, std::u16string* dst);

    bool wchar_to_utf32(wchar_t ch, char32_t* out);
    int wchar_to_utf32(const wchar_t* src, size_t len, char32_t* buf, size_t* buf_len);
    int wchar_to_utf32(const std::wstring_view& sv, char32_t* buf, size_t* buf_len);
    bool wchar_to_utf32(const wchar_t* src, size_t len, std::u32string* dst);
    bool wchar_to_utf32(const std::wstring_view& sv, std::u32string* dst);

}

#endif  // UTILS_STRINGS_UNICODE_CONV_H_