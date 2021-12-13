// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/string_utils.h"
#include "utils/unicode.h"
#include "utils/unit_test/test_collector.h"

using namespace utl;

TEST_CASE(UnicodeUnitTest) {

    TEST_DEF("UTF-8  -> UTF-16") {
        std::u16string result;
        TEST_TRUE(Unicode::UTF8ToUTF16(u8p(""), &result));
        TEST_TRUE(result.empty());

        TEST_TRUE(Unicode::UTF8ToUTF16(u8p("a"), &result));
        TEST_E(result, u"a");

        TEST_TRUE(Unicode::UTF8ToUTF16(u8p("ab"), &result));
        TEST_E(result, u"ab");

        TEST_TRUE(Unicode::UTF8ToUTF16(u8p(" "), &result));
        TEST_E(result, u" ");

        TEST_TRUE(Unicode::UTF8ToUTF16(u8p(" a "), &result));
        TEST_E(result, u" a ");

        TEST_TRUE(Unicode::UTF8ToUTF16(u8p("这是 一行文本。"), &result));
        TEST_E(result, u"这是 一行文本。");

        TEST_TRUE(Unicode::UTF8ToUTF16(u8p("a鷗🦊"), &result));
        TEST_E(result, u"a鷗🦊");

        TEST_TRUE(
            Unicode::UTF8ToUTF16(
                u8p("😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹"), &result));
        TEST_E(result, u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹");

        return true;
    };

    TEST_DEF("UTF-8  -> UTF-32") {
        std::u32string result;
        TEST_TRUE(Unicode::UTF8ToUTF32(u8p(""), &result));
        TEST_TRUE(result.empty());

        TEST_TRUE(Unicode::UTF8ToUTF32(u8p("a"), &result));
        TEST_E(result, U"a");

        TEST_TRUE(Unicode::UTF8ToUTF32(u8p("ab"), &result));
        TEST_E(result, U"ab");

        TEST_TRUE(Unicode::UTF8ToUTF32(u8p(" "), &result));
        TEST_E(result, U" ");

        TEST_TRUE(Unicode::UTF8ToUTF32(u8p(" a "), &result));
        TEST_E(result, U" a ");

        TEST_TRUE(Unicode::UTF8ToUTF32(u8p("这是 一行文本。"), &result));
        TEST_E(result, U"这是 一行文本。");

        TEST_TRUE(Unicode::UTF8ToUTF32(u8p("a鷗🦊"), &result));
        TEST_E(result, U"a鷗🦊");

        TEST_TRUE(
            Unicode::UTF8ToUTF32(
                u8p("😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹"), &result));
        TEST_E(result, U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹");

        return true;
    };

    TEST_DEF("UTF-16 -> UTF-8") {
        std::string result;
        TEST_TRUE(Unicode::UTF16ToUTF8(u"", &result));
        TEST_TRUE(result.empty());

        TEST_TRUE(Unicode::UTF16ToUTF8(u"a", &result));
        TEST_E(result, u8p("a"));

        TEST_TRUE(Unicode::UTF16ToUTF8(u"ab", &result));
        TEST_E(result, u8p("ab"));

        TEST_TRUE(Unicode::UTF16ToUTF8(u" ", &result));
        TEST_E(result, u8p(" "));

        TEST_TRUE(Unicode::UTF16ToUTF8(u" a ", &result));
        TEST_E(result, u8p(" a "));

        TEST_TRUE(Unicode::UTF16ToUTF8(u"这是 一行文本。", &result));
        TEST_E(result, u8p("这是 一行文本。"));

        TEST_TRUE(Unicode::UTF16ToUTF8(u"a鷗🦊", &result));
        TEST_E(result, u8p("a鷗🦊"));

        TEST_TRUE(
            Unicode::UTF16ToUTF8(
                u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹", &result));
        TEST_E(result, u8p("😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹"));

        return true;
    };

    TEST_DEF("UTF-16 -> UTF-32") {
        std::u32string result;
        TEST_TRUE(Unicode::UTF16ToUTF32(u"", &result));
        TEST_TRUE(result.empty());

        TEST_TRUE(Unicode::UTF16ToUTF32(u"a", &result));
        TEST_E(result, U"a");

        TEST_TRUE(Unicode::UTF16ToUTF32(u"ab", &result));
        TEST_E(result, U"ab");

        TEST_TRUE(Unicode::UTF16ToUTF32(u" ", &result));
        TEST_E(result, U" ");

        TEST_TRUE(Unicode::UTF16ToUTF32(u" a ", &result));
        TEST_E(result, U" a ");

        TEST_TRUE(Unicode::UTF16ToUTF32(u"这是 一行文本。", &result));
        TEST_E(result, U"这是 一行文本。");

        TEST_TRUE(Unicode::UTF16ToUTF32(u"a鷗🦊", &result));
        TEST_E(result, U"a鷗🦊");

        TEST_TRUE(
            Unicode::UTF16ToUTF32(
                u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹", &result));
        TEST_E(result, U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹");

        return true;
    };

    TEST_DEF("UTF-32 -> UTF-8") {
        std::string result;
        Unicode::UTF32ToUTF8(U"", &result);
        TEST_TRUE(result.empty());

        Unicode::UTF32ToUTF8(U"a", &result);
        TEST_E(result, u8p("a"));

        Unicode::UTF32ToUTF8(U"ab", &result);
        TEST_E(result, u8p("ab"));

        Unicode::UTF32ToUTF8(U" ", &result);
        TEST_E(result, u8p(" "));

        Unicode::UTF32ToUTF8(U" a ", &result);
        TEST_E(result, u8p(" a "));

        Unicode::UTF32ToUTF8(U"这是 一行文本。", &result);
        TEST_E(result, u8p("这是 一行文本。"));

        Unicode::UTF32ToUTF8(U"a鷗🦊", &result);
        TEST_E(result, u8p("a鷗🦊"));

        Unicode::UTF32ToUTF8(
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹", &result);
        TEST_E(result, u8p("😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹"));

        return true;
    };

    TEST_DEF("UTF-32 -> UTF-16") {
        std::u16string result;
        Unicode::UTF32ToUTF16(U"", &result);
        TEST_TRUE(result.empty());

        Unicode::UTF32ToUTF16(U"a", &result);
        TEST_E(result, u"a");

        Unicode::UTF32ToUTF16(U"ab", &result);
        TEST_E(result, u"ab");

        Unicode::UTF32ToUTF16(U" ", &result);
        TEST_E(result, u" ");

        Unicode::UTF32ToUTF16(U" a ", &result);
        TEST_E(result, u" a ");

        Unicode::UTF32ToUTF16(U"这是 一行文本。", &result);
        TEST_E(result, u"这是 一行文本。");

        Unicode::UTF32ToUTF16(U"a鷗🦊", &result);
        TEST_E(result, u"a鷗🦊");

        Unicode::UTF32ToUTF16(
            U"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹", &result);
        TEST_E(result, u"😍💖😜👀🚲🛴🏍⛅🧼🌁💒👱‍♂️👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄🤩😞☹☹");

        return true;
    };

}