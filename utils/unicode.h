// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_UNICODE_H_
#define UTILS_UNICODE_H_

#include <string>

#define IS_HIGH_SURROGATES(c16)  ((c16) >= 0xD800 && (c16) <= 0xDBFF)
#define IS_LOW_SURROGATES(c16)   ((c16) >= 0xDC00 && (c16) <= 0xDFFF)
#define IS_SURROGATES(c16)       (IS_HIGH_SURROGATES(c16) || IS_LOW_SURROGATES(c16))


namespace utl {

    class Unicode {
    public:
        static bool UTF8ToUTF16(const std::string_view& src, std::u16string* dst);
        static bool UTF8ToUTF32(const std::string_view& src, std::u32string* dst);
        static bool UTF16ToUTF8(const std::u16string_view& src, std::string* dst);
        static bool UTF16ToUTF32(const std::u16string_view& src, std::u32string* dst);
        static void UTF32ToUTF8(const std::u32string_view& src, std::string* dst);
        static void UTF32ToUTF16(const std::u32string_view& src, std::u16string* dst);

    private:
        static bool UTF8ToOthers(const std::string_view& src, std::u16string* dst16, std::u32string* dst32);
        static bool UTF16ToOthers(const std::u16string_view& src, std::string* dst8, std::u32string* dst32);

        static void SVToUTF8(uint32_t sv, char buf[4], uint8_t* count);
        static void SVToUTF16(uint32_t sv, char16_t buf[2], uint8_t* count);

        inline static uint32_t getScalarValue(uint8_t byte);
        inline static uint32_t getScalarValue(uint8_t byte1, uint8_t byte2);
        inline static uint32_t getScalarValue(uint8_t byte1, uint8_t byte2, uint8_t byte3);
        inline static uint32_t getScalarValue(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4);

        inline static uint32_t getScalarValue(uint16_t word);
        inline static uint32_t getScalarValue(uint16_t word1, uint16_t word2);
    };

}

#endif  // UTILS_UNICODE_H_