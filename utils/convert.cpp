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
        std::string out;
        if (!wchar_to_utf8(str, &out)) {
            out.clear();
        }
        return out;
    }

    std::u16string WideToUTF16(const std::wstring_view& str) {
        std::u16string out;
        if (!wchar_to_utf16(str, &out)) {
            out.clear();
        }
        return out;
    }

}