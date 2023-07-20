// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "unicode_conv.h"

#include <cassert>


#define IN_RANGE(var, min, max)  \
    (var) >= (min) && (var) <= (max)

#define CHECK_LENGTH(l)  \
    if (se - s < (l)) { return false; }

#define CHECK_IN_RANGE(var, min, max)  \
    if (!(IN_RANGE(var, min, max))) { return false; }

#define GET_BYTE(no)  \
    auto byte##no = static_cast<uint_fast8_t>(*(s + (no) - 1))

#define GET_BYTE_AND_CHECK_RANGE(no, min, max)  \
    GET_BYTE(no);  \
    CHECK_IN_RANGE(byte##no, min, max)

#define GET_WORD(no)  \
    auto word##no = static_cast<uint_fast16_t>(*(s + (no) - 1))

#define GET_WORD_AND_CHECK_RANGE(no, min, max)  \
    GET_WORD(no);  \
    CHECK_IN_RANGE(word##no, min, max)


namespace {

    bool sv16_to_utf8(uint_fast16_t sv, char buf[4], size_t* len) {
        if ((sv & 0xFF80) == 0) {
            // 1 byte
            if (buf && *len >= 1) {
                buf[0] = (char)(sv & 0x7F);
                *len = 1;
            } else {
                *len = 1;
                return false;
            }
        } else if ((sv & 0xF800) == 0) {
            // 2 byte
            if (buf && *len >= 2) {
                buf[0] = (char)(((sv & 0x7C0) >> 6) + 0xC0);
                buf[1] = (char)((sv & 0x3F) + 0x80);
                *len = 2;
            } else {
                *len = 2;
                return false;
            }
        } else {
            // 3 byte
            if (buf && *len >= 3) {
                buf[0] = (char)(((sv & 0xF000) >> 12) + 0xE0);
                buf[1] = (char)(((sv & 0xFC0) >> 6) + 0x80);
                buf[2] = (char)((sv & 0x3F) + 0x80);
                *len = 3;
            } else {
                *len = 3;
                return false;
            }
        }
        return true;
    }

    bool sv32_to_utf8(uint_fast32_t sv, char buf[4], size_t* len) {
        if ((sv & 0xFFFFFF80) == 0) {
            // 1 byte
            if (buf && *len >= 1) {
                buf[0] = (char)(sv & 0x7F);
                *len = 1;
            } else {
                *len = 1;
                return false;
            }
        } else if ((sv & 0xFFFFF800) == 0) {
            // 2 byte
            if (buf && *len >= 2) {
                buf[0] = (char)(((sv & 0x7C0) >> 6) + 0xC0);
                buf[1] = (char)((sv & 0x3F) + 0x80);
                *len = 2;
            } else {
                *len = 2;
                return false;
            }
        } else if ((sv & 0xFFFF0000) == 0) {
            // 3 byte
            if (buf && *len >= 3) {
                buf[0] = (char)(((sv & 0xF000) >> 12) + 0xE0);
                buf[1] = (char)(((sv & 0xFC0) >> 6) + 0x80);
                buf[2] = (char)((sv & 0x3F) + 0x80);
                *len = 3;
            } else {
                *len = 3;
                return false;
            }
        } else {
            // 4 byte
            if (buf && *len >= 4) {
                buf[0] = (char)(((sv & 0x1C0000) >> 18) + 0xF0);
                buf[1] = (char)(((sv & 0x3F000) >> 12) + 0x80);
                buf[2] = (char)(((sv & 0xFC0) >> 6) + 0x80);
                buf[3] = (char)((sv & 0x3F) + 0x80);
                *len = 4;
            } else {
                *len = 4;
                return false;
            }
        }
        return true;
    }

    inline size_t sv16_to_utf8_len(uint_fast16_t sv) {
        if ((sv & 0xFF80) == 0) {
            // 1 byte
            return 1;
        } else if ((sv & 0xF800) == 0) {
            // 2 byte
            return 2;
        } else {
            // 3 byte
            return 3;
        }
    }

    inline size_t sv16_to_utf8_ava(uint_fast16_t sv, char buf[3]) {
        if ((sv & 0xFF80) == 0) {
            // 1 byte
            buf[0] = (char)(sv & 0x7F);
            return 1;
        } else if ((sv & 0xF800) == 0) {
            // 2 byte
            buf[0] = (char)(((sv & 0x7C0) >> 6) + 0xC0);
            buf[1] = (char)((sv & 0x3F) + 0x80);
            return 2;
        } else {
            // 3 byte
            buf[0] = (char)(((sv & 0xF000) >> 12) + 0xE0);
            buf[1] = (char)(((sv & 0xFC0) >> 6) + 0x80);
            buf[2] = (char)((sv & 0x3F) + 0x80);
            return 3;
        }
    }

    inline size_t sv32_to_utf8_len(uint_fast32_t sv) {
        if ((sv & 0xFFFFFF80) == 0) {
            // 1 byte
            return 1;
        } else if ((sv & 0xFFFFF800) == 0) {
            // 2 byte
            return 2;
        } else if ((sv & 0xFFFF0000) == 0) {
            // 3 byte
            return 3;
        } else {
            // 4 byte
            return 4;
        }
    }

    inline size_t sv32_to_utf8_ava(uint_fast32_t sv, char buf[4]) {
        if ((sv & 0xFFFFFF80) == 0) {
            // 1 byte
            buf[0] = (char)(sv & 0x7F);
            return 1;
        } else if ((sv & 0xFFFFF800) == 0) {
            // 2 byte
            buf[0] = (char)(((sv & 0x7C0) >> 6) + 0xC0);
            buf[1] = (char)((sv & 0x3F) + 0x80);
            return 2;
        } else if ((sv & 0xFFFF0000) == 0) {
            // 3 byte
            buf[0] = (char)(((sv & 0xF000) >> 12) + 0xE0);
            buf[1] = (char)(((sv & 0xFC0) >> 6) + 0x80);
            buf[2] = (char)((sv & 0x3F) + 0x80);
            return 3;
        } else {
            // 4 byte
            buf[0] = (char)(((sv & 0x1C0000) >> 18) + 0xF0);
            buf[1] = (char)(((sv & 0x3F000) >> 12) + 0x80);
            buf[2] = (char)(((sv & 0xFC0) >> 6) + 0x80);
            buf[3] = (char)((sv & 0x3F) + 0x80);
            return 4;
        }
    }

    bool sv32_to_utf16(uint_fast32_t sv, char16_t buf[2], size_t* len) {
        if ((sv & 0xFFFF0000) == 0) {
            // 1 word
            if (buf && *len >= 1) {
                buf[0] = (char16_t)(sv & 0xFFFF);
                *len = 1;
            } else {
                *len = 1;
                return false;
            }
        } else {
            // 2 word
            if (buf && *len >= 2) {
                buf[0] = (char16_t)(
                    ((sv & 0xFC00) >> 10) +
                    ((((sv & 0x1F0000) - 1) & 0xF0000) >> 10) + 0xD800);
                buf[1] = (char16_t)((sv & 0x3FF) + 0xDC00);
                *len = 2;
            } else {
                *len = 2;
                return false;
            }
        }
        return true;
    }

    inline size_t sv32_to_utf16_len(uint_fast32_t sv) {
        if ((sv & 0xFFFF0000) == 0) {
            // 1 word
            return 1;
        } else {
            // 2 word
            return 2;
        }
    }

    inline size_t sv32_to_utf16_ava(uint_fast32_t sv, char16_t buf[2]) {
        if ((sv & 0xFFFF0000) == 0) {
            // 1 word
            buf[0] = (char16_t)sv;
            return 1;
        } else {
            // 2 word
            buf[0] = (char16_t)(
                ((sv & 0xFC00) >> 10) +
                ((((sv & 0x1F0000) - 1) & 0xF0000) >> 10) + 0xD800);
            buf[1] = (char16_t)((sv & 0x3FF) + 0xDC00);
            return 2;
        }
    }

    inline uint_fast16_t sv8(uint_fast8_t byte1, uint_fast8_t byte2) {
        return ((uint_fast16_t)(byte1 & 0x1F) << 6) + (byte2 & 0x3F);
    }

    inline uint_fast16_t sv8(uint_fast8_t byte1, uint_fast8_t byte2, uint_fast8_t byte3) {
        uint_fast16_t result = (uint_fast16_t)(byte1 & 0xF) << 12;
        result += (uint_fast16_t)(byte2 & 0x3F) << 6;
        result += (byte3 & 0x3F);
        return result;
    }

    inline uint_fast32_t sv8(uint_fast8_t byte1, uint_fast8_t byte2, uint_fast8_t byte3, uint_fast8_t byte4) {
        uint_fast32_t result = uint_fast32_t(byte1 & 0x7) << 18;
        result += uint_fast32_t(byte2 & 0x3F) << 12;
        result += uint_fast32_t(byte3 & 0x3F) << 6;
        result += (byte4 & 0x3F);
        return result;
    }

    inline uint_fast32_t sv16(uint_fast16_t word1, uint_fast16_t word2) {
        return (uint_fast32_t((word1 & 0x3C0) + 0x40) << 10)
            + (uint_fast32_t(word1 & 0x3F) << 10)
            + (word2 & 0x3FF);
    }

    template <typename Cy>
    bool nextsv8_ava(const Cy*& s, const Cy* se, uint_fast32_t* sv) {
        GET_BYTE(1);
        if (IN_RANGE(byte1, 0x00, 0x7F)) {   // 1 byte
            *sv = byte1;
            ++s;
        } else if (IN_RANGE(byte1, 0xC2, 0xDF)) { // 2 byte
            CHECK_LENGTH(2);
            GET_BYTE_AND_CHECK_RANGE(2, 0x80, 0xBF);
            *sv = sv8(byte1, byte2);
            s += 2;
        } else if (IN_RANGE(byte1, 0xE0, 0xEF)) { // 3 byte
            CHECK_LENGTH(3);
            GET_BYTE_AND_CHECK_RANGE(2,
                0x80u + ((uint_fast8_t)(byte1 == 0xE0u) << 5u),  // byte1 == 0xE0 ? 0xA0 : 0x80
                0x9Fu + ((uint_fast8_t)(byte1 != 0xEDu) << 5u)); // byte1 == 0xED ? 0x9F : 0xBF
            GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF);
            *sv = sv8(byte1, byte2, byte3);
            s += 3;
        } else if (IN_RANGE(byte1, 0xF0, 0xF4)) { // 4 byte
            CHECK_LENGTH(4);
            GET_BYTE_AND_CHECK_RANGE(2,
                0x80u + ((uint_fast8_t)(byte1 == 0xF0u) << 4u),        // byte1 == 0xF0 ? 0x90 : 0x80
                0x8Fu + (((uint_fast8_t)(byte1 != 0xF4u) * 3) << 4u)); // byte1 == 0xF4 ? 0x8F : 0xBF
            GET_BYTE_AND_CHECK_RANGE(3, 0x80, 0xBF);
            GET_BYTE_AND_CHECK_RANGE(4, 0x80, 0xBF);
            *sv = sv8(byte1, byte2, byte3, byte4);
            s += 4;
        } else {
            return false;
        }

        return true;
    }

    template <typename Cy>
    bool nextsv8_adv(const Cy*& s, const Cy* se) {
        GET_BYTE(1);
        if (IN_RANGE(byte1, 0x00, 0x7F)) {   // 1 byte
            ++s;
        } else if (IN_RANGE(byte1, 0xC2, 0xDF)) { // 2 byte
            CHECK_LENGTH(2);
            s += 2;
        } else if (IN_RANGE(byte1, 0xE0, 0xEF)) { // 3 byte
            CHECK_LENGTH(3);
            s += 3;
        } else if (IN_RANGE(byte1, 0xF0, 0xF4)) { // 4 byte
            CHECK_LENGTH(4);
            s += 4;
        } else {
            return false;
        }

        return true;
    }

    template <typename Cy>
    bool nextsv16_ava(const Cy*& s, const Cy* se, uint_fast32_t* sv) {
        GET_WORD(1);
        if (IS_SURROGATES(word1)) { // 2 word
            CHECK_LENGTH(2);
            GET_WORD(2);
            *sv = sv16(word1, word2);
            s += 2;
        } else {                    // 1 word
            *sv = word1;
            ++s;
        }
        return true;
    }

    template <typename Cy>
    bool nextsv16_adv(const Cy*& s, const Cy* se) {
        GET_WORD(1);
        if (IS_SURROGATES(word1)) { // 2 word
            CHECK_LENGTH(2);
            s += 2;
        } else {                    // 1 word
            ++s;
        }
        return true;
    }

}

namespace utl {

    bool utf8_substr(const char* src, size_t len, size_t* sub_len) {
        assert(src && sub_len);
        size_t hc = *sub_len;
        if (hc >= len) {
            *sub_len = len;
            return true;
        }

        auto s = src;
        auto se = s + len;

        for (size_t i = 0; i < hc && s < se; ++i) {
            if (!nextsv8_adv(s, se)) {
                return false;
            }
        }

        *sub_len = (size_t)(s - src);
        return true;
    }

    bool utf8_substr(const std::string_view& sv, size_t* sub_len) {
        return utf8_substr(sv.data(), sv.size(), sub_len);
    }

    bool utf16_substr(const char16_t* src, size_t len, size_t* sub_len) {
        assert(src && sub_len);
        size_t hc = *sub_len;
        if (hc >= len) {
            *sub_len = len;
            return true;
        }

        auto s = src;
        auto se = s + len;

        for (size_t i = 0; i < hc && s < se; ++i) {
            if (IS_SURROGATES(*s++)) {
                if (s >= se) return false;
                ++s;
            }
        }

        *sub_len = (size_t)(s - src);
        return true;
    }

    bool utf16_substr(const std::u16string_view& sv, size_t* sub_len) {
        return utf16_substr(sv.data(), sv.size(), sub_len);
    }

    void utf32_substr(const char32_t* src, size_t len, size_t* sub_len) {
        assert(src && sub_len);
        if (*sub_len >= len) {
            *sub_len = len;
        }
    }

    void utf32_substr(const std::u32string_view& sv, size_t* sub_len) {
        utf32_substr(sv.data(), sv.size(), sub_len);
    }

    bool wchar_substr(const wchar_t* src, size_t len, size_t* sub_len) {
        assert(src && sub_len);

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            size_t hc = *sub_len;
            if (hc >= len) {
                *sub_len = len;
                return true;
            }

            auto s = src;
            auto se = s + len;

            for (size_t i = 0; i < hc && s < se; ++i) {
                if (IS_SURROGATES(*s++)) {
                    if (s >= se) return false;
                    ++s;
                }
            }

            *sub_len = (size_t)(s - src);
            return true;
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            if (*sub_len >= len) {
                *sub_len = len;
            }
            return true;
        } else {
            assert(false);
            return false;
        }
    }

    bool wchar_substr(const std::wstring_view& sv, size_t* sub_len) {
        return wchar_substr(sv.data(), sv.size(), sub_len);
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
        assert(src && buf_len);

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto s16 = buf;
        auto s16e = s16 ? (buf + *buf_len) : nullptr;

        uint_fast32_t scalar_value;
        while (s < se) {
            if (!nextsv8_ava(s, se, &scalar_value)) {
                return SCR_FAIL;
            }

            char16_t _buf[2];
            size_t count = sv32_to_utf16_ava(scalar_value, _buf);

            if (s16) {
                for (size_t i = 0; i < count && s16 < s16e; ++i) {
                    *s16++ = _buf[i];
                }
            }
            _buf_len += count;
        }

        *buf_len = _buf_len;
        return s16 && s16 - buf == _buf_len ? SCR_OK : SCR_BUF;
    }

    int utf8_to_utf16(const std::string_view& sv, char16_t* buf, size_t* buf_len) {
        return utf8_to_utf16(sv.data(), sv.size(), buf, buf_len);
    }

    bool utf8_to_utf16(const char* src, size_t len, std::u16string* dst) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;

        uint_fast32_t scalar_value;
        std::u16string u16str;

        while (s < se) {
            if (!nextsv8_ava(s, se, &scalar_value)) {
                return false;
            }

            char16_t buf[2];
            size_t count = sv32_to_utf16_ava(scalar_value, buf);
            u16str.append(buf, count);
        }

        *dst = std::move(u16str);
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
        assert(src && buf_len);

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto s32 = buf;
        auto s32e = s32 ? (buf + *buf_len) : nullptr;

        uint_fast32_t scalar_value;
        while (s < se) {
            if (!nextsv8_ava(s, se, &scalar_value)) {
                return SCR_FAIL;
            }

            if (s32 && s32 < s32e) {
                *s32++ = scalar_value;
            }
            ++_buf_len;
        }

        *buf_len = _buf_len;
        return s32 && s32 - buf == _buf_len ? SCR_OK : SCR_BUF;
    }

    int utf8_to_utf32(const std::string_view& sv, char32_t* buf, size_t* buf_len) {
        return utf8_to_utf32(sv.data(), sv.size(), buf, buf_len);
    }

    bool utf8_to_utf32(const char* src, size_t len, std::u32string* dst) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;

        uint_fast32_t scalar_value;
        std::u32string u32str;

        while (s < se) {
            if (!nextsv8_ava(s, se, &scalar_value)) {
                return false;
            }

            u32str.push_back(scalar_value);
        }

        *dst = std::move(u32str);
        return true;
    }

    bool utf8_to_utf32(const std::string_view& sv, std::u32string* dst) {
        return utf8_to_utf32(sv.data(), sv.size(), dst);
    }


    bool utf16_to_utf8(char16_t ch, char* dst, size_t* len) {
        if (IS_SURROGATES(ch)) {
            return false;
        }
        return sv16_to_utf8((uint_fast32_t)ch, dst, len);
    }

    int utf16_to_utf8(const char16_t* src, size_t len, char* buf, size_t* buf_len) {
        assert(src && buf_len);

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto s8 = buf;
        auto s8e = s8 ? (buf + *buf_len) : nullptr;

        uint_fast32_t scalar_value;
        while (s < se) {
            if (!nextsv16_ava(s, se, &scalar_value)) {
                return SCR_FAIL;
            }

            char _buf[4];
            size_t count = sv32_to_utf8_ava(scalar_value, _buf);

            if (s8) {
                for (size_t i = 0; i < count && s8 < s8e; ++i) {
                    *s8++ = _buf[i];
                }
            }
            _buf_len += count;
        }

        *buf_len = _buf_len;
        return s8 && s8 - buf == _buf_len ? SCR_OK : SCR_BUF;
    }

    int utf16_to_utf8(const std::u16string_view& sv, char* buf, size_t* buf_len) {
        return utf16_to_utf8(sv.data(), sv.size(), buf, buf_len);
    }

    bool utf16_to_utf8(const char16_t* src, size_t len, std::string* dst) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;

        uint_fast32_t scalar_value;
        std::string u8str;

        while (s < se) {
            if (!nextsv16_ava(s, se, &scalar_value)) {
                return false;
            }

            char buf[4];
            size_t count = sv32_to_utf8_ava(scalar_value, buf);
            u8str.append(buf, count);
        }

        *dst = std::move(u8str);
        return true;
    }

    bool utf16_to_utf8(const std::u16string_view& sv, std::string* dst) {
        return utf16_to_utf8(sv.data(), sv.size(), dst);
    }


    bool utf16_to_utf32(char16_t ch, char32_t* out) {
        if (IS_SURROGATES(ch)) {
            return false;
        }
        *out = ch;
        return true;
    }

    int utf16_to_utf32(const char16_t* src, size_t len, char32_t* buf, size_t* buf_len) {
        assert(src && buf_len);

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto s32 = buf;
        auto s32e = s32 ? (buf + *buf_len) : nullptr;

        uint_fast32_t scalar_value;
        while (s < se) {
            if (!nextsv16_ava(s, se, &scalar_value)) {
                return SCR_FAIL;
            }

            if (s32 && s32 < s32e) {
                *s32++ = scalar_value;
            }
            ++_buf_len;
        }

        *buf_len = _buf_len;
        return s32 && s32 - buf == _buf_len ? SCR_OK : SCR_BUF;
    }

    int utf16_to_utf32(const std::u16string_view& sv, char32_t* buf, size_t* buf_len) {
        return utf16_to_utf32(sv.data(), sv.size(), buf, buf_len);
    }

    bool utf16_to_utf32(const char16_t* src, size_t len, std::u32string* dst) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;

        uint_fast32_t scalar_value;
        std::u32string u32str;

        while (s < se) {
            if (!nextsv16_ava(s, se, &scalar_value)) {
                return false;
            }

            u32str.push_back(scalar_value);
        }

        *dst = std::move(u32str);
        return true;
    }

    bool utf16_to_utf32(const std::u16string_view& sv, std::u32string* dst) {
        return utf16_to_utf32(sv.data(), sv.size(), dst);
    }


    bool utf32_to_utf8(char32_t ch, char* dst, size_t* len) {
        return sv32_to_utf8((uint_fast32_t)ch, dst, len);
    }

    bool utf32_to_utf8(const char32_t* src, size_t len, char* buf, size_t* buf_len) {
        assert(src && buf_len);

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto s8 = buf;
        auto s8e = s8 ? (buf + *buf_len) : nullptr;

        char _buf[4];
        size_t count;
        for (; s < se; ++s) {
            count = sv32_to_utf8_ava(*s, _buf);
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
        assert(src && dst);

        auto s = src;
        auto se = s + len;
        std::string u8str;

        char buf[4];
        size_t count;
        for (; s < se; ++s) {
            count = sv32_to_utf8_ava(*s, buf);
            u8str.append(buf, count);
        }

        *dst = std::move(u8str);
    }

    void utf32_to_utf8(const std::u32string_view& sv, std::string* dst) {
        utf32_to_utf8(sv.data(), sv.size(), dst);
    }


    bool utf32_to_utf16(char32_t ch, char16_t* dst, size_t* len) {
        return sv32_to_utf16((uint_fast32_t)ch, dst, len);
    }

    bool utf32_to_utf16(const char32_t* src, size_t len, char16_t* buf, size_t* buf_len) {
        assert(src && buf_len);

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto s16 = buf;
        auto s16e = s16 ? (buf + *buf_len) : nullptr;

        char16_t _buf[2];
        size_t count;
        for (; s < se; ++s) {
            count = sv32_to_utf16_ava(*s, _buf);
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
        assert(src && dst);

        auto s = src;
        auto se = s + len;
        std::u16string u16s;

        char16_t buf[2];
        size_t count;
        for (; s < se; ++s) {
            count = sv32_to_utf16_ava(*s, buf);
            u16s.append(buf, count);
        }
        *dst = std::move(u16s);
    }

    void utf32_to_utf16(const std::u32string_view& sv, std::u16string* dst) {
        utf32_to_utf16(sv.data(), sv.size(), dst);
    }


    int wchar_to_utf8(wchar_t ch, char* dst, size_t* len) {
        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            if (IS_SURROGATES(ch)) return SCR_FAIL;
            return sv16_to_utf8((uint_fast16_t)ch, dst, len) ? SCR_OK : SCR_BUF;
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            return sv32_to_utf8((uint_fast32_t)ch, dst, len) ? SCR_OK : SCR_BUF;
        } else {
            assert(false);
            return SCR_FAIL;
        }
    }

    int wchar_to_utf8(const wchar_t* src, size_t len, char* buf, size_t* buf_len) {
        assert(src && buf_len);

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto s8 = buf;
        auto s8e = s8 ? (buf + *buf_len) : nullptr;

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            uint_fast32_t scalar_value;
            while (s < se) {
                if (!nextsv16_ava(s, se, &scalar_value)) {
                    return SCR_FAIL;
                }

                char _buf[4];
                size_t count = sv32_to_utf8_ava(scalar_value, _buf);

                if (s8) {
                    for (size_t i = 0; i < count && s8 < s8e; ++i) {
                        *s8++ = _buf[i];
                    }
                }
                _buf_len += count;
            }
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            char _buf[4];
            size_t count;
            for (; s < se; ++s) {
                count = sv32_to_utf8_ava(*s, _buf);
                if (s8) {
                    for (size_t i = 0; i < count && s8 < s8e; ++i) {
                        *s8++ = _buf[i];
                    }
                }
                _buf_len += count;
            }
        } else {
            assert(false);
            return SCR_FAIL;
        }

        *buf_len = _buf_len;
        return s8 && s8 - buf == _buf_len ? SCR_OK : SCR_BUF;
    }

    int wchar_to_utf8(const std::wstring_view& sv, char* buf, size_t* buf_len) {
        return wchar_to_utf8(sv.data(), sv.size(), buf, buf_len);
    }

    bool wchar_to_utf8(const wchar_t* src, size_t len, std::string* dst) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;
        std::string u8s;

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            char buf[4];
            size_t count;

            while (s < se) {
                auto ch = *s++;
                if (IS_SURROGATES(ch)) {
                    // 2 word
                    if (s >= se) return false;
                    count = sv32_to_utf8_ava(sv16(ch, *s++), buf);
                } else {
                    // 1 word
                    count = sv16_to_utf8_ava(ch, buf);
                }
                u8s.append(buf, count);
            }

            *dst = std::move(u8s);
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            char buf[4];
            size_t count;
            for (; s < se; ++s) {
                count = sv32_to_utf8_ava(*s, buf);
                u8s.append(buf, count);
            }
            *dst = std::move(u8s);
        } else {
            assert(false);
            return false;
        }
        return true;
    }

    bool wchar_to_utf8(const std::wstring_view& sv, std::string* dst) {
        return wchar_to_utf8(sv.data(), sv.size(), dst);
    }


    int wchar_to_utf16(wchar_t ch, char16_t* dst, size_t* len) {
        assert(len);

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            if (dst && *len >= 1) {
                *dst = (char16_t)ch;
                *len = 1;
                return SCR_OK;
            }
            *len = 1;
            return SCR_BUF;
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            return sv32_to_utf16((uint_fast32_t)ch, dst, len) ? SCR_OK : SCR_BUF;
        } else {
            assert(false);
            return SCR_FAIL;
        }
    }

    int wchar_to_utf16(const wchar_t* src, size_t len, char16_t* buf, size_t* buf_len) {
        assert(src && buf_len);

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto s16 = buf;
        auto s16e = s16 ? (buf + *buf_len) : nullptr;

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            if (s16) {
                while ( s < se && s16 < s16e) {
                    *s16++ = *s++;
                }
            }
            _buf_len = len;
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            char16_t _buf[2];
            size_t count;
            for (; s < se; ++s) {
                count = sv32_to_utf16_ava(*s, _buf);
                if (s16) {
                    for (size_t i = 0; i < count && s16 < s16e; ++i, ++s16) {
                        *s16 = _buf[i];
                    }
                }
                _buf_len += count;
            }
        } else {
            assert(false);
            return SCR_FAIL;
        }

        *buf_len = _buf_len;
        return s16 && s16 - buf == _buf_len ? SCR_OK : SCR_BUF;
    }

    int wchar_to_utf16(const std::wstring_view& sv, char16_t* buf, size_t* buf_len) {
        return wchar_to_utf16(sv.data(), sv.size(), buf, buf_len);
    }

    bool wchar_to_utf16(const wchar_t* src, size_t len, std::u16string* dst) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;
        std::u16string out;

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            out.resize(len);
            for (size_t i = 0; i < len; ++i) {
                out[i] = (char16_t)*s++;
            }
            *dst = std::move(out);
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            char16_t buf[2];
            size_t count;
            for (; s < se; ++s) {
                count = sv32_to_utf16_ava(*s, buf);
                out.append(buf, count);
            }
            *dst = std::move(out);
        } else {
            assert(false);
            return false;
        }
        return true;
    }

    bool wchar_to_utf16(const std::wstring_view& sv, std::u16string* dst) {
        return wchar_to_utf16(sv.data(), sv.size(), dst);
    }


    bool wchar_to_utf32(wchar_t ch, char32_t* out) {
        assert(out);

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            if (!IS_SURROGATES(ch)) {
                *out = ch;
                return true;
            }
            return false;
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            *out = ch;
            return true;
        } else {
            assert(false);
            return false;
        }
    }

    int wchar_to_utf32(const wchar_t* src, size_t len, char32_t* buf, size_t* buf_len) {
        assert(src && buf_len);

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto s32 = buf;
        auto s32e = s32 ? (buf + *buf_len) : nullptr;

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            uint_fast32_t scalar_value;
            while (s < se) {
                if (!nextsv16_ava(s, se, &scalar_value)) {
                    return SCR_FAIL;
                }

                if (s32 && s32 < s32e) {
                    *s32++ = scalar_value;
                }
                ++_buf_len;
            }
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            if (s32) {
                while (s < se && s32 < s32e) {
                    *s32++ = *s++;
                }
            }
            _buf_len = len;
        } else {
            assert(false);
            return SCR_FAIL;
        }

        *buf_len = _buf_len;
        return s32 && s32 - buf == _buf_len ? SCR_OK : SCR_BUF;
    }

    int wchar_to_utf32(const std::wstring_view& sv, char32_t* buf, size_t* buf_len) {
        return wchar_to_utf32(sv.data(), sv.size(), buf, buf_len);
    }

    bool wchar_to_utf32(const wchar_t* src, size_t len, std::u32string* dst) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;
        std::u32string out;

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            uint_fast32_t scalar_value;
            while (s < se) {
                if (!nextsv16_ava(s, se, &scalar_value)) {
                    return false;
                }
                out.push_back(scalar_value);
            }
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            out.resize(len);
            for (size_t i = 0; i < len; ++i) {
                out[i] = *s++;
            }
        } else {
            assert(false);
            return false;
        }

        *dst = std::move(out);
        return true;
    }

    bool wchar_to_utf32(const std::wstring_view& sv, std::u32string* dst) {
        return wchar_to_utf32(sv.data(), sv.size(), dst);
    }


    bool utf8_to_wchar(char ch, wchar_t* out) {
        if (sizeof(wchar_t) * CHAR_BIT == 16u ||
            sizeof(wchar_t) * CHAR_BIT == 32u)
        {
            if (ch >= 0x00 && ch <= 0x7F) {
                *out = ch;
                return true;
            }
        }
        return false;
    }

    int utf8_to_wchar(const char* src, size_t len, wchar_t* buf, size_t* buf_len) {
        assert(src && buf_len);

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto sw = buf;
        auto swe = sw ? (buf + *buf_len) : nullptr;

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            uint_fast32_t scalar_value;
            while (s < se) {
                if (!nextsv8_ava(s, se, &scalar_value)) {
                    return SCR_FAIL;
                }

                char16_t _buf[2];
                size_t count = sv32_to_utf16_ava(scalar_value, _buf);

                if (sw) {
                    for (size_t i = 0; i < count && sw < swe; ++i) {
                        *sw++ = _buf[i];
                    }
                }
                _buf_len += count;
            }
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            uint_fast32_t scalar_value;
            while (s < se) {
                if (!nextsv8_ava(s, se, &scalar_value)) {
                    return SCR_FAIL;
                }

                if (sw && sw < swe) {
                    *sw++ = scalar_value;
                }
                ++_buf_len;
            }
        } else {
            assert(false);
            return SCR_FAIL;
        }

        *buf_len = _buf_len;
        return sw && sw - buf == _buf_len ? SCR_OK : SCR_BUF;
    }

    int utf8_to_wchar(const std::string_view& sv, wchar_t* buf, size_t* buf_len) {
        return utf8_to_wchar(sv.data(), sv.size(), buf, buf_len);
    }

    bool utf8_to_wchar(const char* src, size_t len, std::wstring* dst) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;
        uint_fast32_t scalar_value;
        std::wstring out;

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            while (s < se) {
                if (!nextsv8_ava(s, se, &scalar_value)) {
                    return false;
                }

                char16_t buf[2];
                size_t count = sv32_to_utf16_ava(scalar_value, buf);
                for (size_t i = 0; i < count; ++i) {
                    out.push_back(buf[i]);
                }
            }
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            while (s < se) {
                if (!nextsv8_ava(s, se, &scalar_value)) {
                    return false;
                }
                out.push_back(scalar_value);
            }
        } else {
            assert(false);
            return false;
        }

        *dst = std::move(out);
        return true;
    }

    bool utf8_to_wchar(const std::string_view& sv, std::wstring* dst) {
        return utf8_to_wchar(sv.data(), sv.size(), dst);
    }


    bool utf16_to_wchar(char16_t ch, wchar_t* out) {
        if (sizeof(wchar_t) * CHAR_BIT == 16u ||
            sizeof(wchar_t) * CHAR_BIT == 32u)
        {
            if (IS_SURROGATES(ch)) {
                return false;
            }
            *out = ch;
            return true;
        }
        return false;
    }

    int utf16_to_wchar(const char16_t* src, size_t len, wchar_t* buf, size_t* buf_len) {
        assert(src && buf_len);

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto sw = buf;
        auto swe = sw ? (buf + *buf_len) : nullptr;

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            if (sw) {
                while (s < se && sw < swe) {
                    *sw++ = *s++;
                }
            }
            _buf_len = len;
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            uint_fast32_t scalar_value;
            while (s < se) {
                if (!nextsv16_ava(s, se, &scalar_value)) {
                    return SCR_FAIL;
                }

                if (sw && sw < swe) {
                    *sw++ = scalar_value;
                }
                ++_buf_len;
            }
        } else {
            assert(false);
            return SCR_FAIL;
        }

        *buf_len = _buf_len;
        return sw && sw - buf == _buf_len ? SCR_OK : SCR_BUF;
    }

    int utf16_to_wchar(const std::u16string_view& sv, wchar_t* buf, size_t* buf_len) {
        return utf16_to_wchar(sv.data(), sv.size(), buf, buf_len);
    }

    bool utf16_to_wchar(const char16_t* src, size_t len, std::wstring* dst) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;
        std::wstring out;

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            out.resize(len);
            for (size_t i = 0; i < len; ++i) {
                out[i] = *s++;
            }
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            uint_fast32_t scalar_value;
            while (s < se) {
                if (!nextsv16_ava(s, se, &scalar_value)) {
                    return false;
                }
                out.push_back(scalar_value);
            }
        } else {
            assert(false);
            return false;
        }

        *dst = std::move(out);
        return true;
    }

    bool utf16_to_wchar(const std::u16string_view& sv, std::wstring* dst) {
        return utf16_to_wchar(sv.data(), sv.size(), dst);
    }


    int utf32_to_wchar(char32_t ch, wchar_t* dst, size_t* len) {
        assert(len);

        bool is_ok = true;
        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            char16_t buf[2];
            if (!sv32_to_utf16((uint_fast32_t)ch, dst ? buf : nullptr, len)) {
                return SCR_BUF;
            }
            if (dst) {
                for (size_t i = 0; i < *len; ++i) {
                    dst[i] = (wchar_t)buf[i];
                }
            } else {
                is_ok = false;
            }
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            if (dst && *len > 0) {
                *dst = (wchar_t)ch;
            } else {
                is_ok = false;
            }
            *len = 1;
        } else {
            assert(false);
            return SCR_FAIL;
        }
        return is_ok ? SCR_OK : SCR_BUF;
    }

    int utf32_to_wchar(const char32_t* src, size_t len, wchar_t* buf, size_t* buf_len) {
        assert(src && buf_len);

        auto s = src;
        auto se = s + len;
        size_t _buf_len = 0;

        auto sw = buf;
        auto swe = sw ? (buf + *buf_len) : nullptr;

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            char16_t _buf[2];
            size_t count;
            for (; s < se; ++s) {
                count = sv32_to_utf16_ava(*s, _buf);
                if (sw) {
                    for (size_t i = 0; i < count && sw < swe; ++i) {
                        *sw++ = (wchar_t)_buf[i];
                    }
                }
                _buf_len += count;
            }
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            if (sw) {
                while (s < se && sw < swe) {
                    *sw++ = (wchar_t)*s++;
                }
            }
            _buf_len = len;
        } else {
            assert(false);
            return SCR_FAIL;
        }

        *buf_len = _buf_len;
        return sw && sw - buf == _buf_len ? SCR_OK : SCR_BUF;
    }

    int utf32_to_wchar(const std::u32string_view& sv, wchar_t* buf, size_t* buf_len) {
        return utf32_to_wchar(sv.data(), sv.size(), buf, buf_len);
    }

    bool utf32_to_wchar(const char32_t* src, size_t len, std::wstring* dst) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;
        std::wstring out;

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            char16_t buf[2];
            size_t count;
            for (; s < se; ++s) {
                count = sv32_to_utf16_ava(*s, buf);
                for (size_t i = 0; i < count; ++i) {
                    out.push_back((wchar_t)buf[i]);
                }
            }
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            out.resize(len);
            for (size_t i = 0; i < len; ++i) {
                out[i] = (wchar_t)*s++;
            }
        } else {
            assert(false);
            return false;
        }

        *dst = std::move(out);
        return true;
    }

    bool utf32_to_wchar(const std::u32string_view& sv, std::wstring* dst) {
        return utf32_to_wchar(sv.data(), sv.size(), dst);
    }

}