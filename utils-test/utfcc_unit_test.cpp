// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/strings/utfcc.h"
#include "utils/unit_test/test_collector.h"

#define T_U8LEN(str)   std::char_traits<char>::length((const char*)str)
#define T_U16LEN(str)  std::char_traits<char16_t>::length(str)
#define T_U32LEN(str)  std::char_traits<char32_t>::length(str)
#define T_WLEN(str)    std::char_traits<wchar_t>::length(str)

// UTF-8: 测试正确的字符串截断。
#define T0_U8SUB(str, si, so, f)  sub_len = si;  \
    TEST_E(utf8_substr((const char*)str, T_U8LEN(str), &sub_len, f), 0);  \
    TEST_E(sub_len, so);

// UTF-8: 测试错误的字符串截断。
#define TF_U8SUB(str)  \
    TEST_E(utf8_substr((const char*)str, T_U8LEN(str), &sub_len, UTFCF_CHK), UTFC_ERR);

// UTF-16: 测试正确的字符串截断。
#define T0_U16SUB(str, si, so, f)  sub_len = si;  \
    TEST_E(utf16_substr(str, T_U16LEN(str), &sub_len, f), 0);  \
    TEST_E(sub_len, so);

// UTF-16: 测试错误的字符串截断。
#define TF_U16SUB(str)  \
    TEST_E(utf16_substr(str, T_U16LEN(str), &sub_len, UTFCF_CHK), UTFC_ERR);

// UTF-32: 测试正确的字符串截断。
#define T0_U32SUB(str, si, so, f)  sub_len = si;  \
    TEST_E(utf32_substr(str, T_U32LEN(str), &sub_len, f), 0);  \
    TEST_E(sub_len, so);

// UTF-32: 测试错误的字符串截断。
#define TF_U32SUB(str)  \
    TEST_E(utf32_substr(str, T_U32LEN(str), &sub_len, UTFCF_CHK), UTFC_ERR);

// WCHAR: 测试正确的字符串截断。
#define T0_WSUB(str, si, so, f)  sub_len = si;  \
    TEST_E(wchar_substr(str, T_WLEN(str), &sub_len, f), 0);  \
    TEST_E(sub_len, so);

// WCHAR: 测试错误的字符串截断。
#define TF_WSUB(str)  \
    TEST_E(wchar_substr(str, T_WLEN(str), &sub_len, UTFCF_CHK), UTFC_ERR);

// UTF-16C 到 UTF-8 正常转换
#define T0_U16C2U8(src, sl, dst, dl, f)  len = sl;  \
    TEST_E(utf16c_to_utf8(src, ch, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(ch, dst, len * sizeof(char)), 0);

// UTF-16C 到 UTF-8 错误转换
#define TF_U16C2U8(src, sl, ch, dl, f, ret)  len = sl;  \
    TEST_E(utf16c_to_utf8(src, ch, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// UTF-32C 到 UTF-8 正常转换
#define T0_U32C2U8(src, sl, dst, dl, f)  len = sl;  \
    TEST_E(utf32c_to_utf8(src, ch, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(ch, dst, len * sizeof(char)), 0);

// UTF-32C 到 UTF-8 错误转换
#define TF_U32C2U8(src, sl, ch, dl, f, ret)  len = sl;  \
    TEST_E(utf32c_to_utf8(src, ch, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// UTF-32C 到 UTF-16 正常转换
#define T0_U32C2U16(src, sl, dst, dl, f)  len = sl;  \
    TEST_E(utf32c_to_utf16(src, ch, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(ch, dst, len * sizeof(char16_t)), 0);

// UTF-32C 到 UTF-16 错误转换
#define TF_U32C2U16(src, sl, ch, dl, f, ret)  len = sl;  \
    TEST_E(utf32c_to_utf16(src, ch, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// WCHARC 到 UTF-8 正常转换
#define T0_WC2U8(src, sl, dst, dl, f)  len = sl;  \
    TEST_E(wcharc_to_utf8(src, ch, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(ch, dst, len * sizeof(char)), 0);

// WCHARC 到 UTF-8 错误转换
#define TF_WC2U8(src, sl, ch, dl, f, ret)  len = sl;  \
    TEST_E(wcharc_to_utf8(src, ch, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// WCHARC 到 UTF-16 正常转换
#define T0_WC2U16(src, sl, dst, dl, f)  len = sl;  \
    TEST_E(wcharc_to_utf16(src, ch, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(ch, dst, len * sizeof(char16_t)), 0);

// WCHARC 到 UTF-16 错误转换
#define TF_WC2U16(src, sl, ch, dl, f, ret)  len = sl;  \
    TEST_E(wcharc_to_utf16(src, ch, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// UTF-32C 到 WCHAR 正常转换
#define T0_U32C2W(src, sl, dst, dl, f)  len = sl;  \
    TEST_E(utf32c_to_wchar(src, ch, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(ch, dst, len * sizeof(wchar_t)), 0);

// UTF-32C 到 WCHAR 错误转换
#define TF_U32C2W(src, sl, ch, dl, f, ret)  len = sl;  \
    TEST_E(utf32c_to_wchar(src, ch, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// UTF-8 到 UTF-16 正常转换
#define T0_U8_16(src, sl, dst, dl, f)  len = sl; \
    TEST_E(utf8_to_utf16((const char*)src, T_U8LEN(src), buf, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(buf, dst, len * sizeof(char16_t)), 0);

// UTF-8 到 UTF-16 错误转换
#define TF_U8_16(src, sl, buf, dl, f, ret)  len = sl; \
    TEST_E(utf8_to_utf16((const char*)src, T_U8LEN(src), buf, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// UTF-8 到 UTF-32 正常转换
#define T0_U8_32(src, sl, dst, dl, f)  len = sl; \
    TEST_E(utf8_to_utf32((const char*)src, T_U8LEN(src), buf, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(buf, dst, len * sizeof(char32_t)), 0);

// UTF-8 到 UTF-32 错误转换
#define TF_U8_32(src, sl, buf, dl, f, ret)  len = sl; \
    TEST_E(utf8_to_utf32((const char*)src, T_U8LEN(src), buf, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// UTF-16 到 UTF-8 正常转换
#define T0_U16_8(src, sl, dst, dl, f)  len = sl; \
    TEST_E(utf16_to_utf8(src, T_U16LEN(src), buf, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(buf, dst, len * sizeof(char)), 0);

// UTF-16 到 UTF-8 错误转换
#define TF_U16_8(src, sl, buf, dl, f, ret)  len = sl; \
    TEST_E(utf16_to_utf8(src, T_U16LEN(src), buf, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// UTF-16 到 UTF-32 正常转换
#define T0_U16_32(src, sl, dst, dl, f)  len = sl; \
    TEST_E(utf16_to_utf32(src, T_U16LEN(src), buf, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(buf, dst, len * sizeof(char32_t)), 0);

// UTF-16 到 UTF-32 错误转换
#define TF_U16_32(src, sl, buf, dl, f, ret)  len = sl; \
    TEST_E(utf16_to_utf32(src, T_U16LEN(src), buf, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// UTF-32 到 UTF-8 正常转换
#define T0_U32_8(src, sl, dst, dl, f)  len = sl; \
    TEST_E(utf32_to_utf8(src, T_U32LEN(src), buf, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(buf, dst, len * sizeof(char)), 0);

// UTF-32 到 UTF-8 错误转换
#define TF_U32_8(src, sl, buf, dl, f, ret)  len = sl; \
    TEST_E(utf32_to_utf8(src, T_U32LEN(src), buf, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// UTF-32 到 UTF-16 正常转换
#define T0_U32_16(src, sl, dst, dl, f)  len = sl; \
    TEST_E(utf32_to_utf16(src, T_U32LEN(src), buf, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(buf, dst, len * sizeof(char16_t)), 0);

// UTF-32 到 UTF-16 错误转换
#define TF_U32_16(src, sl, buf, dl, f, ret)  len = sl; \
    TEST_E(utf32_to_utf16(src, T_U32LEN(src), buf, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// WCHAR 到 UTF-8 正常转换
#define T0_W_U8(src, sl, dst, dl, f)  len = sl; \
    TEST_E(wchar_to_utf8(src, T_WLEN(src), buf, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(buf, dst, len * sizeof(char)), 0);

// WCHAR 到 UTF-8 错误转换
#define TF_W_U8(src, sl, buf, dl, f, ret)  len = sl; \
    TEST_E(wchar_to_utf8(src, T_WLEN(src), buf, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// WCHAR 到 UTF-16 正常转换
#define T0_W_U16(src, sl, dst, dl, f)  len = sl; \
    TEST_E(wchar_to_utf16(src, T_WLEN(src), buf, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(buf, dst, len * sizeof(char16_t)), 0)

// WCHAR 到 UTF-16 错误转换
#define TF_W_U16(src, sl, buf, dl, f, ret)  len = sl; \
    TEST_E(wchar_to_utf16(src, T_WLEN(src), buf, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// WCHAR 到 UTF-32 正常转换
#define T0_W_U32(src, sl, dst, dl, f)  len = sl; \
    TEST_E(wchar_to_utf32(src, T_WLEN(src), buf, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(buf, dst, len * sizeof(char32_t)), 0);

// WCHAR 到 UTF-32 错误转换
#define TF_W_U32(src, sl, buf, dl, f, ret)  len = sl; \
    TEST_E(wchar_to_utf32(src, T_WLEN(src), buf, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// UTF-8 到 WCHAR 正常转换
#define T0_U8_W(src, sl, dst, dl, f)  len = sl; \
    TEST_E(utf8_to_wchar((const char*)src, T_U8LEN(src), buf, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(buf, dst, len * sizeof(wchar_t)), 0);

// UTF-8 到 WCHAR 错误转换
#define TF_U8_W(src, sl, buf, dl, f, ret)  len = sl; \
    TEST_E(utf8_to_wchar((const char*)src, T_U8LEN(src), buf, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// UTF-16 到 WCHAR 正常转换
#define T0_U16_W(src, sl, dst, dl, f)  len = sl; \
    TEST_E(utf16_to_wchar(src, T_U16LEN(src), buf, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(buf, dst, len * sizeof(wchar_t)), 0);

// UTF-16 到 WCHAR 错误转换
#define TF_U16_W(src, sl, buf, dl, f, ret)  len = sl; \
    TEST_E(utf16_to_wchar(src, T_U16LEN(src), buf, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);

// UTF-32 到 WCHAR 正常转换
#define T0_U32_W(src, sl, dst, dl, f)  len = sl; \
    TEST_E(utf32_to_wchar(src, T_U32LEN(src), buf, &len, f), 0);  \
    TEST_E(len, dl); TEST_E(std::memcmp(buf, dst, len * sizeof(wchar_t)), 0);

// UTF-32 到 WCHAR 错误转换
#define TF_U32_W(src, sl, buf, dl, f, ret)  len = sl; \
    TEST_E(utf32_to_wchar(src, T_U32LEN(src), buf, &len, f), ret);  \
    if (ret == UTFC_BUF) TEST_E(len, dl);


TEST_CASE(UTFCCUnitTest) {

    TEST_DEF("UTF-8  substr") {
        size_t sub_len;
        
        // 0
        T0_U8SUB("", 0, 0, UTFCF_CHK);
        T0_U8SUB("", 1, 0, UTFCF_CHK);
        T0_U8SUB("", 1, 0, UTFCF_IGN);

        // 1
        T0_U8SUB("a",   1, 1, UTFCF_CHK);
        T0_U8SUB("abc", 2, 2, UTFCF_CHK);
        T0_U8SUB("abc", 2, 2, UTFCF_IGN);

        // 2
        T0_U8SUB(u8"åaåå", 2, 3, UTFCF_CHK);
        T0_U8SUB(u8"åaåå", 2, 3, UTFCF_IGN);

        // 3
        T0_U8SUB(u8"我是",    1, 3, UTFCF_CHK);
        T0_U8SUB(u8"我是",   10, 6, UTFCF_CHK);
        T0_U8SUB(u8"a我b是c", 3, 5, UTFCF_CHK);
        T0_U8SUB(u8"a我b是c", 3, 5, UTFCF_IGN);

        // 4
        T0_U8SUB(u8"𭬨𭬨𭬨", 2, 8, UTFCF_CHK);
        T0_U8SUB(u8"𭬨𭬨𭬨", 2, 8, UTFCF_IGN);

        // ignore
        T0_U8SUB("\xC1",     1, 1, UTFCF_IGN);
        T0_U8SUB("\xC2",     1, 1, UTFCF_IGN);
        T0_U8SUB("\xC2\x7F", 2, 2, UTFCF_IGN);
        // MSVC 存在的 bug 导致16进制数无法正常写在 u8"" 字符串中
        // u8"我\xC2"
        T0_U8SUB("\xE6\x88\x91\xC2", 2, 4, UTFCF_IGN);

        // Grapheme cluster
        // https://unicode.org/reports/tr29/
        sub_len = 1;
        TEST_E(utf8_substr((const char*)u8"g̈g̈g̈", 9, &sub_len, UTFCF_CHK), 0);
        // TODO:
        //TEST_E(sub_len, 2);

        // fail case
        sub_len = 100;

        TF_U8SUB("\xC1");
        TF_U8SUB("\xC2");
        TF_U8SUB("\xC2\x7F");
        // MSVC 存在的 bug 导致16进制数无法正常写在 u8"" 字符串中
        TF_U8SUB("\xE6\x88\x91\xC2");  // 我\xC2

        return true;
    };

    TEST_DEF("UTF-16 substr") {
        size_t sub_len;

        // 0
        T0_U16SUB(u"", 0, 0, UTFCF_CHK);
        T0_U16SUB(u"", 1, 0, UTFCF_CHK);
        T0_U16SUB(u"", 1, 0, UTFCF_IGN);

        // 1
        T0_U16SUB(u"a",    1, 1, UTFCF_CHK);
        T0_U16SUB(u"abc",  2, 2, UTFCF_CHK);
        T0_U16SUB(u"åaåå", 2, 2, UTFCF_CHK);
        T0_U16SUB(u"我是", 1, 1, UTFCF_CHK);
        T0_U16SUB(u"我是", 10, 2, UTFCF_CHK);
        T0_U16SUB(u"a我b是c", 3, 3, UTFCF_CHK);
        T0_U16SUB(u"a我b是c", 3, 3, UTFCF_IGN);

        // 2
        T0_U16SUB(u"𭬨𭬨𭬨", 2, 4, UTFCF_CHK);
        T0_U16SUB(u"𭬨𭬨𭬨", 2, 4, UTFCF_IGN);

        // ignore
        T0_U16SUB(u"\xD800", 1, 1, UTFCF_IGN);
        T0_U16SUB(u"\xDBFF", 1, 1, UTFCF_IGN);
        T0_U16SUB(u"\xDFFF", 1, 1, UTFCF_IGN);
        T0_U16SUB(u"我\xD800", 2, 2, UTFCF_IGN);

        // Grapheme cluster
        // https://unicode.org/reports/tr29/
        sub_len = 1;
        TEST_E(utf16_substr(u"g̈g̈g̈", 6, &sub_len, UTFCF_CHK), 0);
        // TODO:
        //TEST_E(sub_len, 2);

        // fail case
        sub_len = 100;

        TF_U16SUB(u"\xD800");
        TF_U16SUB(u"\xDBFF");
        TF_U16SUB(u"\xDFFF");
        TF_U16SUB(u"我\xD800");

        return true;
    };

    TEST_DEF("UTF-32 substr") {
        size_t sub_len;

        // 0
        T0_U32SUB(U"", 0, 0, UTFCF_CHK);
        T0_U32SUB(U"", 1, 0, UTFCF_CHK);
        T0_U32SUB(U"", 1, 0, UTFCF_IGN);

        // 1
        T0_U32SUB(U"a",    1, 1, UTFCF_CHK);
        T0_U32SUB(U"abc",  2, 2, UTFCF_CHK);
        T0_U32SUB(U"åaåå", 2, 2, UTFCF_CHK);
        T0_U32SUB(U"我是", 1, 1, UTFCF_CHK);
        T0_U32SUB(U"我是", 10, 2, UTFCF_CHK);
        T0_U32SUB(U"a我b是c", 3, 3, UTFCF_CHK);
        T0_U32SUB(U"𭬨𭬨𭬨", 2, 2, UTFCF_CHK);
        T0_U32SUB(U"𭬨𭬨𭬨", 2, 2, UTFCF_IGN);

        // ignore
        T0_U32SUB(U"\xD800", 1, 1, UTFCF_IGN);
        T0_U32SUB(U"\xDBFF", 1, 1, UTFCF_IGN);
        T0_U32SUB(U"我\xD800", 2, 2, UTFCF_IGN);
        T0_U32SUB(U"我\x110000", 2, 2, UTFCF_IGN);

        // Grapheme cluster
        // https://unicode.org/reports/tr29/
        sub_len = 1;
        TEST_E(utf32_substr(U"g̈g̈g̈", 6, &sub_len, UTFCF_CHK), 0);
        // TODO:
        //TEST_E(sub_len, 2);

        // fail case
        sub_len = 100;

        TF_U32SUB(U"\xD800");
        TF_U32SUB(U"\xDBFF");
        TF_U32SUB(U"我\xD800");
        TF_U32SUB(U"我\x110000");

        return true;
    };

    TEST_DEF("WCHAR  substr") {
        size_t sub_len;

        // 0
        T0_WSUB(L"", 0, 0, UTFCF_CHK);
        T0_WSUB(L"", 1, 0, UTFCF_CHK);
        T0_WSUB(L"", 1, 0, UTFCF_IGN);

        // 1
        T0_WSUB(L"a",    1, 1, UTFCF_CHK);
        T0_WSUB(L"abc",  2, 2, UTFCF_CHK);
        T0_WSUB(L"åaåå", 2, 2, UTFCF_CHK);
        T0_WSUB(L"我是", 1, 1, UTFCF_CHK);
        T0_WSUB(L"我是", 10, 2, UTFCF_CHK);
        T0_WSUB(L"a我b是c", 3, 3, UTFCF_CHK);
        T0_WSUB(L"a我b是c", 3, 3, UTFCF_IGN);

        if (sizeof(wchar_t) * CHAR_BIT == 16u) {
            // 2
            T0_WSUB(L"𭬨𭬨𭬨", 2, 4, UTFCF_CHK);
            T0_WSUB(L"𭬨𭬨𭬨", 2, 4, UTFCF_IGN);
        } else {
            T0_WSUB(L"𭬨𭬨𭬨", 2, 2, UTFCF_CHK);
            T0_WSUB(L"𭬨𭬨𭬨", 2, 2, UTFCF_IGN);
        }

        // ignore
        T0_WSUB(L"\xD800",    1, 1, UTFCF_IGN);
        T0_WSUB(L"\xDBFF",    1, 1, UTFCF_IGN);
        T0_WSUB(L"\xDFFF",    1, 1, UTFCF_IGN);
        T0_WSUB(L"我\xD800",  2, 2, UTFCF_IGN);
#if WCHAR_IS_UTF32
        T0_WSUB(L"我\x110000", 2, 2, UTFCF_IGN);
#endif

        // Grapheme cluster
        // https://unicode.org/reports/tr29/
        sub_len = 1;
        TEST_E(wchar_substr(L"g̈g̈g̈", 6, &sub_len, UTFCF_CHK), 0);
        // TODO:
        //TEST_E(sub_len, 2);

        // fail case
        sub_len = 100;

        TF_WSUB(L"\xD800");
        TF_WSUB(L"\xDBFF");
        TF_WSUB(L"\xDFFF");
        TF_WSUB(L"我\xD800");
#if WCHAR_IS_UTF32
        TF_U32SUB(U"我\x110000");
#endif

        return true;
    };

    TEST_DEF("UTF-8  Char -> UTF-16") {
        char16_t ch;

        TEST_TRUE(utf8c_to_utf16('\0', &ch, UTFCF_CHK) == 0   && ch == u'\0');
        TEST_TRUE(utf8c_to_utf16('a', &ch, UTFCF_CHK) == 0    && ch == u'a');
        TEST_TRUE(utf8c_to_utf16('\x7F', &ch, UTFCF_CHK) == 0 && ch == u'\x7F');
        TEST_TRUE(utf8c_to_utf16('\x7F', &ch, UTFCF_IGN) == 0 && ch == u'\x7F');
        TEST_TRUE(utf8c_to_utf16('\x7F', &ch, UTFCF_DEF) == 0 && ch == u'\x7F');
        TEST_TRUE(utf8c_to_utf16('\x80', &ch, UTFCF_CHK) == UTFC_ERR);

        // not error
        TEST_TRUE(utf8c_to_utf16('\x80', &ch, UTFCF_IGN) == 0 && ch == u'\x80');
        TEST_TRUE(utf8c_to_utf16('\x80', &ch, UTFCF_DEF) == 0 && ch == u'\xFFFD');

        return true;
    };

    TEST_DEF("UTF-8  Char -> UTF-32") {
        char32_t ch;

        TEST_TRUE(utf8c_to_utf32('\0', &ch, UTFCF_CHK) == 0   && ch == U'\0');
        TEST_TRUE(utf8c_to_utf32('a', &ch, UTFCF_CHK) == 0    && ch == U'a');
        TEST_TRUE(utf8c_to_utf32('\x7F', &ch, UTFCF_CHK) == 0 && ch == U'\x7F');
        TEST_TRUE(utf8c_to_utf32('\x7F', &ch, UTFCF_IGN) == 0 && ch == U'\x7F');
        TEST_TRUE(utf8c_to_utf32('\x7F', &ch, UTFCF_DEF) == 0 && ch == U'\x7F');
        TEST_TRUE(utf8c_to_utf32('\x80', &ch, UTFCF_CHK) == UTFC_ERR);

        // not error
        TEST_TRUE(utf8c_to_utf32('\x80', &ch, UTFCF_IGN) == 0 && ch == U'\x80');
        TEST_TRUE(utf8c_to_utf32('\x80', &ch, UTFCF_DEF) == 0 && ch == U'\xFFFD');

        return true;
    };

    TEST_DEF("UTF-16 Char -> UTF-8") {
        char ch[8];
        size_t len;
        len = 0;

        // 1
        T0_U16C2U8(u'\0',   3, u8"\0",   1, UTFCF_CHK);
        T0_U16C2U8(u'\x7F', 3, u8"\x7F", 1, UTFCF_CHK);
        T0_U16C2U8(u'a',    3, u8"a",    1, UTFCF_CHK);
        T0_U16C2U8(u'a',    3, u8"a",    1, UTFCF_IGN);
        T0_U16C2U8(u'a',    3, u8"a",    1, UTFCF_DEF);

        // 2
        T0_U16C2U8(u'\x80', 3, u8"\u0080", 2, UTFCF_CHK);
        T0_U16C2U8(u'\x80', 3, u8"\u0080", 2, UTFCF_IGN);
        T0_U16C2U8(u'\x80', 3, u8"\u0080", 2, UTFCF_DEF);

        // 3
        T0_U16C2U8(u'我', 3, u8"我", 3, UTFCF_CHK);
        T0_U16C2U8(u'我', 3, u8"我", 3, UTFCF_IGN);
        T0_U16C2U8(u'我', 3, u8"我", 3, UTFCF_DEF);

        // not error 
        T0_U16C2U8(u'\xD800', 3, "\xED\xA0\x80", 3, UTFCF_IGN);
        T0_U16C2U8(u'\xDFFF', 3, "\xED\xBF\xBF", 3, UTFCF_IGN);
        T0_U16C2U8(u'\xDFFF', 3, "\xEF\xBF\xBD", 3, UTFCF_DEF);

        // false case
        TF_U16C2U8(u'\xD800', 3, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_U16C2U8(u'\xDFFF', 3, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_U16C2U8(u'\xDFFF', 0, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_U16C2U8(u'\xDFFF', 0, ch, 3, UTFCF_IGN, UTFC_BUF);
        TF_U16C2U8(u'\xDFFF', 0, ch, 3, UTFCF_DEF, UTFC_BUF);
        TF_U16C2U8(u'a',      0, ch,      1, UTFCF_CHK, UTFC_BUF);
        TF_U16C2U8(u'a',      0, nullptr, 1, UTFCF_CHK, UTFC_BUF);
        TF_U16C2U8(u'\x80',   1, ch,      2, UTFCF_CHK, UTFC_BUF);
        TF_U16C2U8(u'\x80',   1, nullptr, 2, UTFCF_CHK, UTFC_BUF);
        TF_U16C2U8(u'我',     2, ch,      3, UTFCF_CHK, UTFC_BUF);
        TF_U16C2U8(u'我',     2, nullptr, 3, UTFCF_CHK, UTFC_BUF);

        return true;
    };

    TEST_DEF("UTF-16 Char -> UTF-32") {
        char32_t ch;

        TEST_TRUE(utf16c_to_utf32(u'\0', &ch, UTFCF_CHK) == 0 && ch == U'\0');
        TEST_TRUE(utf16c_to_utf32(u'我', &ch, UTFCF_CHK) == 0 && ch == U'我');
        TEST_TRUE(utf16c_to_utf32(u'我', &ch, UTFCF_IGN) == 0 && ch == U'我');
        TEST_TRUE(utf16c_to_utf32(u'我', &ch, UTFCF_DEF) == 0 && ch == U'我');
        TEST_TRUE(utf16c_to_utf32(u'\xD800', &ch, UTFCF_CHK) == UTFC_ERR);

        // not error 
        TEST_TRUE(utf16c_to_utf32(u'\xD800', &ch, UTFCF_IGN) == 0 && ch == U'\xD800');
        TEST_TRUE(utf16c_to_utf32(u'\xDFFF', &ch, UTFCF_IGN) == 0 && ch == U'\xDFFF');
        TEST_TRUE(utf16c_to_utf32(u'\xDFFF', &ch, UTFCF_DEF) == 0 && ch == U'\xFFFD');

        return true;
    };

    TEST_DEF("UTF-32 Char -> UTF-8") {
        char ch[4];
        size_t len;

        // 1
        T0_U32C2U8(U'\0',   4, u8"\0",   1, UTFCF_CHK);
        T0_U32C2U8(U'\x7F', 4, u8"\x7F", 1, UTFCF_CHK);
        T0_U32C2U8(U'a',    4, u8"a",    1, UTFCF_CHK);
        T0_U32C2U8(U'a',    4, u8"a",    1, UTFCF_IGN);
        T0_U32C2U8(U'a',    4, u8"a",    1, UTFCF_DEF);

        // 2
        T0_U32C2U8(U'\x80', 4, u8"\u0080", 2, UTFCF_CHK);
        T0_U32C2U8(U'\x80', 4, u8"\u0080", 2, UTFCF_IGN);
        T0_U32C2U8(U'\x80', 4, u8"\u0080", 2, UTFCF_DEF);

        // 3
        T0_U32C2U8(U'我', 4, u8"我", 3, UTFCF_CHK);
        T0_U32C2U8(U'我', 4, u8"我", 3, UTFCF_IGN);
        T0_U32C2U8(U'我', 4, u8"我", 3, UTFCF_DEF);

        // 4
        T0_U32C2U8(U'𭬨', 4, u8"𭬨", 4, UTFCF_CHK);
        T0_U32C2U8(U'🦊', 4, u8"🦊", 4, UTFCF_CHK);
        T0_U32C2U8(U'🦊', 4, u8"🦊", 4, UTFCF_IGN);
        T0_U32C2U8(U'🦊', 4, u8"🦊", 4, UTFCF_DEF);

        // not error
        T0_U32C2U8(u'\xD800',   3, "\xED\xA0\x80", 3, UTFCF_IGN);
        T0_U32C2U8(u'\xDFFF',   3, "\xED\xBF\xBF", 3, UTFCF_IGN);
        T0_U32C2U8(U'\x110000', 4, "\xF4\x90\x80\x80", 4, UTFCF_IGN);
        T0_U32C2U8(u'\xDFFF',   3, "\xEF\xBF\xBD", 3, UTFCF_DEF);
        T0_U32C2U8(U'\x110000', 4, "\xEF\xBF\xBD", 3, UTFCF_DEF);

        // false case
        TF_U32C2U8(U'\xD800',   4, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32C2U8(U'\xDFFF',   4, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32C2U8(U'\xDFFF',   0, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32C2U8(U'\xDFFF',   0, ch, 3, UTFCF_IGN, UTFC_BUF);
        TF_U32C2U8(U'\xDFFF',   0, ch, 3, UTFCF_DEF, UTFC_BUF);
        TF_U32C2U8(U'\x110000', 4, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32C2U8(U'\x110000', 0, ch, 4, UTFCF_IGN, UTFC_BUF);
        TF_U32C2U8(U'a',        0, ch, 1, UTFCF_CHK, UTFC_BUF);
        TF_U32C2U8(U'a',        0, nullptr, 1, UTFCF_CHK, UTFC_BUF);
        TF_U32C2U8(U'\x80',     1, ch,      2, UTFCF_CHK, UTFC_BUF);
        TF_U32C2U8(U'\x80',     1, nullptr, 2, UTFCF_CHK, UTFC_BUF);
        TF_U32C2U8(U'我',       2, ch,      3, UTFCF_CHK, UTFC_BUF);
        TF_U32C2U8(U'我',       2, nullptr, 3, UTFCF_CHK, UTFC_BUF);
        TF_U32C2U8(U'𭬨',       3, ch,      4, UTFCF_CHK, UTFC_BUF);
        TF_U32C2U8(U'𭬨',       3, nullptr, 4, UTFCF_CHK, UTFC_BUF);
        TF_U32C2U8(U'🦊',      3, ch,      4, UTFCF_CHK, UTFC_BUF);
        TF_U32C2U8(U'🦊',      3, nullptr, 4, UTFCF_CHK, UTFC_BUF);

        return true;
    };

    TEST_DEF("UTF-32 Char -> UTF-16") {
        char16_t ch[2];
        size_t len;

        // 1
        T0_U32C2U16(U'\0',   2, u"\0",   1, UTFCF_CHK);
        T0_U32C2U16(U'\x7F', 2, u"\x7F", 1, UTFCF_CHK);
        T0_U32C2U16(U'a',    2, u"a",    1, UTFCF_CHK);
        T0_U32C2U16(U'\x80', 2, u"\u0080", 1, UTFCF_CHK);
        T0_U32C2U16(U'我',   2, u"我",   1, UTFCF_CHK);
        T0_U32C2U16(U'我',   2, u"我",   1, UTFCF_IGN);
        T0_U32C2U16(U'我',   2, u"我",   1, UTFCF_DEF);

        // 2
        T0_U32C2U16(U'𭬨',  2, u"𭬨", 2, UTFCF_CHK);
        T0_U32C2U16(U'🦊', 2, u"🦊", 2, UTFCF_CHK);
        T0_U32C2U16(U'🦊', 2, u"🦊", 2, UTFCF_IGN);
        T0_U32C2U16(U'🦊', 2, u"🦊", 2, UTFCF_DEF);

        // not error
        T0_U32C2U16(U'\xD800',   2, u"\xD800", 1, UTFCF_IGN);
        T0_U32C2U16(U'\xDFFF',   2, u"\xDFFF", 1, UTFCF_IGN);
        T0_U32C2U16(U'\x110000', 2, u"\xD800\xDC00", 2, UTFCF_IGN);
        T0_U32C2U16(U'\x110000', 2, u"\xFFFD", 1, UTFCF_DEF);

        // false case 
        TF_U32C2U16(U'\xD800',   2, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32C2U16(U'\xDFFF',   2, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32C2U16(U'\xDFFF',   0, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32C2U16(U'\xDFFF',   0, ch, 1, UTFCF_IGN, UTFC_BUF);
        TF_U32C2U16(U'\xDFFF',   0, ch, 1, UTFCF_DEF, UTFC_BUF);
        TF_U32C2U16(U'\x110000', 2, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32C2U16(U'\x110000', 0, ch, 2, UTFCF_IGN, UTFC_BUF);
        TF_U32C2U16(U'a',        0, ch, 1, UTFCF_CHK, UTFC_BUF);
        TF_U32C2U16(U'a',        0, nullptr, 1, UTFCF_CHK, UTFC_BUF);
        TF_U32C2U16(U'\x80',     0, ch, 1, UTFCF_CHK, UTFC_BUF);
        TF_U32C2U16(U'我',       0, ch, 1, UTFCF_CHK, UTFC_BUF);
        TF_U32C2U16(U'𭬨',       1, ch, 2, UTFCF_CHK, UTFC_BUF);
        TF_U32C2U16(U'𭬨',       1, nullptr, 2, UTFCF_CHK, UTFC_BUF);
        TF_U32C2U16(U'🦊',       1, ch,      2, UTFCF_CHK, UTFC_BUF);
        TF_U32C2U16(U'🦊',       1, nullptr, 2, UTFCF_CHK, UTFC_BUF);

        return true;
    };

    TEST_DEF("WCHAR  Char -> UTF-8") {
        char ch[4];
        size_t len;

        // 1
        T0_WC2U8(L'\0',   4, u8"\0",   1, UTFCF_CHK);
        T0_WC2U8(L'\x7F', 4, u8"\x7F", 1, UTFCF_CHK);
        T0_WC2U8(L'a',    4, u8"a",    1, UTFCF_CHK);
        T0_WC2U8(L'a',    4, u8"a",    1, UTFCF_IGN);
        T0_WC2U8(L'a',    4, u8"a",    1, UTFCF_DEF);

        // 2
        T0_WC2U8(L'\x80', 4, u8"\u0080", 2, UTFCF_CHK);
        T0_WC2U8(L'\x80', 4, u8"\u0080", 2, UTFCF_IGN);
        T0_WC2U8(L'\x80', 4, u8"\u0080", 2, UTFCF_DEF);

        // 3
        T0_WC2U8(L'我', 4, u8"我", 3, UTFCF_CHK);
        T0_WC2U8(L'我', 4, u8"我", 3, UTFCF_IGN);
        T0_WC2U8(L'我', 4, u8"我", 3, UTFCF_DEF);

        // not error 
        T0_WC2U8(L'\xD800', 3, "\xED\xA0\x80", 3, UTFCF_IGN);
        T0_WC2U8(L'\xDFFF', 3, "\xED\xBF\xBF", 3, UTFCF_IGN);
        T0_WC2U8(L'\xDFFF', 3, "\xEF\xBF\xBD", 3, UTFCF_DEF);

        // false case
        TF_WC2U8(L'\xD800', 4, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_WC2U8(L'\xDFFF', 4, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_WC2U8(L'\xDFFF', 0, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_WC2U8(L'\xDFFF', 0, ch, 3, UTFCF_IGN, UTFC_BUF);
        TF_WC2U8(L'\xDFFF', 0, ch, 3, UTFCF_DEF, UTFC_BUF);
        TF_WC2U8(L'a',      0, ch, 1, UTFCF_CHK, UTFC_BUF);
        TF_WC2U8(L'a',      0, nullptr, 1, UTFCF_CHK, UTFC_BUF);
        TF_WC2U8(L'\x80',   1, ch,      2, UTFCF_CHK, UTFC_BUF);
        TF_WC2U8(L'\x80',   1, nullptr, 2, UTFCF_CHK, UTFC_BUF);
        TF_WC2U8(L'我',     2, ch,      3, UTFCF_CHK, UTFC_BUF);
        TF_WC2U8(L'我',     2, nullptr, 3, UTFCF_CHK, UTFC_BUF);

#if WCHAR_IS_UTF32
        // 4
        T0_WC2U8(L'𭬨',  4, u8"𭬨", 4, UTFCF_CHK);
        T0_WC2U8(L'🦊', 4, u8"🦊", 4, UTFCF_CHK);

        // not error
        T0_WC2U8(L'\x110000', 4, "\xF4\x90\x80\x80", 4, UTFCF_IGN);
        T0_WC2U8(L'\x110000', 4, "\xEF\xBF\xBD", 3, UTFCF_DEF);

        // false case
        TF_WC2U8(L'\x110000', 4, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_WC2U8(L'\x110000', 0, ch, 4, UTFCF_IGN, UTFC_BUF);
        TF_WC2U8(L'𭬨',       3, ch, 4, UTFCF_CHK, UTFC_BUF);
        TF_WC2U8(L'🦊',      3, ch, 4, UTFCF_CHK, UTFC_BUF);
        TF_WC2U8(L'🦊',      3, nullptr, 4, UTFCF_CHK, UTFC_BUF);
#endif
        return true;
    };

    TEST_DEF("WCHAR  Char -> UTF-16") {
        char16_t ch[2];
        size_t len;
        len = 1;

        // 1
        T0_WC2U16(L'\0',   2, u"\0",   1, UTFCF_CHK);
        T0_WC2U16(L'\x7F', 2, u"\x7F", 1, UTFCF_CHK);
        T0_WC2U16(L'a',    2, u"a",    1, UTFCF_CHK);
        T0_WC2U16(L'\x80', 2, u"\u0080", 1, UTFCF_CHK);
        T0_WC2U16(L'我',   2, u"我",   1, UTFCF_CHK);
        T0_WC2U16(L'我',   2, u"我",   1, UTFCF_IGN);
        T0_WC2U16(L'我',   2, u"我",   1, UTFCF_DEF);

        // not error
        T0_WC2U16(L'\xD800', 2, u"\xD800", 1, UTFCF_IGN);
        T0_WC2U16(L'\xDFFF', 2, u"\xDFFF", 1, UTFCF_IGN);
        T0_WC2U16(L'\xDFFF', 2, u"\xFFFD", 1, UTFCF_DEF);

        // false case
        TF_WC2U16(L'\xD800', 2, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_WC2U16(L'\xDFFF', 2, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_WC2U16(L'\xDFFF', 0, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_WC2U16(L'\xDFFF', 0, ch, 1, UTFCF_IGN, UTFC_BUF);
        TF_WC2U16(L'\xDFFF', 0, ch, 1, UTFCF_DEF, UTFC_BUF);
        TF_WC2U16(L'\x80',   0, ch, 1, UTFCF_CHK, UTFC_BUF);
        TF_WC2U16(L'a',  0, ch, 1, UTFCF_CHK, UTFC_BUF);
        TF_WC2U16(L'我', 0, ch, 1, UTFCF_CHK, UTFC_BUF);
        TF_WC2U16(L'我', 0, nullptr, 1, UTFCF_CHK, UTFC_BUF);

#if WCHAR_IS_UTF32
        // 2
        T0_WC2U16(L'𭬨',  2, u"𭬨", 2, UTFCF_CHK);
        T0_WC2U16(L'🦊', 2, u"🦊", 2, UTFCF_CHK);
        T0_WC2U16(L'🦊', 2, u"🦊", 2, UTFCF_IGN);
        T0_WC2U16(L'🦊', 2, u"🦊", 2, UTFCF_DEF);

        // not error
        T0_WC2U16(L'\x110000', 2, u"\xD800\xDC00", 2, UTFCF_IGN);
        T0_WC2U16(L'\x110000', 2, u"\xFFFD", 1, UTFCF_DEF);

        // false case
        TF_WC2U16(L'\x110000', 2, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_WC2U16(U'\x110000', 0, ch, 2, UTFCF_IGN, UTFC_BUF);
        TF_WC2U16(L'𭬨',       1, ch, 2, UTFCF_CHK, UTFC_BUF);
        TF_WC2U16(L'🦊',      1, ch, 2, UTFCF_CHK, UTFC_BUF);
        TF_WC2U16(L'🦊',      1, nullptr, 2, UTFCF_CHK, UTFC_BUF);
#endif
        return true;
    };

    TEST_DEF("WCHAR  Char -> UTF-32") {
        char32_t ch;
        ch = 0;

        TEST_TRUE(wcharc_to_utf32(L'\0', &ch, UTFCF_CHK) == 0 && ch == U'\0');
        TEST_TRUE(wcharc_to_utf32(L'我', &ch, UTFCF_CHK) == 0 && ch == U'我');
        TEST_TRUE(wcharc_to_utf32(L'我', &ch, UTFCF_IGN) == 0 && ch == U'我');
        TEST_TRUE(wcharc_to_utf32(L'我', &ch, UTFCF_DEF) == 0 && ch == U'我');
        TEST_TRUE(wcharc_to_utf32(L'\xD800', &ch, UTFCF_CHK) == UTFC_ERR);
        TEST_TRUE(wcharc_to_utf32(L'\xDFFF', &ch, UTFCF_CHK) == UTFC_ERR);

        // not error
        TEST_TRUE(wcharc_to_utf32(L'\xD800', &ch, UTFCF_IGN) == 0 && ch == U'\xD800');
        TEST_TRUE(wcharc_to_utf32(L'\xDFFF', &ch, UTFCF_IGN) == 0 && ch == U'\xDFFF');
        TEST_TRUE(wcharc_to_utf32(L'\xDFFF', &ch, UTFCF_DEF) == 0 && ch == U'\xFFFD');

#if WCHAR_IS_UTF32
        TEST_TRUE(wcharc_to_utf32(L'🦊', &ch, UTFCF_CHK) == 0 && ch == U'🦊');
        TEST_TRUE(wcharc_to_utf32(L'🦊', &ch, UTFCF_IGN) == 0 && ch == U'🦊');
        TEST_TRUE(wcharc_to_utf32(L'🦊', &ch, UTFCF_DEF) == 0 && ch == U'🦊');
        TEST_TRUE(wcharc_to_utf32(L'\x110000', &ch, UTFCF_CHK) == UTFC_ERR);

        // not error
        TEST_TRUE(wcharc_to_utf32(L'\x110000', &ch, UTFCF_DEF) == 0 && ch == U'\xFFFD');
#endif
        return true;
    };

    TEST_DEF("UTF-8  Char -> WCHAR") {
        wchar_t ch;

        TEST_TRUE(utf8c_to_wchar('a', &ch, UTFCF_CHK) == 0 && ch == L'a');
        TEST_TRUE(utf8c_to_wchar('a', &ch, UTFCF_IGN) == 0 && ch == L'a');
        TEST_TRUE(utf8c_to_wchar('a', &ch, UTFCF_DEF) == 0 && ch == L'a');

        // not error
        TEST_TRUE(utf8c_to_wchar('\x80', &ch, UTFCF_IGN) == 0 && ch == L'\x80');
        TEST_TRUE(utf8c_to_wchar('\x80', &ch, UTFCF_DEF) == 0 && ch == L'\xFFFD');

        // false case
        TEST_TRUE(utf8c_to_wchar('\x80', &ch, UTFCF_CHK) == UTFC_ERR);

        return true;
    };

    TEST_DEF("UTF-16 Char -> WCHAR") {
        wchar_t ch;

        TEST_TRUE(utf16c_to_wchar(u'a', &ch, UTFCF_CHK) == 0 && ch == L'a');
        TEST_TRUE(utf16c_to_wchar(u'我', &ch, UTFCF_CHK) == 0 && ch == L'我');
        TEST_TRUE(utf16c_to_wchar(u'我', &ch, UTFCF_IGN) == 0 && ch == L'我');
        TEST_TRUE(utf16c_to_wchar(u'我', &ch, UTFCF_DEF) == 0 && ch == L'我');
        TEST_TRUE(utf16c_to_wchar(u'\xD800', &ch, UTFCF_CHK) == UTFC_ERR);
        TEST_TRUE(utf16c_to_wchar(u'\xDFFF', &ch, UTFCF_CHK) == UTFC_ERR);

        // not error
        TEST_TRUE(utf16c_to_wchar(u'\xD800', &ch, UTFCF_IGN) == 0 && ch == L'\xD800');
        TEST_TRUE(utf16c_to_wchar(u'\xDFFF', &ch, UTFCF_IGN) == 0 && ch == L'\xDFFF');
        TEST_TRUE(utf16c_to_wchar(u'\xDFFF', &ch, UTFCF_DEF) == 0 && ch == L'\xFFFD');

        return true;
    };

    TEST_DEF("UTF-32 Char -> WCHAR") {
        wchar_t ch[2];
        size_t len;

        // 1
        T0_U32C2W(U'a',  2, L"a", 1, UTFCF_CHK);
        T0_U32C2W(U'我', 2, L"我", 1, UTFCF_CHK);
        T0_U32C2W(U'我', 2, L"我", 1, UTFCF_IGN);
        T0_U32C2W(U'我', 2, L"我", 1, UTFCF_DEF);
#if WCHAR_IS_UTF16
        // 2
        T0_U32C2W(U'𭬨',  2, L"𭬨", 2, UTFCF_CHK);
        T0_U32C2W(U'🦊', 2, L"🦊", 2, UTFCF_CHK);
        T0_U32C2W(U'🦊', 2, L"🦊", 2, UTFCF_IGN);
        T0_U32C2W(U'🦊', 2, L"🦊", 2, UTFCF_DEF);
#elif WCHAR_IS_UTF32
        T0_U32C2W(U'𭬨',  2, L"𭬨", 1, UTFCF_CHK);
        T0_U32C2W(U'🦊', 2, L"🦊", 1, UTFCF_CHK);
        T0_U32C2W(U'🦊', 2, L"🦊", 1, UTFCF_IGN);
        T0_U32C2W(U'🦊', 2, L"🦊", 1, UTFCF_DEF);
#else
        return false;
#endif

        // not error
        T0_U32C2W(U'\xD800',   2, L"\xD800", 1, UTFCF_IGN);
        T0_U32C2W(U'\xDFFF',   2, L"\xDFFF", 1, UTFCF_IGN);
#if WCHAR_IS_UTF16
        T0_U32C2W(U'\x110000', 2, L"\xD800\xDC00", 2, UTFCF_IGN);
        T0_U32C2W(U'\x110000', 2, L"\xFFFD", 1, UTFCF_DEF);
#elif WCHAR_IS_UTF32
        T0_U32C2W(U'\x110000', 2, L"\x110000", 1, UTFCF_IGN);
        T0_U32C2W(U'\x110000', 2, L"\xFFFD", 1, UTFCF_DEF);
#else
        return false;
#endif

        // false case
        TF_U32C2W(U'\xD800',   2, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32C2W(U'\xDFFF',   2, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32C2W(U'\xDFFF',   0, ch, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32C2W(U'\xDFFF',   0, ch, 1, UTFCF_IGN, UTFC_BUF);
        TF_U32C2W(U'\xDFFF',   0, ch, 1, UTFCF_DEF, UTFC_BUF);
        TF_U32C2W(U'\x110000', 2, ch, 0, UTFCF_CHK, UTFC_ERR);
#if WCHAR_IS_UTF16
        TF_U32C2W(U'\x110000', 0, ch, 2, UTFCF_IGN, UTFC_BUF);
#elif WCHAR_IS_UTF32
        TF_U32C2W(U'\x110000', 0, ch, 1, UTFCF_IGN, UTFC_BUF);
#else
        return false;
#endif
        TF_U32C2W(U'a',    0, ch, 1, UTFCF_CHK, UTFC_BUF);
        TF_U32C2W(U'\x80', 0, ch, 1, UTFCF_CHK, UTFC_BUF);
        TF_U32C2W(U'我',   0, ch, 1, UTFCF_CHK, UTFC_BUF);
#if WCHAR_IS_UTF16
        TF_U32C2W(U'𭬨',  1, ch, 2, UTFCF_CHK, UTFC_BUF);
        TF_U32C2W(U'🦊', 1, ch, 2, UTFCF_CHK, UTFC_BUF);
        TF_U32C2W(U'🦊', 1, nullptr, 2, UTFCF_CHK, UTFC_BUF);
#elif WCHAR_IS_UTF32
        TF_U32C2W(U'𭬨',  0, ch, 1, UTFCF_CHK, UTFC_BUF);
        TF_U32C2W(U'🦊', 0, ch, 1, UTFCF_CHK, UTFC_BUF);
        TF_U32C2W(U'🦊', 1, nullptr, 1, UTFCF_CHK, UTFC_BUF);
#else
        return false;
#endif
        return true;
    };

    TEST_DEF("UTF-8  -> UTF-16") {
        char16_t buf[72];
        size_t len;

        T0_U8_16(u8"",    72, u"",    0, UTFCF_CHK);
        T0_U8_16(u8"a",   72, u"a",   1, UTFCF_CHK);
        T0_U8_16(u8"ab",  72, u"ab",  2, UTFCF_CHK);
        T0_U8_16(u8" ",   72, u" ",   1, UTFCF_CHK);
        T0_U8_16(u8" a ", 72, u" a ", 3, UTFCF_CHK);
        T0_U8_16(u8"abcdefg",       72, u"abcdefg", 7, UTFCF_CHK);
        T0_U8_16(u8"这是 一行文本。", 72, u"这是 一行文本。", 8, UTFCF_CHK);
        T0_U8_16(u8"a鷗🦊",         72, u"a鷗🦊", 4, UTFCF_CHK);
        T0_U8_16(u8"a鷗🦊",         72, u"a鷗🦊", 4, UTFCF_IGN);
        T0_U8_16(u8"a鷗🦊",         72, u"a鷗🦊", 4, UTFCF_DEF);
        T0_U8_16(
            u8"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            72,
            u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            69, UTFCF_CHK);

        // not error
        // MSVC 存在的 bug 导致16进制数无法正常写在 u8"" 字符串中
        T0_U8_16("\xE6\x88\x91\xC2", 8, u"我\xC2", 2, UTFCF_IGN);
        T0_U8_16("\xE6\x88\x91\xC2", 8, u"我\xFFFD", 2, UTFCF_DEF);

        // false case
        TF_U8_16(u8"abcdefg", 6, buf,     7, UTFCF_CHK, UTFC_BUF);
        TF_U8_16(u8"abcdefg", 6, nullptr, 7, UTFCF_CHK, UTFC_BUF);
        TF_U8_16(u8"a鷗🦊",   2, buf,     4, UTFCF_CHK, UTFC_BUF);
        TF_U8_16(u8"a鷗🦊",   2, nullptr, 4, UTFCF_CHK, UTFC_BUF);
        // MSVC 存在的 bug 导致16进制数无法正常写在 u8"" 字符串中
        TF_U8_16("\xE6\x88\x91\xC2", 1, buf,     0, UTFCF_CHK, UTFC_ERR);  // 我\xC2
        TF_U8_16("\xE6\x88\x91\xC2", 0, buf,     2, UTFCF_IGN, UTFC_BUF);  // 我\xC2
        TF_U8_16("\xE6\x88\x91\xC2", 0, buf,     2, UTFCF_DEF, UTFC_BUF);  // 我\xC2
        TF_U8_16("\xE6\x88\x91\xC2", 2, nullptr, 0, UTFCF_CHK, UTFC_ERR);  // 我\xC2
        TF_U8_16("\xE6\x88\x91\xC2", 1, nullptr, 2, UTFCF_IGN, UTFC_BUF);  // 我\xC2
        TF_U8_16("\xE6\x88\x91\xC2", 1, nullptr, 2, UTFCF_DEF, UTFC_BUF);  // 我\xC2

        return true;
    };

    TEST_DEF("UTF-8  -> UTF-32") {
        char32_t buf[48];
        size_t len;

        T0_U8_32(u8"",    48, U"",    0, UTFCF_CHK);
        T0_U8_32(u8"a",   48, U"a",   1, UTFCF_CHK);
        T0_U8_32(u8"ab",  48, U"ab",  2, UTFCF_CHK);
        T0_U8_32(u8" ",   48, U" ",   1, UTFCF_CHK);
        T0_U8_32(u8" a ", 48, U" a ", 3, UTFCF_CHK);
        T0_U8_32(u8"abcdefg",       48, U"abcdefg", 7, UTFCF_CHK);
        T0_U8_32(u8"这是 一行文本。", 48, U"这是 一行文本。", 8, UTFCF_CHK);
        T0_U8_32(u8"a鷗🦊",         48, U"a鷗🦊", 3, UTFCF_CHK);
        T0_U8_32(u8"a鷗🦊",         48, U"a鷗🦊", 3, UTFCF_IGN);
        T0_U8_32(u8"a鷗🦊",         48, U"a鷗🦊", 3, UTFCF_DEF);
        T0_U8_32(
            u8"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            48,
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            43, UTFCF_CHK);

        // not error
        T0_U8_32("\xE6\x88\x91\xC2", 8, U"我\xC2", 2, UTFCF_IGN);
        T0_U8_32("\xE6\x88\x91\xC2", 8, U"我\xFFFD", 2, UTFCF_DEF);

        // false case
        TF_U8_32(u8"abcdefg", 6, buf,     7, UTFCF_CHK, UTFC_BUF);
        TF_U8_32(u8"abcdefg", 6, nullptr, 7, UTFCF_CHK, UTFC_BUF);
        TF_U8_32(u8"a鷗🦊",   2, buf,     3, UTFCF_CHK, UTFC_BUF);
        TF_U8_32(u8"a鷗🦊",   2, nullptr, 3, UTFCF_CHK, UTFC_BUF);
        // MSVC 存在的 bug 导致16进制数无法正常写在 u8"" 字符串中
        TF_U8_32("\xE6\x88\x91\xC2", 2, buf,     0, UTFCF_CHK, UTFC_ERR);  // 我\xC2
        TF_U8_32("\xE6\x88\x91\xC2", 0, buf,     2, UTFCF_IGN, UTFC_BUF);  // 我\xC2
        TF_U8_32("\xE6\x88\x91\xC2", 0, buf,     2, UTFCF_DEF, UTFC_BUF);  // 我\xC2
        TF_U8_32("\xE6\x88\x91\xC2", 2, nullptr, 0, UTFCF_CHK, UTFC_ERR);  // 我\xC2
        TF_U8_32("\xE6\x88\x91\xC2", 1, nullptr, 2, UTFCF_IGN, UTFC_BUF);  // 我\xC2
        TF_U8_32("\xE6\x88\x91\xC2", 1, nullptr, 2, UTFCF_DEF, UTFC_BUF);  // 我\xC2

        return true;
    };

    TEST_DEF("UTF-16 -> UTF-8") {
        char buf[160];
        size_t len;

        T0_U16_8(u"",    160, u8"",    0, UTFCF_CHK);
        T0_U16_8(u"a",   160, u8"a",   1, UTFCF_CHK);
        T0_U16_8(u"ab",  160, u8"ab",  2, UTFCF_CHK);
        T0_U16_8(u" ",   160, u8" ",   1, UTFCF_CHK);
        T0_U16_8(u" a ", 160, u8" a ", 3, UTFCF_CHK);
        T0_U16_8(u"abcdefg",       160, u8"abcdefg", 7, UTFCF_CHK);
        T0_U16_8(u"这是 一行文本。", 160, u8"这是 一行文本。", 22, UTFCF_CHK);
        T0_U16_8(u"a鷗🦊",         160, u8"a鷗🦊", 8, UTFCF_CHK);
        T0_U16_8(u"a鷗🦊",         160, u8"a鷗🦊", 8, UTFCF_IGN);
        T0_U16_8(u"a鷗🦊",         160, u8"a鷗🦊", 8, UTFCF_DEF);
        T0_U16_8(
            u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            160,
            u8"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            155, UTFCF_CHK);

        // not error
        T0_U16_8(u"我\xD800", 8, "\xE6\x88\x91\xED\xA0\x80", 6, UTFCF_IGN);
        T0_U16_8(u"我\xDFFF", 8, "\xE6\x88\x91\xED\xBF\xBF", 6, UTFCF_IGN);
        T0_U16_8(u"我\xDFFF", 8, "\xE6\x88\x91\xEF\xBF\xBD", 6, UTFCF_DEF);
        T0_U16_8(u"\x0040\xd83d\xd83d\xde00\x0040", 10, u8"@�😀@", 9, UTFCF_DEF);

        // false case
        TF_U16_8(u"abcdefg",   6, buf,     7, UTFCF_CHK, UTFC_BUF);
        TF_U16_8(u"abcdefg",   6, nullptr, 7, UTFCF_CHK, UTFC_BUF);
        TF_U16_8(u"a鷗🦊",     2, buf,     8, UTFCF_CHK, UTFC_BUF);
        TF_U16_8(u"a鷗🦊",     2, nullptr, 8, UTFCF_CHK, UTFC_BUF);
        TF_U16_8(u"我\xD800",  3, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_U16_8(u"我\xD800",  0, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_U16_8(u"我\xD800",  0, buf, 6, UTFCF_IGN, UTFC_BUF);
        TF_U16_8(u"我\xD800",  0, buf, 6, UTFCF_DEF, UTFC_BUF);
        TF_U16_8(u"我\xD800",  6, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_U16_8(u"我\xDFFF",  6, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_U16_8(u"我\xDFFF",  6, nullptr, 6, UTFCF_IGN, UTFC_BUF);
        TF_U16_8(u"我\xDFFF",  6, nullptr, 6, UTFCF_DEF, UTFC_BUF);

        return true;
    };

    TEST_DEF("UTF-16 -> UTF-32") {
        char32_t buf[48];
        size_t len;

        T0_U16_32(u"",    48, U"",    0, UTFCF_CHK);
        T0_U16_32(u"a",   48, U"a",   1, UTFCF_CHK);
        T0_U16_32(u"ab",  48, U"ab",  2, UTFCF_CHK);
        T0_U16_32(u" ",   48, U" ",   1, UTFCF_CHK);
        T0_U16_32(u" a ", 48, U" a ", 3, UTFCF_CHK);
        T0_U16_32(u"abcdefg",        48, U"abcdefg", 7, UTFCF_CHK);
        T0_U16_32(u"这是 一行文本。", 48, U"这是 一行文本。", 8, UTFCF_CHK);
        T0_U16_32(u"a鷗🦊",         48, U"a鷗🦊", 3, UTFCF_CHK);
        T0_U16_32(u"a鷗🦊",         48, U"a鷗🦊", 3, UTFCF_IGN);
        T0_U16_32(u"a鷗🦊",         48, U"a鷗🦊", 3, UTFCF_DEF);
        T0_U16_32(
            u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            48,
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            43, UTFCF_CHK);

        // not error
        T0_U16_32(u"我\xD800", 3, U"我\xD800", 2, UTFCF_IGN);
        T0_U16_32(u"我\xDFFF", 3, U"我\xDFFF", 2, UTFCF_IGN);
        T0_U16_32(u"我\xDFFF", 3, U"我\xFFFD", 2, UTFCF_DEF);

        // false case
        TF_U16_32(u"abcdefg", 6, buf,     7, UTFCF_CHK, UTFC_BUF);
        TF_U16_32(u"abcdefg", 6, nullptr, 7, UTFCF_CHK, UTFC_BUF);
        TF_U16_32(u"a鷗🦊",   2, buf,     3, UTFCF_CHK, UTFC_BUF);
        TF_U16_32(u"a鷗🦊",   2, nullptr, 3, UTFCF_CHK, UTFC_BUF);
        TF_U16_32(u"我\xD800", 2, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_U16_32(u"我\xD800", 0, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_U16_32(u"我\xD800", 0, buf, 2, UTFCF_IGN, UTFC_BUF);
        TF_U16_32(u"我\xD800", 0, buf, 2, UTFCF_DEF, UTFC_BUF);
        TF_U16_32(u"我\xD800", 2, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_U16_32(u"我\xDFFF", 2, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_U16_32(u"我\xDFFF", 2, nullptr, 2, UTFCF_IGN, UTFC_BUF);
        TF_U16_32(u"我\xDFFF", 2, nullptr, 2, UTFCF_DEF, UTFC_BUF);

        return true;
    };

    TEST_DEF("UTF-32 -> UTF-8") {
        char buf[160];
        size_t len;
        len = 1;

        T0_U32_8(U"",    160, u8"",    0, UTFCF_CHK);
        T0_U32_8(U"a",   160, u8"a",   1, UTFCF_CHK);
        T0_U32_8(U"ab",  160, u8"ab",  2, UTFCF_CHK);
        T0_U32_8(U" ",   160, u8" ",   1, UTFCF_CHK);
        T0_U32_8(U" a ", 160, u8" a ", 3, UTFCF_CHK);
        T0_U32_8(U"abcdefg",       160, u8"abcdefg", 7, UTFCF_CHK);
        T0_U32_8(U"这是 一行文本。", 160, u8"这是 一行文本。", 22, UTFCF_CHK);
        T0_U32_8(U"a鷗🦊",         160, u8"a鷗🦊", 8, UTFCF_CHK);
        T0_U32_8(U"a鷗🦊",         160, u8"a鷗🦊", 8, UTFCF_IGN);
        T0_U32_8(U"a鷗🦊",         160, u8"a鷗🦊", 8, UTFCF_DEF);
        T0_U32_8(
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            160,
            u8"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            155, UTFCF_CHK);

        // not error
        T0_U32_8(U"我\xD800",   7, "\xE6\x88\x91\xED\xA0\x80", 6, UTFCF_IGN);
        T0_U32_8(U"我\xDFFF",   7, "\xE6\x88\x91\xED\xBF\xBF", 6, UTFCF_IGN);
        T0_U32_8(U"我\x110000", 7, "\xE6\x88\x91\xF4\x90\x80\x80", 7, UTFCF_IGN);
        T0_U32_8(U"我\x110000", 7, "\xE6\x88\x91\xEF\xBF\xBD", 6, UTFCF_DEF);

        // false case
        TF_U32_8(U"abcdefg",   6, buf,     7, UTFCF_CHK, UTFC_BUF);
        TF_U32_8(U"abcdefg",   6, nullptr, 7, UTFCF_CHK, UTFC_BUF);
        TF_U32_8(U"a鷗🦊",     2, buf,     8, UTFCF_CHK, UTFC_BUF);
        TF_U32_8(U"a鷗🦊",     2, nullptr, 8, UTFCF_CHK, UTFC_BUF);
        TF_U32_8(U"我\xD800",   6, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32_8(U"我\xD800",   0, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32_8(U"我\xD800",   0, buf, 6, UTFCF_IGN, UTFC_BUF);
        TF_U32_8(U"我\xD800",   0, buf, 6, UTFCF_DEF, UTFC_BUF);
        TF_U32_8(U"我\xD800",   6, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32_8(U"我\xDFFF",   6, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32_8(U"我\xDFFF",   6, nullptr, 6, UTFCF_IGN, UTFC_BUF);
        TF_U32_8(U"我\xDFFF",   6, nullptr, 6, UTFCF_DEF, UTFC_BUF);
        TF_U32_8(U"我\x110000", 6, nullptr, 0, UTFCF_CHK, UTFC_ERR);

        return true;
    };

    TEST_DEF("UTF-32 -> UTF-16") {
        char16_t buf[72];
        size_t len;

        T0_U32_16(U"",    72, u"",    0, UTFCF_CHK);
        T0_U32_16(U"a",   72, u"a",   1, UTFCF_CHK);
        T0_U32_16(U"ab",  72, u"ab",  2, UTFCF_CHK);
        T0_U32_16(U" ",   72, u" ",   1, UTFCF_CHK);
        T0_U32_16(U" a ", 72, u" a ", 3, UTFCF_CHK);
        T0_U32_16(U"abcdefg",       72, u"abcdefg", 7, UTFCF_CHK);
        T0_U32_16(U"这是 一行文本。", 72, u"这是 一行文本。", 8, UTFCF_CHK);
        T0_U32_16(U"a鷗🦊",         72, u"a鷗🦊", 4, UTFCF_CHK);
        T0_U32_16(U"a鷗🦊",         72, u"a鷗🦊", 4, UTFCF_IGN);
        T0_U32_16(U"a鷗🦊",         72, u"a鷗🦊", 4, UTFCF_DEF);
        T0_U32_16(
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            72,
            u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            69, UTFCF_CHK);

        // not error
        T0_U32_16(U"我\xD800",   2, u"我\xD800",   2, UTFCF_IGN);
        T0_U32_16(U"我\xD800",   3, u"我\xD800",   2, UTFCF_IGN);
        T0_U32_16(U"我\x110000", 3, u"我\xD800\xDC00", 3, UTFCF_IGN);
        T0_U32_16(U"我\x110000", 3, u"我\xFFFD", 2, UTFCF_DEF);

        // false case
        TF_U32_16(U"abcdefg",   6, buf,     7, UTFCF_CHK, UTFC_BUF);
        TF_U32_16(U"abcdefg",   6, nullptr, 7, UTFCF_CHK, UTFC_BUF);
        TF_U32_16(U"a鷗🦊",     2, buf,     4, UTFCF_CHK, UTFC_BUF);
        TF_U32_16(U"a鷗🦊",     2, nullptr, 4, UTFCF_CHK, UTFC_BUF);
        TF_U32_16(U"我\xD800",   1, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32_16(U"我\xD800",   0, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32_16(U"我\xD800",   0, buf, 2, UTFCF_IGN, UTFC_BUF);
        TF_U32_16(U"我\xD800",   0, buf, 2, UTFCF_DEF, UTFC_BUF);
        TF_U32_16(U"我\xD800",   1, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32_16(U"我\xDFFF",   1, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32_16(U"我\x110000", 1, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32_16(U"我\x110000", 2, nullptr, 3, UTFCF_IGN, UTFC_BUF);
        TF_U32_16(U"我\x110000", 2, nullptr, 2, UTFCF_DEF, UTFC_BUF);

        return true;
    };

    TEST_DEF("WCHAR  -> UTF-8") {
        char buf[160];
        size_t len;

        T0_W_U8(L"",    160, u8"",    0, UTFCF_CHK);
        T0_W_U8(L"a",   160, u8"a",   1, UTFCF_CHK);
        T0_W_U8(L"ab",  160, u8"ab",  2, UTFCF_CHK);
        T0_W_U8(L" ",   160, u8" ",   1, UTFCF_CHK);
        T0_W_U8(L" a ", 160, u8" a ", 3, UTFCF_CHK);
        T0_W_U8(L"abcdefg",       160, u8"abcdefg", 7, UTFCF_CHK);
        T0_W_U8(L"这是 一行文本。", 160, u8"这是 一行文本。", 22, UTFCF_CHK);
        T0_W_U8(L"a鷗🦊",         160, u8"a鷗🦊", 8, UTFCF_CHK);
        T0_W_U8(L"a鷗🦊",         160, u8"a鷗🦊", 8, UTFCF_IGN);
        T0_W_U8(L"a鷗🦊",         160, u8"a鷗🦊", 8, UTFCF_DEF);
        T0_W_U8(
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            160,
            u8"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            155, UTFCF_CHK);

        // not error
        T0_W_U8(L"我\xD800", 7, "\xE6\x88\x91\xED\xA0\x80", 6, UTFCF_IGN);
        T0_W_U8(L"我\xDFFF", 6, "\xE6\x88\x91\xED\xBF\xBF", 6, UTFCF_IGN);
        T0_W_U8(L"我\xDFFF", 6, "\xE6\x88\x91\xEF\xBF\xBD", 6, UTFCF_DEF);
#if WCHAR_IS_UTF32
        T0_W_U8(L"我\x110000", 7, "\xE6\x88\x91\xF4\x90\x80\x80", 7, UTFCF_IGN);
        T0_W_U8(L"我\x110000", 7, "\xE6\x88\x91\xEF\xBF\xBD", 6, UTFCF_DEF);
#endif

        // false case
        TF_W_U8(L"abcdefg",  6, buf,     7, UTFCF_CHK, UTFC_BUF);
        TF_W_U8(L"abcdefg",  6, nullptr, 7, UTFCF_CHK, UTFC_BUF);
        TF_W_U8(L"a鷗🦊",    2, buf,     8, UTFCF_CHK, UTFC_BUF);
        TF_W_U8(L"a鷗🦊",    2, nullptr, 8, UTFCF_CHK, UTFC_BUF);
        TF_W_U8(L"我\xD800", 1, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_W_U8(L"我\xD800", 0, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_W_U8(L"我\xD800", 0, buf, 6, UTFCF_IGN, UTFC_BUF);
        TF_W_U8(L"我\xD800", 0, buf, 6, UTFCF_DEF, UTFC_BUF);
        TF_W_U8(L"我\xD800", 1, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_W_U8(L"我\xD800", 1, nullptr, 6, UTFCF_IGN, UTFC_BUF);
        TF_W_U8(L"我\xD800", 1, nullptr, 6, UTFCF_DEF, UTFC_BUF);
        TF_W_U8(L"我\xDFFF", 1, buf, 0, UTFCF_CHK, UTFC_ERR);
#if WCHAR_IS_UTF32
        TF_W_U8(L"我\x110000", 1, buf,     0, UTFCF_CHK, UTFC_ERR);
        TF_W_U8(L"我\x110000", 1, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_W_U8(L"我\x110000", 1, nullptr, 7, UTFCF_IGN, UTFC_BUF);
#endif

        return true;
    };

    TEST_DEF("WCHAR  -> UTF-16") {
        char16_t buf[72];
        size_t len;

        T0_W_U16(L"",    72, u"",    0, UTFCF_CHK);
        T0_W_U16(L"a",   72, u"a",   1, UTFCF_CHK);
        T0_W_U16(L"ab",  72, u"ab",  2, UTFCF_CHK);
        T0_W_U16(L" ",   72, u" ",   1, UTFCF_CHK);
        T0_W_U16(L" a ", 72, u" a ", 3, UTFCF_CHK);
        T0_W_U16(L"abcdefg",       72, u"abcdefg", 7, UTFCF_CHK);
        T0_W_U16(L"这是 一行文本。", 72, u"这是 一行文本。", 8, UTFCF_CHK);
        T0_W_U16(L"a鷗🦊",         72, u"a鷗🦊", 4, UTFCF_CHK);
        T0_W_U16(L"a鷗🦊",         72, u"a鷗🦊", 4, UTFCF_IGN);
        T0_W_U16(L"a鷗🦊",         72, u"a鷗🦊", 4, UTFCF_DEF);
        T0_W_U16(
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            72,
            u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            69, UTFCF_CHK);

        // not error
        T0_W_U16(L"我\xD800", 2, u"我\xD800", 2, UTFCF_IGN);
        T0_W_U16(L"我\xDFFF", 3, u"我\xDFFF", 2, UTFCF_IGN);
        T0_W_U16(L"我\xD800", 2, u"我\xFFFD", 2, UTFCF_DEF);
#if WCHAR_IS_UTF32
        T0_W_U16(L"我\x110000", 3, u"我\xD800\xDC00", 3, UTFCF_IGN);
        T0_W_U16(L"我\x110000", 2, u"我\xFFFD", 2, UTFCF_DEF);
#endif

        // false case
        TF_W_U16(L"abcdefg",  6, buf,     7, UTFCF_CHK, UTFC_BUF);
        TF_W_U16(L"abcdefg",  6, nullptr, 7, UTFCF_CHK, UTFC_BUF);
        TF_W_U16(L"a鷗🦊",    2, buf,     4, UTFCF_CHK, UTFC_BUF);
        TF_W_U16(L"a鷗🦊",    2, nullptr, 4, UTFCF_CHK, UTFC_BUF);
        TF_W_U16(L"我\xD800", 2, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_W_U16(L"我\xD800", 0, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_W_U16(L"我\xD800", 0, buf, 2, UTFCF_IGN, UTFC_BUF);
        TF_W_U16(L"我\xD800", 0, buf, 2, UTFCF_DEF, UTFC_BUF);
        TF_W_U16(L"我\xDFFF", 1, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_W_U16(L"我\xD800", 2, nullptr, 2, UTFCF_CHK, UTFC_ERR);
        TF_W_U16(L"我\xD800", 2, nullptr, 2, UTFCF_IGN, UTFC_BUF);
        TF_W_U16(L"我\xD800", 0, nullptr, 2, UTFCF_DEF, UTFC_BUF);
#if WCHAR_IS_UTF32
        TF_W_U16(L"我\x110000", 1, buf,     0, UTFCF_CHK, UTFC_ERR);
        TF_W_U16(L"我\x110000", 1, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_W_U16(L"我\x110000", 1, nullptr, 3, UTFCF_IGN, UTFC_BUF);
#endif

        return true;
    };

    TEST_DEF("WCHAR  -> UTF-32") {
        char32_t buf[48];
        size_t len;

        T0_W_U32(L"",    48, U"",    0, UTFCF_CHK);
        T0_W_U32(L"a",   48, U"a",   1, UTFCF_CHK);
        T0_W_U32(L"ab",  48, U"ab",  2, UTFCF_CHK);
        T0_W_U32(L" ",   48, U" ",   1, UTFCF_CHK);
        T0_W_U32(L" a ", 48, U" a ", 3, UTFCF_CHK);
        T0_W_U32(L"abcdefg",       48, U"abcdefg", 7, UTFCF_CHK);
        T0_W_U32(L"这是 一行文本。", 48, U"这是 一行文本。", 8, UTFCF_CHK);
        T0_W_U32(L"a鷗🦊",         48, U"a鷗🦊", 3, UTFCF_CHK);
        T0_W_U32(L"a鷗🦊",         48, U"a鷗🦊", 3, UTFCF_IGN);
        T0_W_U32(L"a鷗🦊",         48, U"a鷗🦊", 3, UTFCF_DEF);
        T0_W_U32(
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            48,
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            43, UTFCF_CHK);

        // not error
        T0_W_U32(L"我\xD800", 2, U"我\xD800", 2, UTFCF_IGN);
        T0_W_U32(L"我\xDFFF", 3, U"我\xDFFF", 2, UTFCF_IGN);
        T0_W_U32(L"我\xD800", 2, U"我\xFFFD", 2, UTFCF_DEF);
#if WCHAR_IS_UTF32
        T0_W_U32(L"我\x110000", 3, U"我\x110000", 2, UTFCF_IGN);
        T0_W_U32(L"我\x110000", 2, U"我\xFFFD", 2, UTFCF_DEF);
#endif

        // false case
        TF_W_U32(L"abcdefg",  6, buf,     7, UTFCF_CHK, UTFC_BUF);
        TF_W_U32(L"abcdefg",  6, nullptr, 7, UTFCF_CHK, UTFC_BUF);
        TF_W_U32(L"a鷗🦊",    2, buf,     3, UTFCF_CHK, UTFC_BUF);
        TF_W_U32(L"a鷗🦊",    2, nullptr, 3, UTFCF_CHK, UTFC_BUF);
        TF_W_U32(L"我\xD800", 1, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_W_U32(L"我\xD800", 0, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_W_U32(L"我\xD800", 0, buf, 2, UTFCF_IGN, UTFC_BUF);
        TF_W_U32(L"我\xD800", 0, buf, 2, UTFCF_DEF, UTFC_BUF);
        TF_W_U32(L"我\xDFFF", 1, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_W_U32(L"我\xD800", 1, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_W_U32(L"我\xD800", 2, nullptr, 2, UTFCF_IGN, UTFC_BUF);
        TF_W_U32(L"我\xD800", 0, nullptr, 2, UTFCF_DEF, UTFC_BUF);
#if WCHAR_IS_UTF32
        TF_W_U32(L"我\x110000", 1, buf,     0, UTFCF_CHK, UTFC_ERR);
        TF_W_U32(L"我\x110000", 1, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_W_U32(L"我\x110000", 1, nullptr, 2, UTFCF_IGN, UTFC_BUF);
#endif

        return true;
    };

    TEST_DEF("UTF-8  -> WCHAR") {
        wchar_t buf[72];
        size_t len;

        T0_U8_W(u8"",    72, L"",    0, UTFCF_CHK);
        T0_U8_W(u8"a",   72, L"a",   1, UTFCF_CHK);
        T0_U8_W(u8"ab",  72, L"ab",  2, UTFCF_CHK);
        T0_U8_W(u8" ",   72, L" ",   1, UTFCF_CHK);
        T0_U8_W(u8" a ", 72, L" a ", 3, UTFCF_CHK);
        T0_U8_W(u8"abcdefg",       72, L"abcdefg", 7, UTFCF_CHK);
        T0_U8_W(u8"这是 一行文本。", 72, L"这是 一行文本。", 8, UTFCF_CHK);
#if WCHAR_IS_UTF16
        T0_U8_W(u8"a鷗🦊",         72, L"a鷗🦊", 4, UTFCF_CHK);
        T0_U8_W(u8"a鷗🦊",         72, L"a鷗🦊", 4, UTFCF_IGN);
        T0_U8_W(u8"a鷗🦊",         72, L"a鷗🦊", 4, UTFCF_DEF);
        T0_U8_W(
            u8"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            72,
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            69, UTFCF_CHK);
#elif WCHAR_IS_UTF32
        T0_U8_W(u8"a鷗🦊", 72, L"a鷗🦊", 3, UTFCF_CHK);
        T0_U8_W(u8"a鷗🦊", 72, L"a鷗🦊", 3, UTFCF_IGN);
        T0_U8_W(u8"a鷗🦊", 72, L"a鷗🦊", 3, UTFCF_DEF);
        T0_U8_W(
            u8"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            72,
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            43, UTFCF_CHK);
#else
        return false;
#endif

        // not error
        // MSVC 存在的 bug 导致16进制数无法正常写在 u8"" 字符串中
        T0_U8_W("\xE6\x88\x91\xC2", 8, L"我\xC2", 2, UTFCF_IGN);
        T0_U8_W("\xE6\x88\x91\xC2", 8, L"我\xFFFD", 2, UTFCF_DEF);

        // false case
        TF_U8_W(u8"abcdefg", 6, buf,     7, UTFCF_CHK, UTFC_BUF);
        TF_U8_W(u8"abcdefg", 6, nullptr, 7, UTFCF_CHK, UTFC_BUF);
#if WCHAR_IS_UTF16
        TF_U8_W(u8"a鷗🦊", 2, buf, 4, UTFCF_CHK, UTFC_BUF);
        TF_U8_W(u8"a鷗🦊", 2, nullptr, 4, UTFCF_CHK, UTFC_BUF);
#elif WCHAR_IS_UTF32
        TF_U8_W(u8"a鷗🦊", 2, buf, 3, UTFCF_CHK, UTFC_BUF);
        TF_U8_W(u8"a鷗🦊", 2, nullptr, 3, UTFCF_CHK, UTFC_BUF);
#else
        return false;
#endif
        // MSVC 存在的 bug 导致16进制数无法正常写在 u8"" 字符串中
        TF_U8_W("\xE6\x88\x91\xC2", 1, buf, 0, UTFCF_CHK, UTFC_ERR);  // 我\xC2
        TF_U8_W("\xE6\x88\x91\xC2", 0, buf, 2, UTFCF_IGN, UTFC_BUF);  // 我\xC2
        TF_U8_W("\xE6\x88\x91\xC2", 0, buf, 2, UTFCF_DEF, UTFC_BUF);  // 我\xC2
        TF_U8_W("\xE6\x88\x91\xC2", 1, nullptr, 0, UTFCF_CHK, UTFC_ERR);  // 我\xC2
        TF_U8_W("\xE6\x88\x91\xC2", 1, nullptr, 2, UTFCF_IGN, UTFC_BUF);  // 我\xC2
        TF_U8_W("\xE6\x88\x91\xC2", 1, nullptr, 2, UTFCF_DEF, UTFC_BUF);  // 我\xC2

        return true;
    };
    
    TEST_DEF("UTF-16 -> WCHAR") {
        wchar_t buf[72];
        size_t len;

        T0_U16_W(u"",    72, L"",    0, UTFCF_CHK);
        T0_U16_W(u"a",   72, L"a",   1, UTFCF_CHK);
        T0_U16_W(u"ab",  72, L"ab",  2, UTFCF_CHK);
        T0_U16_W(u" ",   72, L" ",   1, UTFCF_CHK);
        T0_U16_W(u" a ", 72, L" a ", 3, UTFCF_CHK);
        T0_U16_W(u"abcdefg",       72, L"abcdefg", 7, UTFCF_CHK);
        T0_U16_W(u"这是 一行文本。", 72, L"这是 一行文本。", 8, UTFCF_CHK);
#if WCHAR_IS_UTF16
        T0_U16_W(u"a鷗🦊", 72, L"a鷗🦊", 4, UTFCF_CHK);
        T0_U16_W(u"a鷗🦊", 72, L"a鷗🦊", 4, UTFCF_IGN);
        T0_U16_W(u"a鷗🦊", 72, L"a鷗🦊", 4, UTFCF_DEF);
        T0_U16_W(
            u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            72,
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            69, UTFCF_CHK);
#elif WCHAR_IS_UTF32
        T0_U16_W(u"a鷗🦊", 72, L"a鷗🦊", 3, UTFCF_CHK);
        T0_U16_W(u"a鷗🦊", 72, L"a鷗🦊", 3, UTFCF_IGN);
        T0_U16_W(u"a鷗🦊", 72, L"a鷗🦊", 3, UTFCF_DEF);
        T0_U16_W(
            u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            72,
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            43, UTFCF_CHK);
#else
        return false;
#endif

        // not error
        T0_U16_W(u"我\xD800", 3, L"我\xD800", 2, UTFCF_IGN);
        T0_U16_W(u"我\xDFFF", 3, L"我\xDFFF", 2, UTFCF_IGN);
        T0_U16_W(u"我\xDFFF", 3, L"我\xFFFD", 2, UTFCF_DEF);

        // false case
        TF_U16_W(u"abcdefg", 6, buf,     7, UTFCF_CHK, UTFC_BUF);
        TF_U16_W(u"abcdefg", 6, nullptr, 7, UTFCF_CHK, UTFC_BUF);
#if WCHAR_IS_UTF16
        TF_U16_W(u"a鷗🦊", 2, buf,     4, UTFCF_CHK, UTFC_BUF);
        TF_U16_W(u"a鷗🦊", 2, nullptr, 4, UTFCF_CHK, UTFC_BUF);
#elif WCHAR_IS_UTF32
        TF_U16_W(u"a鷗🦊", 2, buf,     3, UTFCF_CHK, UTFC_BUF);
        TF_U16_W(u"a鷗🦊", 2, nullptr, 3, UTFCF_CHK, UTFC_BUF);
#else
        return false;
#endif
        TF_U16_W(u"我\xD800", 1, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_U16_W(u"我\xD800", 0, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_U16_W(u"我\xD800", 0, buf, 2, UTFCF_IGN, UTFC_BUF);
        TF_U16_W(u"我\xD800", 0, buf, 2, UTFCF_DEF, UTFC_BUF);
        TF_U16_W(u"我\xD800", 2, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_U16_W(u"我\xD800", 1, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_U16_W(u"我\xDFFF", 1, buf, 0, UTFCF_CHK, UTFC_ERR);
        TF_U16_W(u"我\xDFFF", 2, nullptr, 2, UTFCF_IGN, UTFC_BUF);
        TF_U16_W(u"我\xDFFF", 2, nullptr, 2, UTFCF_DEF, UTFC_BUF);

        return true;
    };
    
    TEST_DEF("UTF-32 -> WCHAR") {
        wchar_t buf[72];
        size_t len;

        T0_U32_W(U"",    72, L"",    0, UTFCF_CHK);
        T0_U32_W(U"a",   72, L"a",   1, UTFCF_CHK);
        T0_U32_W(U"ab",  72, L"ab",  2, UTFCF_CHK);
        T0_U32_W(U" ",   72, L" ",   1, UTFCF_CHK);
        T0_U32_W(U" a ", 72, L" a ", 3, UTFCF_CHK);
        T0_U32_W(U"abcdefg",       72, L"abcdefg", 7, UTFCF_CHK);
        T0_U32_W(U"这是 一行文本。", 72, L"这是 一行文本。", 8, UTFCF_CHK);
#if WCHAR_IS_UTF16
        T0_U32_W(U"a鷗🦊", 72, L"a鷗🦊", 4, UTFCF_CHK);
        T0_U32_W(U"a鷗🦊", 72, L"a鷗🦊", 4, UTFCF_IGN);
        T0_U32_W(U"a鷗🦊", 72, L"a鷗🦊", 4, UTFCF_DEF);
        T0_U32_W(
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            72,
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            69, UTFCF_CHK);
#elif WCHAR_IS_UTF32
        T0_U32_W(U"a鷗🦊", 72, L"a鷗🦊", 3, UTFCF_CHK);
        T0_U32_W(U"a鷗🦊", 72, L"a鷗🦊", 3, UTFCF_IGN);
        T0_U32_W(U"a鷗🦊", 72, L"a鷗🦊", 3, UTFCF_DEF);
        T0_U32_W(
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            72,
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            43, UTFCF_CHK);
#else
        return false;
#endif

        // not error
        T0_U32_W(U"我\xD800",   2, L"我\xD800", 2, UTFCF_IGN);
        T0_U32_W(U"我\xD800",   3, L"我\xD800", 2, UTFCF_IGN);
#if WCHAR_IS_UTF16
        T0_U32_W(U"我\x110000", 3, L"我\xD800\xDC00", 3, UTFCF_IGN);
#elif WCHAR_IS_UTF32
        T0_U32_W(U"我\x110000", 3, L"我\x110000", 2, UTFCF_IGN);
#else
        return false;
#endif
        T0_U32_W(U"我\x110000", 3, L"我\xFFFD", 2, UTFCF_DEF);

        // false case
        TF_U32_W(U"abcdefg", 6, buf,     7, UTFCF_CHK, UTFC_BUF);
        TF_U32_W(U"abcdefg", 6, nullptr, 7, UTFCF_CHK, UTFC_BUF);
#if WCHAR_IS_UTF16
        TF_U32_W(U"a鷗🦊", 2, buf,     4, UTFCF_CHK, UTFC_BUF);
        TF_U32_W(U"a鷗🦊", 2, nullptr, 4, UTFCF_CHK, UTFC_BUF);
#elif WCHAR_IS_UTF32
        TF_U32_W(U"a鷗🦊", 2, buf,     3, UTFCF_CHK, UTFC_BUF);
        TF_U32_W(U"a鷗🦊", 2, nullptr, 3, UTFCF_CHK, UTFC_BUF);
#else
        return false;
#endif

        TF_U32_W(U"我\xD800",   1, buf,     0, UTFCF_CHK, UTFC_ERR);
        TF_U32_W(U"我\xD800",   0, buf,     0, UTFCF_CHK, UTFC_ERR);
        TF_U32_W(U"我\xD800",   0, buf,     2, UTFCF_IGN, UTFC_BUF);
        TF_U32_W(U"我\xD800",   0, buf,     2, UTFCF_DEF, UTFC_BUF);
        TF_U32_W(U"我\xD800",   1, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32_W(U"我\xDFFF",   1, nullptr, 0, UTFCF_CHK, UTFC_ERR);
        TF_U32_W(U"我\x110000", 1, buf,     0, UTFCF_CHK, UTFC_ERR);
        TF_U32_W(U"我\x110000", 1, nullptr, 0, UTFCF_CHK, UTFC_ERR);
#if WCHAR_IS_UTF16
        TF_U32_W(U"我\x110000", 2, nullptr, 3, UTFCF_IGN, UTFC_BUF);
#elif WCHAR_IS_UTF32
        TF_U32_W(U"我\x110000", 2, nullptr, 2, UTFCF_IGN, UTFC_BUF);
#else
        return false;
#endif
        TF_U32_W(U"我\x110000", 2, nullptr, 2, UTFCF_DEF, UTFC_BUF);

        return true;
    };

}
