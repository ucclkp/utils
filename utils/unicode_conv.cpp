// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/unicode_conv.h"

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
    if (se - s < (l)) {  \
        return false;  \
    END_IF

#define CHECK_IN_RANGE(var, min, max)  \
    if (!(IN_RANGE(var, min, max))) {  \
        return false;  \
    END_IF

#define GET_BYTE(no)  \
    auto byte##no = static_cast<uint8_t>(*(s + (no) - 1))

#define GET_BYTE_AND_CHECK_RANGE(no, min, max)  \
    GET_BYTE(no);  \
    CHECK_IN_RANGE(byte##no, min, max)

#define GET_WORD(no)  \
    auto word##no = static_cast<uint16_t>(*(s + (no) - 1))

#define GET_WORD_AND_CHECK_RANGE(no, min, max)  \
    GET_WORD(no);  \
    CHECK_IN_RANGE(word##no, min, max)


namespace {

    void sv_to_utf8(uint32_t sv, char buf[4], size_t* len) {
        if ((sv & 0xFFFFFF80) == 0) {
            // 1 byte
            buf[0] = char(sv & 0x7F);
            *len = 1;
        } else if ((sv & 0xFFFFF800) == 0) {
            // 2 byte
            buf[0] = char(((sv & 0x7C0) >> 6) + 0xC0);
            buf[1] = char((sv & 0x3F) + 0x80);
            *len = 2;
        } else if ((sv & 0xFFFF0000) == 0) {
            // 3 byte
            buf[0] = char(((sv & 0xF000) >> 12) + 0xE0);
            buf[1] = char(((sv & 0xFC0) >> 6) + 0x80);
            buf[2] = char((sv & 0x3F) + 0x80);
            *len = 3;
        } else {
            // 4 byte
            buf[0] = char(((sv & 0x1C0000) >> 18) + 0xF0);
            buf[1] = char(((sv & 0x3F000) >> 12) + 0x80);
            buf[2] = char(((sv & 0xFC0) >> 6) + 0x80);
            buf[3] = char((sv & 0x3F) + 0x80);
            *len = 4;
        }
    }

    void sv_to_utf16(uint32_t sv, char16_t buf[2], size_t* len) {
        if ((sv & 0xFFFF0000) == 0) {
            // 1 word
            buf[0] = char16_t(sv & 0xFFFF);
            *len = 1;
        } else {
            // 2 word
            buf[0] = char16_t(
                ((sv & 0xFC00) >> 10) +
                ((((sv & 0x1F0000) - 1) & 0xF0000) >> 10) + 0xD800);
            buf[1] = char16_t((sv & 0x3FF) + 0xDC00);
            *len = 2;
        }
    }

    uint32_t sv8(uint8_t byte) {
        return byte;
    }

    uint32_t sv8(uint8_t byte1, uint8_t byte2) {
        uint32_t result = uint32_t(byte1 & 0x1F) << 6;
        result += (byte2 & 0x3F);
        return result;
    }

    uint32_t sv8(uint8_t byte1, uint8_t byte2, uint8_t byte3) {
        uint32_t result = uint32_t(byte1 & 0xF) << 12;
        result += uint32_t(byte2 & 0x3F) << 6;
        result += (byte3 & 0x3F);
        return result;
    }

    uint32_t sv8(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4) {
        uint32_t result = uint32_t(byte1 & 0x7) << 18;
        result += uint32_t(byte2 & 0x3F) << 12;
        result += uint32_t(byte3 & 0x3F) << 6;
        result += (byte4 & 0x3F);
        return result;
    }

    uint32_t sv16(uint16_t word) {
        return word;
    }

    uint32_t sv16(uint16_t word1, uint16_t word2) {
        uint32_t result = uint32_t((word1 & 0x3C0) + 0x40) << 10;
        result += uint32_t(word1 & 0x3F) << 10;
        result += (word2 & 0x3FF);
        return result;
    }

    bool nextsv8(const char*& s, const char* se, uint32_t* sv) {
        GET_BYTE(1);
        IF_IN_RANGE(byte1, 0x00, 0x7F)   // 1 byte
            if (sv) *sv = sv8(byte1);
        ++s;
        ELIF_IN_RANGE(byte1, 0xC2, 0xDF) // 2 byte
            CHECK_LENGTH(2)
            GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0xBF)
            if (sv) *sv = sv8(byte1, byte2);
        s += 2;
        ELIF_EQUAL(byte1, 0xE0)          // 3 byte
            CHECK_LENGTH(3)
            GET_BYTE_AND_CHECK_RANGE(2, 0xA0, 0xBF)
            GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
            if (sv) *sv = sv8(byte1, byte2, byte3);
        s += 3;
        ELIF_IN_RANGE(byte1, 0xE1, 0xEC) // 3 byte
            CHECK_LENGTH(3)
            GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0xBF)
            GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
            if (sv) *sv = sv8(byte1, byte2, byte3);
        s += 3;
        ELIF_EQUAL(byte1, 0xED)          // 3 byte
            CHECK_LENGTH(3)
            GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0x9F)
            GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
            if (sv) *sv = sv8(byte1, byte2, byte3);
        s += 3;
        ELIF_IN_RANGE(byte1, 0xEE, 0xEF) // 3 byte
            CHECK_LENGTH(3)
            GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0xBF)
            GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
            if (sv) *sv = sv8(byte1, byte2, byte3);
        s += 3;
        ELIF_EQUAL(byte1, 0xF0)          // 4 byte
            CHECK_LENGTH(4)
            GET_BYTE_AND_CHECK_RANGE(2, 0x90, 0xBF)
            GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
            GET_BYTE_AND_CHECK_RANGE(4, 0x80, 0xBF)
            if (sv) *sv = sv8(byte1, byte2, byte3, byte4);
        s += 4;
        ELIF_IN_RANGE(byte1, 0xF1, 0xF3) // 4 byte
            CHECK_LENGTH(4)
            GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0xBF)
            GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
            GET_BYTE_AND_CHECK_RANGE(4, 0x80, 0xBF)
            if (sv) *sv = sv8(byte1, byte2, byte3, byte4);
        s += 4;
        ELIF_EQUAL(byte1, 0xF4)          // 4 byte
            CHECK_LENGTH(4)
            GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0x8F)
            GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF)
            GET_BYTE_AND_CHECK_RANGE(4, 0x80, 0xBF)
            if (sv) *sv = sv8(byte1, byte2, byte3, byte4);
        s += 4;
        ELSE_RET_FALSE

        return true;
    }

    uint32_t nextsv16(const char16_t*& s, const char16_t* se, uint32_t* sv) {
        GET_WORD(1);
        IF_IN_RANGE(word1, 0x0000, 0xD7FF)   // 1 word
            if (sv) *sv = sv16(word1);
        ++s;
        ELIF_IN_RANGE(word1, 0xE000, 0xFFFF) // 1 word
            if (sv) *sv = sv16(word1);
        ++s;
        ELSE                                 // 2 word
            CHECK_LENGTH(2)
            GET_WORD(2);
            if (sv) *sv = sv16(word1, word2);
            s += 2;
        END_IF

        return true;
    }

}

namespace utl {

    bool utf8_substr(const char* src, size_t len, size_t* sub_len) {
        if (*sub_len >= len) {
            *sub_len = len;
            return true;
        }

        auto s = src;
        auto se = s + len;

        for (size_t i = 0; i < *sub_len; ++i) {
            if (s >= se) {
                return false;
            }
            if (!nextsv8(s, se, nullptr)) {
                return false;
            }
        }

        *sub_len = size_t(s - src);
        return true;
    }

    bool utf8_substr(const std::string_view& sv, size_t* sub_len) {
        return utf8_substr(sv.data(), sv.size(), sub_len);
    }

    bool utf16_substr(const char16_t* src, size_t len, size_t* sub_len) {
        if (*sub_len >= len) {
            *sub_len = len;
            return true;
        }

        auto s = src;
        auto se = s + len;

        for (size_t i = 0; i < *sub_len; ++i) {
            if (s >= se) {
                return false;
            }
            if (!nextsv16(s, se, nullptr)) {
                return false;
            }
        }

        *sub_len = size_t(s - src);
        return true;
    }

    bool utf16_substr(const std::u16string_view& sv, size_t* sub_len) {
        return utf16_substr(sv.data(), sv.size(), sub_len);
    }

    void utf32_substr(const char32_t* src, size_t len, size_t* sub_len) {
        if (*sub_len >= len) {
            *sub_len = len;
        }
    }

    void utf32_substr(const std::u32string_view& sv, size_t* sub_len) {
        utf32_substr(sv.data(), sv.size(), sub_len);
    }

    bool utf8_to_utf16(char ch, char16_t* out) {
        if (ch >= 0x00 && ch <= 0x7F) {
            *out = ch;
            return true;
        }
        return false;
    }

    int utf8_to_utf16(
        const char* src, size_t len, char16_t* buf, size_t* buf_len)
    {
        if (!len || !src) {
            *buf_len = 0;
            return UCR_OK;
        }

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto s16 = buf;
        auto s16e = s16 ? (buf + *buf_len) : nullptr;

        uint32_t scalar_value;
        while (s < se) {
            if (!nextsv8(s, se, &scalar_value)) {
                return UCR_FAILED;
            }

            char16_t _buf[2];
            size_t count;
            sv_to_utf16(scalar_value, _buf, &count);

            if (s16) {
                for (size_t i = 0; i < count && s16 < s16e; ++i) {
                    *s16++ = _buf[i];
                }
            }
            _buf_len += count;
        }

        *buf_len = _buf_len;
        if (s16 && s16 - buf == _buf_len) {
            return UCR_OK;
        }
        return UCR_BUFFER;
    }

    int utf8_to_utf16(const std::string_view& sv, char16_t* buf, size_t* buf_len) {
        return utf8_to_utf16(sv.data(), sv.size(), buf, buf_len);
    }

    bool utf8_to_utf16(const char* src, size_t len, std::u16string* dst) {
        if (!len || !src) {
            dst->clear();
            return true;
        }

        auto s = src;
        auto se = s + len;

        uint32_t scalar_value;
        std::u16string utf16_string;

        while (s < se) {
            if (!nextsv8(s, se, &scalar_value)) {
                return false;
            }

            char16_t buf[2];
            size_t count;
            sv_to_utf16(scalar_value, buf, &count);
            utf16_string.append(buf, count);
        }

        *dst = std::move(utf16_string);
        return true;
    }

    bool utf8_to_utf16(const std::string_view& sv, std::u16string* dst) {
        return utf8_to_utf16(sv.data(), sv.size(), dst);
    }

    bool utf8_to_utf32(char ch, char32_t* out) {
        if (ch >= 0x00 && ch <= 0x7F) {
            *out = ch;
            return true;
        }
        return false;
    }

    int utf8_to_utf32(
        const char* src, size_t len, char32_t* buf, size_t* buf_len)
    {
        if (!len || !src) {
            *buf_len = 0;
            return UCR_OK;
        }

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto s32 = buf;
        auto s32e = s32 ? (buf + *buf_len) : nullptr;

        uint32_t scalar_value;
        while (s < se) {
            if (!nextsv8(s, se, &scalar_value)) {
                return UCR_FAILED;
            }

            if (s32 && s32 < s32e) {
                *s32++ = scalar_value;
            }
            ++_buf_len;
        }

        *buf_len = _buf_len;
        if (s32 && s32 - buf == _buf_len) {
            return UCR_OK;
        }
        return UCR_BUFFER;
    }

    int utf8_to_utf32(const std::string_view& sv, char32_t* buf, size_t* buf_len) {
        return utf8_to_utf32(sv.data(), sv.size(), buf, buf_len);
    }

    bool utf8_to_utf32(const char* src, size_t len, std::u32string* dst) {
        if (!len || !src) {
            dst->clear();
            return true;
        }

        auto s = src;
        auto se = s + len;

        uint32_t scalar_value;
        std::u16string utf16_string;
        std::u32string utf32_string;

        while (s < se) {
            if (!nextsv8(s, se, &scalar_value)) {
                return false;
            }

            utf32_string.push_back(scalar_value);
        }

        *dst = std::move(utf32_string);
        return true;
    }

    bool utf8_to_utf32(const std::string_view& sv, std::u32string* dst) {
        return utf8_to_utf32(sv.data(), sv.size(), dst);
    }

    bool utf16_to_utf8(char16_t ch, char* dst, size_t* len) {
        if ((ch >= 0x0000 && ch <= 0xD7FF) ||
            (ch >= 0xE000 && ch <= 0xFFFF))
        {
            sv_to_utf8(uint32_t(ch), dst, len);
            return true;
        }
        return false;
    }

    int utf16_to_utf8(const char16_t* src, size_t len, char* buf, size_t* buf_len) {
        if (!len || !src) {
            *buf_len = 0;
            return UCR_OK;
        }

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto s8 = buf;
        auto s8e = s8 ? (buf + *buf_len) : nullptr;

        uint32_t scalar_value;
        while (s < se) {
            if (!nextsv16(s, se, &scalar_value)) {
                return UCR_FAILED;
            }

            char _buf[4];
            size_t count;
            sv_to_utf8(scalar_value, _buf, &count);

            if (s8) {
                for (size_t i = 0; i < count && s8 < s8e; ++i) {
                    *s8++ = _buf[i];
                }
            }
            _buf_len += count;
        }

        *buf_len = _buf_len;
        if (s8 && s8 - buf == _buf_len) {
            return UCR_OK;
        }
        return UCR_BUFFER;
    }

    int utf16_to_utf8(const std::u16string_view& sv, char* buf, size_t* buf_len) {
        return utf16_to_utf8(sv.data(), sv.size(), buf, buf_len);
    }

    bool utf16_to_utf8(const char16_t* src, size_t len, std::string* dst) {
        if (!len || !src) {
            dst->clear();
            return true;
        }

        auto s = src;
        auto se = s + len;

        uint32_t scalar_value;
        std::string utf8_string;

        while (s < se) {
            if (!nextsv16(s, se, &scalar_value)) {
                return false;
            }

            char buf[4];
            size_t count;
            sv_to_utf8(scalar_value, buf, &count);
            utf8_string.append(buf, count);
        }

        *dst = std::move(utf8_string);
        return true;
    }

    bool utf16_to_utf8(const std::u16string_view& sv, std::string* dst) {
        return utf16_to_utf8(sv.data(), sv.size(), dst);
    }

    bool utf16_to_utf32(char16_t ch, char32_t* out) {
        if ((ch >= 0x0000 && ch <= 0xD7FF) ||
            (ch >= 0xE000 && ch <= 0xFFFF))
        {
            *out = ch;
            return true;
        }
        return false;
    }

    int utf16_to_utf32(const char16_t* src, size_t len, char32_t* buf, size_t* buf_len) {
        if (!len || !src) {
            *buf_len = 0;
            return UCR_OK;
        }

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto s32 = buf;
        auto s32e = s32 ? (buf + *buf_len) : nullptr;

        uint32_t scalar_value;
        while (s < se) {
            if (!nextsv16(s, se, &scalar_value)) {
                return UCR_FAILED;
            }

            if (s32 && s32 < s32e) {
                *s32++ = scalar_value;
            }
            ++_buf_len;
        }

        *buf_len = _buf_len;
        if (s32 && s32 - buf == _buf_len) {
            return UCR_OK;
        }
        return UCR_BUFFER;
    }

    int utf16_to_utf32(const std::u16string_view& sv, char32_t* buf, size_t* buf_len) {
        return utf16_to_utf32(sv.data(), sv.size(), buf, buf_len);
    }

    bool utf16_to_utf32(const char16_t* src, size_t len, std::u32string* dst) {
        if (!len || !src) {
            dst->clear();
            return true;
        }

        auto s = src;
        auto se = s + len;

        uint32_t scalar_value;
        std::string utf8_string;
        std::u32string utf32_string;

        while (s < se) {
            if (!nextsv16(s, se, &scalar_value)) {
                return false;
            }

            utf32_string.push_back(scalar_value);
        }

        *dst = std::move(utf32_string);
        return true;
    }

    bool utf16_to_utf32(const std::u16string_view& sv, std::u32string* dst) {
        return utf16_to_utf32(sv.data(), sv.size(), dst);
    }

    void utf32_to_utf8(char32_t ch, char* dst, size_t* len) {
        sv_to_utf8(uint32_t(ch), dst, len);
    }

    bool utf32_to_utf8(const char32_t* src, size_t len, char* buf, size_t* buf_len) {
        if (!len || !src) {
            *buf_len = 0;
            return true;
        }

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto s8 = buf;
        auto s8e = s8 ? (buf + *buf_len) : nullptr;

        char _buf[4];
        size_t count;
        for (; s < se; ++s) {
            sv_to_utf8(*s, _buf, &count);
            if (s8) {
                for (size_t i = 0; i < count && s8 < s8e; ++i) {
                    *s8++ = _buf[i];
                }
            }
            _buf_len += count;
        }

        *buf_len = _buf_len;
        return s8 && s8 - buf == _buf_len;
    }

    bool utf32_to_utf8(const std::u32string_view& sv, char* buf, size_t* buf_len) {
        return utf32_to_utf8(sv.data(), sv.size(), buf, buf_len);
    }

    void utf32_to_utf8(const char32_t* src, size_t len, std::string* dst) {
        dst->clear();
        if (!len || !src) {
            return;
        }

        auto s = src;
        auto se = s + len;

        char buf[4];
        size_t count;
        for (; s < se; ++s) {
            sv_to_utf8(*s, buf, &count);
            dst->append(buf, count);
        }
    }

    void utf32_to_utf8(const std::u32string_view& sv, std::string* dst) {
        utf32_to_utf8(sv.data(), sv.size(), dst);
    }

    void utf32_to_utf16(char32_t ch, char16_t* dst, size_t* len) {
        sv_to_utf16(uint32_t(ch), dst, len);
    }

    bool utf32_to_utf16(const char32_t* src, size_t len, char16_t* buf, size_t* buf_len) {
        if (!len || !src) {
            *buf_len = 0;
            return true;
        }

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto s16 = buf;
        auto s16e = s16 ? (buf + *buf_len) : nullptr;

        char16_t _buf[2];
        size_t count;
        for (; s < se; ++s) {
            sv_to_utf16(*s, _buf, &count);
            if (s16) {
                for (size_t i = 0; i < count && s16 < s16e; ++i) {
                    *s16++ = _buf[i];
                }
            }
            _buf_len += count;
        }

        *buf_len = _buf_len;
        return s16 && s16 - buf == _buf_len;
    }

    bool utf32_to_utf16(const std::u32string_view& sv, char16_t* buf, size_t* buf_len) {
        return utf32_to_utf16(sv.data(), sv.size(), buf, buf_len);
    }

    void utf32_to_utf16(const char32_t* src, size_t len, std::u16string* dst) {
        dst->clear();
        if (!len || !src) {
            return;
        }

        auto s = src;
        auto se = s + len;

        char16_t buf[2];
        size_t count;
        for (; s < se; ++s) {
            sv_to_utf16(*s, buf, &count);
            dst->append(buf, count);
        }
    }

    void utf32_to_utf16(const std::u32string_view& sv, std::u16string* dst) {
        utf32_to_utf16(sv.data(), sv.size(), dst);
    }

}