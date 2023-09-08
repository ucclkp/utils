// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STRINGS_STRING_UTILS_HPP_
#define UTILS_STRINGS_STRING_UTILS_HPP_

#include <string>
#include <vector>

#include "utils/strings/string_utils_internal.hpp"
#include "utils/strings/unicode_conv.h"


namespace utl {

    inline char     tolatcu(char ch)     { return internal::tolatu(ch); }
    inline char16_t tolatcu(char16_t ch) { return internal::tolatu(ch); }
    inline char32_t tolatcu(char32_t ch) { return internal::tolatu(ch); }
    inline wchar_t  tolatcu(wchar_t ch)  { return internal::tolatu(ch); }
    inline int      tolatcu(int ch)      { return internal::tolatu(ch); }

    inline char     tolatcl(char ch)     { return internal::tolatl(ch); }
    inline char16_t tolatcl(char16_t ch) { return internal::tolatl(ch); }
    inline char32_t tolatcl(char32_t ch) { return internal::tolatl(ch); }
    inline wchar_t  tolatcl(wchar_t ch)  { return internal::tolatl(ch); }
    inline int      tolatcl(int ch)      { return internal::tolatl(ch); }

    inline void tolatu_self(std::string* text)    { internal::tolatu_self(text); }
    inline void tolatu_self(std::u16string* text) { internal::tolatu_self(text); }
    inline void tolatu_self(std::u32string* text) { internal::tolatu_self(text); }
    inline void tolatu_self(std::wstring* text)   { internal::tolatu_self(text); }

    inline void tolatl_self(std::string* text)    { internal::tolatl_self(text); }
    inline void tolatl_self(std::u16string* text) { internal::tolatl_self(text); }
    inline void tolatl_self(std::u32string* text) { internal::tolatl_self(text); }
    inline void tolatl_self(std::wstring* text)   { internal::tolatl_self(text); }
    
    inline std::string    tolatu(const std::string_view& text)    { return internal::tolatu(text); }
    inline std::u16string tolatu(const std::u16string_view& text) { return internal::tolatu(text); }
    inline std::u32string tolatu(const std::u32string_view& text) { return internal::tolatu(text); }
    inline std::wstring   tolatu(const std::wstring_view& text)   { return internal::tolatu(text); }

    inline std::string    tolatl(const std::string_view& text)    { return internal::tolatl(text); }
    inline std::u16string tolatl(const std::u16string_view& text) { return internal::tolatl(text); }
    inline std::u32string tolatl(const std::u32string_view& text) { return internal::tolatl(text); }
    inline std::wstring   tolatl(const std::wstring_view& text)   { return internal::tolatl(text); }

    inline void trim_self(
        std::string* text,
        const std::string_view& tokens,
        int flags = TRF_SE) { internal::trim_self(text, tokens, flags); }
    inline void trim_self(
        std::u16string* text,
        const std::u16string_view& tokens,
        int flags = TRF_SE) { internal::trim_self(text, tokens, flags); }
    inline void trim_self(
        std::u32string* text,
        const std::u32string_view& tokens,
        int flags = TRF_SE) { internal::trim_self(text, tokens, flags); }
    inline void trim_self(
        std::wstring* text,
        const std::wstring_view& tokens,
        int flags = TRF_SE) { internal::trim_self(text, tokens, flags); }

    inline void trim_self(
        std::string* text, int flags = TRF_SE)    { trim_self(text, " ", flags); }
    inline void trim_self(
        std::u16string* text, int flags = TRF_SE) { trim_self(text, u" ", flags); }
    inline void trim_self(
        std::u32string* text, int flags = TRF_SE) { trim_self(text, U" ", flags); }
    inline void trim_self(
        std::wstring* text, int flags = TRF_SE)   { trim_self(text, L" ", flags); }
    
    inline std::string trim(
        const std::string_view& text,
        const std::string_view& tokens,
        int flags = TRF_SE) { return internal::trim(text, tokens, flags); }
    inline std::u16string trim(
        const std::u16string_view& text,
        const std::u16string_view& tokens,
        int flags = TRF_SE) { return internal::trim(text, tokens, flags); }
    inline std::u32string trim(
        const std::u32string_view& text,
        const std::u32string_view& tokens,
        int flags = TRF_SE) { return internal::trim(text, tokens, flags); }
    inline std::wstring trim(
        const std::wstring_view& text,
        const std::wstring_view& tokens,
        int flags = TRF_SE) { return internal::trim(text, tokens, flags); }

    inline std::string trim(
        const std::string_view& text, int flags = TRF_SE)    { return trim(text, " ", flags); }
    inline std::u16string trim(
        const std::u16string_view& text, int flags = TRF_SE) { return trim(text, u" ", flags); }
    inline std::u32string trim(
        const std::u32string_view& text, int flags = TRF_SE) { return trim(text, U" ", flags); }
    inline std::wstring trim(
        const std::wstring_view& text, int flags = TRF_SE)   { return trim(text, L" ", flags); }

    inline std::vector<std::string> split(
        const std::string_view& text,
        const std::string_view& tokens,
        bool filter_empty = false)
    {
        return internal::split(text, tokens, filter_empty);
    }

    inline std::vector<std::u16string> split(
        const std::u16string_view& text,
        const std::u16string_view& tokens,
        bool filter_empty = false)
    {
        return internal::split(text, tokens, filter_empty);
    }

    inline std::vector<std::u32string> split(
        const std::u32string_view& text,
        const std::u32string_view& tokens,
        bool filter_empty = false)
    {
        return internal::split(text, tokens, filter_empty);
    }

    inline std::vector<std::wstring> split(
        const std::wstring_view& text,
        const std::wstring_view& tokens,
        bool filter_empty = false)
    {
        return internal::split(text, tokens, filter_empty);
    }

    inline bool startWith(
        const std::string_view& base,
        const std::string_view& match,
        size_t off = 0, bool latin_case_sens = true)
    {
        return internal::startWith(base, match, off, latin_case_sens);
    }

    inline bool startWith(
        const std::u16string_view& base,
        const std::u16string_view& match,
        size_t off = 0, bool latin_case_sens = true)
    {
        return internal::startWith(base, match, off, latin_case_sens);
    }

    inline bool startWith(
        const std::u32string_view& base,
        const std::u32string_view& match,
        size_t off = 0, bool latin_case_sens = true)
    {
        return internal::startWith(base, match, off, latin_case_sens);
    }

    inline bool startWith(
        const std::wstring_view& base,
        const std::wstring_view& match,
        size_t off = 0, bool latin_case_sens = true)
    {
        return internal::startWith(base, match, off, latin_case_sens);
    }

    inline bool endWith(
        const std::string_view& base,
        const std::string_view& match,
        bool latin_case_sens = true)
    {
        return internal::endWith(base, match, latin_case_sens);
    }

    inline bool endWith(
        const std::u16string_view& base,
        const std::u16string_view& match,
        bool latin_case_sens = true)
    {
        return internal::endWith(base, match, latin_case_sens);
    }

    inline bool endWith(
        const std::u32string_view& base,
        const std::u32string_view& match,
        bool latin_case_sens = true)
    {
        return internal::endWith(base, match, latin_case_sens);
    }

    inline bool endWith(
        const std::wstring_view& base,
        const std::wstring_view& match,
        bool latin_case_sens = true)
    {
        return internal::endWith(base, match, latin_case_sens);
    }

    inline bool isLitEqual(
        const std::string_view& s1,
        const std::string_view& s2,
        bool latin_case_sens = false)
    {
        return internal::isLitEqual(s1, s2, latin_case_sens);
    }

    inline bool isLitEqual(
        const std::u16string_view& s1,
        const std::u16string_view& s2,
        bool latin_case_sens = false)
    {
        return internal::isLitEqual(s1, s2, latin_case_sens);
    }

    inline bool isLitEqual(
        const std::u32string_view& s1,
        const std::u32string_view& s2,
        bool latin_case_sens = false)
    {
        return internal::isLitEqual(s1, s2, latin_case_sens);
    }

    inline bool isLitEqual(
        const std::wstring_view& s1,
        const std::wstring_view& s2,
        bool latin_case_sens = false)
    {
        return internal::isLitEqual(s1, s2, latin_case_sens);
    }

    inline void replaceAll(
        std::string* base,
        const std::string_view& sub,
        const std::string_view& new_s)
    {
        return internal::replaceAll(base, sub, new_s);
    }

    inline void replaceAll(
        std::u16string* base,
        const std::u16string_view& sub,
        const std::u16string_view& new_s)
    {
        return internal::replaceAll(base, sub, new_s);
    }

    inline void replaceAll(
        std::u32string* base,
        const std::u32string_view& sub,
        const std::u32string_view& new_s)
    {
        return internal::replaceAll(base, sub, new_s);
    }

    inline void replaceAll(
        std::wstring* base,
        const std::wstring_view& sub,
        const std::wstring_view& new_s)
    {
        return internal::replaceAll(base, sub, new_s);
    }

    inline void replaceAllTokens(
        std::string* base,
        const std::string_view& token,
        const std::string_view& new_s)
    {
        return internal::replaceAllTokens(base, token, new_s);
    }

    inline void replaceAllTokens(
        std::u16string* base,
        const std::u16string_view& token,
        const std::u16string_view& new_s)
    {
        return internal::replaceAllTokens(base, token, new_s);
    }

    inline void replaceAllTokens(
        std::u32string* base,
        const std::u32string_view& token,
        const std::u32string_view& new_s)
    {
        return internal::replaceAllTokens(base, token, new_s);
    }

    inline void replaceAllTokens(
        std::wstring* base,
        const std::wstring_view& token,
        const std::wstring_view& new_s)
    {
        return internal::replaceAllTokens(base, token, new_s);
    }

    inline std::string u16to8(const std::u16string_view& str) {
        std::string utf8_str;
        if (utf16_to_utf8(str, &utf8_str)) {
            return utf8_str;
        }

        return {};
    }

    inline std::u16string u8to16(const std::string_view& str) {
        std::u16string utf16_str;
        if (utf8_to_utf16(str, &utf16_str)) {
            return utf16_str;
        }

        return {};
    }

    inline std::string wtou8(const std::wstring_view& str) {
        std::string out;
        if (!wchar_to_utf8(str, &out)) {
            out.clear();
        }
        return out;
    }

    inline std::u16string wtou16(const std::wstring_view& str) {
        std::u16string out;
        if (!wchar_to_utf16(str, &out)) {
            out.clear();
        }
        return out;
    }

    inline std::wstring u8tow(const std::string_view& sv) {
        std::wstring out;
        if (!utf8_to_wchar(sv, &out)) {
            out.clear();
        }
        return out;
    }

    inline std::wstring u16tow(const std::u16string_view& sv) {
        std::wstring out;
        if (!utf16_to_wchar(sv, &out)) {
            out.clear();
        }
        return out;
    }

}


#endif  // UTILS_STRINGS_STRING_UTILS_HPP_