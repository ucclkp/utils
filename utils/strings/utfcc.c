#include "utfcc.h"

#include <assert.h>
#include <limits.h>
#include <string.h>

#define IN_RANGE(var, min, max)  \
    (var) >= (min) && (var) <= (max)

#define CHECK_LENGTH(l)  \
    if (se - s < (l))

#define CHECK_RANGE(var, min, max)  \
    if (!(IN_RANGE(var, min, max)))

// D90
#define IS_ILL_U32CU(ch)  (IS_SURROGATES(ch) || ch > 0x10FFFFu)


static inline int sv16_to_utf8(uint_fast16_t sv, char buf[3], size_t* len) {
    size_t buf_len;

    buf_len = *len;
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

static inline int sv32_to_utf8(uint_fast32_t sv, char buf[4], size_t* len) {
    size_t buf_len;

    buf_len = *len;
    if ((sv & 0xFFFFFF80) == 0) {
        if (buf && buf_len >= 1) {
            buf[0] = (char)(sv & 0x7F);
        }
        *len = 1;
    } else if ((sv & 0xFFFFF800) == 0) {
        if (buf && buf_len >= 2) {
            buf[0] = (char)(((sv & 0x7C0) >> 6) + 0xC0);
            buf[1] = (char)((sv & 0x3F) + 0x80);
        }
        *len = 2;
    } else if ((sv & 0xFFFF0000) == 0) {
        if (buf && buf_len >= 3) {
            buf[0] = (char)(((sv & 0xF000) >> 12) + 0xE0);
            buf[1] = (char)(((sv & 0xFC0) >> 6) + 0x80);
            buf[2] = (char)((sv & 0x3F) + 0x80);
        }
        *len = 3;
    } else {
        if (buf && buf_len >= 4) {
            buf[0] = (char)(((sv & 0x1C0000) >> 18) + 0xF0);
            buf[1] = (char)(((sv & 0x3F000) >> 12) + 0x80);
            buf[2] = (char)(((sv & 0xFC0) >> 6) + 0x80);
            buf[3] = (char)((sv & 0x3F) + 0x80);
        }
        *len = 4;
    }

    if (buf_len < *len) {
        return UTFC_BUF;
    }
    return 0;
}

static inline size_t sv32_to_utf8_len(uint_fast32_t sv) {
    if ((sv & 0xFFFFFF80) == 0) {
        return 1;
    } else if ((sv & 0xFFFFF800) == 0) {
        return 2;
    } else if ((sv & 0xFFFF0000) == 0) {
        return 3;
    } else {
        return 4;
    }
}

static inline int sv32_to_utf16(uint_fast32_t sv, char16_t buf[2], size_t* len) {
    size_t buf_len;

    buf_len = *len;
    if ((sv & 0xFFFF0000) == 0) {
        if (buf && buf_len >= 1) {
            buf[0] = (char16_t)sv;
        }
        *len = 1;
    } else {
        if (buf && buf_len >= 2) {
            buf[0] = (char16_t)(
                ((sv & 0xFC00) >> 10) +
                ((((sv & 0x1F0000) - 1) & 0xF0000) >> 10) + 0xD800);
            buf[1] = (char16_t)((sv & 0x3FF) + 0xDC00);
        }
        *len = 2;
    }

    if (buf_len < *len) {
        return UTFC_BUF;
    }
    return 0;
}

static inline int sv32_to_w16(uint_fast32_t sv, wchar_t buf[2], size_t* len) {
    size_t buf_len;

    buf_len = *len;
    if ((sv & 0xFFFF0000) == 0) {
        if (buf && buf_len >= 1) {
            buf[0] = (wchar_t)sv;
        }
        *len = 1;
    } else {
        if (buf && buf_len >= 2) {
            buf[0] = (wchar_t)(
                ((sv & 0xFC00) >> 10) +
                ((((sv & 0x1F0000) - 1) & 0xF0000) >> 10) + 0xD800);
            buf[1] = (wchar_t)((sv & 0x3FF) + 0xDC00);
        }
        *len = 2;
    }

    if (buf_len < *len) {
        return UTFC_BUF;
    }
    return 0;
}

static inline size_t sv32_to_utf16_len(uint_fast32_t sv) {
    if ((sv & 0xFFFF0000) == 0) {
        return 1;
    } else {
        return 2;
    }
}

static inline uint_fast16_t sv8d(uint_fast8_t byte1, uint_fast8_t byte2) {
    return ((uint_fast16_t)(byte1 & 0x1F) << 6) + (byte2 & 0x3F);
}

static inline uint_fast16_t sv8t(uint_fast8_t byte1, uint_fast8_t byte2, uint_fast8_t byte3) {
    uint_fast16_t result;
    result = (uint_fast16_t)(byte1 & 0xF) << 12;
    result += (uint_fast16_t)(byte2 & 0x3F) << 6;
    result += (byte3 & 0x3F);
    return result;
}

static inline uint_fast32_t sv8q(uint_fast8_t byte1, uint_fast8_t byte2, uint_fast8_t byte3, uint_fast8_t byte4) {
    uint_fast32_t result;
    result = (uint_fast32_t)(byte1 & 0x7) << 18;
    result += (uint_fast32_t)(byte2 & 0x3F) << 12;
    result += (uint_fast32_t)(byte3 & 0x3F) << 6;
    result += (byte4 & 0x3F);
    return result;
}

static inline uint_fast32_t sv16(uint_fast16_t word1, uint_fast16_t word2) {
    return ((uint_fast32_t)((word1 & 0x3C0) + 0x40) << 10)
        + ((uint_fast32_t)(word1 & 0x3F) << 10)
        + (word2 & 0x3FF);
}

static inline int nextsv8(const char* s, const char* se, uint_fast32_t* sv, size_t* adv) {
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
        *sv = sv8d(b1, b2);
        *adv = 2;
    } else if (b1 <= 0xEF) {
        CHECK_LENGTH(3) { return UTFC_ERR; }
        b2 = (unsigned char)*++s;
        CHECK_RANGE(b2,
            0x80u + ((unsigned char)(b1 == 0xE0u) << 5u),  // b1 == 0xE0 ? 0xA0 : 0x80
            0x9Fu + ((unsigned char)(b1 != 0xEDu) << 5u))  // b1 == 0xED ? 0x9F : 0xBF
        { return UTFC_ERR; }
        b3 = (unsigned char)*++s;
        CHECK_RANGE(b3, 0x80, 0xBF) { return UTFC_ERR; }
        *sv = sv8t(b1, b2, b3);
        *adv = 3;
    } else if (b1 <= 0xF4) {
        CHECK_LENGTH(4) { return UTFC_ERR; }
        b2 = (unsigned char)*++s;
        CHECK_RANGE(b2,
            0x80u + ((unsigned char)(b1 == 0xF0u) << 4u),        // b1 == 0xF0 ? 0x90 : 0x80
            0x8Fu + (((unsigned char)(b1 != 0xF4u) * 3) << 4u))  // b1 == 0xF4 ? 0x8F : 0xBF
        { return UTFC_ERR; }
        b3 = (unsigned char)*++s;
        CHECK_RANGE(b3, 0x80, 0xBF) { return UTFC_ERR; }
        b4 = (unsigned char)*++s;
        CHECK_RANGE(b4, 0x80, 0xBF) { return UTFC_ERR; }
        *sv = sv8q(b1, b2, b3, b4);
        *adv = 4;
    } else {
        return UTFC_ERR;
    }
    return 0;
}

static inline int nextsv8_adv(const char* s, const char* se, size_t* adv) {
    unsigned char b1, b2, b3, b4;

    b1 = (unsigned char)*s;
    if (b1 <= 0x7F) {
        *adv = 1;
    } else if (b1 <= 0xC1) {
        return UTFC_ERR;
    } else if (b1 <= 0xDF) {
        CHECK_LENGTH(2) { return UTFC_ERR; }
        b2 = (unsigned char)*++s;
        CHECK_RANGE(b2, 0x80, 0xBF) { return UTFC_ERR; }
        *adv = 2;
    } else if (b1 <= 0xEF) {
        CHECK_LENGTH(3) { return UTFC_ERR; }
        b2 = (unsigned char)*++s;
        CHECK_RANGE(b2,
            0x80u + ((unsigned char)(b1 == 0xE0u) << 5u),  // b1 == 0xE0 ? 0xA0 : 0x80
            0x9Fu + ((unsigned char)(b1 != 0xEDu) << 5u))  // b1 == 0xED ? 0x9F : 0xBF
        { return UTFC_ERR; }
        b3 = (unsigned char)*++s;
        CHECK_RANGE(b3, 0x80, 0xBF) { return UTFC_ERR; }
        *adv = 3;
    } else if (b1 <= 0xF4) {
        CHECK_LENGTH(4) { return UTFC_ERR; }
        b2 = (unsigned char)*++s;
        CHECK_RANGE(b2,
            0x80u + ((unsigned char)(b1 == 0xF0u) << 4u),        // b1 == 0xF0 ? 0x90 : 0x80
            0x8Fu + (((unsigned char)(b1 != 0xF4u) * 3) << 4u))  // b1 == 0xF4 ? 0x8F : 0xBF
        { return UTFC_ERR; }
        b3 = (unsigned char)*++s;
        CHECK_RANGE(b3, 0x80, 0xBF) { return UTFC_ERR; }
        b4 = (unsigned char)*++s;
        CHECK_RANGE(b4, 0x80, 0xBF) { return UTFC_ERR; }
        *adv = 4;
    } else {
        return UTFC_ERR;
    }
    return 0;
}

static inline int nextsv16w(const wchar_t* s, const wchar_t* se, uint_fast32_t* sv, size_t* adv) {
    uint_fast16_t w1, w2;

    w1 = (uint_fast16_t)*s;
    if (IS_START_SURROGATES(w1)) {
        CHECK_LENGTH(2) { return UTFC_ERR; }
        w2 = (uint_fast16_t)*++s;
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

static inline int nextsv16w_adv(const wchar_t* s, const wchar_t* se, size_t* adv) {
    uint_fast16_t w1, w2;

    w1 = (uint_fast16_t)*s;
    if (IS_START_SURROGATES(w1)) {
        CHECK_LENGTH(2) { return UTFC_ERR; }
        w2 = (uint_fast16_t) * ++s;
        if (!IS_END_SURROGATES(w2)) {
            return UTFC_ERR;
        }
        *adv = 2;
    } else if (IS_END_SURROGATES(w1)) {
        return UTFC_ERR;
    } else {
        *adv = 1;
    }
    return 0;
}

static inline int nextsv16c(const char16_t* s, const char16_t* se, uint_fast32_t* sv, size_t* adv) {
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

static inline int nextsv16c_adv(const char16_t* s, const char16_t* se, size_t* adv) {
    char16_t w1, w2;

    w1 = *s;
    if (IS_START_SURROGATES(w1)) {
        CHECK_LENGTH(2) { return UTFC_ERR; }
        w2 = *++s;
        if (!IS_END_SURROGATES(w2)) {
            return UTFC_ERR;
        }
        *adv = 2;
    } else if (IS_END_SURROGATES(w1)) {
        return UTFC_ERR;
    } else {
        *adv = 1;
    }
    return 0;
}


int utf8_substr(const char* src, size_t len, size_t* sub_len, int flags) {
    size_t i, hc, adv;
    const char* s, *se;

    assert(src && sub_len);

    hc = *sub_len;
    s = src;
    se = s + len;

    if (flags & UTFCF_IGN) {
        for (i = 0; i < hc && s < se; ++i) {
            if (nextsv8_adv(s, se, &adv) != 0) {
                adv = 1;
            }
            s += adv;
        }
    } else {
        for (i = 0; i < hc && s < se; ++i) {
            if (nextsv8_adv(s, se, &adv) != 0) {
                return UTFC_ERR;
            }
            s += adv;
        }
    }

    *sub_len = (size_t)(s - src);
    return 0;
}

int utf16_substr(const char16_t* src, size_t len, size_t* sub_len, int flags) {
    size_t hc, i;
    char16_t ch;
    const char16_t* s, *se;

    assert(src && sub_len);

    hc = *sub_len;
    s = src;
    se = s + len;

    if (flags & UTFCF_IGN) {
        for (i = 0; i < hc && s < se; ++i) {
            ch = *s++;
            // D91 检查
            if (IS_START_SURROGATES(ch)) {
                if (s >= se) break;
                ch = *s;
                if (IS_END_SURROGATES(ch)) {
                    ++s;
                }
            }
        }
    } else {
        for (i = 0; i < hc && s < se; ++i) {
            ch = *s++;
            // D91 检查
            if (IS_START_SURROGATES(ch)) {
                if (s >= se) return UTFC_ERR;
                ch = *s++;
                if (!IS_END_SURROGATES(ch)) {
                    return UTFC_ERR;
                }
            } else if (IS_END_SURROGATES(ch)) {
                return UTFC_ERR;
            }
        }
    }

    *sub_len = (size_t)(s - src);
    return 0;
}

int utf32_substr(const char32_t* src, size_t len, size_t* sub_len, int flags) {
    size_t hc, i;
    char32_t ch;
    const char32_t* s, * se;

    assert(src && sub_len);

    hc = *sub_len;
    if (flags & UTFCF_IGN) {
        if (hc > len) {
            *sub_len = len;
        }
        return 0;
    }

    s = src;
    se = s + len;

    for (i = 0; i < hc && s < se; ++i) {
        ch = *s++;
        if (IS_ILL_U32CU(ch)) {
            return UTFC_ERR;
        }
    }

    *sub_len = (size_t)(s - src);
    return 0;
}

int wchar_substr(const wchar_t* src, size_t len, size_t* sub_len, int flags) {
    size_t hc, i;
    uint_fast32_t ch;
    const wchar_t* s, *se;

    assert(src && sub_len);

    hc = *sub_len;
    s = src;
    se = s + len;

    if (sizeof(wchar_t) * CHAR_BIT == 16u) {
        if (flags & UTFCF_IGN) {
            for (i = 0; i < hc && s < se; ++i) {
                ch = (uint_fast16_t)*s++;
                if (IS_START_SURROGATES(ch)) {
                    if (s >= se) break;
                    ch = (uint_fast16_t)*s;
                    if (IS_END_SURROGATES(ch)) {
                        ++s;
                    }
                }
            }
        } else {
            for (i = 0; i < hc && s < se; ++i) {
                ch = (uint_fast16_t)*s++;
                if (IS_START_SURROGATES(ch)) {
                    if (s >= se) return UTFC_ERR;
                    ch = (uint_fast16_t)*s++;
                    if (!IS_END_SURROGATES(ch)) {
                        return UTFC_ERR;
                    }
                } else if (IS_END_SURROGATES(ch)) {
                    return UTFC_ERR;
                }
            }
        }
    } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
        if (flags & UTFCF_IGN) {
            if (hc > len) {
                *sub_len = len;
            }
            return 0;
        }

        for (i = 0; i < hc && s < se; ++i) {
            ch = (uint_fast32_t)*s++;
            if (IS_ILL_U32CU(ch)) {
                return UTFC_ERR;
            }
        }
    } else {
        assert(0);
        return UTFC_ERR;
    }

    *sub_len = (size_t)(s - src);
    return 0;
}


int utf8c_to_utf16(char ch, char16_t* out, int flags) {
    assert(out);

    if (flags & UTFCF_CHK) {
        if ((unsigned char)ch > 0x7F) {
            return UTFC_ERR;
        }
        *out = (unsigned char)ch;
    } else if (flags & UTFCF_IGN) {
        *out = (unsigned char)ch;
    } else {
        if ((unsigned char)ch > 0x7F) {
            *out = UTFC_INVALID_CHAR;
        } else {
            *out = (unsigned char)ch;
        }
    }
    return 0;
}

int utf8c_to_utf32(char ch, char32_t* out, int flags) {
    assert(out);

    if (flags & UTFCF_CHK) {
        if ((unsigned char)ch > 0x7F) {
            return UTFC_ERR;
        }
        *out = (unsigned char)ch;
    } else if (flags & UTFCF_IGN) {
        *out = (unsigned char)ch;
    } else {
        if ((unsigned char)ch > 0x7F) {
            *out = UTFC_INVALID_CHAR;
        } else {
            *out = (unsigned char)ch;
        }
    }
    return 0;
}

int utf16c_to_utf8(char16_t ch, char* dst, size_t* len, int flags) {
    assert(len);

    if (flags & UTFCF_CHK) {
        if (IS_SURROGATES(ch)) {
            return UTFC_ERR;
        }
    } else if (flags & UTFCF_IGN) {
    } else {
        if (IS_SURROGATES(ch)) {
            ch = UTFC_INVALID_CHAR;
        }
    }
    return sv16_to_utf8((uint_fast16_t)ch, dst, len);
}

int utf16c_to_utf32(char16_t ch, char32_t* out, int flags) {
    assert(out);

    if (flags & UTFCF_CHK) {
        if (IS_SURROGATES(ch)) {
            return UTFC_ERR;
        }
    } else if (flags & UTFCF_IGN) {
    } else {
        if (IS_SURROGATES(ch)) {
            ch = UTFC_INVALID_CHAR;
        }
    }
    *out = (char32_t)ch;
    return 0;
}

int utf32c_to_utf8(char32_t ch, char* dst, size_t* len, int flags) {
    assert(len);

    if (flags & UTFCF_CHK) {
        if (IS_ILL_U32CU(ch)) return UTFC_ERR;
    } else if (flags & UTFCF_IGN) {
    } else {
        if (IS_ILL_U32CU(ch)) ch = UTFC_INVALID_CHAR;
    }
    return sv32_to_utf8((uint_fast32_t)ch, dst, len);
}

int utf32c_to_utf16(char32_t ch, char16_t* dst, size_t* len, int flags) {
    assert(len);

    if (flags & UTFCF_CHK) {
        if (IS_ILL_U32CU(ch)) return UTFC_ERR;
    } else if (flags & UTFCF_IGN) {
    } else {
        if (IS_ILL_U32CU(ch)) ch = UTFC_INVALID_CHAR;
    }
    return sv32_to_utf16((uint_fast32_t)ch, dst, len);
}

int wcharc_to_utf8(wchar_t ch, char* dst, size_t* len, int flags) {
    assert(len);

    if (sizeof(wchar_t) * CHAR_BIT == 16u) {
        if (flags & UTFCF_CHK) {
            if (IS_SURROGATES((uint_fast16_t)ch)) return UTFC_ERR;
        } else if (flags & UTFCF_IGN) {
        } else {
            if (IS_SURROGATES((uint_fast16_t)ch)) ch = (wchar_t)UTFC_INVALID_CHAR;
        }
        return sv16_to_utf8((uint_fast16_t)ch, dst, len);
    } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
        if (flags & UTFCF_CHK) {
            if (IS_ILL_U32CU((uint_fast32_t)ch)) return UTFC_ERR;
        } else if (flags & UTFCF_IGN) {
        } else {
            if (IS_ILL_U32CU((uint_fast32_t)ch)) ch = (wchar_t)UTFC_INVALID_CHAR;
        }
        return sv32_to_utf8((uint_fast32_t)ch, dst, len);
    } else {
        assert(0);
        return UTFC_ERR;
    }
}

int wcharc_to_utf16(wchar_t ch, char16_t* dst, size_t* len, int flags) {
    assert(len);

    if (sizeof(wchar_t) * CHAR_BIT == 16u) {
        if (flags & UTFCF_CHK) {
            if (IS_SURROGATES((char16_t)ch)) {
                return UTFC_ERR;
            }
        } else if (flags & UTFCF_IGN) {
        } else {
            if (IS_SURROGATES((char16_t)ch)) {
                ch = (wchar_t)UTFC_INVALID_CHAR;
            }
        }
        if (dst && *len >= 1) {
            *dst = (char16_t)ch;
            *len = 1;
            return 0;
        }
        *len = 1;
        return UTFC_BUF;
    } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
        if (flags & UTFCF_CHK) {
            if (IS_ILL_U32CU((uint_fast32_t)ch)) return UTFC_ERR;
        } else if (flags & UTFCF_IGN) {
        } else {
            if (IS_ILL_U32CU((uint_fast32_t)ch)) ch = (wchar_t)UTFC_INVALID_CHAR;
        }
        return sv32_to_utf16((uint_fast32_t)ch, dst, len);
    } else {
        assert(0);
        return UTFC_ERR;
    }
}

int wcharc_to_utf32(wchar_t ch, char32_t* out, int flags) {
    assert(out);

    if (sizeof(wchar_t) * CHAR_BIT == 16u) {
        if (flags & UTFCF_CHK) {
            if (IS_SURROGATES((char32_t)ch)) {
                return UTFC_ERR;
            }
        } else if (flags & UTFCF_IGN) {
        } else {
            if (IS_SURROGATES((char32_t)ch)) {
                ch = (wchar_t)UTFC_INVALID_CHAR;
            }
        }
        *out = (char32_t)ch;
    } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
        if (flags & UTFCF_CHK) {
            if (IS_ILL_U32CU((char32_t)ch)) return UTFC_ERR;
        } else if (flags & UTFCF_IGN) {
        } else {
            if (IS_ILL_U32CU((char32_t)ch)) ch = (wchar_t)UTFC_INVALID_CHAR;
        }
        *out = (char32_t)ch;
    } else {
        assert(0);
        return UTFC_ERR;
    }
    return 0;
}

int utf8c_to_wchar(char ch, wchar_t* out, int flags) {
    assert(out);

    if (flags & UTFCF_CHK) {
        if ((unsigned char)ch > 0x7F) {
            return UTFC_ERR;
        }
        *out = (wchar_t)ch;
    } else if (flags & UTFCF_IGN) {
        *out = (wchar_t)(unsigned char)ch;
    } else {
        if ((unsigned char)ch > 0x7F) {
            *out = (wchar_t)UTFC_INVALID_CHAR;
        } else {
            *out = (wchar_t)(unsigned char)ch;
        }
    }
    return 0;
}

int utf16c_to_wchar(char16_t ch, wchar_t* out, int flags) {
    assert(out);

    if (sizeof(wchar_t) * CHAR_BIT != 16u &&
        sizeof(wchar_t) * CHAR_BIT != 32u)
    {
        return UTFC_ERR;
    }

    if (flags & UTFCF_CHK) {
        if (IS_SURROGATES(ch)) return UTFC_ERR;
    } else if (flags & UTFCF_IGN) {
    } else {
        if (IS_SURROGATES(ch)) ch = UTFC_INVALID_CHAR;
    }

    *out = (wchar_t)ch;
    return 0;
}

int utf32c_to_wchar(char32_t ch, wchar_t* dst, size_t* len, int flags) {
    int ret;

    assert(len);

    if (flags & UTFCF_CHK) {
        if (IS_ILL_U32CU(ch)) return UTFC_ERR;
    } else if (flags & UTFCF_IGN) {
    } else {
        if (IS_ILL_U32CU(ch)) ch = UTFC_INVALID_CHAR;
    }

    ret = 0;
    if (sizeof(wchar_t) * CHAR_BIT == 16u) {
        if (dst) {
            return sv32_to_w16((uint_fast32_t)ch, dst, len);
        } else {
            *len = sv32_to_utf16_len((uint_fast32_t)ch);
            ret = UTFC_BUF;
        }
    } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
        if (dst && *len > 0) {
            *dst = (wchar_t)ch;
        } else {
            ret = UTFC_BUF;
        }
        *len = 1;
    } else {
        assert(0);
        return UTFC_ERR;
    }

    return ret;
}


int utf8_to_utf16(const char* src, size_t len, char16_t* buf, size_t* buf_len, int flags) {
    const char* s, *se;
    char16_t* s16, *s16e;
    size_t _buf_len, adv;
    uint_fast32_t sv;

    assert(src && buf_len);

    _buf_len = 0;
    s = src;
    se = s + len;

    if (buf) {
        s16 = buf;
        s16e = buf + *buf_len;

        while (s < se) {
            if (nextsv8(s, se, &sv, &adv) != 0) {
                if (flags & UTFCF_CHK) {
                    return UTFC_ERR;
                } else if (flags & UTFCF_IGN) {
                    if (s16 < s16e) *s16++ = (unsigned char)*s;
                } else {
                    if (s16 < s16e) *s16++ = UTFC_INVALID_CHAR;
                }
                ++s;
                ++_buf_len;
            } else {
                s += adv;
                adv = s16e - s16;
                if (sv32_to_utf16(sv, s16, &adv) == 0) {
                    s16 += adv;
                } else {
                    s16 = s16e;
                }
                _buf_len += adv;
            }
        }
        *buf_len = _buf_len;
        if (s16 - buf == _buf_len) {
            return 0;
        }
    } else {
        while (s < se) {
            if (nextsv8(s, se, &sv, &adv) != 0) {
                if (flags & UTFCF_CHK) {
                    return UTFC_ERR;
                }
                ++s;
                ++_buf_len;
            } else {
                s += adv;
                _buf_len += sv32_to_utf16_len(sv);
            }
        }
        *buf_len = _buf_len;
    }
    return UTFC_BUF;
}

int utf8_to_utf32(const char* src, size_t len, char32_t* buf, size_t* buf_len, int flags) {
    const char* s, *se;
    char32_t* s32, *s32e;
    size_t _buf_len, adv;
    uint_fast32_t sv;

    assert(src && buf_len);

    s = src;
    se = s + len;
    _buf_len = 0;

    if (buf) {
        s32 = buf;
        s32e = buf + *buf_len;

        while (s < se) {
            if (nextsv8(s, se, &sv, &adv) != 0) {
                if (flags & UTFCF_CHK) {
                    return UTFC_ERR;
                } else if (flags & UTFCF_IGN) {
                    if (s32 < s32e) *s32++ = (unsigned char)*s;
                } else {
                    if (s32 < s32e) *s32++ = UTFC_INVALID_CHAR;
                }
                ++s;
            } else {
                s += adv;
                if (s32 < s32e) *s32++ = sv;
            }
            ++_buf_len;
        }
        *buf_len = _buf_len;
        if (s32 - buf == _buf_len) {
            return 0;
        }
    } else {
        while (s < se) {
            if (nextsv8_adv(s, se, &adv) != 0) {
                if (flags & UTFCF_CHK) {
                    return UTFC_ERR;
                }
                ++s;
            } else {
                s += adv;
            }
            ++_buf_len;
        }
        *buf_len = _buf_len;
    }
    return UTFC_BUF;
}

int utf16_to_utf8(const char16_t* src, size_t len, char* buf, size_t* buf_len, int flags) {
    const char16_t* s, *se;
    char* s8, *s8e;
    size_t _buf_len, adv;
    uint_fast32_t sv;

    assert(src && buf_len);

    s = src;
    se = s + len;
    _buf_len = 0;

    if (buf) {
        s8 = buf;
        s8e = buf + *buf_len;

        while (s < se) {
            if (nextsv16c(s, se, &sv, &adv) != 0) {
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
            adv = s8e - s8;
            if (sv32_to_utf8(sv, s8, &adv) == 0) {
                s8 += adv;
            } else {
                s8 = s8e;
            }
            _buf_len += adv;
        }
        *buf_len = _buf_len;
        if (s8 - buf == _buf_len) return 0;
    } else {
        while (s < se) {
            if (nextsv16c(s, se, &sv, &adv) != 0) {
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
            _buf_len += sv32_to_utf8_len(sv);
        }
        *buf_len = _buf_len;
    }

    return UTFC_BUF;
}

int utf16_to_utf32(const char16_t* src, size_t len, char32_t* buf, size_t* buf_len, int flags) {
    const char16_t* s, *se;
    char32_t* s32, *s32e;
    size_t _buf_len, adv;
    uint_fast32_t sv;

    assert(src && buf_len);

    s = src;
    se = s + len;
    _buf_len = 0;

    if (buf) {
        s32 = buf;
        s32e = buf + *buf_len;

        while (s < se) {
            if (nextsv16c(s, se, &sv, &adv) != 0) {
                if (flags & UTFCF_CHK) {
                    return UTFC_ERR;
                } else if (flags & UTFCF_IGN) {
                    if (s32 < s32e) *s32++ = (char32_t)*s;
                } else {
                    if (s32 < s32e) *s32++ = UTFC_INVALID_CHAR;
                }
                ++s;
            } else {
                s += adv;
                if (s32 < s32e) *s32++ = sv;
            }
            ++_buf_len;
        }
        *buf_len = _buf_len;
        if (s32 - buf == _buf_len) return 0;
    } else {
        while (s < se) {
            if (nextsv16c_adv(s, se, &adv) != 0) {
                if (flags & UTFCF_CHK) {
                    return UTFC_ERR;
                }
                ++s;
            } else {
                s += adv;
            }
            ++_buf_len;
        }

        *buf_len = _buf_len;
    }

    return UTFC_BUF;
}

int utf32_to_utf8(const char32_t* src, size_t len, char* buf, size_t* buf_len, int flags) {
    const char32_t* s, *se;
    char* s8, *s8e;
    size_t _buf_len, adv;
    uint_fast32_t ch;

    assert(src && buf_len);

    s = src;
    se = s + len;
    _buf_len = 0;

    if (buf) {
        s8 = buf;
        s8e = buf + *buf_len;
        
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
            adv = s8e - s8;
            if (sv32_to_utf8(ch, s8, &adv) == 0) {
                s8 += adv;
            } else {
                s8 = s8e;
            }
            _buf_len += adv;
        }
        *buf_len = _buf_len;
        if (s8 - buf == _buf_len) return 0;
    } else {
        while (s < se) {
            ch = (uint_fast32_t)*s++;
            if (IS_ILL_U32CU(ch)) {
                if (flags & UTFCF_CHK) {
                    return UTFC_ERR;
                } else if (flags & UTFCF_IGN) {
                } else {
                    ch = UTFC_INVALID_CHAR;
                }
            }
            _buf_len += sv32_to_utf8_len(ch);
        }
        *buf_len = _buf_len;
    }

    return UTFC_BUF;
}

int utf32_to_utf16(const char32_t* src, size_t len, char16_t* buf, size_t* buf_len, int flags) {
    const char32_t* s, *se;
    char16_t* s16, *s16e;
    size_t _buf_len, adv;
    uint_fast32_t ch;

    assert(src && buf_len);

    s = src;
    se = s + len;
    _buf_len = 0;

    if (buf) {
        s16 = buf;
        s16e = buf + *buf_len;
        
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
            adv = s16e - s16;
            if (sv32_to_utf16(ch, s16, &adv) == 0) {
                s16 += adv;
            } else {
                s16 = s16e;
            }
            _buf_len += adv;
        }

        *buf_len = _buf_len;
        if (s16 - buf == _buf_len) return 0;
    } else {
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
            _buf_len += sv32_to_utf16_len(ch);
        }
        *buf_len = _buf_len;
    }

    return UTFC_BUF;
}

int wchar_to_utf8(const wchar_t* src, size_t len, char* buf, size_t* buf_len, int flags) {
    const wchar_t* s, *se;
    char* s8, *s8e;
    size_t _buf_len, adv;
    uint_fast32_t sv;

    assert(src && buf_len);

    s = src;
    se = s + len;
    _buf_len = 0;

    if (sizeof(wchar_t) * CHAR_BIT == 16u) {
        if (buf) {
            s8 = buf;
            s8e = buf + *buf_len;
            while (s < se) {
                if (nextsv16w(s, se, &sv, &adv) != 0) {
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
                adv = s8e - s8;
                if (sv32_to_utf8(sv, s8, &adv) == 0) {
                    s8 += adv;
                } else {
                    s8 = s8e;
                }
                _buf_len += adv;
            }
            *buf_len = _buf_len;
            if (s8 - buf == _buf_len) return 0;
        } else {
            while (s < se) {
                if (nextsv16w(s, se, &sv, &adv) != 0) {
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
                _buf_len += sv32_to_utf8_len(sv);
            }
            *buf_len = _buf_len;
        }
    } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
        if (buf) {
            s8 = buf;
            s8e = buf + *buf_len;
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
                adv = s8e - s8;
                if (sv32_to_utf8(sv, s8, &adv) == 0) {
                    s8 += adv;
                } else {
                    s8 = s8e;
                }
                _buf_len += adv;
            }
            *buf_len = _buf_len;
            if (s8 - buf == _buf_len) return 0;
        } else {
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
                _buf_len += sv32_to_utf8_len(sv);
            }
            *buf_len = _buf_len;
        }
    } else {
        assert(0);
        return UTFC_ERR;
    }

    return UTFC_BUF;
}

int wchar_to_utf16(const wchar_t* src, size_t len, char16_t* buf, size_t* buf_len, int flags) {
    const wchar_t* s, *se;
    char16_t* s16, *s16e, ch16, ch16_2;
    size_t _buf_len, adv;
    uint_fast32_t ch;

    assert(src && buf_len);

    s = src;
    se = s + len;
    _buf_len = 0;

    if (sizeof(wchar_t) * CHAR_BIT == 16u) {
        if (buf) {
            s16 = buf;
            s16e = buf + *buf_len;
            if (flags & UTFCF_CHK) {
                while (s < se) {
                    // D91
                    ch16 = (char16_t)*s++;
                    if (IS_START_SURROGATES(ch16)) {
                        if (s >= se) return UTFC_ERR;
                        if (s16 < s16e) *s16++ = ch16;
                        ch16 = (char16_t)*s++;
                        if (!IS_END_SURROGATES(ch16)) {
                            return UTFC_ERR;
                        }
                    } else if (IS_END_SURROGATES(ch16)) {
                        return UTFC_ERR;
                    }

                    if (s16 < s16e) *s16++ = ch16;
                }
            } else if (flags & UTFCF_IGN) {
                while (s < se && s16 < s16e) {
                    *s16++ = (char16_t)*s++;
                }
            } else {
                while (s < se && s16 < s16e) {
                    // D91
                    ch16 = (char16_t)*s++;
                    if (IS_START_SURROGATES(ch16)) {
                        if (s >= se) {
                            *s16++ = UTFC_INVALID_CHAR;
                            break;
                        }
                        ch16_2 = (char16_t)*s;
                        if (!IS_END_SURROGATES(ch16_2)) {
                            *s16++ = UTFC_INVALID_CHAR;
                            continue;
                        }
                        ++s;
                        *s16++ = ch16;
                        if (s16 < s16e) *s16++ = ch16_2;
                    } else if (IS_END_SURROGATES(ch16)) {
                        *s16++ = UTFC_INVALID_CHAR;
                    } else {
                        *s16++ = ch16;
                    }
                }
            }
            *buf_len = len;
            if (s16 - buf == len) return 0;
        } else {
            // D91
            if (flags & UTFCF_CHK) {
                while (s < se) {
                    ch16 = (char16_t)*s++;
                    if (IS_START_SURROGATES(ch16)) {
                        if (s >= se) return UTFC_ERR;
                        ch16 = (char16_t)*s++;
                        if (!IS_END_SURROGATES(ch16)) {
                            return UTFC_ERR;
                        }
                    } else if (IS_END_SURROGATES(ch16)) {
                        return UTFC_ERR;
                    }
                }
            }
            *buf_len = len;
        }
    } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
        if (buf) {
            s16 = buf;
            s16e = buf + *buf_len;
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
                adv = s16e - s16;
                if (sv32_to_utf16(ch, s16, &adv) == 0) {
                    s16 += adv;
                } else {
                    s16 = s16e;
                }
                _buf_len += adv;
            }
            *buf_len = _buf_len;
            if (s16 - buf == _buf_len) return 0;
        } else {
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
                _buf_len += sv32_to_utf16_len(ch);
            }
            *buf_len = _buf_len;
        }
    } else {
        assert(0);
        return UTFC_ERR;
    }

    return UTFC_BUF;
}

int wchar_to_utf32(const wchar_t* src, size_t len, char32_t* buf, size_t* buf_len, int flags) {
    const wchar_t* s, *se;
    char32_t* s32, *s32e;
    size_t _buf_len, adv;
    uint_fast32_t sv;

    assert(src && buf_len);

    s = src;
    se = s + len;
    _buf_len = 0;

    if (sizeof(wchar_t) * CHAR_BIT == 16u) {
        if (buf) {
            s32 = buf;
            s32e = buf + *buf_len;
            while (s < se) {
                if (nextsv16w(s, se, &sv, &adv) != 0) {
                    if (flags & UTFCF_CHK) {
                        return UTFC_ERR;
                    } else if (flags & UTFCF_IGN) {
                        if (s32 < s32e) *s32++ = (char32_t)*s;
                    } else {
                        if (s32 < s32e) *s32++ = UTFC_INVALID_CHAR;
                    }
                    ++s;
                } else {
                    s += adv;
                    if (s32 < s32e) *s32++ = (char32_t)sv;
                }
                ++_buf_len;
            }
            *buf_len = _buf_len;
            if (s32 - buf == _buf_len) return 0;
        } else {
            while (s < se) {
                if (nextsv16w_adv(s, se, &adv) != 0) {
                    if (flags & UTFCF_CHK) {
                        return UTFC_ERR;
                    }
                    ++s;
                } else {
                    s += adv;
                }
                ++_buf_len;
            }
            *buf_len = _buf_len;
        }
    } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
        if (buf) {
            s32 = buf;
            s32e = buf + *buf_len;
            if (flags & UTFCF_CHK) {
                while (s < se) {
                    sv = (uint_fast32_t)*s++;
                    if (IS_ILL_U32CU(sv)) {
                        return UTFC_ERR;
                    }
                    if (s32 < s32e) *s32++ = (char32_t)sv;
                }
            } else if (flags & UTFCF_IGN) {
                while (s < se && s32 < s32e) {
                    *s32++ = (char32_t)*s++;
                }
            } else {
                while (s < se && s32 < s32e) {
                    sv = (uint_fast32_t)*s++;
                    if (IS_ILL_U32CU(sv)) {
                        sv = UTFC_INVALID_CHAR;
                    }
                    *s32++ = (char32_t)sv;
                }
            }
            *buf_len = len;
            if (s32 - buf == len) return 0;
        } else {
            if (flags & UTFCF_CHK) {
                while (s < se) {
                    sv = (uint_fast32_t)*s++;
                    if (IS_ILL_U32CU(sv)) return UTFC_ERR;
                }
            }
            *buf_len = len;
        }
    } else {
        assert(0);
        return UTFC_ERR;
    }

    return UTFC_BUF;
}

int utf8_to_wchar(const char* src, size_t len, wchar_t* buf, size_t* buf_len, int flags) {
    const char* s, *se;
    size_t _buf_len, adv;
    wchar_t* sw, *swe;
    uint_fast32_t sv;

    assert(src && buf_len);

    s = src;
    se = s + len;
    _buf_len = 0;

    if (sizeof(wchar_t) * CHAR_BIT == 16u) {
        if (buf) {
            sw = buf;
            swe = buf + *buf_len;
            while (s < se) {
                if (nextsv8(s, se, &sv, &adv) != 0) {
                    if (flags & UTFCF_CHK) {
                        return UTFC_ERR;
                    } else if (flags & UTFCF_IGN) {
                        if (sw < swe) *sw++ = (unsigned char)*s;
                    } else {
                        if (sw < swe) *sw++ = (wchar_t)UTFC_INVALID_CHAR;
                    }
                    ++s;
                    ++_buf_len;
                } else {
                    s += adv;
                    adv = swe - sw;
                    if (sv32_to_w16(sv, sw, &adv) == 0) {
                        sw += adv;
                    } else {
                        sw = swe;
                    }
                    _buf_len += adv;
                }
            }
            *buf_len = _buf_len;
            if (sw - buf == _buf_len) return 0;
        } else {
            while (s < se) {
                if (nextsv8(s, se, &sv, &adv) != 0) {
                    if (flags & UTFCF_CHK) {
                        return UTFC_ERR;
                    }
                    ++s;
                    ++_buf_len;
                } else {
                    s += adv;
                    _buf_len += sv32_to_utf16_len(sv);
                }
            }
            *buf_len = _buf_len;
        }
    } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
        if (buf) {
            sw = buf;
            swe = buf + *buf_len;
            while (s < se) {
                if (nextsv8(s, se, &sv, &adv) != 0) {
                    if (flags & UTFCF_CHK) {
                        return UTFC_ERR;
                    } else if (flags & UTFCF_IGN) {
                        if (sw < swe) *sw++ = (unsigned char)*s;
                    } else {
                        if (sw < swe) *sw++ = (wchar_t)UTFC_INVALID_CHAR;
                    }
                    ++s;
                } else {
                    s += adv;
                    if (sw < swe) *sw++ = (wchar_t)sv;
                }
                ++_buf_len;
            }
            *buf_len = _buf_len;
            if (sw - buf == _buf_len) return 0;
        } else {
            while (s < se) {
                if (nextsv8_adv(s, se, &adv) != 0) {
                    if (flags & UTFCF_CHK) {
                        return UTFC_ERR;
                    }
                    ++s;
                } else {
                    s += adv;
                }
                ++_buf_len;
            }
            *buf_len = _buf_len;
        }
    } else {
        assert(0);
        return UTFC_ERR;
    }

    return UTFC_BUF;
}

int utf16_to_wchar(const char16_t* src, size_t len, wchar_t* buf, size_t* buf_len, int flags) {
    const char16_t* s, *se;
    wchar_t* sw, *swe, w16, w16_2;
    size_t _buf_len, adv;
    uint_fast32_t sv;

    assert(src && buf_len);

    s = src;
    se = s + len;
    _buf_len = 0;

    if (sizeof(wchar_t) * CHAR_BIT == 16u) {
        if (buf) {
            sw = buf;
            swe = buf + *buf_len;
            if (flags & UTFCF_CHK) {
                while (s < se) {
                    // D91
                    w16 = (wchar_t)*s++;
                    if (IS_START_SURROGATES(w16)) {
                        if (s >= se) return UTFC_ERR;
                        if (sw < swe) *sw++ = w16;
                        w16 = (wchar_t)*s++;
                        if (!IS_END_SURROGATES(w16)) {
                            return UTFC_ERR;
                        }
                    } else if (IS_END_SURROGATES(w16)) {
                        return UTFC_ERR;
                    }

                    if (sw < swe) *sw++ = w16;
                }
            } else if (flags & UTFCF_IGN) {
                while (s < se && sw < swe) {
                    *sw++ = (wchar_t)*s++;
                }
            } else {
                while (s < se && sw < swe) {
                    // D91
                    w16 = (wchar_t)*s++;
                    if (IS_START_SURROGATES(w16)) {
                        if (s >= se) {
                            *sw++ = (wchar_t)UTFC_INVALID_CHAR;
                            break;
                        }
                        w16_2 = (wchar_t)*s;
                        if (!IS_END_SURROGATES(w16_2)) {
                            *sw++ = (wchar_t)UTFC_INVALID_CHAR;
                            continue;
                        }
                        ++s;
                        *sw++ = w16;
                        if (sw < swe) *sw++ = w16_2;
                    } else if (IS_END_SURROGATES(w16)) {
                        *sw++ = (wchar_t)UTFC_INVALID_CHAR;
                    } else {
                        *sw++ = w16;
                    }
                }
            }
            *buf_len = len;
            if (sw - buf == len) return 0;
        } else {
            if (flags & UTFCF_CHK) {
                while (s < se) {
                    // D91
                    w16 = (wchar_t)*s++;
                    if (IS_START_SURROGATES(w16)) {
                        if (s >= se) return UTFC_ERR;
                        w16 = (wchar_t)*s++;
                        if (!IS_END_SURROGATES(w16)) {
                            return UTFC_ERR;
                        }
                    } else if (IS_END_SURROGATES(w16)) {
                        return UTFC_ERR;
                    }
                }
            }
            *buf_len = len;
        }
    } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
        if (buf) {
            sw = buf;
            swe = buf + *buf_len;
            while (s < se) {
                if (nextsv16c(s, se, &sv, &adv) != 0) {
                    if (flags & UTFCF_CHK) {
                        return UTFC_ERR;
                    } else if (flags & UTFCF_IGN) {
                        if (sw < swe) *sw++ = (wchar_t)*s;
                    } else {
                        if (sw < swe) *sw++ = (wchar_t)UTFC_INVALID_CHAR;
                    }
                    ++s;
                } else {
                    s += adv;
                    if (sw < swe) *sw++ = (wchar_t)sv;
                }
                ++_buf_len;
            }
            *buf_len = _buf_len;
            if (sw - buf == _buf_len)return 0;
        } else {
            while (s < se) {
                if (nextsv16c_adv(s, se, &adv) != 0) {
                    if (flags & UTFCF_CHK) {
                        return UTFC_ERR;
                    }
                    ++s;
                } else {
                    s += adv;
                }
                ++_buf_len;
            }
            *buf_len = _buf_len;
        }
    } else {
        assert(0);
        return UTFC_ERR;
    }

    return UTFC_BUF;
}

int utf32_to_wchar(const char32_t* src, size_t len, wchar_t* buf, size_t* buf_len, int flags) {
    const char32_t* s, *se;
    wchar_t* sw, *swe;
    size_t _buf_len, adv;
    char32_t ch;

    assert(src && buf_len);

    s = src;
    se = s + len;
    _buf_len = 0;

    if (sizeof(wchar_t) * CHAR_BIT == 16u) {
        if (buf) {
            sw = buf;
            swe = buf + *buf_len;
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
                adv = swe - sw;
                if (sv32_to_w16((uint_fast32_t)ch, sw, &adv) == 0) {
                    sw += adv;
                } else {
                    sw = swe;
                }
                _buf_len += adv;
            }
            *buf_len = _buf_len;
            if (sw - buf == _buf_len) return 0;
        } else {
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
                _buf_len += sv32_to_utf16_len((uint_fast32_t)ch);
            }
            *buf_len = _buf_len;
        }
    } else if (sizeof(wchar_t) * CHAR_BIT == 32u) {
        if (buf) {
            sw = buf;
            swe = buf + *buf_len;
            if (flags & UTFCF_CHK) {
                while (s < se) {
                    ch = *s++;
                    if (IS_ILL_U32CU(ch)) return UTFC_ERR;
                    if (sw < swe) *sw++ = (wchar_t)ch;
                }
            } else if (flags & UTFCF_IGN) {
                while (s < se && sw < swe) {
                    *sw++ = (wchar_t)*s++;
                }
            } else {
                while (s < se && sw < swe) {
                    ch = *s++;
                    if (IS_ILL_U32CU(ch)) ch = UTFC_INVALID_CHAR;
                    *sw++ = (wchar_t)ch;
                }
            }
            *buf_len = len;
            if (sw - buf == len) return 0;
        } else {
            if (flags & UTFCF_CHK) {
                while (s < se) {
                    ch = *s++;
                    if (IS_ILL_U32CU(ch)) return UTFC_ERR;
                }
            }
            *buf_len = len;
        }
    } else {
        assert(0);
        return UTFC_ERR;
    }

    return UTFC_BUF;
}
