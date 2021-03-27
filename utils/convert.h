// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_CONVERT_H_
#define UTILS_CONVERT_H_

#include <charconv>
#include <cstdint>
#include <limits>
#include <string_view>

#include "utils/log.h"
#include "utils/platform_utils.h"


namespace utl {

    std::string UTF16ToUTF8(const std::u16string_view& str);
    std::u16string UTF8ToUTF16(const std::string_view& str);

    std::string WideToUTF8(const std::wstring_view& str);
    std::u16string WideToUTF16(const std::wstring_view& str);

    char toASCIIUpper(char ch);
    char toASCIILower(char ch);
    std::string toASCIIUpper(const std::string_view& str);
    std::string toASCIILower(const std::string_view& str);

    char16_t toASCIIUpper(char16_t ch);
    char16_t toASCIILower(char16_t ch);
    std::u16string toASCIIUpper(const std::u16string_view& str);
    std::u16string toASCIILower(const std::u16string_view& str);

    inline std::u16string to_u16string(int val) {
        return UTF8ToUTF16(std::to_string(val));
    }
    inline std::u16string to_u16string(unsigned int val) {
        return UTF8ToUTF16(std::to_string(val));
    }
    inline std::u16string to_u16string(long val) {
        return UTF8ToUTF16(std::to_string(val));
    }
    inline std::u16string to_u16string(unsigned long val) {
        return UTF8ToUTF16(std::to_string(val));
    }
    inline std::u16string to_u16string(long long val) {
        return UTF8ToUTF16(std::to_string(val));
    }
    inline std::u16string to_u16string(unsigned long long val) {
        return UTF8ToUTF16(std::to_string(val));
    }
    inline std::u16string to_u16string(float val) {
        return UTF8ToUTF16(std::to_string(val));
    }
    inline std::u16string to_u16string(double val) {
        return UTF8ToUTF16(std::to_string(val));
    }
    inline std::u16string to_u16string(long double val) {
        return UTF8ToUTF16(std::to_string(val));
    }

    template <typename T>
    std::string toString8Hex(T i) {
        std::ostringstream ss;
        ss << std::hex << std::uppercase << i;
        return ss.str();
    }

    template <typename T>
    std::u16string toString16Hex(T i) {
        return UTF8ToUTF16(toString8Hex(i));
    }

    template <typename T>
    bool stringToNumber(const std::string_view& text, T* out) {
#ifdef OS_MAC
        // libc++ 似乎没有实现 float/double 版本的 from_char
        T result;
        std::string str(text);
        std::istringstream ss(str);
        if (!(ss >> result)) {
            return false;
        }
        *out = result;
        return true;
#else
        auto result = std::from_chars(text.data(), text.data() + text.length(), *out);
        if (result.ec != std::errc()) {
            return false;
        }
        return true;
#endif
    }

    template <typename T>
    bool stringToNumber(const std::u16string_view& text, T* out) {
        return stringToNumber(UTF16ToUTF8(text), out);
    }

    template <typename T>
    bool hexStringToNumber(const std::string_view& text, T* out) {
#ifdef OS_MAC
        // libc++ 似乎没有实现 float/double 版本的 from_char
        T result;
        std::string str(text);
        std::istringstream ss(str);
        if (!(ss >> std::hex >> result)) {
            return false;
        }
        *out = result;
        return true;
#else
        auto result = std::from_chars(text.data(), text.data() + text.length(), *out, 16);
        if (result.ec != std::errc()) {
            return false;
        }
        return true;
#endif
    }

    template <typename T>
    bool hexStringToNumber(const std::u16string_view& text, T* out) {
        return hexStringToNumber(UTF16ToUTF8(text), out);
    }

}

#endif  // UTILS_CONVERT_H_
