// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/string_utils.hpp"
#include "utils/unicode_conv.h"
#include "utils/unit_test/test_collector.h"

using namespace utl;

TEST_CASE(UnicodeUnitTest) {

    TEST_DEF("UTF-8  -> UTF-16") {
        std::u16string result;
        TEST_TRUE(utf8_to_utf16(u8p(""), &result));
        TEST_TRUE(result.empty());

        TEST_TRUE(utf8_to_utf16(u8p("a"), &result));
        TEST_E(result, u"a");

        TEST_TRUE(utf8_to_utf16(u8p("ab"), &result));
        TEST_E(result, u"ab");

        TEST_TRUE(utf8_to_utf16(u8p(" "), &result));
        TEST_E(result, u" ");

        TEST_TRUE(utf8_to_utf16(u8p(" a "), &result));
        TEST_E(result, u" a ");

        TEST_TRUE(utf8_to_utf16(u8p("这是 一行文本。"), &result));
        TEST_E(result, u"这是 一行文本。");

        TEST_TRUE(utf8_to_utf16(u8p("a鷗🦊"), &result));
        TEST_E(result, u"a鷗🦊");

        TEST_TRUE(
            utf8_to_utf16(
                u8p("😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹"), &result));
        TEST_E(result, u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹");

        {
            const char* test = "abcdefg";
            char16_t buf[10];
            size_t buf_len = 7;
            int ret = utf8_to_utf16(test, std::char_traits<char>::length(test), buf, &buf_len);
            TEST_E(ret, UCR_OK);

            buf_len = 6;
            ret = utf8_to_utf16(test, std::char_traits<char>::length(test), buf, &buf_len);
            TEST_E(ret, UCR_BUFFER);
            TEST_E(buf_len, 7);
        }

        return true;
    };

    TEST_DEF("UTF-8  -> UTF-32") {
        std::u32string result;
        TEST_TRUE(utf8_to_utf32(u8p(""), &result));
        TEST_TRUE(result.empty());

        TEST_TRUE(utf8_to_utf32(u8p("a"), &result));
        TEST_E(result, U"a");

        TEST_TRUE(utf8_to_utf32(u8p("ab"), &result));
        TEST_E(result, U"ab");

        TEST_TRUE(utf8_to_utf32(u8p(" "), &result));
        TEST_E(result, U" ");

        TEST_TRUE(utf8_to_utf32(u8p(" a "), &result));
        TEST_E(result, U" a ");

        TEST_TRUE(utf8_to_utf32(u8p("这是 一行文本。"), &result));
        TEST_E(result, U"这是 一行文本。");

        TEST_TRUE(utf8_to_utf32(u8p("a鷗🦊"), &result));
        TEST_E(result, U"a鷗🦊");

        TEST_TRUE(
            utf8_to_utf32(
                u8p("😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹"), &result));
        TEST_E(result, U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹");

        {
            const char* test = "abcdefg";
            char32_t buf[10];
            size_t buf_len = 7;
            int ret = utf8_to_utf32(test, std::char_traits<char>::length(test), buf, &buf_len);
            TEST_E(ret, UCR_OK);

            buf_len = 6;
            ret = utf8_to_utf32(test, std::char_traits<char>::length(test), buf, &buf_len);
            TEST_E(ret, UCR_BUFFER);
            TEST_E(buf_len, 7);
        }

        return true;
    };

    TEST_DEF("UTF-16 -> UTF-8") {
        std::string result;
        TEST_TRUE(utf16_to_utf8(u"", &result));
        TEST_TRUE(result.empty());

        TEST_TRUE(utf16_to_utf8(u"a", &result));
        TEST_E(result, u8p("a"));

        TEST_TRUE(utf16_to_utf8(u"ab", &result));
        TEST_E(result, u8p("ab"));

        TEST_TRUE(utf16_to_utf8(u" ", &result));
        TEST_E(result, u8p(" "));

        TEST_TRUE(utf16_to_utf8(u" a ", &result));
        TEST_E(result, u8p(" a "));

        TEST_TRUE(utf16_to_utf8(u"这是 一行文本。", &result));
        TEST_E(result, u8p("这是 一行文本。"));

        TEST_TRUE(utf16_to_utf8(u"a鷗🦊", &result));
        TEST_E(result, u8p("a鷗🦊"));

        TEST_TRUE(
            utf16_to_utf8(
                u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹", &result));
        TEST_E(result, u8p("😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹"));

        {
            const char16_t* test = u"abcdefg";
            char buf[10];
            size_t buf_len = 7;
            int ret = utf16_to_utf8(test, std::char_traits<char16_t>::length(test), buf, &buf_len);
            TEST_E(ret, UCR_OK);

            buf_len = 6;
            ret = utf16_to_utf8(test, std::char_traits<char16_t>::length(test), buf, &buf_len);
            TEST_E(ret, UCR_BUFFER);
            TEST_E(buf_len, 7);
        }

        return true;
    };

    TEST_DEF("UTF-16 -> UTF-32") {
        std::u32string result;
        TEST_TRUE(utf16_to_utf32(u"", &result));
        TEST_TRUE(result.empty());

        TEST_TRUE(utf16_to_utf32(u"a", &result));
        TEST_E(result, U"a");

        TEST_TRUE(utf16_to_utf32(u"ab", &result));
        TEST_E(result, U"ab");

        TEST_TRUE(utf16_to_utf32(u" ", &result));
        TEST_E(result, U" ");

        TEST_TRUE(utf16_to_utf32(u" a ", &result));
        TEST_E(result, U" a ");

        TEST_TRUE(utf16_to_utf32(u"这是 一行文本。", &result));
        TEST_E(result, U"这是 一行文本。");

        TEST_TRUE(utf16_to_utf32(u"a鷗🦊", &result));
        TEST_E(result, U"a鷗🦊");

        TEST_TRUE(
            utf16_to_utf32(
                u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹", &result));
        TEST_E(result, U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹");

        {
            const char16_t* test = u"abcdefg";
            char32_t buf[10];
            size_t buf_len = 7;
            int ret = utf16_to_utf32(test, std::char_traits<char16_t>::length(test), buf, &buf_len);
            TEST_E(ret, UCR_OK);

            buf_len = 6;
            ret = utf16_to_utf32(test, std::char_traits<char16_t>::length(test), buf, &buf_len);
            TEST_E(ret, UCR_BUFFER);
            TEST_E(buf_len, 7);
        }

        return true;
    };

    TEST_DEF("UTF-32 -> UTF-8") {
        std::string result;
        utf32_to_utf8(U"", &result);
        TEST_TRUE(result.empty());

        utf32_to_utf8(U"a", &result);
        TEST_E(result, u8p("a"));

        utf32_to_utf8(U"ab", &result);
        TEST_E(result, u8p("ab"));

        utf32_to_utf8(U" ", &result);
        TEST_E(result, u8p(" "));

        utf32_to_utf8(U" a ", &result);
        TEST_E(result, u8p(" a "));

        utf32_to_utf8(U"这是 一行文本。", &result);
        TEST_E(result, u8p("这是 一行文本。"));

        utf32_to_utf8(U"a鷗🦊", &result);
        TEST_E(result, u8p("a鷗🦊"));

        utf32_to_utf8(
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹", &result);
        TEST_E(result, u8p("😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹"));

        {
            const char32_t* test = U"abcdefg";
            char buf[10];
            size_t buf_len = 7;
            TEST_TRUE(utf32_to_utf8(test, std::char_traits<char32_t>::length(test), buf, &buf_len));

            buf_len = 6;
            TEST_FALSE(utf32_to_utf8(test, std::char_traits<char32_t>::length(test), buf, &buf_len));
            TEST_E(buf_len, 7);
        }

        return true;
    };

    TEST_DEF("UTF-32 -> UTF-16") {
        std::u16string result;
        utf32_to_utf16(U"", &result);
        TEST_TRUE(result.empty());

        utf32_to_utf16(U"a", &result);
        TEST_E(result, u"a");

        utf32_to_utf16(U"ab", &result);
        TEST_E(result, u"ab");

        utf32_to_utf16(U" ", &result);
        TEST_E(result, u" ");

        utf32_to_utf16(U" a ", &result);
        TEST_E(result, u" a ");

        utf32_to_utf16(U"这是 一行文本。", &result);
        TEST_E(result, u"这是 一行文本。");

        utf32_to_utf16(U"a鷗🦊", &result);
        TEST_E(result, u"a鷗🦊");

        utf32_to_utf16(
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹", &result);
        TEST_E(result, u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹");

        {
            const char32_t* test = U"abcdefg";
            char16_t buf[10];
            size_t buf_len = 7;
            TEST_TRUE(utf32_to_utf16(test, std::char_traits<char32_t>::length(test), buf, &buf_len));

            buf_len = 6;
            TEST_FALSE(utf32_to_utf16(test, std::char_traits<char32_t>::length(test), buf, &buf_len));
            TEST_E(buf_len, 7);
        }

        return true;
    };

}