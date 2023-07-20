#ifndef UTILS_STRINGS_UTFCC_H_
#define UTILS_STRINGS_UTFCC_H_

#ifdef __cplusplus
#include <climits>
#include <cstddef>
#else
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
typedef uint_least16_t char16_t;
typedef uint_least32_t char32_t;
#endif

#define WCHAR_IS_UTF16 (WCHAR_MAX == 0xFFFF)
#define WCHAR_IS_UTF32 (WCHAR_MAX  > 0xFFFF)

#define IS_START_SURROGATES(ch)  ((ch) >= 0xD800 && (ch) <= 0xDBFF)
#define IS_END_SURROGATES(ch)    ((ch) >= 0xDC00 && (ch) <= 0xDFFF)
#define IS_SURROGATES(ch)        ((ch) >= 0xD800 && (ch) <= 0xDFFF)

typedef enum _UTF_CONV_RETURN_CODE {
    UTFC_ERR = 1,
    UTFC_BUF = 2,
}UTF_CONV_RETURN_CODE;

typedef enum _UTF_CONV_FLAGS_CHK {
    UTFCF_DEF = 0,
    UTFCF_CHK = 1,
    UTFCF_IGN = 2,
}UTF_CONV_FLAGS_CHK;

typedef enum _UTF_CONV_FLAGS {
    UTFCF_COMBO = 4,
}UTF_CONV_FLAGS;

#ifdef __cplusplus
extern "C" {
#endif


int utf8_substr(const char* src, size_t len, size_t* sub_len, int flags);
int utf16_substr(const char16_t* src, size_t len, size_t* sub_len, int flags);
int utf32_substr(const char32_t* src, size_t len, size_t* sub_len, int flags);
int wchar_substr(const wchar_t* src, size_t len, size_t* sub_len, int flags);

int utf8c_to_utf16(char ch, char16_t* out, int flags);
int utf8c_to_utf32(char ch, char32_t* out, int flags);
int utf16c_to_utf8(char16_t ch, char* dst, size_t* len, int flags);
int utf16c_to_utf32(char16_t ch, char32_t* out, int flags);
int utf32c_to_utf8(char32_t ch, char* dst, size_t* len, int flags);
int utf32c_to_utf16(char32_t ch, char16_t* dst, size_t* len, int flags);
int wcharc_to_utf8(wchar_t ch, char* dst, size_t* len, int flags);
int wcharc_to_utf16(wchar_t ch, char16_t* dst, size_t* len, int flags);
int wcharc_to_utf32(wchar_t ch, char32_t* out, int flags);
int utf8c_to_wchar(char ch, wchar_t* out, int flags);
int utf16c_to_wchar(char16_t ch, wchar_t* out, int flags);
int utf32c_to_wchar(char32_t ch, wchar_t* dst, size_t* len, int flags);

int utf8_to_utf16(const char* src, size_t len, char16_t* buf, size_t* buf_len, int flags);
int utf8_to_utf32(const char* src, size_t len, char32_t* buf, size_t* buf_len, int flags);

int utf16_to_utf8(const char16_t* src, size_t len, char* buf, size_t* buf_len, int flags);
int utf16_to_utf32(const char16_t* src, size_t len, char32_t* buf, size_t* buf_len, int flags);

int utf32_to_utf8(const char32_t* src, size_t len, char* buf, size_t* buf_len, int flags);
int utf32_to_utf16(const char32_t* src, size_t len, char16_t* buf, size_t* buf_len, int flags);

int wchar_to_utf8(const wchar_t* src, size_t len, char* buf, size_t* buf_len, int flags);
int wchar_to_utf16(const wchar_t* src, size_t len, char16_t* buf, size_t* buf_len, int flags);
int wchar_to_utf32(const wchar_t* src, size_t len, char32_t* buf, size_t* buf_len, int flags);

int utf8_to_wchar(const char* src, size_t len, wchar_t* buf, size_t* buf_len, int flags);
int utf16_to_wchar(const char16_t* src, size_t len, wchar_t* buf, size_t* buf_len, int flags);
int utf32_to_wchar(const char32_t* src, size_t len, wchar_t* buf, size_t* buf_len, int flags);


#ifdef __cplusplus
}
#endif

#endif  // UTILS_STRINGS_UTFCC_H_
