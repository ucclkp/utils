// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utfccpp.h"

#include <cassert>


#define IN_RANGE(var, min, max)  \
    (var) >= (min) && (var) <= (max)

#define CHECK_LENGTH(l)  \
    if (se - s < (l))

#define CHECK_RANGE(var, min, max)  \
    if (!(IN_RANGE(var, min, max)))

// D90
#define IS_ILL_U32CU(ch)  (IS_SURROGATES(ch) || ch > 0x10FFFFu)


namespace {

    int sv16_to_utf8(uint_fast16_t sv, char buf[4], size_t* len) {
        size_t buf_len = *len;
        if ((sv & 0xFF80) == 0) {
            if (buf && buf_len >= 1) {
                buf[0] = (char)(sv & 0x7F);
            }
            *len = 1;
        } else if ((sv & 0xF800) == 0) {
            if (buf && buf_len >= 2) {
                buf[0] = (char)(((sv & 0x7C0) >> 6) + 0xC0);
                buf[1] = (char)((sv & 0x3F) + 0x80);
            }
            *len = 2;
        } else {
            if (buf && buf_len >= 3) {
                buf[0] = (char)(((sv & 0xF000) >> 12) + 0xE0);
                buf[1] = (char)(((sv & 0xFC0) >> 6) + 0x80);
                buf[2] = (char)((sv & 0x3F) + 0x80);
            }
            *len = 3;
        }
        
        if (buf_len < *len) {
            return UTFC_BUF;
        }
        return 0;
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

    static inline int nextsv8_ava(const char* s, const char* se, uint_fast32_t* sv, size_t* adv) {
        unsigned char b1, b2, b3, b4;

        b1 = (unsigned char)*s;
        if (b1 <= 0x7F) {
            *sv = b1;
            *adv = 1;
        } else if (b1 <= 0xC1) {
            return UTFC_ERR;
        } else if (b1 <= 0xDF) {
            CHECK_LENGTH(2) { return UTFC_ERR; }
            b2 = (unsigned char)*++s;
            CHECK_RANGE(b2, 0x80, 0xBF) { return UTFC_ERR; }
            *sv = sv8(b1, b2);
            *adv = 2;
        } else if (b1 <= 0xEF) {
            CHECK_LENGTH(3) { return UTFC_ERR; }
            b2 = (unsigned char)*++s;
            CHECK_RANGE(b2,
                0x80u + ((unsigned char)(b1 == 0xE0u) << 5u),  // b1 == 0xE0 ? 0xA0 : 0x80
                0x9Fu + ((unsigned char)(b1 != 0xEDu) << 5u))  // b1 == 0xED ? 0x9F : 0xBF
            {
                return UTFC_ERR;
            }
            b3 = (unsigned char)*++s;
            CHECK_RANGE(b3, 0x80, 0xBF) { return UTFC_ERR; }
            *sv = sv8(b1, b2, b3);
            *adv = 3;
        } else if (b1 <= 0xF4) {
            CHECK_LENGTH(4) { return UTFC_ERR; }
            b2 = (unsigned char)*++s;
            CHECK_RANGE(b2,
                0x80u + ((unsigned char)(b1 == 0xF0u) << 4u),        // b1 == 0xF0 ? 0x90 : 0x80
                0x8Fu + (((unsigned char)(b1 != 0xF4u) * 3) << 4u))  // b1 == 0xF4 ? 0x8F : 0xBF
            {
                return UTFC_ERR;
            }
            b3 = (unsigned char)*++s;
            CHECK_RANGE(b3, 0x80, 0xBF) { return UTFC_ERR; }
            b4 = (unsigned char)*++s;
            CHECK_RANGE(b4, 0x80, 0xBF) { return UTFC_ERR; }
            *sv = sv8(b1, b2, b3, b4);
            *adv = 4;
        } else {
            return UTFC_ERR;
        }
        return 0;
    }

    static inline int nextsv16c_ava(const char16_t* s, const char16_t* se, uint_fast32_t* sv, size_t* adv) {
        char16_t w1, w2;

        w1 = *s;
        if (IS_START_SURROGATES(w1)) {
            CHECK_LENGTH(2) { return UTFC_ERR; }
            w2 = *++s;
            if (!IS_END_SURROGATES(w2)) {
                return UTFC_ERR;
            }
            *sv = sv16((uint_fast16_t)w1, (uint_fast16_t)w2);
            *adv = 2;
        } else if (IS_END_SURROGATES(w1)) {
            return UTFC_ERR;
        } else {
            *sv = (uint_fast32_t)w1;
            *adv = 1;
        }
        return 0;
    }

    static inline int nextsv16w_ava(const wchar_t* s, const wchar_t* se, uint_fast32_t* sv, size_t* adv) {
        uint_fast16_t w1, w2;

        w1 = (uint_fast16_t)*s;
        if (IS_START_SURROGATES(w1)) {
            CHECK_LENGTH(2) { return UTFC_ERR; }
            w2 = (uint_fast16_t) * ++s;
            if (!IS_END_SURROGATES(w2)) {
                return UTFC_ERR;
            }
            *sv = sv16(w1, w2);
            *adv = 2;
        } else if (IS_END_SURROGATES(w1)) {
            return UTFC_ERR;
        } else {
            *sv = w1;
            *adv = 1;
        }
        return 0;
    }

}

namespace utl {

    int utf8_substr(const std::string_view& sv, size_t* sub_len, int flags) {
        return ::utf8_substr(sv.data(), sv.size(), sub_len, flags);
    }

    int utf16_substr(const std::u16string_view& sv, size_t* sub_len, int flags) {
        return ::utf16_substr(sv.data(), sv.size(), sub_len, flags);
    }

    int utf32_substr(const std::u32string_view& sv, size_t* sub_len, int flags) {
        return ::utf32_substr(sv.data(), sv.size(), sub_len, flags);
    }

    int wchar_substr(const std::wstring_view& sv, size_t* sub_len, int flags) {
        return ::wchar_substr(sv.data(), sv.size(), sub_len, flags);
    }


    int utf8_to_utf16(const std::string_view& sv, char16_t* buf, size_t* buf_len, int flags) {
        return ::utf8_to_utf16(sv.data(), sv.size(), buf, buf_len, flags);
    }

    int utf8_to_utf16(const char* src, size_t len, std::u16string* dst, int flags) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;

        size_t adv;
        char16_t _buf[2];
        uint_fast32_t scalar_value;
        std::u16string u16str;

        while (s < se) {
            if (nextsv8_ava(s, se, &scalar_value, &adv) != 0) {
                if (flags & UTFCF_CHK) {
                    return UTFC_ERR;
                } else if (flags & UTFCF_IGN) {
                    u16str.push_back((unsigned char)*s);
                } else {
                    u16str.push_back(UTFC_INVALID_CHAR);
                }
                ++s;
            } else {
                s += adv;
                adv = sv32_to_utf16_ava(scalar_value, _buf);
                u16str.append(_buf, adv);
            }
        }

        *dst = std::move(u16str);
        return 0;
    }

    int utf8_to_utf16(const std::string_view& sv, std::u16string* dst, int flags) {
        return utf8_to_utf16(sv.data(), sv.size(), dst, flags);
    }

    int utf8_to_utf32(const std::string_view& sv, char32_t* buf, size_t* buf_len, int flags) {
        return ::utf8_to_utf32(sv.data(), sv.size(), buf, buf_len, flags);
    }

    int utf8_to_utf32(const char* src, size_t len, std::u32string* dst, int flags) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;

        size_t adv;
        uint_fast32_t scalar_value;
        std::u32string u32str;

        while (s < se) {
            if (nextsv8_ava(s, se, &scalar_value, &adv) != 0) {
                if (flags & UTFCF_CHK) {
                    return UTFC_ERR;
                } else if (flags & UTFCF_IGN) {
                    u32str.push_back((unsigned char)*s);
                } else {
                    u32str.push_back(UTFC_INVALID_CHAR);
                }
                ++s;
            } else {
                s += adv;
                u32str.push_back(scalar_value);
            }
        }

        *dst = std::move(u32str);
        return 0;
    }

    int utf8_to_utf32(const std::string_view& sv, std::u32string* dst, int flags) {
        return utf8_to_utf32(sv.data(), sv.size(), dst, flags);
    }

    int utf16_to_utf8(const std::u16string_view& sv, char* buf, size_t* buf_len, int flags) {
        return ::utf16_to_utf8(sv.data(), sv.size(), buf, buf_len, flags);
    }

    int utf16_to_utf8(const char16_t* src, size_t len, std::string* dst, int flags) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;

        size_t adv;
        std::string u8str;
        uint_fast32_t sv;

        while (s < se) {
            if (nextsv16c_ava(s, se, &sv, &adv) != 0) {
                if (flags & UTFCF_CHK) {
                    return UTFC_ERR;
                } else if (flags & UTFCF_IGN) {
                    adv = 1;
                    sv = (uint_fast32_t)*s;
                } else {
                    adv = 1;
                    sv = UTFC_INVALID_CHAR;
                }
            }
            s += adv;

            char buf[4];
            size_t count = sv32_to_utf8_ava(sv, buf);
            u8str.append(buf, count);
        }

        *dst = std::move(u8str);
        return 0;
    }

    int utf16_to_utf8(const std::u16string_view& sv, std::string* dst, int flags) {
        return utf16_to_utf8(sv.data(), sv.size(), dst, flags);
    }

    int utf16_to_utf32(const std::u16string_view& sv, char32_t* buf, size_t* buf_len, int flags) {
        return ::utf16_to_utf32(sv.data(), sv.size(), buf, buf_len, flags);
    }

    int utf16_to_utf32(const char16_t* src, size_t len, std::u32string* dst, int flags) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;

        size_t adv;
        std::u32string u32str;
        uint_fast32_t sv;

        while (s < se) {
            if (nextsv16c_ava(s, se, &sv, &adv) != 0) {
                if (flags & UTFCF_CHK) {
                    return UTFC_ERR;
                } else if (flags & UTFCF_IGN) {
                    u32str.push_back((char32_t)*s);
                } else {
                    u32str.push_back(UTFC_INVALID_CHAR);
                }
                ++s;
            } else {
                s += adv;
                u32str.push_back(sv);
            }
        }

        *dst = std::move(u32str);
        return 0;
    }

    int utf16_to_utf32(const std::u16string_view& sv, std::u32string* dst, int flags) {
        return utf16_to_utf32(sv.data(), sv.size(), dst, flags);
    }

    int utf32_to_utf8(const std::u32string_view& sv, char* buf, size_t* buf_len, int flags) {
        return ::utf32_to_utf8(sv.data(), sv.size(), buf, buf_len, flags);
    }

    int utf32_to_utf8(const char32_t* src, size_t len, std::string* dst, int flags) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;
        std::string u8str;
        uint_fast32_t ch;

        char buf[4];
        size_t count;
        for (; s < se; ++s) {
            ch = (uint_fast32_t)*s;
            if (IS_ILL_U32CU(ch)) {
                if (flags & UTFCF_CHK) {
                    return UTFC_ERR;
                } else if (flags & UTFCF_IGN) {
                } else {
                    ch = UTFC_INVALID_CHAR;
                }
            }
            count = sv32_to_utf8_ava(ch, buf);
            u8str.append(buf, count);
        }

        *dst = std::move(u8str);
        return 0;
    }

    int utf32_to_utf8(const std::u32string_view& sv, std::string* dst, int flags) {
        return utf32_to_utf8(sv.data(), sv.size(), dst, flags);
    }

    int utf32_to_utf16(const std::u32string_view& sv, char16_t* buf, size_t* buf_len, int flags) {
        return ::utf32_to_utf16(sv.data(), sv.size(), buf, buf_len, flags);
    }

    int utf32_to_utf16(const char32_t* src, size_t len, std::u16string* dst, int flags) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;
        std::u16string u16s;
        uint_fast32_t ch;

        char16_t buf[2];
        size_t count;
        for (; s < se; ++s) {
            ch = (uint_fast32_t)*s;
            if (IS_ILL_U32CU(ch)) {
                if (flags & UTFCF_CHK) {
                    return UTFC_ERR;
                } else if (flags & UTFCF_IGN) {
                } else {
                    ch = UTFC_INVALID_CHAR;
                }
            }
            count = sv32_to_utf16_ava(ch, buf);
            u16s.append(buf, count);
        }
        *dst = std::move(u16s);
        return 0;
    }

    int utf32_to_utf16(const std::u32string_view& sv, std::u16string* dst, int flags) {
        return utf32_to_utf16(sv.data(), sv.size(), dst, flags);
    }

    int wchar_to_utf8(const std::wstring_view& sv, char* buf, size_t* buf_len, int flags) {
        return ::wchar_to_utf8(sv.data(), sv.size(), buf, buf_len, flags);
    }

    int wchar_to_utf8(const wchar_t* src, size_t len, std::string* dst, int flags) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;
        size_t adv;
        std::string u8s;
        uint_fast32_t sv;
        char _buf[4];

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            while (s < se) {
                if (nextsv16w_ava(s, se, &sv, &adv) != 0) {
                    if (flags & UTFCF_CHK) {
                        return UTFC_ERR;
                    } else if (flags & UTFCF_IGN) {
                        adv = 1;
                        sv = (uint_fast32_t)*s;
                    } else {
                        adv = 1;
                        sv = UTFC_INVALID_CHAR;
                    }
                }
                s += adv;
                adv = sv32_to_utf8_ava(sv, _buf);
                u8s.append(_buf, adv);
            }
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            for (; s < se; ++s) {
                sv = (uint_fast32_t)*s;
                if (IS_ILL_U32CU(sv)) {
                    if (flags & UTFCF_CHK) {
                        return UTFC_ERR;
                    } else if (flags & UTFCF_IGN) {
                    } else {
                        sv = UTFC_INVALID_CHAR;
                    }
                }
                adv = sv32_to_utf8_ava(*s, _buf);
                u8s.append(_buf, adv);
            }
        } else {
            assert(false);
            return UTFC_ERR;
        }

        *dst = std::move(u8s);
        return 0;
    }

    int wchar_to_utf8(const std::wstring_view& sv, std::string* dst, int flags) {
        return wchar_to_utf8(sv.data(), sv.size(), dst, flags);
    }

    int wchar_to_utf16(const std::wstring_view& sv, char16_t* buf, size_t* buf_len, int flags) {
        return ::wchar_to_utf16(sv.data(), sv.size(), buf, buf_len, flags);
    }

    int wchar_to_utf16(const wchar_t* src, size_t len, std::u16string* dst, int flags) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;
        std::u16string out;
        char16_t ch16, ch16_2;

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            if (flags & UTFCF_CHK) {
                while (s < se) {
                    // D91
                    ch16 = (char16_t)*s++;
                    if (IS_START_SURROGATES(ch16)) {
                        if (s >= se) return UTFC_ERR;
                        out.push_back(ch16);
                        ch16 = (char16_t)*s++;
                        if (!IS_END_SURROGATES(ch16)) {
                            return UTFC_ERR;
                        }
                    } else if (IS_END_SURROGATES(ch16)) {
                        return UTFC_ERR;
                    }

                    out.push_back(ch16);
                }
            } else if (flags & UTFCF_IGN) {
                while (s < se) {
                    out.push_back((char16_t)*s++);
                }
            } else {
                while (s < se) {
                    // D91
                    ch16 = (char16_t)*s++;
                    if (IS_START_SURROGATES(ch16)) {
                        if (s >= se) {
                            out.push_back(UTFC_INVALID_CHAR);
                            break;
                        }
                        ch16_2 = (char16_t)*s;
                        if (!IS_END_SURROGATES(ch16_2)) {
                            out.push_back(UTFC_INVALID_CHAR);
                            continue;
                        }
                        ++s;
                        out.push_back(ch16);
                        out.push_back(ch16_2);
                    } else if (IS_END_SURROGATES(ch16)) {
                        out.push_back(UTFC_INVALID_CHAR);
                    } else {
                        out.push_back(ch16);
                    }
                }
            }
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            char16_t buf[2];
            size_t count;
            for (; s < se; ++s) {
                count = sv32_to_utf16_ava(*s, buf);
                out.append(buf, count);
            }
        } else {
            assert(false);
            return UTFC_ERR;
        }
        *dst = std::move(out);
        return 0;
    }

    int wchar_to_utf16(const std::wstring_view& sv, std::u16string* dst, int flags) {
        return wchar_to_utf16(sv.data(), sv.size(), dst, flags);
    }

    int wchar_to_utf32(const std::wstring_view& sv, char32_t* buf, size_t* buf_len, int flags) {
        return ::wchar_to_utf32(sv.data(), sv.size(), buf, buf_len, flags);
    }

    int wchar_to_utf32(const wchar_t* src, size_t len, std::u32string* dst, int flags) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;
        size_t adv;
        std::u32string out;
        uint_fast32_t sv;

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            while (s < se) {
                if (nextsv16w_ava(s, se, &sv, &adv) != 0) {
                    if (flags & UTFCF_CHK) {
                        return UTFC_ERR;
                    } else if (flags & UTFCF_IGN) {
                        out.push_back((char32_t)*s);
                    } else {
                        out.push_back(UTFC_INVALID_CHAR);
                    }
                    ++s;
                } else {
                    s += adv;
                    out.push_back(sv);
                }
            }
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            if (flags & UTFCF_CHK) {
                while (s < se) {
                    sv = (uint_fast32_t)*s++;
                    if (IS_ILL_U32CU(sv)) {
                        return UTFC_ERR;
                    }
                    out.push_back((char32_t)sv);
                }
            } else if (flags & UTFCF_IGN) {
                while (s < se) {
                    out.push_back((char32_t)*s++);
                }
            } else {
                while (s < se) {
                    sv = (uint_fast32_t)*s++;
                    if (IS_ILL_U32CU(sv)) {
                        sv = UTFC_INVALID_CHAR;
                    }
                    out.push_back((char32_t)sv);
                }
            }
        } else {
            assert(false);
            return UTFC_ERR;
        }

        *dst = std::move(out);
        return 0;
    }

    int wchar_to_utf32(const std::wstring_view& sv, std::u32string* dst, int flags) {
        return wchar_to_utf32(sv.data(), sv.size(), dst, flags);
    }

    int utf8_to_wchar(const std::string_view& sv, wchar_t* buf, size_t* buf_len, int flags) {
        return ::utf8_to_wchar(sv.data(), sv.size(), buf, buf_len, flags);
    }

    int utf8_to_wchar(const char* src, size_t len, std::wstring* dst, int flags) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;
        size_t adv;
        uint_fast32_t sv;
        std::wstring out;
        char16_t buf[2];

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            while (s < se) {
                if (nextsv8_ava(s, se, &sv, &adv) != 0) {
                    if (flags & UTFCF_CHK) {
                        return UTFC_ERR;
                    } else if (flags & UTFCF_IGN) {
                        out.push_back((unsigned char)*s);
                    } else {
                        out.push_back((wchar_t)UTFC_INVALID_CHAR);
                    }
                    ++s;
                } else {
                    s += adv;
                    adv = sv32_to_utf16_ava(sv, buf);
                    for (size_t i = 0; i < adv; ++i) {
                        out.push_back(buf[i]);
                    }
                }
            }
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            while (s < se) {
                if (nextsv8_ava(s, se, &sv, &adv) != 0) {
                    if (flags & UTFCF_CHK) {
                        return UTFC_ERR;
                    } else if (flags & UTFCF_IGN) {
                        out.push_back((unsigned char)*s);
                    } else {
                        out.push_back((wchar_t)UTFC_INVALID_CHAR);
                    }
                    ++s;
                } else {
                    s += adv;
                    out.push_back(sv);
                }
            }
        } else {
            assert(false);
            return UTFC_ERR;
        }

        *dst = std::move(out);
        return 0;
    }

    int utf8_to_wchar(const std::string_view& sv, std::wstring* dst, int flags) {
        return utf8_to_wchar(sv.data(), sv.size(), dst, flags);
    }

    int utf16_to_wchar(const std::u16string_view& sv, wchar_t* buf, size_t* buf_len, int flags) {
        return ::utf16_to_wchar(sv.data(), sv.size(), buf, buf_len, flags);
    }

    int utf16_to_wchar(const char16_t* src, size_t len, std::wstring* dst, int flags) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;
        size_t adv;
        wchar_t w16, w16_2;
        std::wstring out;
        uint_fast32_t sv;

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            if (flags & UTFCF_CHK) {
                while (s < se) {
                    // D91
                    w16 = (wchar_t)*s++;
                    if (IS_START_SURROGATES(w16)) {
                        if (s >= se) return UTFC_ERR;
                        out.push_back(w16);
                        w16 = (wchar_t)*s++;
                        if (!IS_END_SURROGATES(w16)) {
                            return UTFC_ERR;
                        }
                    } else if (IS_END_SURROGATES(w16)) {
                        return UTFC_ERR;
                    }

                    out.push_back(w16);
                }
            } else if (flags & UTFCF_IGN) {
                while (s < se) {
                    out.push_back((wchar_t)*s++);
                }
            } else {
                while (s < se) {
                    // D91
                    w16 = (wchar_t)*s++;
                    if (IS_START_SURROGATES(w16)) {
                        if (s >= se) {
                            out.push_back((wchar_t)UTFC_INVALID_CHAR);
                            break;
                        }
                        w16_2 = (wchar_t)*s;
                        if (!IS_END_SURROGATES(w16_2)) {
                            out.push_back((wchar_t)UTFC_INVALID_CHAR);
                            continue;
                        }
                        ++s;
                        out.push_back(w16);
                        out.push_back(w16_2);
                    } else if (IS_END_SURROGATES(w16)) {
                        out.push_back((wchar_t)UTFC_INVALID_CHAR);
                    } else {
                        out.push_back(w16);
                    }
                }
            }
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            while (s < se) {
                if (nextsv16c_ava(s, se, &sv, &adv) != 0) {
                    if (flags & UTFCF_CHK) {
                        return UTFC_ERR;
                    } else if (flags & UTFCF_IGN) {
                        out.push_back((wchar_t)*s);
                    } else {
                        out.push_back((wchar_t)UTFC_INVALID_CHAR);
                    }
                    ++s;
                } else {
                    s += adv;
                    out.push_back(sv);
                }
            }
        } else {
            assert(false);
            return UTFC_ERR;
        }

        *dst = std::move(out);
        return 0;
    }

    int utf16_to_wchar(const std::u16string_view& sv, std::wstring* dst, int flags) {
        return utf16_to_wchar(sv.data(), sv.size(), dst, flags);
    }

    int utf32_to_wchar(const std::u32string_view& sv, wchar_t* buf, size_t* buf_len, int flags) {
        return ::utf32_to_wchar(sv.data(), sv.size(), buf, buf_len, flags);
    }

    int utf32_to_wchar(const char32_t* src, size_t len, std::wstring* dst, int flags) {
        assert(src && dst);

        auto s = src;
        auto se = s + len;
        std::wstring out;
        char32_t ch;

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            char16_t buf[2];
            size_t count;
            for (; s < se; ++s) {
                ch = *s;
                if (IS_ILL_U32CU(ch)) {
                    if (flags & UTFCF_CHK) {
                        return UTFC_ERR;
                    } else if (flags & UTFCF_IGN) {
                    } else {
                        ch = UTFC_INVALID_CHAR;
                    }
                }
                count = sv32_to_utf16_ava(ch, buf);
                for (size_t i = 0; i < count; ++i) {
                    out.push_back((wchar_t)buf[i]);
                }
            }
        } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
            if (flags & UTFCF_CHK) {
                while (s < se) {
                    ch = *s++;
                    if (IS_ILL_U32CU(ch)) return UTFC_ERR;
                    out.push_back((wchar_t)ch);
                }
            } else if (flags & UTFCF_IGN) {
                while (s < se) {
                    out.push_back((wchar_t)*s++);
                }
            } else {
                while (s < se) {
                    ch = *s++;
                    if (IS_ILL_U32CU(ch)) ch = UTFC_INVALID_CHAR;
                    out.push_back((wchar_t)ch);
                }
            }
        } else {
            assert(false);
            return UTFC_ERR;
        }

        *dst = std::move(out);
        return 0;
    }

    int utf32_to_wchar(const std::u32string_view& sv, std::wstring* dst, int flags) {
        return utf32_to_wchar(sv.data(), sv.size(), dst, flags);
    }

}