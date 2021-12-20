// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/convert.h"

#include "utils/unicode_conv.h"


namespace utl {

    std::string UTF16ToUTF8(const std::u16string_view& str) {
        std::string utf8_str;
        if (utf16_to_utf8(str, &utf8_str)) {
            return utf8_str;
        }

        return {};
    }

    std::u16string UTF8ToUTF16(const std::string_view& str) {
        std::u16string utf16_str;
        if (utf8_to_utf16(str, &utf16_str)) {
            return utf16_str;
        }

        return {};
    }

    std::string WideToUTF8(const std::wstring_view& str) {
        /**
         * wchar_t 转换为 char16_t 或 char32_t。
         * 逐个转换，以防止强制类型转换出现未定义行为。
         */
#ifdef OS_WINDOWS
        return UTF16ToUTF8(std::u16string(str.begin(), str.end()));
#elif defined OS_MAC
        std::string out;
        utf32_to_utf8(
            std::u32string(str.begin(), str.end()), &out);
        return out;
#endif
    }

    std::u16string WideToUTF16(const std::wstring_view& str) {
        /**
         * wchar_t 转换为 char16_t 或 char32_t。
         * 逐个转换，以防止强制类型转换出现未定义行为。
         */
#ifdef OS_WINDOWS
        return std::u16string(str.begin(), str.end());
#elif defined OS_MAC
        std::u16string out;
        utf32_to_utf16(
            std::u32string(str.begin(), str.end()), &out);
        return out;
#endif
    }

}