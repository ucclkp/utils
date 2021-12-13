// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include <charconv>

#include "utils/float_conv.h"
#include "utils/int_conv.h"
#include "utils/unit_test/test_collector.h"


std::string float2string_std(double val) {
    char buf[64]{ 0 };
    std::to_chars(std::begin(buf), std::end(buf), val, std::chars_format::fixed, 63);
    return buf;
}

TEST_CASE(FloatingUtils) {

    TEST_DEF("Integer -> String.") {
        std::u16string str;
        utl::itos(0, 10, &str);
        TEST_E(str, u"0");
        utl::itos(-0, 10, &str);
        TEST_E(str, u"0");
        utl::itos(1, 10, &str);
        TEST_E(str, u"1");
        utl::itos(-1, 10, &str);
        TEST_E(str, u"-1");

        utl::itos(0, 3, &str);
        TEST_E(str, u"0");
        utl::itos(-0, 3, &str);
        TEST_E(str, u"0");
        utl::itos(1, 3, &str);
        TEST_E(str, u"1");
        utl::itos(-1, 3, &str);
        TEST_E(str, u"-1");

        utl::itos(1234567890, 10, &str);
        TEST_E(str, u"1234567890");
        utl::itos(-1234567890, 10, &str);
        TEST_E(str, u"-1234567890");
        utl::itos(9876543210, 10, &str);
        TEST_E(str, u"9876543210");
        utl::itos(-9876543210, 10, &str);
        TEST_E(str, u"-9876543210");

        utl::itos(127, 10, &str);
        TEST_E(str, u"127");
        utl::itos(-127, 10, &str);
        TEST_E(str, u"-127");
        utl::itos(128, 10, &str);
        TEST_E(str, u"128");
        utl::itos(-128, 10, &str);
        TEST_E(str, u"-128");
        utl::itos(65536, 10, &str);
        TEST_E(str, u"65536");
        utl::itos(-65536, 10, &str);
        TEST_E(str, u"-65536");

        utl::itos(4598625455542265485, 10, &str);
        TEST_E(str, u"4598625455542265485");
        utl::itos(-4598625455542265485, 10, &str);
        TEST_E(str, u"-4598625455542265485");

        utl::itos(128, 16, &str);
        TEST_E(str, u"80");
        utl::itos(-128, 16, &str);
        TEST_E(str, u"-80");
        utl::itos(4598625455542265485, 16, &str);
        TEST_E(str, u"3FD1997CBCAA668D");

        return true;
    };

    TEST_DEF("String  -> Integer.") {
        int64_t result;
        TEST_TRUE(utl::stoi(std::u16string(u"0"), 10, &result));
        TEST_E(result, 0);
        TEST_TRUE(utl::stoi(std::u16string(u"-0"), 10, &result));
        TEST_E(result, 0);
        TEST_TRUE(utl::stoi(std::u16string(u"1"), 10, &result));
        TEST_E(result, 1);
        TEST_TRUE(utl::stoi(std::u16string(u"-1"), 10, &result));
        TEST_E(result, -1);

        TEST_TRUE(utl::stoi(std::u16string(u"0"), 3, &result));
        TEST_E(result, 0);
        TEST_TRUE(utl::stoi(std::u16string(u"-0"), 3, &result));
        TEST_E(result, 0);
        TEST_TRUE(utl::stoi(std::u16string(u"1"), 3, &result));
        TEST_E(result, 1);
        TEST_TRUE(utl::stoi(std::u16string(u"-1"), 3, &result));
        TEST_E(result, -1);

        TEST_TRUE(utl::stoi(std::u16string(u"1234567890"), 10, &result));
        TEST_E(result, 1234567890);
        TEST_TRUE(utl::stoi(std::u16string(u"-1234567890"), 10, &result));
        TEST_E(result, -1234567890);
        TEST_TRUE(utl::stoi(std::u16string(u"9876543210"), 10, &result));
        TEST_E(result, 9876543210);
        TEST_TRUE(utl::stoi(std::u16string(u"-9876543210"), 10, &result));
        TEST_E(result, -9876543210);

        {
            int8_t result2;
            TEST_TRUE(utl::stoi(std::u16string(u"127"), 10, &result2));
            TEST_E(result2, 127);
            TEST_FALSE(utl::stoi(std::u16string(u"128"), 10, &result2));
            TEST_E(result2, 127);
            TEST_TRUE(utl::stoi(std::u16string(u"-128"), 10, &result2));
            TEST_E(result2, -128);

            uint8_t result3;
            TEST_TRUE(utl::stoi(std::u16string(u"255"), 10, &result3));
            TEST_E(result3, 255);
            TEST_FALSE(utl::stoi(std::u16string(u"256"), 10, &result3));
            TEST_E(result3, 255);
        }

        {
            int32_t result2;
            TEST_TRUE(utl::stoi(std::u16string(u"2147483647"), 10, &result2));
            TEST_E(result2, 2147483647);
            TEST_FALSE(utl::stoi(std::u16string(u"2147483648"), 10, &result2));
            TEST_E(result2, 2147483647);
            TEST_TRUE(utl::stoi(std::u16string(u"-2147483648"), 10, &result2));
            TEST_E(result2, -2147483648);

            uint32_t result3;
            TEST_TRUE(utl::stoi(std::u16string(u"4294967295"), 10, &result3));
            TEST_E(result3, 4294967295);
            TEST_FALSE(utl::stoi(std::u16string(u"4294967296"), 10, &result3));
            TEST_E(result3, 4294967295);
        }

        TEST_TRUE(utl::stoi(std::u16string(u"3FD1997CBCAA668D"), 16, &result));
        TEST_E(result, 4598625455542265485);

        return true;
    };

    TEST_DEF("Float   -> String.") {
        utl::testBigFloat();

        //double test = 0.34578;
        //double test = 1.0 / (uint64_t(1) << 32);
        //double test = 1234567978598848646898790.3;
        //double test = 0.000000000000000000000000000000000003;
        double test = 0.3;

        std::string result;
        utl::floatToString(0.3, &result);

        auto t1 = float2string_std(test);
        std::string t2;
        utl::floatToString(test, &t2);
        return true;
    };

}
