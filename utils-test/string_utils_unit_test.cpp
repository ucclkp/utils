// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include <charconv>
#include <variant>
#include <filesystem>

#include "utils/strings/string_utils.hpp"
#include "utils/strings/usprintf.h"
#include "utils/strings/usprintf_internal.hpp"
#include "utils/strings/usformat.h"
#include "utils/unit_test/test_collector.h"


namespace {

std::string stringPrintf(const char* format, ...) {
    va_list vars;
    va_start(vars, format);
    va_list vars2;
    va_copy(vars2, vars);

    int ret = vsnprintf(nullptr, 0, format, vars);
    va_end(vars);
    if (ret <= 0) {
        return {};
    }

    ++ret;
    std::unique_ptr<char[]> buf(new char[ret]);
    ret = vsnprintf(buf.get(), ret, format, vars2);
    va_end(vars2);
    if (ret <= 0) {
        return {};
    }

    return std::string(buf.get(), ret);
}

}

using namespace utl;

TEST_CASE(StringUtilsUnitTest) {

    TEST_DEF("tolatcx test.") {
        TEST_E(tolatcu('a'), 'A');
        TEST_E(tolatcu('A'), 'A');
        TEST_E(tolatcu('z'), 'Z');
        TEST_E(tolatcu('Z'), 'Z');
        TEST_E(tolatcu('0'), '0');

        TEST_E(tolatcl('A'), 'a');
        TEST_E(tolatcl('Z'), 'z');
        TEST_E(tolatcl('0'), '0');
        TEST_E(tolatcl('a'), 'a');
        TEST_E(tolatcl('z'), 'z');
        TEST_E(tolatcl('0'), '0');

        return true;
    };

    TEST_DEF("tolatx test.") {
        TEST_E(tolatu("a"), "A");
        TEST_E(tolatu("A"), "A");
        TEST_E(tolatu("z"), "Z");
        TEST_E(tolatu("Z"), "Z");
        TEST_E(tolatu("0"), "0");
        TEST_E(tolatu("aBcDeF"), "ABCDEF");

        TEST_E(tolatl("A"), "a");
        TEST_E(tolatl("Z"), "z");
        TEST_E(tolatl("0"), "0");
        TEST_E(tolatl("a"), "a");
        TEST_E(tolatl("z"), "z");
        TEST_E(tolatl("0"), "0");
        TEST_E(tolatl("aBcDeF"), "abcdef");

        return true;
    };

    TEST_DEF("Trim test.") {
        TEST_E(trim(""), "");
        TEST_E(trim(" 1 2 3 "), "1 2 3");
        TEST_E(trim(" 1 2 3 ", "1", 0), " 1 2 3 ");
        TEST_E(trim(" 1 2 3 ", " ", TRF_START), "1 2 3 ");
        TEST_E(trim(" 1 2 3 ", " ", TRF_END), " 1 2 3");
        TEST_E(trim(" 1 2 3 ", " ", TRF_ALL), "123");

        return true;
    };

    TEST_DEF("Split test.") {
        auto r = split("1, 2, 3, 4", ",");
        TEST_TRUE(r.size() == 4);
        TEST_E(r[0], "1");
        TEST_E(r[1], " 2");
        TEST_E(r[2], " 3");
        TEST_E(r[3], " 4");

        return true;
    };

    TEST_DEF("StartWith, EndWidth test.") {
        TEST_TRUE(startWith("12345678", "123"));
        TEST_FALSE(startWith("12345678", "124"));
        TEST_FALSE(startWith("12", "123"));
        TEST_FALSE(startWith("abc", "A", 0, true));
        TEST_TRUE(startWith("abc", "A", 0, false));

        TEST_TRUE(endWith("12345678", "678"));
        TEST_FALSE(endWith("12345678", "123"));
        TEST_FALSE(endWith("12", "123"));
        TEST_FALSE(endWith("abc", "C", true));
        TEST_TRUE(endWith("abc", "C", false));

        return true;
    };

    TEST_DEF("IsLitEqual test.") {
        TEST_TRUE(isLitEqual("", ""));
        TEST_FALSE(isLitEqual("", "1"));
        TEST_TRUE(isLitEqual("12345678", "12345678"));
        TEST_FALSE(isLitEqual("12345678", "12345679"));
        TEST_FALSE(isLitEqual("22345678", "12345678"));

        TEST_TRUE(isLitEqual("abcd", "abcd"));
        TEST_TRUE(isLitEqual("abcd", "ABCD"));
        TEST_FALSE(isLitEqual("abcd", "ABCD", true));
        TEST_TRUE(isLitEqual("ABCD", "ABCD", true));

        return true;
    };

    TEST_DEF("ReplaceAll test.") {
        std::string text = "1,2,3,,4";
        replaceAll(&text, ",", "");
        TEST_E(text, "1234");

        return true;
    };

    TEST_DEF("usprintf -> string test.") {
        int* p = reinterpret_cast<int*>(0x12345678);

        auto test0 = stringPrintf("%##++9%d,%d, f=%f, e=%e", 233, 10 / 11.0, 10 / 11.0);
        auto test1 = stringPrintf("%+u", 233);
        auto test2 = stringPrintf("%a", 1.0);

        TEST_E(usprintf(u"", 233), u"");

        TEST_E(usprintf(u"%d", 233),  u"233");
        TEST_E(usprintf(u"%+d", 233), u"+233");
        TEST_E(usprintf(u"%1d", 233), u"233");
        TEST_E(usprintf(u"%5d", 233), u"  233");
        TEST_E(usprintf(u"%+5d", 233), u" +233");
        TEST_E(usprintf(u"%-+5d", 233), u"+233 ");
        TEST_E(usprintf(u"%i", -233), u"-233");

        TEST_E(usprintf(u"%o", 857), u"1531");
        TEST_E(usprintf(u"%+5o", 857), u" 1531");
        TEST_E(usprintf(u"%-+5o", 857), u"1531 ");

        TEST_E(usprintf(u"%x", 1234), u"4d2");
        TEST_E(usprintf(u"%X", 5678), u"162E");
        TEST_E(usprintf(u"%+X", 5678), u"162E");
        TEST_E(usprintf(u"%+5X", 5678), u" 162E");
        TEST_E(usprintf(u"%+-5X", 5678), u"162E ");

        TEST_E(usprintf(u"%f", 86.395), u"86.395000");
        TEST_E(usprintf(u"%F", 15234.0), u"15234.000000");
        TEST_E(usprintf(u"%e", 609.0), u"6.090000e+02");
        TEST_E(usprintf(u"%E", 9607.387), u"9.607387E+03");
        TEST_E(usprintf(u"%g", 574.26), u"574.26");
        TEST_E(usprintf(u"%G", 879.09), u"879.09");
        TEST_E(usprintf(u"%a", 85767.9576), u"0x1.4f07f525460aap+16");
        TEST_E(usprintf(u"%A", 756.3), u"0X1.7A26666666666P+9");
        TEST_E(usprintf(u"%c", u']'), u"]");
        TEST_E(usprintf(u"%ls", u"abcd"), u"abcd");
        TEST_E(usprintf(u"%p", p), u"12345678");
        TEST_E(usprintf(u"%%"), u"%");
        TEST_E(usprintf("%- 5d", 233), " 233 ");
        TEST_E(usprintf("%010.3f", -0.1), "-00000.100");
        TEST_E(
            usprintf(u"%##++9%d,%d, s=%s, c=%c, f=%f, e=%e", 233, u8p("顬"), '8', 10 / 11.0, 10 / 11.0),
            u"%d,233, s=顬, c=8, f=0.909091, e=9.090909e-01");
        TEST_E(
            usprintf(
                u"😍💖😜👀%s🚲🛴🏍⛅🧼🌁💒👱‍♂️%d👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄%c🤩😞☹☹",
                u8p("顬"), 4567, '7'),
            u"😍💖😜👀顬🚲🛴🏍⛅🧼🌁💒👱‍♂️4567👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄7🤩😞☹☹");

        return true;
    };

    TEST_DEF("usprintf -> buf    test.") {
        auto _TEST_FUNC = [](const std::string& r, const char* format, ...) -> bool {
            internal::vlw vars;
            va_start(vars.args, format);
            char buf[200];
            size_t len = 200;
            int ret = internal::usprintf_base(
                format, std::char_traits<char>::length(format), buf, &len, &vars);
            va_end(vars.args);
            TEST_E(ret, 0);
            TEST_E(len, r.size());
            TEST_E(std::string(buf, len), r);
            return true;
        };

        int* p = reinterpret_cast<int*>(0x12345678);
        TEST_TRUE(_TEST_FUNC(" 233 ", "%- 5d", 233));
        TEST_TRUE(_TEST_FUNC("-00000.100", "%010.3f", -0.1));
        TEST_TRUE(_TEST_FUNC(u8p("%d,233, s=顬, c=8, f=0.909091, e=9.090909e-01"), "%##++9%d,%d, s=%s, c=%c, f=%f, e=%e", 233, u8p("顬"), '8', 10 / 11.0, 10 / 11.0));

        TEST_TRUE(_TEST_FUNC("233", "%d", 233));
        TEST_TRUE(_TEST_FUNC("-233", "%i", -233));
        TEST_TRUE(_TEST_FUNC("1531", "%o", 857));
        TEST_TRUE(_TEST_FUNC("4d2", "%x", 1234));
        TEST_TRUE(_TEST_FUNC("162E", "%X", 5678));
        TEST_TRUE(_TEST_FUNC("86.395000", "%f", 86.395));
        TEST_TRUE(_TEST_FUNC("15234.000000", "%F", 15234.0));
        TEST_TRUE(_TEST_FUNC("6.090000e+02", "%e", 609.0));
        TEST_TRUE(_TEST_FUNC("9.607387E+03", "%E", 9607.387));
        TEST_TRUE(_TEST_FUNC("574.26", "%g", 574.26));
        TEST_TRUE(_TEST_FUNC("879.09", "%G", 879.09));
        TEST_TRUE(_TEST_FUNC("0x1.4f07f525460aap+16", "%a", 85767.9576));
        TEST_TRUE(_TEST_FUNC("0X1.7A26666666666P+9", "%A", 756.3));
        TEST_TRUE(_TEST_FUNC("]", "%c", ']'));
        TEST_TRUE(_TEST_FUNC("abcd", "%ls", u"abcd"));
        TEST_E(usprintf(u"%p", p), u"12345678");
        TEST_TRUE(_TEST_FUNC("%", "%%"));
        TEST_TRUE(_TEST_FUNC(
            u8p("😍💖😜👀顬🚲🛴🏍⛅🧼🌁💒👱‍♂️4567👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄7🤩😞☹☹"),
            u8p("😍💖😜👀%s🚲🛴🏍⛅🧼🌁💒👱‍♂️%d👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄%c🤩😞☹☹"),
            u8p("顬"), 4567, '7'));

        return true;
    };

    TEST_DEF("usformat tests.") {
        const char16_t* buf = u"sdfdsf";
        std::filesystem::path fp = u"test";
        int* p = reinterpret_cast<int*>(0x12345678);

        TEST_E(usformat(u"", 233), u"");

        TEST_E(usformat(u"%d", 233), u"233");
        TEST_E(usformat(u"%+d", 233), u"+233");
        TEST_E(usformat(u"%1d", 233), u"233");
        TEST_E(usformat(u"%5d", 233), u"  233");
        TEST_E(usformat(u"%+5d", 233), u" +233");
        TEST_E(usformat(u"%-+5d", 233), u"+233 ");
        TEST_E(usformat(u"%i", -233), u"-233");

        TEST_E(usformat(u"%o", 857), u"1531");
        TEST_E(usformat(u"%+5o", 857), u"+1531");
        TEST_E(usformat(u"%-+5o", 857), u"+1531");

        TEST_E(usformat(u"%x", 1234), u"4d2");
        TEST_E(usformat(u"%X", 5678), u"162E");
        TEST_E(usformat(u"%+X", 5678), u"+162E");
        TEST_E(usformat(u"%+5X", 5678), u"+162E");
        TEST_E(usformat(u"%+-5X", 5678), u"+162E");

        TEST_E(usformat(u"%f", 86.395), u"86.395000");
        TEST_E(usformat(u"%F", 15234.0), u"15234.000000");
        TEST_E(usformat(u"%e", 609.0), u"6.090000e+02");
        TEST_E(usformat(u"%E", 9607.387), u"9.607387E+03");
        TEST_E(usformat(u"%g", 574.26), u"574.26");
        TEST_E(usformat(u"%G", 879.09), u"879.09");
        TEST_E(usformat(u"%a", 85767.9576), u"0x1.4f07f525460aap+16");
        TEST_E(usformat(u"%A", 756.3), u"0X1.7A26666666666P+9");
        TEST_E(usformat(u"%c", u']'), u"]");
        TEST_E(usformat(u"%ls", u"abcd"), u"abcd");
        TEST_E(usformat(u"%p", p), u"12345678");
        TEST_E(usformat(u"%%"), u"%");
        TEST_E(usformat("%- 5d", 233), " 233 ");
        TEST_E(usformat("%010.3f", -0.1), "-00000.100");
        TEST_E(
            usformat(u"%##++9%d,%d, s=%s, c=%c, f=%f, e=%e", 233, u8p("顬"), '8', 10 / 11.0, 10 / 11.0),
            u"%d,233, s=顬, c=8, f=0.909091, e=9.090909e-01");
        TEST_E(
            usformat(U"This %s, is a %s, and %.1f: %s\n", buf, std::string("123"), 0.1, fp),
            U"This sdfdsf, is a 123, and 0.1: test\n");
        TEST_E(
            usformat(
                u"😍💖😜👀%s🚲🛴🏍⛅🧼🌁💒👱‍♂️%d👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄%c🤩😞☹☹",
                u8p("顬"), 4567, '7'),
            u"😍💖😜👀顬🚲🛴🏍⛅🧼🌁💒👱‍♂️4567👨‍🦰👨‍🦱👩‍🎨👩‍🎤👨‍💻🧜‍♂️🧛‍♂️🙄7🤩😞☹☹");

        TEST_E(usformat("%d", 453468.5678569), "453468");
        TEST_E(usformat("%f", 453468), "453468.000000");
        TEST_E(usformat("%5", 453468), "");

        char16_t _buf[32];
        for (size_t i = 0; i < 19; ++i) {
            size_t _buf_len = i;
            int ret = usformatb(_buf, &_buf_len, u"This is a %d, %s", 233, fp);
            TEST_E(ret, SCR_BUF);
            TEST_E(_buf_len, 19);
        }

        return true;
    };

}