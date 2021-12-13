// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include <charconv>

#include "utils/string_utils.h"
#include "utils/unit_test/test_collector.h"
#include "utils/string_printf_u16.h"


using namespace utl;

TEST_CASE(StringUtilsUnitTest) {

    TEST_DEF("stringPrintf test.") {
        int* p = reinterpret_cast<int*>(0x12345678);

        char buf[64];
        std::to_chars(std::begin(buf), std::end(buf), 10 / 11.0, std::chars_format::general, 19);

        auto test = stringPrintf("%##++9%d,%d, f=%.19f, e=%e", 233, 10 / 11.0, 10 / 11.0);
        auto test2 = stringPrintf_u16(u"%##++9%d,%d, s=%s, c=%c, f=%f, e=%e", 233, u8p("顬"), '8', 10 / 11.0, 10 / 11.0);

        TEST_E(stringPrintf("%d", 233), "233");
        TEST_E(stringPrintf("%i", -233), "-233");
        TEST_E(stringPrintf("%o", 857), "1531");
        TEST_E(stringPrintf("%x", 1234), "4d2");
        TEST_E(stringPrintf("%X", 5678), "162E");
        TEST_E(stringPrintf("%f", 86.395), "86.395000");
        TEST_E(stringPrintf("%F", 15234.0), "15234.000000");
        TEST_E(stringPrintf("%e", 609.0), "6.090000e+02");
        TEST_E(stringPrintf("%E", 9607.387), "9.607387E+03");
        TEST_E(stringPrintf("%g", 574.26), "574.26");
        TEST_E(stringPrintf("%G", 879.09), "879.09");
        TEST_E(stringPrintf("%a", 85767.9576), "0x1.4f07f525460aap+16");
        TEST_E(stringPrintf("%A", 756.3), "0X1.7A26666666666P+9");
        TEST_E(stringPrintf("%c", ']'), "]");
        TEST_E(stringPrintf("%s", "abcd"), "abcd");
        //TEST_E(stringPrintf("%p", p), "12345678");
        TEST_E(stringPrintf("%%"), "%");

        TEST_E(stringPrintf(u"%d", 233), u"233");
        TEST_E(stringPrintf(u"%i", -233), u"-233");
        TEST_E(stringPrintf(u"%o", 857), u"1531");
        TEST_E(stringPrintf(u"%x", 1234), u"4d2");
        TEST_E(stringPrintf(u"%X", 5678), u"162E");
        TEST_E(stringPrintf(u"%f", 86.395), u"86.395000");
        TEST_E(stringPrintf(u"%F", 15234.0), u"15234.000000");
        TEST_E(stringPrintf(u"%e", 609.0), u"6.090000e+02");
        TEST_E(stringPrintf(u"%E", 9607.387), u"9.607387E+03");
        TEST_E(stringPrintf(u"%g", 574.26), u"574.26");
        TEST_E(stringPrintf(u"%G", 879.09), u"879.09");
        TEST_E(stringPrintf(u"%a", 85767.9576), u"0x1.4f07f525460aap+16");
        TEST_E(stringPrintf(u"%A", 756.3), u"0X1.7A26666666666P+9");
        TEST_E(stringPrintf(u"%c", u']'), u"]");
        TEST_E(stringPrintf(u"%s", u"abcd"), u"abcd");
        //TEST_E(stringPrintf(u"%p", p), u"12345678");
        TEST_E(stringPrintf(u"%%"), u"%");

        TEST_E(
            stringPrintf(
                u8p("😍💖😜👀%s🚲🛴🏍⛅🧼🌁💒👱‍♂️%d👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄%c🤩😞☹☹"),
                u8p("顬"), 4567, '7'),
            u8p("😍💖😜👀顬🚲🛴🏍⛅🧼🌁💒👱‍♂️4567👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄7🤩😞☹☹"));

        TEST_E(
            stringPrintf(
                u"😍💖😜👀%s🚲🛴🏍⛅🧼🌁💒👱‍♂️%d👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄%c🤩😞☹☹",
                u8p("顬"), 4567, '7'),
            u"😍💖😜👀顬🚲🛴🏍⛅🧼🌁💒👱‍♂️4567👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄7🤩😞☹☹");

        return true;
    };

}