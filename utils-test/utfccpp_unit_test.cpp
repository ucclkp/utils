// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/strings/string_utils.hpp"
#include "utils/strings/utfccpp.h"
#include "utils/unit_test/test_collector.h"

// UTF-8 到 UTF-16 正常转换
#define T0_U8_16(src, dst, f) \
    TEST_E(utf8_to_utf16((const char*)src, &buf, f), 0);  \
    TEST_E(buf, dst);

// UTF-8 到 UTF-16 错误转换
#define TF_U8_16(src, f, ret) \
    TEST_E(utf8_to_utf16((const char*)src, &buf, f), ret);

// UTF-8 到 UTF-32 正常转换
#define T0_U8_32(src, dst, f) \
    TEST_E(utf8_to_utf32((const char*)src, &buf, f), 0);  \
    TEST_E(buf, dst);

// UTF-8 到 UTF-32 错误转换
#define TF_U8_32(src, f, ret) \
    TEST_E(utf8_to_utf32((const char*)src, &buf, f), ret);

// UTF-16 到 UTF-8 正常转换
#define T0_U16_8(src, dst, f) \
    TEST_E(utf16_to_utf8(src, &buf, f), 0);  \
    TEST_E(buf, dst);

// UTF-16 到 UTF-8 错误转换
#define TF_U16_8(src, f, ret) \
    TEST_E(utf16_to_utf8(src, &buf, f), ret);

// UTF-16 到 UTF-32 正常转换
#define T0_U16_32(src, dst, f) \
    TEST_E(utf16_to_utf32(src, &buf, f), 0);  \
    TEST_E(buf, dst);

// UTF-16 到 UTF-32 错误转换
#define TF_U16_32(src, f, ret) \
    TEST_E(utf16_to_utf32(src, &buf, f), ret);

// UTF-32 到 UTF-8 正常转换
#define T0_U32_8(src, dst, f) \
    TEST_E(utf32_to_utf8(src, &buf, f), 0);  \
    TEST_E(buf, dst);

// UTF-32 到 UTF-8 错误转换
#define TF_U32_8(src, f, ret) \
    TEST_E(utf32_to_utf8(src, &buf, f), ret);

// UTF-32 到 UTF-16 正常转换
#define T0_U32_16(src, dst, f) \
    TEST_E(utf32_to_utf16(src, &buf, f), 0);  \
    TEST_E(buf, dst);

// UTF-32 到 UTF-16 错误转换
#define TF_U32_16(src, f, ret) \
    TEST_E(utf32_to_utf16(src, &buf, f), ret);

// WCHAR 到 UTF-8 正常转换
#define T0_W_U8(src, dst, f) \
    TEST_E(wchar_to_utf8(src, &buf, f), 0);  \
    TEST_E(buf, dst);

// WCHAR 到 UTF-8 错误转换
#define TF_W_U8(src, f, ret) \
    TEST_E(wchar_to_utf8(src, &buf, f), ret);

// WCHAR 到 UTF-16 正常转换
#define T0_W_U16(src, dst, f) \
    TEST_E(wchar_to_utf16(src, &buf, f), 0);  \
    TEST_E(buf, dst);

// WCHAR 到 UTF-16 错误转换
#define TF_W_U16(src, f, ret) \
    TEST_E(wchar_to_utf16(src, &buf, f), ret);

// WCHAR 到 UTF-32 正常转换
#define T0_W_U32(src, dst, f) \
    TEST_E(wchar_to_utf32(src, &buf, f), 0);  \
    TEST_E(buf, dst);

// WCHAR 到 UTF-32 错误转换
#define TF_W_U32(src, f, ret) \
    TEST_E(wchar_to_utf32(src, &buf, f), ret);

// UTF-8 到 WCHAR 正常转换
#define T0_U8_W(src, dst, f) \
    TEST_E(utf8_to_wchar((const char*)src, &buf, f), 0);  \
    TEST_E(buf, dst);

// UTF-8 到 WCHAR 错误转换
#define TF_U8_W(src, f, ret) \
    TEST_E(utf8_to_wchar((const char*)src, &buf, f), ret);

// UTF-16 到 WCHAR 正常转换
#define T0_U16_W(src, dst, f) \
    TEST_E(utf16_to_wchar(src, &buf, f), 0);  \
    TEST_E(buf, dst);

// UTF-16 到 WCHAR 错误转换
#define TF_U16_W(src, f, ret) \
    TEST_E(utf16_to_wchar(src, &buf, f), ret);

// UTF-32 到 WCHAR 正常转换
#define T0_U32_W(src, dst, f) \
    TEST_E(utf32_to_wchar(src, &buf, f), 0);  \
    TEST_E(buf, dst);

// UTF-32 到 WCHAR 错误转换
#define TF_U32_W(src, f, ret) \
    TEST_E(utf32_to_wchar(src, &buf, f), ret);


using namespace utl;

TEST_CASE(UTFCCPPUnitTest) {

    TEST_DEF("UTF-8  -> UTF-16") {
        std::u16string buf;

        T0_U8_16(u8"",    u"",    UTFCF_CHK);
        T0_U8_16(u8"a",   u"a",   UTFCF_CHK);
        T0_U8_16(u8"ab",  u"ab",  UTFCF_CHK);
        T0_U8_16(u8" ",   u" ",   UTFCF_CHK);
        T0_U8_16(u8" a ", u" a ", UTFCF_CHK);
        T0_U8_16(u8"abcdefg", u"abcdefg", UTFCF_CHK);
        T0_U8_16(u8"这是 一行文本。", u"这是 一行文本。", UTFCF_CHK);
        T0_U8_16(u8"a鷗🦊", u"a鷗🦊", UTFCF_CHK);
        T0_U8_16(u8"a鷗🦊", u"a鷗🦊", UTFCF_IGN);
        T0_U8_16(u8"a鷗🦊", u"a鷗🦊", UTFCF_DEF);
        T0_U8_16(
            u8"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            UTFCF_CHK);

        // not error
        // MSVC 存在的 bug 导致16进制数无法正常写在 u8"" 字符串中
        T0_U8_16("\xE6\x88\x91\xC2", u"我\xC2",   UTFCF_IGN);
        T0_U8_16("\xE6\x88\x91\xC2", u"我\xFFFD", UTFCF_DEF);

        // false case
        // MSVC 存在的 bug 导致16进制数无法正常写在 u8"" 字符串中
        TF_U8_16("\xE6\x88\x91\xC2", UTFCF_CHK, UTFC_ERR);  // 我\xC2

        return true;
    };

    TEST_DEF("UTF-8  -> UTF-32") {
        std::u32string buf;

        T0_U8_32(u8"",    U"",    UTFCF_CHK);
        T0_U8_32(u8"a",   U"a",   UTFCF_CHK);
        T0_U8_32(u8"ab",  U"ab",  UTFCF_CHK);
        T0_U8_32(u8" ",   U" ",   UTFCF_CHK);
        T0_U8_32(u8" a ", U" a ", UTFCF_CHK);
        T0_U8_32(u8"abcdefg",       U"abcdefg", UTFCF_CHK);
        T0_U8_32(u8"这是 一行文本。", U"这是 一行文本。", UTFCF_CHK);
        T0_U8_32(u8"a鷗🦊",         U"a鷗🦊", UTFCF_CHK);
        T0_U8_32(u8"a鷗🦊",         U"a鷗🦊", UTFCF_IGN);
        T0_U8_32(u8"a鷗🦊",         U"a鷗🦊", UTFCF_DEF);
        T0_U8_32(
            u8"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            UTFCF_CHK);

        // not error
        T0_U8_32("\xE6\x88\x91\xC2", U"我\xC2",   UTFCF_IGN);
        T0_U8_32("\xE6\x88\x91\xC2", U"我\xFFFD", UTFCF_DEF);

        // false case
        // MSVC 存在的 bug 导致16进制数无法正常写在 u8"" 字符串中
        TF_U8_32("\xE6\x88\x91\xC2", UTFCF_CHK, UTFC_ERR);  // 我\xC2

        return true;
    };

    TEST_DEF("UTF-16 -> UTF-8") {
        std::string buf;

        T0_U16_8(u"",    u8"",    UTFCF_CHK);
        T0_U16_8(u"a",   u8"a",   UTFCF_CHK);
        T0_U16_8(u"ab",  u8"ab",  UTFCF_CHK);
        T0_U16_8(u" ",   u8" ",   UTFCF_CHK);
        T0_U16_8(u" a ", u8" a ", UTFCF_CHK);
        T0_U16_8(u"abcdefg",       u8"abcdefg", UTFCF_CHK);
        T0_U16_8(u"这是 一行文本。", u8"这是 一行文本。", UTFCF_CHK);
        T0_U16_8(u"a鷗🦊",         u8"a鷗🦊", UTFCF_CHK);
        T0_U16_8(u"a鷗🦊",         u8"a鷗🦊", UTFCF_IGN);
        T0_U16_8(u"a鷗🦊",         u8"a鷗🦊", UTFCF_DEF);
        T0_U16_8(
            u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            u8"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            UTFCF_CHK);

        // not error
        T0_U16_8(u"我\xD800", "\xE6\x88\x91\xED\xA0\x80", UTFCF_IGN);
        T0_U16_8(u"我\xDFFF", "\xE6\x88\x91\xED\xBF\xBF", UTFCF_IGN);
        T0_U16_8(u"我\xDFFF", "\xE6\x88\x91\xEF\xBF\xBD", UTFCF_DEF);
        T0_U16_8(u"\x0040\xd83d\xd83d\xde00\x0040", u8"@�😀@", UTFCF_DEF);

        // false case
        TF_U16_8(u"我\xD800", UTFCF_CHK, UTFC_ERR);
        TF_U16_8(u"我\xDFFF", UTFCF_CHK, UTFC_ERR);

        return true;
    };

    TEST_DEF("UTF-16 -> UTF-32") {
        std::u32string buf;

        T0_U16_32(u"",    U"",    UTFCF_CHK);
        T0_U16_32(u"a",   U"a",   UTFCF_CHK);
        T0_U16_32(u"ab",  U"ab",  UTFCF_CHK);
        T0_U16_32(u" ",   U" ",   UTFCF_CHK);
        T0_U16_32(u" a ", U" a ", UTFCF_CHK);
        T0_U16_32(u"abcdefg",       U"abcdefg", UTFCF_CHK);
        T0_U16_32(u"这是 一行文本。", U"这是 一行文本。", UTFCF_CHK);
        T0_U16_32(u"a鷗🦊",         U"a鷗🦊", UTFCF_CHK);
        T0_U16_32(u"a鷗🦊",         U"a鷗🦊", UTFCF_IGN);
        T0_U16_32(u"a鷗🦊",         U"a鷗🦊", UTFCF_DEF);
        T0_U16_32(
            u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            UTFCF_CHK);

        // not error
        T0_U16_32(u"我\xD800", U"我\xD800", UTFCF_IGN);
        T0_U16_32(u"我\xDFFF", U"我\xDFFF", UTFCF_IGN);
        T0_U16_32(u"我\xDFFF", U"我\xFFFD", UTFCF_DEF);

        // false case
        TF_U16_32(u"我\xD800", UTFCF_CHK, UTFC_ERR);
        TF_U16_32(u"我\xDFFF", UTFCF_CHK, UTFC_ERR);

        return true;
    };

    TEST_DEF("UTF-32 -> UTF-8") {
        std::string buf;

        T0_U32_8(U"",    u8"",    UTFCF_CHK);
        T0_U32_8(U"a",   u8"a",   UTFCF_CHK);
        T0_U32_8(U"ab",  u8"ab",  UTFCF_CHK);
        T0_U32_8(U" ",   u8" ",   UTFCF_CHK);
        T0_U32_8(U" a ", u8" a ", UTFCF_CHK);
        T0_U32_8(U"abcdefg",       u8"abcdefg", UTFCF_CHK);
        T0_U32_8(U"这是 一行文本。", u8"这是 一行文本。", UTFCF_CHK);
        T0_U32_8(U"a鷗🦊",         u8"a鷗🦊", UTFCF_CHK);
        T0_U32_8(U"a鷗🦊",         u8"a鷗🦊", UTFCF_IGN);
        T0_U32_8(U"a鷗🦊",         u8"a鷗🦊", UTFCF_DEF);
        T0_U32_8(
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            u8"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            UTFCF_CHK);

        // not error
        T0_U32_8(U"我\xD800",   "\xE6\x88\x91\xED\xA0\x80",     UTFCF_IGN);
        T0_U32_8(U"我\xDFFF",   "\xE6\x88\x91\xED\xBF\xBF",     UTFCF_IGN);
        T0_U32_8(U"我\x110000", "\xE6\x88\x91\xF4\x90\x80\x80", UTFCF_IGN);
        T0_U32_8(U"我\x110000", "\xE6\x88\x91\xEF\xBF\xBD",     UTFCF_DEF);

        // false case
        TF_U32_8(U"我\xD800",   UTFCF_CHK, UTFC_ERR);
        TF_U32_8(U"我\xDFFF",   UTFCF_CHK, UTFC_ERR);
        TF_U32_8(U"我\x110000", UTFCF_CHK, UTFC_ERR);

        return true;
    };

    TEST_DEF("UTF-32 -> UTF-16") {
        std::u16string buf;

        T0_U32_16(U"",    u"",    UTFCF_CHK);
        T0_U32_16(U"a",   u"a",   UTFCF_CHK);
        T0_U32_16(U"ab",  u"ab",  UTFCF_CHK);
        T0_U32_16(U" ",   u" ",   UTFCF_CHK);
        T0_U32_16(U" a ", u" a ", UTFCF_CHK);
        T0_U32_16(U"abcdefg",       u"abcdefg", UTFCF_CHK);
        T0_U32_16(U"这是 一行文本。", u"这是 一行文本。", UTFCF_CHK);
        T0_U32_16(U"a鷗🦊",         u"a鷗🦊", UTFCF_CHK);
        T0_U32_16(U"a鷗🦊",         u"a鷗🦊", UTFCF_IGN);
        T0_U32_16(U"a鷗🦊",         u"a鷗🦊", UTFCF_DEF);
        T0_U32_16(
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            UTFCF_CHK);

        // not error
        T0_U32_16(U"我\xD800",   u"我\xD800",       UTFCF_IGN);
        T0_U32_16(U"我\xD800",   u"我\xD800",       UTFCF_IGN);
        T0_U32_16(U"我\x110000", u"我\xD800\xDC00", UTFCF_IGN);
        T0_U32_16(U"我\x110000", u"我\xFFFD",       UTFCF_DEF);

        // false case
        TF_U32_16(U"我\xD800",   UTFCF_CHK, UTFC_ERR);
        TF_U32_16(U"我\xDFFF",   UTFCF_CHK, UTFC_ERR);
        TF_U32_16(U"我\x110000", UTFCF_CHK, UTFC_ERR);

        return true;
    };

    TEST_DEF("WCHAR -> UTF-8") {
        std::string buf;

        T0_W_U8(L"",    u8"",    UTFCF_CHK);
        T0_W_U8(L"a",   u8"a",   UTFCF_CHK);
        T0_W_U8(L"ab",  u8"ab",  UTFCF_CHK);
        T0_W_U8(L" ",   u8" ",   UTFCF_CHK);
        T0_W_U8(L" a ", u8" a ", UTFCF_CHK);
        T0_W_U8(L"abcdefg",       u8"abcdefg", UTFCF_CHK);
        T0_W_U8(L"这是 一行文本。", u8"这是 一行文本。", UTFCF_CHK);
        T0_W_U8(L"a鷗🦊",         u8"a鷗🦊", UTFCF_CHK);
        T0_W_U8(L"a鷗🦊",         u8"a鷗🦊", UTFCF_IGN);
        T0_W_U8(L"a鷗🦊",         u8"a鷗🦊", UTFCF_DEF);
        T0_W_U8(
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            u8"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            UTFCF_CHK);

        // not error
        T0_W_U8(L"我\xD800", "\xE6\x88\x91\xED\xA0\x80", UTFCF_IGN);
        T0_W_U8(L"我\xDFFF", "\xE6\x88\x91\xED\xBF\xBF", UTFCF_IGN);
        T0_W_U8(L"我\xDFFF", "\xE6\x88\x91\xEF\xBF\xBD", UTFCF_DEF);
#if WCHAR_IS_UTF32
        T0_W_U8(L"我\x110000", "\xE6\x88\x91\xF4\x90\x80\x80", UTFCF_IGN);
        T0_W_U8(L"我\x110000", "\xE6\x88\x91\xEF\xBF\xBD", UTFCF_DEF);
#endif

        // false case
        TF_W_U8(L"我\xD800", UTFCF_CHK, UTFC_ERR);
        TF_W_U8(L"我\xDFFF", UTFCF_CHK, UTFC_ERR);
#if WCHAR_IS_UTF32
        TF_W_U8(L"我\x110000", UTFCF_CHK, UTFC_ERR);
#endif

        return true;
    };

    TEST_DEF("WCHAR -> UTF-16") {
        std::u16string buf;

        T0_W_U16(L"",    u"",    UTFCF_CHK);
        T0_W_U16(L"a",   u"a",   UTFCF_CHK);
        T0_W_U16(L"ab",  u"ab",  UTFCF_CHK);
        T0_W_U16(L" ",   u" ",   UTFCF_CHK);
        T0_W_U16(L" a ", u" a ", UTFCF_CHK);
        T0_W_U16(L"abcdefg",       u"abcdefg", UTFCF_CHK);
        T0_W_U16(L"这是 一行文本。", u"这是 一行文本。", UTFCF_CHK);
        T0_W_U16(L"a鷗🦊",         u"a鷗🦊", UTFCF_CHK);
        T0_W_U16(L"a鷗🦊",         u"a鷗🦊", UTFCF_IGN);
        T0_W_U16(L"a鷗🦊",         u"a鷗🦊", UTFCF_DEF);
        T0_W_U16(
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            UTFCF_CHK);

        // not error
        T0_W_U16(L"我\xD800", u"我\xD800", UTFCF_IGN);
        T0_W_U16(L"我\xDFFF", u"我\xDFFF", UTFCF_IGN);
        T0_W_U16(L"我\xD800", u"我\xFFFD", UTFCF_DEF);
#if WCHAR_IS_UTF32
        T0_W_U16(L"我\x110000", u"我\xD800\xDC00", UTFCF_IGN);
        T0_W_U16(L"我\x110000", u"我\xFFFD",       UTFCF_DEF);
#endif

        // false case
        TF_W_U16(L"我\xD800", UTFCF_CHK, UTFC_ERR);
        TF_W_U16(L"我\xDFFF", UTFCF_CHK, UTFC_ERR);
#if WCHAR_IS_UTF32
        TF_W_U16(L"我\x110000", UTFCF_CHK, UTFC_ERR);
#endif

        return true;
    };

    TEST_DEF("WCHAR -> UTF-32") {
        std::u32string buf;

        T0_W_U32(L"",    U"",    UTFCF_CHK);
        T0_W_U32(L"a",   U"a",   UTFCF_CHK);
        T0_W_U32(L"ab",  U"ab",  UTFCF_CHK);
        T0_W_U32(L" ",   U" ",   UTFCF_CHK);
        T0_W_U32(L" a ", U" a ", UTFCF_CHK);
        T0_W_U32(L"abcdefg",       U"abcdefg", UTFCF_CHK);
        T0_W_U32(L"这是 一行文本。", U"这是 一行文本。", UTFCF_CHK);
        T0_W_U32(L"a鷗🦊",         U"a鷗🦊", UTFCF_CHK);
        T0_W_U32(L"a鷗🦊",         U"a鷗🦊", UTFCF_IGN);
        T0_W_U32(L"a鷗🦊",         U"a鷗🦊", UTFCF_DEF);
        T0_W_U32(
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            UTFCF_CHK);

        // not error
        T0_W_U32(L"我\xD800", U"我\xD800", UTFCF_IGN);
        T0_W_U32(L"我\xDFFF", U"我\xDFFF", UTFCF_IGN);
        T0_W_U32(L"我\xD800", U"我\xFFFD", UTFCF_DEF);
#if WCHAR_IS_UTF32
        T0_W_U32(L"我\x110000", U"我\x110000", UTFCF_IGN);
        T0_W_U32(L"我\x110000", U"我\xFFFD",   UTFCF_DEF);
#endif

        // false case
        TF_W_U32(L"我\xD800", UTFCF_CHK, UTFC_ERR);
        TF_W_U32(L"我\xDFFF", UTFCF_CHK, UTFC_ERR);
#if WCHAR_IS_UTF32
        TF_W_U32(L"我\x110000", UTFCF_CHK, UTFC_ERR);
#endif

        return true;
    };

    TEST_DEF("UTF-8  -> WCHAR") {
        std::wstring buf;

        T0_U8_W(u8"",    L"",    UTFCF_CHK);
        T0_U8_W(u8"a",   L"a",   UTFCF_CHK);
        T0_U8_W(u8"ab",  L"ab",  UTFCF_CHK);
        T0_U8_W(u8" ",   L" ",   UTFCF_CHK);
        T0_U8_W(u8" a ", L" a ", UTFCF_CHK);
        T0_U8_W(u8"abcdefg",       L"abcdefg", UTFCF_CHK);
        T0_U8_W(u8"这是 一行文本。", L"这是 一行文本。", UTFCF_CHK);
#if WCHAR_IS_UTF16
        T0_U8_W(u8"a鷗🦊", L"a鷗🦊", UTFCF_CHK);
        T0_U8_W(u8"a鷗🦊", L"a鷗🦊", UTFCF_IGN);
        T0_U8_W(u8"a鷗🦊", L"a鷗🦊", UTFCF_DEF);
        T0_U8_W(
            u8"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            UTFCF_CHK);
#elif WCHAR_IS_UTF32
        T0_U8_W(u8"a鷗🦊", L"a鷗🦊", UTFCF_CHK);
        T0_U8_W(u8"a鷗🦊", L"a鷗🦊", UTFCF_IGN);
        T0_U8_W(u8"a鷗🦊", L"a鷗🦊", UTFCF_DEF);
        T0_U8_W(
            u8"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            UTFCF_CHK);
#else
        return false;
#endif

        // not error
        // MSVC 存在的 bug 导致16进制数无法正常写在 u8"" 字符串中
        T0_U8_W("\xE6\x88\x91\xC2", L"我\xC2",   UTFCF_IGN);
        T0_U8_W("\xE6\x88\x91\xC2", L"我\xFFFD", UTFCF_DEF);

        // false case
        // MSVC 存在的 bug 导致16进制数无法正常写在 u8"" 字符串中
        TF_U8_W("\xE6\x88\x91\xC2", UTFCF_CHK, UTFC_ERR);  // 我\xC2

        return true;
    };

    TEST_DEF("UTF-16 -> WCHAR") {
        std::wstring buf;

        T0_U16_W(u"",    L"",    UTFCF_CHK);
        T0_U16_W(u"a",   L"a",   UTFCF_CHK);
        T0_U16_W(u"ab",  L"ab",  UTFCF_CHK);
        T0_U16_W(u" ",   L" ",   UTFCF_CHK);
        T0_U16_W(u" a ", L" a ", UTFCF_CHK);
        T0_U16_W(u"abcdefg",       L"abcdefg", UTFCF_CHK);
        T0_U16_W(u"这是 一行文本。", L"这是 一行文本。", UTFCF_CHK);
#if WCHAR_IS_UTF16
        T0_U16_W(u"a鷗🦊", L"a鷗🦊", UTFCF_CHK);
        T0_U16_W(u"a鷗🦊", L"a鷗🦊", UTFCF_IGN);
        T0_U16_W(u"a鷗🦊", L"a鷗🦊", UTFCF_DEF);
        T0_U16_W(
            u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            UTFCF_CHK);
#elif WCHAR_IS_UTF32
        T0_U16_W(u"a鷗🦊", L"a鷗🦊", UTFCF_CHK);
        T0_U16_W(u"a鷗🦊", L"a鷗🦊", UTFCF_IGN);
        T0_U16_W(u"a鷗🦊", L"a鷗🦊", UTFCF_DEF);
        T0_U16_W(
            u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            UTFCF_CHK);
#else
        return false;
#endif

        // not error
        T0_U16_W(u"我\xD800", L"我\xD800", UTFCF_IGN);
        T0_U16_W(u"我\xDFFF", L"我\xDFFF", UTFCF_IGN);
        T0_U16_W(u"我\xDFFF", L"我\xFFFD", UTFCF_DEF);

        // false case
        TF_U16_W(u"我\xD800", UTFCF_CHK, UTFC_ERR);
        TF_U16_W(u"我\xDFFF", UTFCF_CHK, UTFC_ERR);

        return true;
    };

    TEST_DEF("UTF-32 -> WCHAR") {
        std::wstring buf;

        T0_U32_W(U"",    L"",    UTFCF_CHK);
        T0_U32_W(U"a",   L"a",   UTFCF_CHK);
        T0_U32_W(U"ab",  L"ab",  UTFCF_CHK);
        T0_U32_W(U" ",   L" ",   UTFCF_CHK);
        T0_U32_W(U" a ", L" a ", UTFCF_CHK);
        T0_U32_W(U"abcdefg",       L"abcdefg", UTFCF_CHK);
        T0_U32_W(U"这是 一行文本。", L"这是 一行文本。", UTFCF_CHK);
#if WCHAR_IS_UTF16
        T0_U32_W(U"a鷗🦊", L"a鷗🦊", UTFCF_CHK);
        T0_U32_W(U"a鷗🦊", L"a鷗🦊", UTFCF_IGN);
        T0_U32_W(U"a鷗🦊", L"a鷗🦊", UTFCF_DEF);
        T0_U32_W(
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            UTFCF_CHK);
#elif WCHAR_IS_UTF32
        T0_U32_W(U"a鷗🦊", L"a鷗🦊", UTFCF_CHK);
        T0_U32_W(U"a鷗🦊", L"a鷗🦊", UTFCF_IGN);
        T0_U32_W(U"a鷗🦊", L"a鷗🦊", UTFCF_DEF);
        T0_U32_W(
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            L"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹",
            UTFCF_CHK);
#else
        return false;
#endif

        // not error
        T0_U32_W(U"我\xD800", L"我\xD800", UTFCF_IGN);
        T0_U32_W(U"我\xD800", L"我\xD800", UTFCF_IGN);
#if WCHAR_IS_UTF16
        T0_U32_W(U"我\x110000", L"我\xD800\xDC00", UTFCF_IGN);
#elif WCHAR_IS_UTF32
        T0_U32_W(U"我\x110000", L"我\x110000", UTFCF_IGN);
#else
        return false;
#endif
        T0_U32_W(U"我\x110000", L"我\xFFFD", UTFCF_DEF);

        // false case
        TF_U32_W(U"我\xD800",   UTFCF_CHK, UTFC_ERR);
        TF_U32_W(U"我\xDFFF",   UTFCF_CHK, UTFC_ERR);
        TF_U32_W(U"我\x110000", UTFCF_CHK, UTFC_ERR);

        return true;
    };

}