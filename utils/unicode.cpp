// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/unicode.h"

#define IN_RANGE(var, min, max)  \
    (var) >= (min) && (var) <= (max)

#define IF_IN_RANGE(var, min, max)  \
    if (IN_RANGE(var, min, max)) {

#define ELIF_IN_RANGE(var, min, max)  \
    } else if (IN_RANGE(var, min, max)) {

#define EQUAL(var, v)  \
    (var) == (v)

#define IF_EQUAL(var, v)  \
    if (EQUAL(var, v)) {

#define ELIF_EQUAL(var, v)  \
    } else if (EQUAL(var, v)) {

#define ELSE  \
    } else {

#define END_IF  \
    }

#define ELSE_RET_FALSE  \
    ELSE return false; END_IF

#define CHECK_LENGTH(l)  \
    if (src_length - index < (l)) {  \
        return false;  \
    END_IF

#define CHECK_IN_RANGE(var, min, max)  \
    if (!(IN_RANGE(var, min, max))) {  \
        return false;  \
    END_IF

#define GET_BYTE(no)  \
    auto byte##no = static_cast<uint8_t>(src[index + (no) - 1])

#define GET_BYTE_AND_CHECK_RANGE(no, min, max)  \
    GET_BYTE(no);  \
    CHECK_IN_RANGE(byte##no, min, max)

#define GET_WORD(no)  \
    auto word##no = static_cast<uint16_t>(src[index + (no) - 1])

#define GET_WORD_AND_CHECK_RANGE(no, min, max)  \
    GET_WORD(no);  \
    CHECK_IN_RANGE(word##no, min, max)


namespace utl {

    bool Unicode::UTF8ToUTF16(const std::string_view& src, std::u16string* dst) {
        return UTF8ToOthers(src, dst, nullptr);
    }

    bool Unicode::UTF8ToUTF32(const std::string_view& src, std::u32string* dst) {
        return UTF8ToOthers(src, nullptr, dst);
    }

    bool Unicode::UTF16ToUTF8(const std::u16string_view& src, std::string* dst) {
        return UTF16ToOthers(src, dst, nullptr);
    }

    bool Unicode::UTF16ToUTF32(const std::u16string_view& src, std::u32string* dst) {
        return UTF16ToOthers(src, nullptr, dst);
    }

    void Unicode::UTF32ToUTF8(const std::u32string_view& src, std::string* dst) {
        char buf[4];
        uint8_t count;
        for (const auto& u32 : src) {
            SVToUTF8(u32, buf, &count);
            dst->append(buf, count);
        }
    }

    void Unicode::UTF32ToUTF16(const std::u32string_view& src, std::u16string* dst) {
        char16_t buf[2];
        uint8_t count;
        for (const auto& u32 : src) {
            SVToUTF16(u32, buf, &count);
            dst->append(buf, count);
        }
    }

    bool Unicode::UTF8ToOthers(const std::string_view& src, std::u16string* dst16, std::u32string* dst32) {
        if (src.empty()) {
            if (dst16) { dst16->clear(); }
            if (dst32) { dst32->clear(); }
            return true;
        }

        std::u16string::size_type index = 0;
        auto src_length = src.length();
        uint32_t scalar_value;

        std::u16string utf16_string;
        std::u32string utf32_string;

        while (index < src_length) {
            GET_BYTE(1);
            IF_IN_RANGE(byte1, 0x00, 0x7F)   // 1 byte
                scalar_value = getScalarValue(byte1);
                ++index;
            ELIF_IN_RANGE(byte1, 0xC2, 0xDF) // 2 byte
                CHECK_LENGTH(2)
                GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0xBF)
                scalar_value = getScalarValue(byte1, byte2);
                index += 2;
            ELIF_EQUAL(byte1, 0xE0)          // 3 byte
                CHECK_LENGTH(3)
                GET_BYTE_AND_CHECK_RANGE(2, 0xA0, 0xBF)
                GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
                scalar_value = getScalarValue(byte1, byte2, byte3);
                index += 3;
            ELIF_IN_RANGE(byte1, 0xE1, 0xEC) // 3 byte
                CHECK_LENGTH(3)
                GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0xBF)
                GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
                scalar_value = getScalarValue(byte1, byte2, byte3);
                index += 3;
            ELIF_EQUAL(byte1, 0xED)          // 3 byte
                CHECK_LENGTH(3)
                GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0x9F)
                GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
                scalar_value = getScalarValue(byte1, byte2, byte3);
                index += 3;
            ELIF_IN_RANGE(byte1, 0xEE, 0xEF) // 3 byte
                CHECK_LENGTH(3)
                GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0xBF)
                GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
                scalar_value = getScalarValue(byte1, byte2, byte3);
                index += 3;
            ELIF_EQUAL(byte1, 0xF0)          // 4 byte
                CHECK_LENGTH(4)
                GET_BYTE_AND_CHECK_RANGE(2, 0x90, 0xBF)
                GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
                GET_BYTE_AND_CHECK_RANGE(4, 0x80, 0xBF)
                scalar_value = getScalarValue(byte1, byte2, byte3, byte4);
                index += 4;
            ELIF_IN_RANGE(byte1, 0xF1, 0xF3) // 4 byte
                CHECK_LENGTH(4)
                GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0xBF)
                GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
                GET_BYTE_AND_CHECK_RANGE(4, 0x80, 0xBF)
                scalar_value = getScalarValue(byte1, byte2, byte3, byte4);
                index += 4;
            ELIF_EQUAL(byte1, 0xF4)          // 4 byte
                CHECK_LENGTH(4)
                GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0x8F)
                GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
                GET_BYTE_AND_CHECK_RANGE(4, 0x80, 0xBF)
                scalar_value = getScalarValue(byte1, byte2, byte3, byte4);
                index += 4;
            ELSE_RET_FALSE

            if (dst16) {
                char16_t buf[2];
                uint8_t count;
                SVToUTF16(scalar_value, buf, &count);
                utf16_string.append(buf, count);
            }
            if (dst32) {
                utf32_string.push_back(scalar_value);
            }
        }

        if (dst16) { *dst16 = std::move(utf16_string); }
        if (dst32) { *dst32 = std::move(utf32_string); }
        return true;
    }

    bool Unicode::UTF16ToOthers(const std::u16string_view& src, std::string* dst8, std::u32string* dst32) {
        if (src.empty()) {
            if (dst8) { dst8->clear(); }
            if (dst32) { dst32->clear(); }
            return true;
        }

        std::string::size_type index = 0;
        auto src_length = src.length();
        uint32_t scalar_value;

        std::string utf8_string;
        std::u32string utf32_string;

        while (index < src_length) {
            GET_WORD(1);
            IF_IN_RANGE(word1, 0x0000, 0xD7FF)   // 1 word
                scalar_value = getScalarValue(word1);
                ++index;
            ELIF_IN_RANGE(word1, 0xE000, 0xFFFF) // 1 word
                scalar_value = getScalarValue(word1);
                ++index;
            ELSE                                 // 2 word
                CHECK_LENGTH(2)
                GET_WORD(2);
                scalar_value = getScalarValue(word1, word2);
                index += 2;
            END_IF

            if (dst8) {
                char buf[4];
                uint8_t count;
                SVToUTF8(scalar_value, buf, &count);
                utf8_string.append(buf, count);
            }
            if (dst32) {
                utf32_string.push_back(scalar_value);
            }
        }

        if (dst8) { *dst8 = std::move(utf8_string); }
        if (dst32) { *dst32 = std::move(utf32_string); }
        return true;
    }

    void Unicode::SVToUTF8(uint32_t sv, char buf[4], uint8_t* count) {
        if((sv & 0xFFFFFF80) == 0) {
            // 1 byte
            buf[0] = char(sv & 0x7F);
            *count = 1;
        } else if((sv & 0xFFFFF800) == 0) {
            // 2 byte
            buf[0] = char(((sv & 0x7C0) >> 6) + 0xC0);
            buf[1] = char((sv & 0x3F) + 0x80);
            *count = 2;
        } else if ((sv & 0xFFFF0000) == 0) {
            // 3 byte
            buf[0] = char(((sv & 0xF000) >> 12) + 0xE0);
            buf[1] = char(((sv & 0xFC0) >> 6) + 0x80);
            buf[2] = char((sv & 0x3F) + 0x80);
            *count = 3;
        } else {
            // 4 byte
            buf[0] = char(((sv & 0x1C0000) >> 18) + 0xF0);
            buf[1] = char(((sv & 0x3F000) >> 12) + 0x80);
            buf[2] = char(((sv & 0xFC0) >> 6) + 0x80);
            buf[3] = char((sv & 0x3F) + 0x80);
            *count = 4;
        }
    }

    void Unicode::SVToUTF16(uint32_t sv, char16_t buf[2], uint8_t* count) {
        if ((sv & 0xFFFF0000) == 0) {
            // 1 word
            buf[0] = char16_t(sv & 0xFFFF);
            *count = 1;
        } else {
            // 2 word
            buf[0] = char16_t(
                ((sv & 0xFC00) >> 10) +
                ((((sv & 0x1F0000) - 1) & 0xF0000) >> 10) + 0xD800);
            buf[1] = char16_t((sv & 0x3FF) + 0xDC00);
            *count = 2;
        }
    }

    uint32_t Unicode::getScalarValue(uint8_t byte) {
        return byte;
    }

    uint32_t Unicode::getScalarValue(uint8_t byte1, uint8_t byte2) {
        uint32_t result = uint32_t(byte1 & 0x1F) << 6;
        result += (byte2 & 0x3F);
        return result;
    }

    uint32_t Unicode::getScalarValue(uint8_t byte1, uint8_t byte2, uint8_t byte3) {
        uint32_t result = uint32_t(byte1 & 0xF) << 12;
        result += uint32_t(byte2 & 0x3F) << 6;
        result += (byte3 & 0x3F);
        return result;
    }

    uint32_t Unicode::getScalarValue(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4) {
        uint32_t result = uint32_t(byte1 & 0x7) << 18;
        result += uint32_t(byte2 & 0x3F) << 12;
        result += uint32_t(byte3 & 0x3F) << 6;
        result += (byte4 & 0x3F);
        return result;
    }

    uint32_t Unicode::getScalarValue(uint16_t word) {
        return word;
    }

    uint32_t Unicode::getScalarValue(uint16_t word1, uint16_t word2) {
        uint32_t result = uint32_t((word1 & 0x3C0) + 0x40) << 10;
        result += uint32_t(word1 & 0x3F) << 10;
        result += (word2 & 0x3FF);
        return result;
    }

}