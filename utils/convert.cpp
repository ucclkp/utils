// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/convert.h"

#include "utils/platform_utils.h"
#include "utils/unicode.h"


namespace utl {

    std::string UTF16ToUTF8(const std::u16string_view& str) {
        std::string utf8_str;
        if (Unicode::UTF16ToUTF8(str, &utf8_str)) {
            return utf8_str;
        }

        return {};
    }

    std::u16string UTF8ToUTF16(const std::string_view& str) {
        std::u16string utf16_str;
        if (Unicode::UTF8ToUTF16(str, &utf16_str)) {
            return utf16_str;
        }

        return {};
    }

    std::string WideToUTF8(const std::wstring_view& str) {
#ifdef OS_WINDOWS
        return UTF16ToUTF8(
            std::u16string_view(
                reinterpret_cast<const char16_t*>(str.data()), str.length()));
#elif defined OS_MAC
        std::string out;
        Unicode::UTF32ToUTF8(
            std::u32string(reinterpret_cast<const char32_t*>(str.data()), str.size()), &out);
        return out;
#endif
    }

    std::u16string WideToUTF16(const std::wstring_view& str) {
#ifdef OS_WINDOWS
        return std::u16string(str.begin(), str.end());
#elif defined OS_MAC
        std::u16string out;
        Unicode::UTF32ToUTF16(
            std::u32string(reinterpret_cast<const char32_t*>(str.data()), str.size()), &out);
        return out;
#endif
    }

    char toASCIIUpper(char ch) {
        if (ch >= 'a' && ch <= 'z') {
            return ch - u'a' + u'A';
        }
        return ch;
    }

    char toASCIILower(char ch) {
        if (ch >= 'A' && ch <= 'Z') {
            return ch - 'A' + 'a';
        }
        return ch;
    }

    std::string toASCIIUpper(const std::string_view& str) {
        std::string result(str.size(), 0);
        auto it = result.begin();
        for (auto c : str) {
            *it = toASCIIUpper(c);
            ++it;
        }
        return result;
    }

    std::string toASCIILower(const std::string_view& str) {
        std::string result(str.size(), 0);
        auto it = result.begin();
        for (auto c : str) {
            *it = toASCIILower(c);
            ++it;
        }
        return result;
    }

    char16_t toASCIIUpper(char16_t ch) {
        if (ch >= u'a' && ch <= u'z') {
            return ch - u'a' + u'A';
        }
        return ch;
    }

    char16_t toASCIILower(char16_t ch) {
        if (ch >= u'A' && ch <= u'Z') {
            return ch - u'A' + u'a';
        }
        return ch;
    }

    std::u16string toASCIIUpper(const std::u16string_view& str) {
        std::u16string result(str.size(), 0);
        auto it = result.begin();
        for (auto c : str) {
            *it = toASCIIUpper(c);
            ++it;
        }
        return result;
    }

    std::u16string toASCIILower(const std::u16string_view& str) {
        std::u16string result(str.size(), 0);
        auto it = result.begin();
        for (auto c : str) {
            *it = toASCIILower(c);
            ++it;
        }
        return result;
    }

}