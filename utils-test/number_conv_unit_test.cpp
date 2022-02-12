// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include <cmath>
#include <random>

#include "utils/log.h"
#include "utils/strings/float_conv.h"
#include "utils/strings/float_conv_big_num.hpp"
#include "utils/strings/int_conv.hpp"
#include "utils/strings/uint_exp.hpp"
#include "utils/unit_test/test_collector.h"

//#define USING_CHARCONV
#define RAND_LOOP 1000

#ifdef USING_CHARCONV
#include <charconv>
#endif

#define DENORM_VAL(val)  \
    (!FORCE_NO_DENORM &&   \
    std::numeric_limits<decltype(val)>::has_denorm == std::denorm_present) ?  \
    val : 0


TEST_CASE(IntegerConv) {

    TEST_DEF("Integer -> String.") {
        for (int16_t i = -128; i < 128; ++i) {
            int8_t t = int8_t(i);
            std::string s1;
            utl::itos(t, &s1, 10);
            TEST_E(std::to_string(i), s1);
        }

        for (int32_t i = -32768; i < 32768; ++i) {
            int16_t t = int16_t(i);
            std::string s1;
            utl::itos(t, &s1, 10);
            TEST_E(std::to_string(i), s1);
        }

        std::u16string str;
        utl::itos(1234567890, &str, 10);
        TEST_E(str, u"1234567890");
        utl::itos(-1234567890, &str, 10);
        TEST_E(str, u"-1234567890");
        utl::itos(9876543210, &str, 10);
        TEST_E(str, u"9876543210");
        utl::itos(-9876543210, &str, 10);
        TEST_E(str, u"-9876543210");

        utl::itos(127, &str, 10);
        TEST_E(str, u"127");
        utl::itos(-127, &str, 10);
        TEST_E(str, u"-127");
        utl::itos(128, &str, 10);
        TEST_E(str, u"128");
        utl::itos(-128, &str, 10);
        TEST_E(str, u"-128");
        utl::itos(65536, &str, 10);
        TEST_E(str, u"65536");
        utl::itos(-65536, &str, 10);
        TEST_E(str, u"-65536");

        utl::itos(4598625455542265485, &str, 10);
        TEST_E(str, u"4598625455542265485");
        utl::itos(-4598625455542265485, &str, 10);
        TEST_E(str, u"-4598625455542265485");

        utl::itos(128, &str, 16);
        TEST_E(str, u"80");
        utl::itos(-128, &str, 16);
        TEST_E(str, u"-80");
        utl::itos(4598625455542265485, &str, 16, true);
        TEST_E(str, u"3FD1997CBCAA668D");

        return true;
    };

    TEST_DEF("String  -> Integer.") {
        int64_t result;
        TEST_TRUE(utl::stoi(std::u16string(u"0"), &result, 10));
        TEST_E(result, 0);
        TEST_TRUE(utl::stoi(std::u16string(u"-0"), &result, 10));
        TEST_E(result, 0);
        TEST_TRUE(utl::stoi(std::u16string(u"1"), &result, 10));
        TEST_E(result, 1);
        TEST_TRUE(utl::stoi(std::u16string(u"-1"), &result, 10));
        TEST_E(result, -1);

        TEST_TRUE(utl::stoi(std::u16string(u"0"), &result, 3));
        TEST_E(result, 0);
        TEST_TRUE(utl::stoi(std::u16string(u"-0"), &result, 3));
        TEST_E(result, 0);
        TEST_TRUE(utl::stoi(std::u16string(u"1"), &result, 3));
        TEST_E(result, 1);
        TEST_TRUE(utl::stoi(std::u16string(u"-1"), &result, 3));
        TEST_E(result, -1);

        TEST_TRUE(utl::stoi(std::u16string(u"1234567890"), &result, 10));
        TEST_E(result, 1234567890);
        TEST_TRUE(utl::stoi(std::u16string(u"-1234567890"), &result, 10));
        TEST_E(result, -1234567890);
        TEST_TRUE(utl::stoi(std::u16string(u"9876543210"), &result, 10));
        TEST_E(result, 9876543210);
        TEST_TRUE(utl::stoi(std::u16string(u"-9876543210"), &result, 10));
        TEST_E(result, -9876543210);

        {
            int8_t result2;
            TEST_TRUE(utl::stoi(std::u16string(u"127"), &result2, 10));
            TEST_E(result2, 127);
            TEST_FALSE(utl::stoi(std::u16string(u"128"), &result2, 10));
            TEST_E(result2, 127);
            TEST_TRUE(utl::stoi(std::u16string(u"-128"), &result2, 10));
            TEST_E(result2, -128);

            uint8_t result3;
            TEST_TRUE(utl::stoi(std::u16string(u"255"), &result3, 10));
            TEST_E(result3, 255);
            TEST_FALSE(utl::stoi(std::u16string(u"256"), &result3, 10));
            TEST_E(result3, 255);
        }

        {
            int32_t result2;
            TEST_TRUE(utl::stoi(std::u16string(u"2147483647"), &result2, 10));
            TEST_E(result2, 2147483647);
            TEST_FALSE(utl::stoi(std::u16string(u"2147483648"), &result2, 10));
            TEST_E(result2, 2147483647);
            TEST_TRUE(utl::stoi(std::u16string(u"-2147483648"), &result2, 10));
            TEST_E(result2, -2147483648);

            uint32_t result3;
            TEST_TRUE(utl::stoi(std::u16string(u"4294967295"), &result3, 10));
            TEST_E(result3, 4294967295);
            TEST_FALSE(utl::stoi(std::u16string(u"4294967296"), &result3, 10));
            TEST_E(result3, 4294967295);
        }

        TEST_TRUE(utl::stoi(std::u16string(u"3FD1997CBCAA668D"), &result, 16));
        TEST_E(result, 4598625455542265485);

        return true;
    };

}

TEST_CASE(InternalBigNum) {

    TEST_DEF("internal: BigUInt<uint32_t, 10>") {
        using namespace utl::internal;

        using BigUInt_b10 = BigUInt_bN<double, uint32_t, 10, 22>;
        {
            BigUInt_b10 bi1(5);
            bi1.muld(8);
            bi1.muld(8);
            bi1.muld(8);
            bi1.muld(8);
            bi1.muld(8);
            bi1.muld(8);
            bi1.muld(8);
            bi1.muld(8);
            bi1.muld(8);
            TEST_E(bi1.raw[bi1.uic - 1], 67108864u);

            BigUInt_b10 bi2(bi1);
            bi2.add(bi1);
            TEST_E(bi2.raw[bi2.uic - 1], 34217728u);
            TEST_E(bi2.raw[bi2.uic - 2], 1u);

            char cs[100];
            size_t cs_l = 8;
            bi1.toChars<false>(cs, &cs_l, 0);
            TEST_E(std::string(cs, cs_l), "67108864");

            cs_l = 3;
            bi1.toChars<false>(cs, &cs_l, 1);
            TEST_E(std::string(cs, cs_l), "886");

            cs_l = 3;
            bi1.toChars<false>(cs, &cs_l, 2);
            TEST_E(std::string(cs, cs_l), "088");
        }
        {
            BigUInt_b10 bi1(5);
            for (int i = 0; i < 62; ++i) {
                bi1.mul2();
            }

            BigUInt_b10 bi2(0);
            bi2.add(bi1);

            TEST_E(bi1.effect, bi2.effect);
            TEST_E(bi1.raw[bi1.uic - 1], bi2.raw[bi2.uic - 1]);
            TEST_E(bi1.raw[bi1.uic - 2], bi2.raw[bi2.uic - 2]);
            TEST_E(bi1.raw[bi1.uic - 3], bi2.raw[bi2.uic - 3]);
        }
        {
            char bins[] = "1234567890";
            const char* r;
            BigUInt_b10 bi(0);
            bi.fromChars(bins, std::size(bins) - 1, 9, false, &r);
            TEST_E(bi.raw[bi.uic - 1], 0u);
            TEST_E(bi.raw[bi.uic - 2], 45678900u);
            TEST_E(bi.raw[bi.uic - 3], 123u);

            bi.round(15, 0, true, utl::UFR_NEAR);
            TEST_E(bi.raw[bi.uic - 1], 0u);
            TEST_E(bi.raw[bi.uic - 2], 55678900u);
            TEST_E(bi.raw[bi.uic - 3], 123u);
        }
        {
            char bins[] = "1235.a67890";
            const char* r;
            BigUInt_b10 bi(0);
            bi.fromChars(bins, std::size(bins) - 1, 9, true, &r);
            TEST_E(bi.raw[bi.uic - 1], 0u);
            TEST_E(bi.raw[bi.uic - 2], 12350u);
            TEST_E(bi.effect, 2u);
            TEST_E(r, bins + 5);
        }
        {
            char bins[] = "1.678901234";
            const char* r;
            BigUInt_b10 bi(0);
            bi.fromChars(bins, std::size(bins) - 1, 0, true, &r);
            TEST_E(bi.raw[bi.uic - 1], 78901234u);
            TEST_E(bi.raw[bi.uic - 2], 16u);
            TEST_E(bi.effect, 2u);
        }
        {
            char bins[] = "123";
            const char* r;
            BigUInt_b10 bi(0);
            bi.fromChars(bins, std::size(bins) - 1, 9, false, &r);
            TEST_E(bi.raw[bi.uic - 1], 0u);
            TEST_E(bi.raw[bi.uic - 2], 1230u);

            bi.round(6, 0, true, utl::UFR_NEAR);
            TEST_E(bi.raw[bi.uic - 1], 0u);
            TEST_E(bi.raw[bi.uic - 2], 1230u);
        }
        {
            char bins[] = "1234567890";
            const char* r;
            BigUInt_b10 bi2(0);
            bi2.fromChars(bins, std::size(bins) - 1, &r);
            TEST_E(bi2.raw[bi2.uic - 1], 34567890u);
            TEST_E(bi2.raw[bi2.uic - 2], 12u);
            TEST_E(bi2.effect, 2u);

            std::string str;
            for (size_t i = 0; i < 1000; ++i) {
                bi2.div2();
                auto b = bi2.getBottom();
                if (b) {
                    bi2.setBottom(0);
                    str.insert(str.begin(), '1');
                } else {
                    str.insert(str.begin(), '0');
                }

                if (bi2.isZero()) {
                    break;
                }
            }
            TEST_E(str, "1001001100101100000001011010010");
        }
        {
            char bins[] = "12345.67890";
            const char* r;
            BigUInt_b10 bi2(0);
            bi2.fromChars(bins, std::size(bins) - 1, &r);
            TEST_E(bi2.raw[bi2.uic - 1], 12345u);
            TEST_E(r, bins + 5);
        }
        {
            BigUInt_b10 bu_2c(5);
            for (size_t i = 0; i < 100; ++i) {
                bu_2c.mul2();
            }

            char cs[100];
            size_t cs_l;
            bu_2c.toChars<false>(cs, &cs_l);
            TEST_E(std::string(cs, cs_l), "00633825300114114700748351602688");

            cs_l = 3;
            bu_2c.toChars<false>(cs, &cs_l, 7);
            TEST_E(std::string(cs, cs_l), "835");

            cs_l = 3;
            bu_2c.toChars<false>(cs, &cs_l, 10);
            TEST_E(std::string(cs, cs_l), "074");
        }
        return true;
    };

    TEST_DEF("internal: BigUInt<uint32_t, 16>") {
        using namespace utl::internal;

        using BigUInt_b16 = BigUInt_bN<double, uint32_t, 16, 20>;

        BigUInt_b16 bi1(5);
        bi1.muld(8);
        bi1.muld(8);
        bi1.muld(8);
        bi1.muld(8);
        bi1.muld(8);
        bi1.muld(8);
        TEST_E(bi1.raw[bi1.uic - 1], 131072u);

        char bins[] = "1234567890";
        const char* r;
        BigUInt_b16 bi2(0);
        bi2.fromChars(bins, std::size(bins) - 1, &r);

        TEST_E(bi2.raw[bi2.uic - 1], 5666960);
        TEST_E(bi2.raw[bi2.uic - 2], 4660);

        std::string str;
        for (size_t i = 0; i < 1000; ++i) {
            bi2.div2();
            auto b = bi2.getBottom();
            if (b) {
                bi2.setBottom(0);
                str.insert(str.begin(), '1');
            } else {
                str.insert(str.begin(), '0');
            }

            if (bi2.isZero()) {
                break;
            }
        }
        TEST_E(str, "1001000110100010101100111100010010000");

        BigUInt_b16 bu_2c(5);
        for (size_t i = 0; i < 100; ++i) {
            bu_2c.mul2();
        }

        char cs[100];
        size_t cs_l;
        bu_2c.toChars<false>(cs, &cs_l);
        TEST_E(std::string(cs, cs_l), "000008000000000000000000000000");

        return true;
    };

    TEST_DEF("internal: BigFloat<uint32_t, 10>") {
        using namespace utl::internal;
        using BigFloat_b10 = BigFloat_bN<double, uint32_t, 10, 22>;
        {
            BigFloat_b10 bf1(1);
            bf1.divd(8);
            TEST_E(bf1.raw[0], 12500000u);
        }
        {
            BigFloat_b10 bf2(1);
            bf2.div2();
            bf2.div2();
            bf2.div2();
            TEST_E(bf2.raw[0], 12500000u);
            bf2.divd(8);
            bf2.divd(8);

            char s[100];
            size_t ls = 3;
            bf2.toChars<false>(s, &ls, 7);
            TEST_E(std::string(s, ls), "250");

            bf2.toChars<false>(s, &ls, 1);
            TEST_E(std::string(s, ls), "019");

            bf2.toChars<false>(s, &ls, 2);
            TEST_E(std::string(s, ls), "195");
        }
        {
            BigFloat_b10 bf(1);
            for (size_t i = 0; i < 52 / 3; ++i) {
                bf.divd(8);
            }
            for (size_t i = 0; i < 52 % 3; ++i) {
                bf.div2();
            }

            TEST_E(bf.raw[0], 0);
            TEST_E(bf.raw[1], 2);
            TEST_E(bf.raw[2], 22044604u);
            TEST_E(bf.raw[3], 92503130u);
            TEST_E(bf.raw[4], 80847263u);
            TEST_E(bf.raw[5], 33618164u);
            TEST_E(bf.raw[6], 6250000u);
            TEST_E(bf.effect, 7u);
        }
        {
            char bfns[] = "123456789";
            const char* r;
            BigFloat_b10 bfn(0);
            bfn.fromChars(bfns, std::size(bfns) - 1, 2, false, &r);
            TEST_E(bfn.raw[0], 123456u);
            TEST_E(bfn.raw[1], 78900000u);

            bfn.round(3, 0, utl::UFR_NEAR);
            TEST_E(bfn.raw[0], 123456u);
            TEST_E(bfn.raw[1], 78900000u);
        }
        {
            char bfns[] = "12345.a6789";
            const char* r;
            BigFloat_b10 bfn(0);
            bfn.fromChars(bfns, std::size(bfns) - 1, 2, false, &r);
            TEST_E(bfn.raw[0], 123450u);
            TEST_E(r, bfns + 5);
        }
        {
            char bfns[] = "1";
            const char* r;
            BigFloat_b10 bfn(0);
            bfn.fromChars(bfns, std::size(bfns) - 1, &r);

            TEST_E(bfn.raw[0], 10000000);
            TEST_E(bfn.effect, 1);

            std::string b_2;
            for (size_t i = 0; i < 1000; ++i) {
                bfn.mul2();
                auto top = bfn.getTop();
                if (top) {
                    b_2.push_back('1');
                    bfn.setTop(0);
                } else {
                    b_2.push_back('0');
                }

                if (bfn.isZero()) {
                    break;
                }
            }

            TEST_E(b_2,
                "000110011001100110011001100110011001100110011001100110011001100110011"
                "00110011001100110011001100110011001100110011001100110011001100110011"
                "00110011001100110011001100110011001100110011001100110011001100110011"
                "00110011001100110011001100110011001100110011001100110011001100110011"
                "00110011001100110011001100110011001100110011001100110011001100110011"
                "00110011001100110011001100110011001100110011001100110011001100110011"
                "00110011001100110011001100110011001100110011001100110011001100110011"
                "00110011001100110011001100110011001100110011001100110011001100110011"
                "00110011001100110011001100110011001100110011001100110011001100110011"
                "00110011001100110011001100110011001100110011001100110011001100110011"
                "00110011001100110011001100110011001100110011001100110011001100110011"
                "00110011001100110011001100110011001100110011001100110011001100110011"
                "00110011001100110011001100110011001100110011001100110011001100110011"
                "00110011001100110011001100110011001100110011001100110011001100110011"
                "00110011001100110011001100110011001100110011001");
        }
        {
            BigFloat_b10 bf_2c(1);
            for (size_t i = 0; i < 40; ++i) {
                bf_2c.div2();
            }

            char cs[100];
            size_t cs_l;
            bf_2c.toChars<false>(cs, &cs_l);

            TEST_E(std::string(cs, cs_l), "0000000000009094947017729282379150390625");
        }
        return true;
    };

    TEST_DEF("internal: BigFloat<uint32_t, 16>") {
        using namespace utl::internal;

        using BigFloat_b16 = BigFloat_bN<double, uint32_t, 16, 20>;

        BigFloat_b16 bf1(1);
        bf1.divd(8);
        TEST_E(bf1.raw[0], 0x200000u);

        BigFloat_b16 bf2(1);
        bf2.div2();
        bf2.div2();
        bf2.div2();
        TEST_E(bf2.raw[0], 0x200000u);

        BigFloat_b16 bf(1);
        for (size_t i = 0; i < 52 / 3; ++i) {
            bf.divd(8);
        }
        for (size_t i = 0; i < 52 % 3; ++i) {
            bf.div2();
        }

        TEST_E(bf.raw[0], 0);
        TEST_E(bf.raw[1], 0);
        TEST_E(bf.raw[2], 0x100000);
        TEST_E(bf.effect, 3);

        char bfns[] = "1";
        const char* r;
        BigFloat_b16 bfn(0);
        bfn.fromChars(bfns, std::size(bfns) - 1, &r);

        TEST_E(bfn.raw[0], 0x100000);
        TEST_E(bfn.effect, 1);

        std::string b_2;
        for (size_t i = 0; i < 1000; ++i) {
            bfn.mul2();
            auto top = bfn.getTop();
            if (top) {
                b_2.push_back('1');
                bfn.setTop(0);
            } else {
                b_2.push_back('0');
            }

            if (bfn.isZero()) {
                break;
            }
        }

        TEST_E(b_2, "0001");

        BigFloat_b16 bf_2c(1);
        for (size_t i = 0; i < 40; ++i) {
            bf_2c.div2();
        }

        char cs[100];
        size_t cs_l;
        bf_2c.toChars<false>(cs, &cs_l);

        TEST_E(std::string(cs, cs_l), "000000000100");

        return true;
    };

}

TEST_CASE(InternalUIntExp) {

    TEST_DEF("UIntExp tests.") {
        using uint128_t = utl::internal::UIntExp<double, 2>;
        uint128_t uv = 0x1234'5678;
        TEST_E(uv, 0x1234'5678);
        uv <<= 96;
        TEST_E(uv.d[1], 0);
        TEST_E(uv.d[0], 0x1234'5678'0000'0000u);
        TEST_FALSE(uv & 1u);
        uv >>= 96;
        TEST_E(uv, 0x1234'5678);
        TEST_FALSE(uv & 1u);
        uv <<= 96;
        uv -= 1;

        return true;
    };

}

TEST_CASE(FloatingConv_Float_Dec) {

    TEST_DEF("Float  -> String. (Dec, Nor)") {
        auto _TEST_FUNC = [](float val, int precision, const std::string& r) -> bool {
            char buf[1000]{ 0 };
            size_t buf_len = 1000;

            std::string result;
            utl::ftos(val, &result, precision);
            TEST_TRUE(utl::ftos(val, buf, &buf_len, precision));
#ifdef USING_CHARCONV
            char buf2[1000]{ 0 };
            std::to_chars(std::begin(buf2), std::end(buf2), val, std::chars_format::fixed, precision);
#endif
            TEST_E(std::string(buf, buf_len), r);
            TEST_E(result, r);
            return true;
        };

        auto _TEST_RAND = []() -> bool {
            std::random_device rd;
            std::default_random_engine eng(rd());
            std::uniform_real_distribution<float> dist_f(
                std::numeric_limits<float>::denorm_min(), (std::numeric_limits<float>::max)());
            std::uniform_int_distribution<int> dist_i(0, 99);

            for (size_t i = 0; i < RAND_LOOP; ++i) {
                float val = dist_f(eng);
                int precision = dist_i(eng);

                std::string result;
                utl::ftos(val, &result, precision);

                char buf[200]{ 0 };
                size_t buf_len = 200;
                TEST_TRUE_BRK(utl::ftos(val, buf, &buf_len, precision));
                TEST_E_BRK(std::string(buf, buf_len), result);

#ifdef USING_CHARCONV
                char buf2[200]{ 0 };
                std::to_chars(std::begin(buf2), std::end(buf2), val, std::chars_format::fixed, precision);

                TEST_E_BRK(result, std::string(buf2));
#endif
            }
            return true;
        };

        TEST_TRUE(_TEST_FUNC(0.0f, 0, "0"));
        TEST_TRUE(_TEST_FUNC(-0.0f, 0, "-0"));
        TEST_TRUE(_TEST_FUNC(0.1f, 0, "0"));
        TEST_TRUE(_TEST_FUNC(0.1f, 1, "0.1"));
        TEST_TRUE(_TEST_FUNC(0.34578f, 5, "0.34578"));
        TEST_TRUE(_TEST_FUNC(1.0f, 3, "1.000"));
        TEST_TRUE(_TEST_FUNC(1.666666666f, 0, "2"));
        TEST_TRUE(_TEST_FUNC(1.666666666f, 2, "1.67"));
        TEST_TRUE(_TEST_FUNC(8.1f, 0, "8"));
        TEST_TRUE(_TEST_FUNC(8.1f, 2, "8.10"));
        TEST_TRUE(_TEST_FUNC(-8.1f, 2, "-8.10"));
        TEST_TRUE(_TEST_FUNC(999.999f, 3, "999.999"));
        TEST_TRUE(_TEST_FUNC(1234567.0f, 2, "1234567.00"));
        TEST_TRUE(_TEST_FUNC(1.0f / (uint64_t(1) << 32), 40, "0.0000000002328306436538696289062500000000"));

        TEST_TRUE(_TEST_FUNC(1234567978598848646898790.0f, 2, "1234567946798590058299392.00"));
        TEST_TRUE(_TEST_FUNC(1234567978598848646898790.3f, 2, "1234567946798590058299392.00"));
        TEST_TRUE(_TEST_FUNC(-1234567978598848646898790.3f, 2, "-1234567946798590058299392.00"));
        TEST_TRUE(_TEST_FUNC(8945745684576845845.45764856784578456f, 2, "8945745900597149696.00"));
        TEST_TRUE(_TEST_FUNC(0.000000000000000000000000000000000003f, 10, "0.0000000000"));
        TEST_TRUE(_TEST_FUNC(1.23456789e-6f, 5, "0.00000"));
        TEST_TRUE(_TEST_FUNC(-1.23456789e-6f, 5, "-0.00000"));
        TEST_TRUE(_TEST_FUNC(std::numeric_limits<float>::quiet_NaN(), 0, "nan"));

        TEST_TRUE(_TEST_FUNC(
            0.000000000000000000000000000000000000666666666f,
            100,
            "0.0000000000000000000000000000000000006666666457845356908353807450461843001623376951223899472250601785"));
        TEST_TRUE(_TEST_FUNC(
            0.000000000000000000000000000000000000000000001f,
            100,
            "0.0000000000000000000000000000000000000000000014012984643248170709237295832899161312802619418765157718"));
        TEST_TRUE(_TEST_FUNC(
            0.000000000000000000000000000001f,
            200,
            "0.00000000000000000000000000000100000000317107685097105134713526475381475147564611094530562240834115073084831237792968750000000"
            "000000000000000000000000000000000000000000000000000000000000000000000000000"));
        TEST_TRUE(_TEST_FUNC(
            99999999999999999999999999999999999999.0f,
            100,
            "99999996802856924650656260769173209088.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000"));
        TEST_TRUE(_TEST_FUNC(3.4e38f, 2, "339999995214436424907732413799364296704.00"));
        TEST_TRUE(_TEST_FUNC(
            (std::numeric_limits<float>::min)(),
            200,
            "0.00000000000000000000000000000000000001175494350822287507968736537222245677818665556772087521508751706278417259454727172851562"
            "500000000000000000000000000000000000000000000000000000000000000000000000000"));
        TEST_TRUE(_TEST_FUNC(
            (std::numeric_limits<float>::max)(),
            200,
            "340282346638528859811704183484516925440.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));

        if constexpr (std::numeric_limits<float>::has_denorm == std::denorm_present) {
            TEST_TRUE(_TEST_FUNC(
                0x1p-127f,
                200,
                "0.00000000000000000000000000000000000000587747175411143753984368268611122838909332778386043760754375853139208629727363586425781"
                "250000000000000000000000000000000000000000000000000000000000000000000000000"));
            TEST_TRUE(_TEST_FUNC(
                std::numeric_limits<float>::denorm_min(),
                200,
                "0.00000000000000000000000000000000000000000000140129846432481707092372958328991613128026194187651577175706828388979108268586060"
                "148663818836212158203125000000000000000000000000000000000000000000000000000"));
        }

        {
            float t = -0.65852354f;
            for (size_t i = 0; i < 10; ++i) {
                size_t buf_size = i;
                bool ret1 = utl::ftos(t, static_cast<char*>(nullptr), &buf_size, 7, utl::UFF_FIX | utl::UFF_UPP, utl::UFR_FLOOR);
                TEST_E(buf_size, 10);
                TEST_FALSE(ret1);
            }
        }

        TEST_TRUE(_TEST_RAND());
        return true;
    };

    TEST_DEF("Float  -> String. (Dec, Sci)") {
        auto _TEST_FUNC = [](float val, int precision, const std::string& r) -> bool {
            char buf[1000]{ 0 };
            size_t buf_len = 1000;

            std::string result;
            utl::ftos(val, &result, precision, utl::UFF_SCI);
            TEST_TRUE(utl::ftos(val, buf, &buf_len, precision, utl::UFF_SCI));
#ifdef USING_CHARCONV
            char buf2[1000]{ 0 };
            std::to_chars(std::begin(buf2), std::end(buf2), val, std::chars_format::scientific, precision);
#endif
            TEST_E(std::string(buf, buf_len), r);
            TEST_E(result, r);
            return true;
        };

        auto _TEST_FUNC_MIX = [](float val, int precision, const std::string& r) -> bool {
            std::string result;
            utl::ftos(val, &result, precision, utl::UFF_SCI | utl::UFF_FIX);
#ifdef USING_CHARCONV
            char buf[1000]{ 0 };
            std::to_chars(std::begin(buf), std::end(buf), val, std::chars_format::general, precision);
#endif
            TEST_E(result, r);
            return true;
        };

        auto _TEST_RAND = []() -> bool {
            std::random_device rd;
            std::default_random_engine eng(rd());
            std::uniform_real_distribution<float> dist_f(
                std::numeric_limits<float>::denorm_min(), (std::numeric_limits<float>::max)());
            std::uniform_int_distribution<int> dist_i(0, 99);

            for (size_t i = 0; i < RAND_LOOP; ++i) {
                float val = dist_f(eng);
                int precision = dist_i(eng);

                std::string result;
                utl::ftos(val, &result, precision, utl::UFF_SCI);

                char buf[200]{ 0 };
                size_t buf_len = 200;
                TEST_TRUE_BRK(utl::ftos(val, buf, &buf_len, precision, utl::UFF_SCI));
                TEST_E_BRK(std::string(buf, buf_len), result);

#ifdef USING_CHARCONV
                char buf2[200]{ 0 };
                std::to_chars(std::begin(buf2), std::end(buf2), val, std::chars_format::scientific, precision);

                TEST_E_BRK(result, std::string(buf2));
#endif
            }
            return true;
        };

        auto _TEST_RAND_MIX = []() -> bool {
            std::random_device rd;
            std::default_random_engine eng(rd());
            std::uniform_real_distribution<float> dist_f(
                std::numeric_limits<float>::denorm_min(), (std::numeric_limits<float>::max)());
            std::uniform_int_distribution<int> dist_i(0, 99);

            for (size_t i = 0; i < RAND_LOOP; ++i) {
                float val = dist_f(eng);
                int precision = dist_i(eng);

                std::string result;
                utl::ftos(val, &result, precision, utl::UFF_SCI | utl::UFF_FIX);

                char buf[200]{ 0 };
                size_t buf_len = 200;
                TEST_TRUE_BRK(utl::ftos(val, buf, &buf_len, precision, utl::UFF_SCI | utl::UFF_FIX));
                TEST_E_BRK(std::string(buf, buf_len), result);

#ifdef USING_CHARCONV
                //char buf2[200]{ 0 };
                //std::to_chars(std::begin(buf2), std::end(buf2), val, std::chars_format::general, precision);

                //TEST_E_BRK(result, std::string(buf2));
#endif
            }
            return true;
        };

        TEST_TRUE(_TEST_FUNC_MIX(0.0f, 0, "0"));
        TEST_TRUE(_TEST_FUNC_MIX(1.666666666f, 2, "1.7"));
        TEST_TRUE(_TEST_FUNC_MIX(16.66666666f, 3, "16.7"));
        TEST_TRUE(_TEST_FUNC_MIX(166.6666666f, 3, "167"));
        TEST_TRUE(_TEST_FUNC_MIX(1666.666666f, 3, "1.67e+03"));
        TEST_TRUE(_TEST_FUNC_MIX(.1666666666f, 2, "0.17"));
        TEST_TRUE(_TEST_FUNC_MIX(.0001666666666f, 2, "0.00017"));
        TEST_TRUE(_TEST_FUNC_MIX(.00001666666666f, 2, "1.7e-05"));
        TEST_TRUE(_TEST_FUNC_MIX(9.79964269e+35f, 0, "1e+36"));

        TEST_TRUE(_TEST_FUNC(0.0f, 0, "0e+00"));
        TEST_TRUE(_TEST_FUNC(0.0f, 4, "0.0000e+00"));
        TEST_TRUE(_TEST_FUNC(-0.0f, 4, "-0.0000e+00"));
        TEST_TRUE(_TEST_FUNC(8.1f, 2, "8.10e+00"));
        TEST_TRUE(_TEST_FUNC(1.666666666f, 2, "1.67e+00"));
        TEST_TRUE(_TEST_FUNC(166.666666666f, 2, "1.67e+02"));
        TEST_TRUE(_TEST_FUNC(-166.666666666f, 2, "-1.67e+02"));
        TEST_TRUE(_TEST_FUNC(16666.666666666f, 2, "1.67e+04"));
        TEST_TRUE(_TEST_FUNC(0.34578f, 4, "3.4578e-01"));
        TEST_TRUE(_TEST_FUNC(1.0f, 4, "1.0000e+00"));
        TEST_TRUE(_TEST_FUNC(999.999f, 3, "1.000e+03"));
        TEST_TRUE(_TEST_FUNC(1.0 / (uint64_t(1) << 32), 40, "2.3283064365386962890625000000000000000000e-10"));
        TEST_TRUE(_TEST_FUNC(1234567978598848646898790.0f, 10, "1.2345679468e+24"));
        TEST_TRUE(_TEST_FUNC(1234567978598848646898790.3f, 10, "1.2345679468e+24"));
        TEST_TRUE(_TEST_FUNC(8945745684576845845.45764856784578456f, 10, "8.9457459006e+18"));
        TEST_TRUE(_TEST_FUNC(0.000000000000000000000000000000000003f, 20, "3.00000001813428775474e-36"));
        TEST_TRUE(_TEST_FUNC(1.23456789e-6f, 10, "1.2345678897e-06"));
        TEST_TRUE(_TEST_FUNC(-1.23456789e-6f, 10, "-1.2345678897e-06"));
        TEST_TRUE(_TEST_FUNC(
            0.000000000000000000000000000000000000666666666f,
            120,
            "6.666666457845356908353807450461843001623376951223899472250601785190335379915893554425565525889396667480468750000000000000e-37"));
        TEST_TRUE(_TEST_FUNC(
            0.000000000000000000000000000000000000000000001f,
            120,
            "1.401298464324817070923729583289916131280261941876515771757068283889791082685860601486638188362121582031250000000000000000e-45"));
        TEST_TRUE(_TEST_FUNC(
            99999999999999999999999999999999999999.0f,
            200,
            "9.99999968028569246506562607691732090880000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000000000000000000e+37"));
        TEST_TRUE(_TEST_FUNC(
            1234567.0,
            200,
            "1.23456700000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000000000000000000e+06"));
        TEST_TRUE(_TEST_FUNC(3.4e38f, 10, "3.3999999521e+38"));
        TEST_TRUE(_TEST_FUNC(
            (std::numeric_limits<float>::min)(),
            200,
            "1.17549435082228750796873653722224567781866555677208752150875170627841725945472717285156250000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000000000000000000e-38"));
        TEST_TRUE(_TEST_FUNC(
            (std::numeric_limits<float>::max)(),
            200,
            "3.40282346638528859811704183484516925440000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000000000000000000e+38"));
        TEST_TRUE(_TEST_FUNC(std::numeric_limits<float>::quiet_NaN(), 0, "nan"));

        if constexpr (std::numeric_limits<float>::has_denorm == std::denorm_present) {
            TEST_TRUE(_TEST_FUNC(
                std::numeric_limits<float>::denorm_min(),
                200,
                "1.40129846432481707092372958328991613128026194187651577175706828388979108268586060148663818836212158203125000000000000000000000"
                "000000000000000000000000000000000000000000000000000000000000000000000000000e-45"));
        }

        {
            float t = -0.65852354f;
            for (size_t i = 0; i < 10; ++i) {
                size_t buf_size = i;
                bool ret1 = utl::ftos(t, static_cast<char*>(nullptr), &buf_size, 3, utl::UFF_SCI | utl::UFF_UPP, utl::UFR_FLOOR);
                TEST_E(buf_size, 10);
                TEST_FALSE(ret1);
            }
        }

        TEST_TRUE(_TEST_RAND());
        TEST_TRUE(_TEST_RAND_MIX());

        return true;
    };

    TEST_DEF("String -> Float.  (Dec, Nor)") {
        auto _TEST_FUNC = [](const std::string& text, float r) -> bool {
            float result1;
            utl::stof(text, &result1);
#ifdef USING_CHARCONV
            float result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::fixed);
            TEST_E(result1, result2);
#endif
            TEST_E(result1, r);
            return true;
        };
        auto _TEST_FUNC_NAN = [](const std::string& text) -> bool {
            float result1;
            utl::stof(text, &result1);
#ifdef USING_CHARCONV
            float result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::fixed);
#endif
            return std::isnan(result1);
        };

        TEST_TRUE(_TEST_FUNC("0", 0.0f));
        TEST_TRUE(_TEST_FUNC("-0", -0.0f));
        TEST_TRUE(_TEST_FUNC("8.1", 8.1f));
        TEST_TRUE(_TEST_FUNC("1.", 1.0f));
        TEST_TRUE(_TEST_FUNC("0.0", 0.0f));
        TEST_TRUE(_TEST_FUNC("1234567.0", 1234567.0f));
        TEST_TRUE(_TEST_FUNC("-1234567.0", -1234567.0f));
        TEST_TRUE(_TEST_FUNC("457686785756874490000000123", 457686785756874490000000123.0f));
        TEST_TRUE(_TEST_FUNC("1234567978598848646898790", 1234567978598848646898790.0f));
        TEST_TRUE(_TEST_FUNC("1234567978598848646898790.3", 1234567978598848646898790.3f));
        TEST_TRUE(_TEST_FUNC("8945745684576845845.45764856784578456", 8945745684576845845.45764856784578456f));
        TEST_TRUE(_TEST_FUNC(
            "0.000000000000000000000000000000000000000000001",
            DENORM_VAL(0.000000000000000000000000000000000000000000001f)));
        TEST_TRUE(_TEST_FUNC(
            "99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999",
            std::numeric_limits<float>::infinity()));
        TEST_TRUE(_TEST_FUNC(
            "99999999999999999999999999999999999999.0",
            99999999999999999999999999999999999999.0f));
        TEST_TRUE(_TEST_FUNC("0.123456", 0.123456f));
        TEST_TRUE(_TEST_FUNC("0.000123456789e-2", 0.000123456789f));
        TEST_TRUE(_TEST_FUNC("170000000000000000000000000000000000000.0", 170000000000000000000000000000000000000.0f));
        TEST_TRUE(_TEST_FUNC("340282346638528859811704183484516925440.000000", (std::numeric_limits<float>::max)()));
        //                          |
        TEST_TRUE(_TEST_FUNC("340282446638528859811704183484516925440.0000000", std::numeric_limits<float>::infinity()));
        TEST_TRUE(_TEST_FUNC(
            "0.0000000000000000000000000000000000000117549435082228750796873653722224567781866555677208752150875170627841725945472717285156250",
            (std::numeric_limits<float>::min)()));
        TEST_TRUE(_TEST_FUNC(
            "0.0000000000000000000000000000000000000058774717541114375398436826861112283890933277838604376075437585313920862972736358642578125",
            DENORM_VAL(0x1p-127f)));

        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000140129846432481707092372958328991613128026194187651577175706828388979108268586060148663818836212158203125000",
            DENORM_VAL(std::numeric_limits<float>::denorm_min())));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000060129846432481707092372958328991613128026194187651577175706828388979108268586060148663818836212158203125000",
            0));

        TEST_TRUE(_TEST_FUNC_NAN("nan"));
        TEST_TRUE(_TEST_FUNC_NAN("nan()"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(ind)"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(snan)"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan(snan)"));

        /*
         * float
         * 4150 -> 8|81
         * 4148 -> 8|80
         * 4140 -> 8|70
         */
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000025144954150", 2.51449549e-29f));
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000025144954148", 2.51449549e-29f));
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000025144954140", 2.51449534e-29f));

        /*
         * float
         * 2650 -> 7|81
         * 2640 -> 7|80
         * 2600 -> 7|79
         */
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000025144952650", 2.51449534e-29f));
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000025144952640", 2.51449534e-29f));
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000025144952600", 2.51449519e-29f));

        return true;
    };

    TEST_DEF("String -> Float.  (Dec, Sci)") {
        auto _TEST_FUNC = [](const std::string& text, float r) -> bool {
            float result1;
            utl::stof(text, &result1, utl::UFF_SCI);
#ifdef USING_CHARCONV
            float result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::scientific);
#endif
            TEST_E(result1, r);
            return true;
        };
        auto _TEST_FUNC_NAN = [](const std::string& text) -> bool {
            float result1;
            utl::stof(text, &result1, utl::UFF_SCI);
#ifdef USING_CHARCONV
            float result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::scientific);
#endif
            return std::isnan(result1);
        };

        TEST_TRUE(_TEST_FUNC("8.1", 8.1f));
        TEST_TRUE(_TEST_FUNC("0.0e0", 0.0f));
        TEST_TRUE(_TEST_FUNC("1", 1.0f));
        TEST_TRUE(_TEST_FUNC("800e-1", 800e-1f));
        TEST_TRUE(_TEST_FUNC("800e-12", 800e-12f));
        TEST_TRUE(_TEST_FUNC("-800e-12", -800e-12f));
        TEST_TRUE(_TEST_FUNC("1234567.0", 1234567.0f));
        TEST_TRUE(_TEST_FUNC("-1234567.0", -1234567.0f));
        TEST_TRUE(_TEST_FUNC("1234567.0e10", 1234567.0e10f));
        TEST_TRUE(_TEST_FUNC("-1234567.0e10", -1234567.0e10f));
        TEST_TRUE(_TEST_FUNC("45768678575", 45768678575.0f));
        TEST_TRUE(_TEST_FUNC("123456797859", 123456797859.0f));
        TEST_TRUE(_TEST_FUNC("123456797859.3", 123456797859.3f));
        TEST_TRUE(_TEST_FUNC("894574568457.45764856784578456", 894574568457.45764856784578456f));
        TEST_TRUE(_TEST_FUNC(
            "0.000000000000000000000000000000000000000000001",
            DENORM_VAL(0.000000000000000000000000000000000000000000001f)));
        TEST_TRUE(_TEST_FUNC(
            "99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999",
            std::numeric_limits<float>::infinity()));
        TEST_TRUE(_TEST_FUNC(
            "99999999999999999999999999999999999999",
            99999999999999999999999999999999999999.0f));

        TEST_TRUE(_TEST_FUNC("0.123456", 0.123456f));
        TEST_TRUE(_TEST_FUNC("0.000123456789e-2", 0.000123456789e-2f));
        TEST_TRUE(_TEST_FUNC("2.900123456789e38", 2.900123456789e38f));
        TEST_TRUE(_TEST_FUNC("9.900123456789e19", 9.900123456789e19f));
        TEST_TRUE(_TEST_FUNC("9.470328e-45", DENORM_VAL(9.470328e-45f)));
        TEST_TRUE(_TEST_FUNC("1.070328e-23", 1.070328e-23f));
        TEST_TRUE(_TEST_FUNC("340282346638528859811704183484516925440.0000e0", (std::numeric_limits<float>::max)()));
        TEST_TRUE(_TEST_FUNC("340282346638528859811704183484516925440.0000e1", std::numeric_limits<float>::infinity()));
        TEST_TRUE(_TEST_FUNC(
            "0.0000000000000000000000000000000000000117549435082228750796873653722224567781866555677208752150875170627841725945472717285156250e0",
            (std::numeric_limits<float>::min)()));
        TEST_TRUE(_TEST_FUNC(
            "0.0000000000000000000000000000000000000058774717541114375398436826861112283890933277838604376075437585313920862972736358642578125",
            DENORM_VAL(0x1p-127f)));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000140129846432481707092372958328991613128026194187651577175706828388979108268586060148663818836212158203125000e0",
            DENORM_VAL(std::numeric_limits<float>::denorm_min())));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000060129846432481707092372958328991613128026194187651577175706828388979108268586060148663818836212158203125000e0",
            0.0f));

        TEST_TRUE(_TEST_FUNC_NAN("nan"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan"));
        TEST_TRUE(_TEST_FUNC_NAN("nan()"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(ind)"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(snan)"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan(snan)"));

        return true;
    };

}

TEST_CASE(FloatingConv_Float_Hex) {

    TEST_DEF("Float  -> String. (Hex, Nor)") {
        auto _TEST_FUNC = [](float val, int precision, const std::string& r) -> bool {
            char buf[1000]{ 0 };
            size_t buf_len = 1000;

            std::string result;
            utl::ftos(val, &result, precision, utl::UFF_HEX);
            TEST_TRUE(utl::ftos(val, buf, &buf_len, precision, utl::UFF_HEX));
#ifdef USING_CHARCONV
            char buf2[1000]{ 0 };
            std::to_chars(std::begin(buf2), std::end(buf2), val, std::chars_format::hex, precision);
#endif
            TEST_E(std::string(buf, buf_len), r);
            TEST_E(result, r);
            return true;
        };

        TEST_TRUE(_TEST_FUNC(18.0f, 2, "12.00"));
        TEST_TRUE(_TEST_FUNC(0.0f, 0, "0"));
        TEST_TRUE(_TEST_FUNC(-0.0f, 0, "-0"));
        TEST_TRUE(_TEST_FUNC(8.1f, 0, "8"));
        TEST_TRUE(_TEST_FUNC(1.0f, 0, "1"));
        TEST_TRUE(_TEST_FUNC(0.34578f, 5, "0.58851"));
        TEST_TRUE(_TEST_FUNC(-0.34578f, 5, "-0.58851"));
        TEST_TRUE(_TEST_FUNC(999.999f, 3, "3e7.ffc"));
        TEST_TRUE(_TEST_FUNC(1234567978598848646898790.3f, 2, "1056e1000000000000000.00"));
        TEST_TRUE(_TEST_FUNC(8945745684576845845.45764856784578456f, 2, "7c25ac8000000000.00"));
        TEST_TRUE(_TEST_FUNC(1.23456789e-6f, 5, "0.00001"));
        TEST_TRUE(_TEST_FUNC(1234567.0f, 2, "12d687.00"));
        TEST_TRUE(_TEST_FUNC(
            (std::numeric_limits<float>::min)(),
            100,
            "0.0000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000000000"));
        TEST_TRUE(_TEST_FUNC(
            (std::numeric_limits<float>::max)(),
            100,
            "ffffff00000000000000000000000000.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
        TEST_TRUE(_TEST_FUNC(
            std::numeric_limits<float>::quiet_NaN(), 0, "nan"));

        if constexpr (std::numeric_limits<float>::has_denorm == std::denorm_present) {
            TEST_TRUE(_TEST_FUNC(
                std::numeric_limits<float>::denorm_min(),
                100,
                "0.0000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000"));
        }

        {
            float t = -0.65852354f;
            for (size_t i = 0; i < 10; ++i) {
                size_t buf_size = i;
                bool ret1 = utl::ftos(t, static_cast<char*>(nullptr), &buf_size, 7, utl::UFF_HEX | utl::UFF_UPP);
                TEST_E(buf_size, 10);
                TEST_FALSE(ret1);
            }
        }

        return true;
    };

    TEST_DEF("Float  -> String. (Hex, Sci)") {
        auto _TEST_FUNC = [](float val, int precision, const std::string& r) -> bool {
            char buf[1000]{ 0 };
            size_t buf_len = 1000;

            std::string result;
            utl::ftos(val, &result, precision, utl::UFF_SCI | utl::UFF_HEX);
            TEST_TRUE(utl::ftos(val, buf, &buf_len, precision, utl::UFF_SCI | utl::UFF_HEX));
#ifdef USING_CHARCONV
            char buf2[1000]{ 0 };
            std::to_chars(std::begin(buf2), std::end(buf2), val, std::chars_format::hex, precision);
#endif
            TEST_E(std::string(buf, buf_len), r);
            TEST_E(result, r);
            return true;
        };
        auto _TEST_FUNC2 = [](float val, int precision, const std::string& r) -> bool {
            char buf[1000]{ 0 };
            std::string result;
            size_t buf_len = 1000;

            utl::ftos(val, &result, precision, utl::UFF_HEX2);
            TEST_TRUE(utl::ftos(val, buf, &buf_len, precision, utl::UFF_HEX2));
#ifdef USING_CHARCONV
            char buf2[1000]{ 0 };
            std::to_chars(std::begin(buf2), std::end(buf2), val, std::chars_format::hex, precision);
#endif
            TEST_E(std::string(buf, buf_len), r);
            TEST_E(result, r);
            return true;
        };

        auto _TEST_RAND = []() -> bool {
            std::random_device rd;
            std::default_random_engine eng(rd());
            std::uniform_real_distribution<float> dist_f(
                std::numeric_limits<float>::denorm_min(), (std::numeric_limits<float>::max)());
            std::uniform_int_distribution<int> dist_i(0, 99);

            for (size_t i = 0; i < RAND_LOOP; ++i) {
                float val = dist_f(eng);
                int precision = dist_i(eng);

                std::string result;
                utl::ftos(val, &result, precision, utl::UFF_HEX2);

                char buf[200]{ 0 };
                size_t buf_len = 200;
                TEST_TRUE_BRK(utl::ftos(val, buf, &buf_len, precision, utl::UFF_HEX2));
                TEST_E_BRK(std::string(buf, buf_len), result);

#ifdef USING_CHARCONV
                char buf2[200]{ 0 };
                std::to_chars(std::begin(buf2), std::end(buf2), val, std::chars_format::hex, precision);
                TEST_E_BRK(result, std::string(buf2));
#endif
            }
            return true;
        };

        TEST_TRUE(_TEST_FUNC(8.1f, 2, "8.1as+00"));
        TEST_TRUE(_TEST_FUNC2(8.1f, 2, "1.03p+03"));
        TEST_TRUE(_TEST_FUNC(0.0f, 4, "0.0000s+00"));
        TEST_TRUE(_TEST_FUNC2(0.0f, 2, "0.00p+00"));
        TEST_TRUE(_TEST_FUNC(-0.0f, 4, "-0.0000s+00"));
        TEST_TRUE(_TEST_FUNC2(-0.0f, 4, "-0.0000p+00"));
        TEST_TRUE(_TEST_FUNC(1.0f, 4, "1.0000s+00"));
        TEST_TRUE(_TEST_FUNC2(1.0f, 4, "1.0000p+00"));
        TEST_TRUE(_TEST_FUNC(0.34578f, 4, "5.8851s-01"));
        TEST_TRUE(_TEST_FUNC2(0.34578f, 4, "1.6214p-02"));
        TEST_TRUE(_TEST_FUNC(999.999f, 3, "3.e80s+02"));
        TEST_TRUE(_TEST_FUNC2(999.999f, 3, "1.f40p+09"));
        TEST_TRUE(_TEST_FUNC(-999.999f, 3, "-3.e80s+02"));
        TEST_TRUE(_TEST_FUNC2(-999.999f, 3, "-1.f40p+09"));
        TEST_TRUE(_TEST_FUNC(1234567978598848646898790.3f, 10, "1.056e100000s+20"));
        TEST_TRUE(_TEST_FUNC2(1234567978598848646898790.3f, 10, "1.056e100000p+80"));
        TEST_TRUE(_TEST_FUNC(8945745684576845845.45764856784578456f, 10, "7.c25ac80000s+15"));
        TEST_TRUE(_TEST_FUNC(1.23456789e-6f, 10, "1.4b66dc0000s-05"));
        TEST_TRUE(_TEST_FUNC(
            (std::numeric_limits<float>::min)(),
            100,
            "4.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s-32"));
        TEST_TRUE(_TEST_FUNC2(
            (std::numeric_limits<float>::min)(),
            100,
            "1.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000p-126"));
        TEST_TRUE(_TEST_FUNC(
            (std::numeric_limits<float>::max)(),
            100,
            "f.fffff00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s+31"));
        TEST_TRUE(_TEST_FUNC2(
            (std::numeric_limits<float>::max)(),
            100,
            "1.fffffe0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000p+127"));
        TEST_TRUE(_TEST_FUNC(std::numeric_limits<float>::quiet_NaN(), 2, "nan"));
        TEST_TRUE(_TEST_FUNC2(1.66600727e+38f, 4, "1.f559p+126"));

        if constexpr (std::numeric_limits<float>::has_denorm == std::denorm_present) {
            TEST_TRUE(_TEST_FUNC(
                std::numeric_limits<float>::denorm_min(),
                100,
                "8.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s-38"));
            TEST_TRUE(_TEST_FUNC2(
                std::numeric_limits<float>::denorm_min(),
                100,
                "0.0000020000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000p-126"));
        }

        {
            float t = -0.65852354f;
            for (size_t i = 0; i < 10; ++i) {
                size_t buf_size = i;
                bool ret1 = utl::ftos(t, static_cast<char*>(nullptr), &buf_size, 3, utl::UFF_HEX | utl::UFF_SCI);
                TEST_E(buf_size, 10);
                TEST_FALSE(ret1);
            }
        }

        TEST_TRUE(_TEST_RAND());
        return true;
    };

    TEST_DEF("String -> Float.  (Hex, Nor)") {
        auto _TEST_FUNC = [](const std::string& text, float r) -> bool {
            float result1;
            utl::stof(text, &result1, utl::UFF_FIX | utl::UFF_HEX);
#ifdef USING_CHARCONV
            float result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::hex);
#endif
            TEST_E(result1, r);
            return true;
        };
        auto _TEST_FUNC_NAN = [](const std::string& text) -> bool {
            float result1;
            utl::stof(text, &result1, utl::UFF_FIX | utl::UFF_HEX);
#ifdef USING_CHARCONV
            float result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::hex);
#endif
            return std::isnan(result1);
        };

        TEST_TRUE(_TEST_FUNC("8.1", 8.0625f));
        TEST_TRUE(_TEST_FUNC("0", 0.0f));
        TEST_TRUE(_TEST_FUNC("-0", -0.0f));
        TEST_TRUE(_TEST_FUNC("0.0", 0.0f));
        TEST_TRUE(_TEST_FUNC(".1", 0.0625f));
        TEST_TRUE(_TEST_FUNC("1.", 1.0f));
        TEST_TRUE(_TEST_FUNC("1234567.0", 19088743.0f));
        TEST_TRUE(_TEST_FUNC("-1234567.0", -19088743.0f));
        TEST_TRUE(_TEST_FUNC("457686785756874490000000123", 8.8054800143969061e+31f));
        TEST_TRUE(_TEST_FUNC("1234567978598848646898790", 9.0144042938397500e+28f));
        TEST_TRUE(_TEST_FUNC("1234567978598848646898790.3", 9.0144042938397500e+28f));
        TEST_TRUE(_TEST_FUNC("8945745684576845845.45764856784578456", 4.0515338532968418e+22f));
        TEST_TRUE(_TEST_FUNC("-8945745684576845845.45764856784578456", -4.0515338532968418e+22f));
        TEST_TRUE(_TEST_FUNC("0.123456", 0.071111083030700684f));
        TEST_TRUE(_TEST_FUNC(
            "ffffff00000000000000000000000000.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
            (std::numeric_limits<float>::max)()));
        TEST_TRUE(_TEST_FUNC(
            "FFFFFFC0000000000000000000000000.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
            std::numeric_limits<float>::infinity()));
        TEST_TRUE(_TEST_FUNC(
            "0.0000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000000000",
            (std::numeric_limits<float>::min)()));
        TEST_TRUE(_TEST_FUNC(
            "0.0000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000000000",
            DENORM_VAL(0x1p-127f)));
        TEST_TRUE(_TEST_FUNC(
            "0.0000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000",
            DENORM_VAL(std::numeric_limits<float>::denorm_min())));
        TEST_TRUE(_TEST_FUNC(
            "0.0000000000000000000000000000000000000100000000000000000000000000000000000000000000000000000000000000",
            0.0f));

        TEST_TRUE(_TEST_FUNC_NAN("nan"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan"));
        TEST_TRUE(_TEST_FUNC_NAN("nan()"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(ind)"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(snan)"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan(snan)"));

        return true;
    };

    TEST_DEF("String -> Float.  (Hex, Sci)") {
        auto _TEST_FUNC = [](const std::string& text, float r) -> bool {
            float result1;
            utl::stof(text, &result1, utl::UFF_SCI | utl::UFF_HEX);
#ifdef USING_CHARCONV
            float result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::hex);
#endif
            TEST_E(result1, r);
            return true;
        };
        auto _TEST_FUNC2 = [](const std::string& text, float r) -> bool {
            float result1;
            utl::stof(text, &result1, utl::UFF_HEX2);
#ifdef USING_CHARCONV
            float result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::hex);
#endif
            TEST_E(result1, r);
            return true;
        };
        auto _TEST_FUNC_NAN = [](const std::string& text) -> bool {
            float result1;
            utl::stof(text, &result1, utl::UFF_SCI);
#ifdef USING_CHARCONV
            float result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::hex);
#endif
            return std::isnan(result1);
        };

        TEST_TRUE(_TEST_FUNC("1.fffffffffffffffffffffff", 2.0f));

        TEST_TRUE(_TEST_FUNC("8.1", 8.0625f));
        TEST_TRUE(_TEST_FUNC2("8.1", 8.0625f));
        TEST_TRUE(_TEST_FUNC("0.0s0", 0.0f));
        TEST_TRUE(_TEST_FUNC2("0.0p0", 0.0f));
        TEST_TRUE(_TEST_FUNC("1", 1.0f));
        TEST_TRUE(_TEST_FUNC2("1", 1.0f));
        TEST_TRUE(_TEST_FUNC("800s-1", 128.0f));
        TEST_TRUE(_TEST_FUNC2("800p-1", 1024.0f));
        TEST_TRUE(_TEST_FUNC("-800s-1", -128.0f));
        TEST_TRUE(_TEST_FUNC2("-800p-1", -1024.0f));
        TEST_TRUE(_TEST_FUNC("800s-12", 7.2759576141834259e-12f));
        TEST_TRUE(_TEST_FUNC2("800p-12", 0.5f));
        TEST_TRUE(_TEST_FUNC2("12345p+17", 9773383680.0f));
        TEST_TRUE(_TEST_FUNC("1234567.0", 19088743.0f));
        TEST_TRUE(_TEST_FUNC2("1234567.0", 19088743.0f));
        TEST_TRUE(_TEST_FUNC("-1234567.0", -19088743.0f));
        TEST_TRUE(_TEST_FUNC("1234567.0s10", 2.0988294888127726e+19f));
        TEST_TRUE(_TEST_FUNC("457686785756874490000000123", 8.8054800143969061e+31f));
        TEST_TRUE(_TEST_FUNC("1234567978598848646898790", 9.0144042938397500e+28f));
        TEST_TRUE(_TEST_FUNC("1234567978598848646898790.3", 9.0144042938397500e+28f));
        TEST_TRUE(_TEST_FUNC("8945745684576845845.45764856784578456", 4.0515338532968418e+22f));
        TEST_TRUE(_TEST_FUNC("0.123456", 0.071111083030700684f));
        TEST_TRUE(_TEST_FUNC("0.000123456789s-2", 6.7816840268464240e-08f));
        TEST_TRUE(_TEST_FUNC("1.600123456789s+308", std::numeric_limits<float>::infinity()));
        TEST_TRUE(_TEST_FUNC("-1.600123456789s+308", -std::numeric_limits<float>::infinity()));
        TEST_TRUE(_TEST_FUNC("2.470328s-324", 0.0));
        TEST_TRUE(_TEST_FUNC(
            "f.ffffF00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s+31",
            (std::numeric_limits<float>::max)()));
        TEST_TRUE(_TEST_FUNC2(
            "1.ffFffe0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000P+127",
            (std::numeric_limits<float>::max)()));
        TEST_TRUE(_TEST_FUNC(
            "f.fffffc0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s+31",
            std::numeric_limits<float>::infinity()));
        TEST_TRUE(_TEST_FUNC(
            "4.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000S-32",
            (std::numeric_limits<float>::min)()));
        TEST_TRUE(_TEST_FUNC2(
            "1.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000p-126",
            (std::numeric_limits<float>::min)()));
        TEST_TRUE(_TEST_FUNC(
            "2.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000S-32",
            DENORM_VAL(0x1p-127f)));
        TEST_TRUE(_TEST_FUNC2(
            "1.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000p-127",
            DENORM_VAL(0x1p-127f)));
        TEST_TRUE(_TEST_FUNC(
            "8.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s-38",
            DENORM_VAL(std::numeric_limits<float>::denorm_min())));
        TEST_TRUE(_TEST_FUNC2(
            "1.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000p-149",
            DENORM_VAL(std::numeric_limits<float>::denorm_min())));
        TEST_TRUE(_TEST_FUNC(
            "1.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s-38",
            0.0f));

        TEST_TRUE(_TEST_FUNC_NAN("nan"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan"));
        TEST_TRUE(_TEST_FUNC_NAN("nan()"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(ind)"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(snan)"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan(snan)"));

        return true;
    };

}

TEST_CASE(FloatingConv_Double_Dec) {

    TEST_DEF("Double -> String. (Dec, Nor)") {
        auto _TEST_FUNC = [](double val, int precision, const std::string& r) -> bool {
            char buf[1000]{ 0 };
            size_t buf_len = 1000;

            std::string result;
            utl::ftos(val, &result, precision);
            TEST_TRUE(utl::ftos(val, buf, &buf_len, precision));
#ifdef USING_CHARCONV
            char buf2[1000]{ 0 };
            std::to_chars(std::begin(buf2), std::end(buf2), val, std::chars_format::fixed, precision);
#endif
            TEST_E(std::string(buf, buf_len), r);
            TEST_E(result, r);
            return true;
        };

        TEST_TRUE(_TEST_FUNC(0.0,         0, "0"));
        TEST_TRUE(_TEST_FUNC(-0.0,        0, "-0"));
        TEST_TRUE(_TEST_FUNC(0.1,         0, "0"));
        TEST_TRUE(_TEST_FUNC(0.1,         1, "0.1"));
        TEST_TRUE(_TEST_FUNC(0.34578,     5, "0.34578"));
        TEST_TRUE(_TEST_FUNC(1.0,         3, "1.000"));
        TEST_TRUE(_TEST_FUNC(1.666666666, 0, "2"));
        TEST_TRUE(_TEST_FUNC(1.666666666, 2, "1.67"));
        TEST_TRUE(_TEST_FUNC(8.1,         0, "8"));
        TEST_TRUE(_TEST_FUNC(8.1,         2, "8.10"));
        TEST_TRUE(_TEST_FUNC(-8.1,        2, "-8.10"));
        TEST_TRUE(_TEST_FUNC(999.999,     3, "999.999"));
        TEST_TRUE(_TEST_FUNC(1234567.0,   2, "1234567.00"));
        TEST_TRUE(_TEST_FUNC(1.0 / (uint64_t(1) << 32), 40, "0.0000000002328306436538696289062500000000"));

        TEST_TRUE(_TEST_FUNC(1234567978598848646898790.0,  2, "1234567978598848698253312.00"));
        TEST_TRUE(_TEST_FUNC(1234567978598848646898790.3,  2, "1234567978598848698253312.00"));
        TEST_TRUE(_TEST_FUNC(-1234567978598848646898790.3, 2, "-1234567978598848698253312.00"));
        TEST_TRUE(_TEST_FUNC(8945745684576845845.45764856784578456,  2,  "8945745684576845824.00"));
        TEST_TRUE(_TEST_FUNC(0.000000000000000000000000000000000003, 10, "0.0000000000"));
        TEST_TRUE(_TEST_FUNC(1.23456789e-6,  5, "0.00000"));
        TEST_TRUE(_TEST_FUNC(-1.23456789e-6, 5, "-0.00000"));
        TEST_TRUE(_TEST_FUNC(std::numeric_limits<double>::quiet_NaN(), 0, "nan"));

        TEST_TRUE(_TEST_FUNC(
            0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000666666666,
            322,
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000067"));
        TEST_TRUE(_TEST_FUNC(
            0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001,
            360,
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000000099999998365971443346061920956311959264775925"));
        TEST_TRUE(_TEST_FUNC(
            0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001,
            360,
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000000000000009881312916824930883531375857364427447"));
        TEST_TRUE(_TEST_FUNC(
            99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0,
            100,
            "1000000000000000052504760255204420248704468581108159154915854115511802457988908195786371375080447864043704443832883878176942523"
            "2353604305756447921847867069828483872009265758037378302337947880900593689532349707999450811190389676408800746527427801424945792"
            "5878882005684283811566947219638686545940054016.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
        TEST_TRUE(_TEST_FUNC(
            1.7e308,
            2,
            "1699999999999999938830795788659981743333460743040758745027731191935377291781605658643300917875847079885722624679831889191699161"
            "0559335717426836996206247363529647463651566046493566304068495784430352436781502855327271229898638631082864451321235392112325331"
            "1675499856875650512437415429217994623324794855339589632.00"));
        TEST_TRUE(_TEST_FUNC(
            (std::numeric_limits<double>::min)(),
            360,
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000022250738585072013830902327173324040642192159804623318"));
        TEST_TRUE(_TEST_FUNC(
            (std::numeric_limits<double>::max)(),
            360,
            "1797693134862315708145274237317043567980705675258449965989174768031572607800285387605895586327668781715404589535143824642343213"
            "2688946418276846754670353751698604991057655128207624549009038932894407586850845513394230458323690322294816580855933212334827479"
            "7826204144723168738177180919299881250404026184124858368.00000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "00000000000000000000000000000000000"));

        if constexpr (std::numeric_limits<float>::has_denorm == std::denorm_present) {
            TEST_TRUE(_TEST_FUNC(
                (std::numeric_limits<double>::denorm_min)(),
                360,
                "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "000000000000000000000000000000000000000000000000000000000000000000000004940656458412465441765687928682213724"));
        }

        {
            double t = -0.65852354;
            for (size_t i = 0; i < 10; ++i) {
                size_t buf_size = i;
                bool ret1 = utl::ftos(t, static_cast<char*>(nullptr), &buf_size, 7, utl::UFF_FIX | utl::UFF_UPP, utl::UFR_FLOOR);
                TEST_E(buf_size, 10);
                TEST_FALSE(ret1);
            }
        }

        return true;
    };

    TEST_DEF("Double -> String. (Dec, Sci)") {
        auto _TEST_FUNC = [](double val, int precision, const std::string& r) -> bool {
            char buf[1000]{ 0 };
            size_t buf_len = 1000;

            std::string result;
            utl::ftos(val, &result, precision, utl::UFF_SCI);
            TEST_TRUE(utl::ftos(val, buf, &buf_len, precision, utl::UFF_SCI));
#ifdef USING_CHARCONV
            char buf2[1000]{ 0 };
            std::to_chars(std::begin(buf2), std::end(buf2), val, std::chars_format::scientific, precision);
#endif
            TEST_E(std::string(buf, buf_len), r);
            TEST_E(result, r);
            return true;
        };

        auto _TEST_FUNC_MIX = [](double val, int precision, const std::string& r) -> bool {
            std::string result;
            utl::ftos(val, &result, precision, utl::UFF_SCI | utl::UFF_FIX);
#ifdef USING_CHARCONV
            char buf[1000]{ 0 };
            std::to_chars(std::begin(buf), std::end(buf), val, std::chars_format::general, precision);
#endif
            TEST_E(result, r);
            return true;
        };

        TEST_TRUE(_TEST_FUNC_MIX(0.0, 0, "0"));
        TEST_TRUE(_TEST_FUNC_MIX(1.666666666, 2, "1.7"));
        TEST_TRUE(_TEST_FUNC_MIX(16.66666666, 3, "16.7"));
        TEST_TRUE(_TEST_FUNC_MIX(166.6666666, 3, "167"));
        TEST_TRUE(_TEST_FUNC_MIX(1666.666666, 3, "1.67e+03"));
        TEST_TRUE(_TEST_FUNC_MIX(.1666666666, 2, "0.17"));
        TEST_TRUE(_TEST_FUNC_MIX(.0001666666666, 2, "0.00017"));
        TEST_TRUE(_TEST_FUNC_MIX(.00001666666666, 2, "1.7e-05"));

        TEST_TRUE(_TEST_FUNC(0.0, 0, "0e+00"));
        TEST_TRUE(_TEST_FUNC(0.0, 4, "0.0000e+00"));
        TEST_TRUE(_TEST_FUNC(-0.0, 4, "-0.0000e+00"));
        TEST_TRUE(_TEST_FUNC(8.1, 2, "8.10e+00"));
        TEST_TRUE(_TEST_FUNC(1.666666666, 2, "1.67e+00"));
        TEST_TRUE(_TEST_FUNC(166.666666666, 2, "1.67e+02"));
        TEST_TRUE(_TEST_FUNC(-166.666666666, 2, "-1.67e+02"));
        TEST_TRUE(_TEST_FUNC(16666.666666666, 2, "1.67e+04"));
        TEST_TRUE(_TEST_FUNC(0.34578, 4, "3.4578e-01"));
        TEST_TRUE(_TEST_FUNC(1.0, 4, "1.0000e+00"));
        TEST_TRUE(_TEST_FUNC(999.999, 3, "1.000e+03"));
        TEST_TRUE(_TEST_FUNC(1.0 / (uint64_t(1) << 32), 40, "2.3283064365386962890625000000000000000000e-10"));
        TEST_TRUE(_TEST_FUNC(1234567978598848646898790.0, 10, "1.2345679786e+24"));
        TEST_TRUE(_TEST_FUNC(1234567978598848646898790.3, 10, "1.2345679786e+24"));
        TEST_TRUE(_TEST_FUNC(8945745684576845845.45764856784578456, 10, "8.9457456846e+18"));
        TEST_TRUE(_TEST_FUNC(0.000000000000000000000000000000000003, 20, "3.00000000000000015721e-36"));
        TEST_TRUE(_TEST_FUNC(1.23456789e-6, 10, "1.2345678900e-06"));
        TEST_TRUE(_TEST_FUNC(-1.23456789e-6, 10, "-1.2345678900e-06"));
        TEST_TRUE(_TEST_FUNC(1.7e308, 10, "1.7000000000e+308"));
        TEST_TRUE(_TEST_FUNC(std::nan(""), 0, "nan"));

        TEST_TRUE(_TEST_FUNC(
            0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001,
            360,
            "9.99999983659714433460619209563119592647759254336952145504584997059223491913816093472283838042269385386536792366287780216044499"
            "0315364051565565391595587327639198904852632370647709618104786126163799632995155486767134548944815532598435214836120691606867323"
            "339473597648426536418734881746971242559593050185515442628522784588118513181984697915381667591534186401310452e-317"));
        TEST_TRUE(_TEST_FUNC(
            0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001,
            360,
            "9.88131291682493088353137585736442744730119605228649528851171365001351014540417503730599672723271984759593129390891435461853313"
            "4207118795927975495920215637562526014263806228090556916343356979642073774372721139974614461000127748183071299687746249467945463"
            "392302800634307707961482524771311823420533171133735363740791206212498638905431829849106586109130888022549603e-324"));
        TEST_TRUE(_TEST_FUNC(
            99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0,
            360,
            "1.00000000000000005250476025520442024870446858110815915491585411551180245798890819578637137508044786404370444383288387817694252"
            "3235360430575644792184786706982848387200926575803737830233794788090059368953234970799945081119038967640880074652742780142494579"
            "258788820056842838115669472196386865459400540160000000000000000000000000000000000000000000000000000000000000e+299"));
        TEST_TRUE(_TEST_FUNC(
            1234567.0,
            360,
            "1.23456700000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000e+06"));
        TEST_TRUE(_TEST_FUNC(
            (std::numeric_limits<double>::min)(),
            360,
            "2.22507385850720138309023271733240406421921598046233183055332741688720443481391819585428315901251102056406733973103581100515243"
            "4161553460108856012385377718821130777993532002330479610147442583636071921565046942503734208375250806650616658158948720491179968"
            "591639648500635908770118304874799780887753749949451580451605050915399856582470818645113537935804992115981086e-308"));
        TEST_TRUE(_TEST_FUNC(
            (std::numeric_limits<double>::max)(),
            360,
            "1.79769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321"
            "3268894641827684675467035375169860499105765512820762454900903893289440758685084551339423045832369032229481658085593321233482747"
            "978262041447231687381771809192998812504040261841248583680000000000000000000000000000000000000000000000000000e+308"));

        if constexpr (std::numeric_limits<float>::has_denorm == std::denorm_present) {
            TEST_TRUE(_TEST_FUNC(
                std::numeric_limits<double>::denorm_min(),
                360,
                "4.94065645841246544176568792868221372365059802614324764425585682500675507270208751865299836361635992379796564695445717730926656"
                "7103559397963987747960107818781263007131903114045278458171678489821036887186360569987307230500063874091535649843873124733972731"
                "696151400317153853980741262385655911710266585566867681870395603106249319452715914924553293054565444011274801e-324"));
        }

        {
            double t = -0.65852354;
            for (size_t i = 0; i < 10; ++i) {
                size_t buf_size = i;
                bool ret1 = utl::ftos(t, static_cast<char*>(nullptr), &buf_size, 3, utl::UFF_SCI | utl::UFF_UPP, utl::UFR_FLOOR);
                TEST_E(buf_size, 10);
                TEST_FALSE(ret1);
            }
        }

        return true;
    };

    TEST_DEF("String -> Double. (Dec, Nor)") {
        auto _TEST_FUNC = [](const std::string& text, double r) -> bool {
            double result1;
            utl::stof(text, &result1);
#ifdef USING_CHARCONV
            double result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::fixed);
#endif
            TEST_E(result1, r);
            return true;
        };
        auto _TEST_FUNC_NAN = [](const std::string& text) -> bool {
            double result1;
            utl::stof(text, &result1);
#ifdef USING_CHARCONV
            double result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::fixed);
#endif
            return std::isnan(result1);
        };

        TEST_TRUE(_TEST_FUNC("0", 0.0));
        TEST_TRUE(_TEST_FUNC("-0", -0.0));
        TEST_TRUE(_TEST_FUNC("8.1", 8.1));
        TEST_TRUE(_TEST_FUNC("1.", 1.0));
        TEST_TRUE(_TEST_FUNC("0.0", 0.0));
        TEST_TRUE(_TEST_FUNC("1234567.0", 1234567.0));
        TEST_TRUE(_TEST_FUNC("-1234567.0", -1234567.0));
        TEST_TRUE(_TEST_FUNC("457686785756874490000000123", 457686785756874490000000123.0));
        TEST_TRUE(_TEST_FUNC("1234567978598848646898790", 1234567978598848646898790.0));
        TEST_TRUE(_TEST_FUNC("1234567978598848646898790.3", 1234567978598848646898790.3));
        TEST_TRUE(_TEST_FUNC("8945745684576845845.45764856784578456", 8945745684576845845.45764856784578456));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000001",
            DENORM_VAL(0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001)));
        TEST_TRUE(_TEST_FUNC(
            "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
            "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
            "999999999999999999999999999999999999999999999999999999999999999999999",
            std::numeric_limits<double>::infinity()));
        TEST_TRUE(_TEST_FUNC(
            "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
            "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
            "999999999999999999999999999999999999999999999",
            99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0));
        TEST_TRUE(_TEST_FUNC("0.123456", 0.123456));
        TEST_TRUE(_TEST_FUNC("0.000123456789e-2", 0.000123456789));
        TEST_TRUE(_TEST_FUNC(
            "1700000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000",
            170000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.0));
        TEST_TRUE(_TEST_FUNC(
            "1797693134862315708145274237317043567980705675258449965989174768031572607800285387605895586327668781715404589535143824642343213"
            "2688946418276846754670353751698604991057655128207624549009038932894407586850845513394230458323690322294816580855933212334827479"
            "7826204144723168738177180919299881250404026184124858368.00000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "00000000000000000000000000000000000",
            (std::numeric_limits<double>::max)()));
        //                   |
        TEST_TRUE(_TEST_FUNC(
            "1797693134862315808145274237317043567980705675258449965989174768031572607800285387605895586327668781715404589535143824642343213"
            "2688946418276846754670353751698604991057655128207624549009038932894407586850845513394230458323690322294816580855933212334827479"
            "7826204144723168738177180919299881250404026184124858368.00000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "00000000000000000000000000000000000",
            std::numeric_limits<double>::infinity()));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000222507385850720138309023271733240406421921598046233183055332741688720443"
            "4813918195854283159012511020564067339731035811005152434161553460108856012385377718821130777993532002330479610147442583636071921"
            "5650469425037342083752508066506166581589487204911799685916396485006359087701183048747997808877537499494515804516050509153998565"
            "8247081864511353793580499211598108576605199243335211435239014879569960959128889160299264151106346631339366347758651302937176204"
            "7325631781485664350872122828637642044846811407613911477062801689853244110024161447421618567166150540154285084716752901903161322"
            "778896729707373123334086988983175067838846926092773977972858659654941091369095406136467568702398678315290680984617210924625396728515625",
            (std::numeric_limits<double>::min)()));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000111253692925360069154511635866620203210960799023116591527666370844360221"
            "7406959097927141579506255510282033669865517905502576217080776730054428006192688859410565388996766001165239805073721291818035960"
            "7825234712518671041876254033253083290794743602455899842958198242503179543850591524373998904438768749747257902258025254576999282"
            "9123540932255676896790249605799054288302599621667605717619507439784980479564444580149632075553173315669683173879325651468588102"
            "3662815890742832175436061414318821022423405703806955738531400844926622055012080723710809283583075270077142542358376450951580661"
            "3894483648536865616670434944915875339194234630463869889864293298274705456845477030682337843511993391576453404923086054623126983642578125",
            DENORM_VAL(0x1p-1023)));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000000000000004940656458412465441765687928682213724",
            DENORM_VAL(std::numeric_limits<double>::denorm_min())));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000000000000001940656458412465441765687928682213724",
            0.0));

        TEST_TRUE(_TEST_FUNC_NAN("nan"));
        TEST_TRUE(_TEST_FUNC_NAN("nan()"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(ind)"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(snan)"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan(snan)"));

        return true;
    };

    TEST_DEF("String -> Double. (Dec, Sci)") {
        auto _TEST_FUNC = [](const std::string& text, double r) -> bool {
            double result1;
            utl::stof(text, &result1, utl::UFF_SCI);
#ifdef USING_CHARCONV
            double result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::scientific);
#endif
            TEST_E(result1, r);
            return true;
        };
        auto _TEST_FUNC_NAN = [](const std::string& text) -> bool {
            double result1;
            utl::stof(text, &result1, utl::UFF_SCI);
#ifdef USING_CHARCONV
            double result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::scientific);
#endif
            return std::isnan(result1);
        };

        TEST_TRUE(_TEST_FUNC("8.1", 8.1));
        TEST_TRUE(_TEST_FUNC("0.0e0", 0.0));
        TEST_TRUE(_TEST_FUNC("1", 1.0));
        TEST_TRUE(_TEST_FUNC("800e-1", 800e-1));
        TEST_TRUE(_TEST_FUNC("800e-12", 800e-12));
        TEST_TRUE(_TEST_FUNC("-800e-12", -800e-12));
        TEST_TRUE(_TEST_FUNC("1234567.0", 1234567.0));
        TEST_TRUE(_TEST_FUNC("-1234567.0", -1234567.0));
        TEST_TRUE(_TEST_FUNC("1234567.0e10", 1234567.0e10));
        TEST_TRUE(_TEST_FUNC("-1234567.0e10", -1234567.0e10));
        TEST_TRUE(_TEST_FUNC("457686785756874490000000123", 457686785756874490000000123.0));
        TEST_TRUE(_TEST_FUNC("1234567978598848646898790", 1234567978598848646898790.0));
        TEST_TRUE(_TEST_FUNC("1234567978598848646898790.3", 1234567978598848646898790.3));
        TEST_TRUE(_TEST_FUNC("8945745684576845845.45764856784578456", 8945745684576845845.45764856784578456));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000001",
            DENORM_VAL(0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001)));
        TEST_TRUE(_TEST_FUNC(
            "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
            "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
            "999999999999999999999999999999999999999999999999999999999999999999999",
            std::numeric_limits<double>::infinity()));
        TEST_TRUE(_TEST_FUNC(
            "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
            "9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"
            "999999999999999999999999999999999999999999999",
            99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0));

        TEST_TRUE(_TEST_FUNC("0.123456", 0.123456));
        TEST_TRUE(_TEST_FUNC("0.000123456789e-2", 0.000123456789e-2));
        TEST_TRUE(_TEST_FUNC("1.700123456789e308", 1.700123456789e308));
        TEST_TRUE(_TEST_FUNC("1.600123456789e+308", 1.600123456789e+308));
        TEST_TRUE(_TEST_FUNC(
            "1797693134862315708145274237317043567980705675258449965989174768031572607800285387605895586327668781715404589535143824642343213"
            "2688946418276846754670353751698604991057655128207624549009038932894407586850845513394230458323690322294816580855933212334827479"
            "7826204144723168738177180919299881250404026184124858368.00000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "00000000000000000000000000000000000e0",
            (std::numeric_limits<double>::max)()));
        //                   |
        TEST_TRUE(_TEST_FUNC(
            "1797693134862315808145274237317043567980705675258449965989174768031572607800285387605895586327668781715404589535143824642343213"
            "2688946418276846754670353751698604991057655128207624549009038932894407586850845513394230458323690322294816580855933212334827479"
            "7826204144723168738177180919299881250404026184124858368.00000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "00000000000000000000000000000000000e0",
            std::numeric_limits<double>::infinity()));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000222507385850720138309023271733240406421921598046233183055332741688720443"
            "4813918195854283159012511020564067339731035811005152434161553460108856012385377718821130777993532002330479610147442583636071921"
            "5650469425037342083752508066506166581589487204911799685916396485006359087701183048747997808877537499494515804516050509153998565"
            "8247081864511353793580499211598108576605199243335211435239014879569960959128889160299264151106346631339366347758651302937176204"
            "7325631781485664350872122828637642044846811407613911477062801689853244110024161447421618567166150540154285084716752901903161322"
            "778896729707373123334086988983175067838846926092773977972858659654941091369095406136467568702398678315290680984617210924625396728515625e0",
            (std::numeric_limits<double>::min)()));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000111253692925360069154511635866620203210960799023116591527666370844360221"
            "7406959097927141579506255510282033669865517905502576217080776730054428006192688859410565388996766001165239805073721291818035960"
            "7825234712518671041876254033253083290794743602455899842958198242503179543850591524373998904438768749747257902258025254576999282"
            "9123540932255676896790249605799054288302599621667605717619507439784980479564444580149632075553173315669683173879325651468588102"
            "3662815890742832175436061414318821022423405703806955738531400844926622055012080723710809283583075270077142542358376450951580661"
            "3894483648536865616670434944915875339194234630463869889864293298274705456845477030682337843511993391576453404923086054623126983642578125e0",
            DENORM_VAL(0x1p-1023)));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000000000000001940656458412465441765687928682213724e0",
            0.0));

        TEST_TRUE(_TEST_FUNC_NAN("nan"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan"));
        TEST_TRUE(_TEST_FUNC_NAN("nan()"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(ind)"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(snan)"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan(snan)"));

        return true;
    };

}

TEST_CASE(FloatingConv_Double_Hex) {

    TEST_DEF("Double -> String. (Hex, Nor)") {
        auto _TEST_FUNC = [](double val, int precision, const std::string& r) -> bool {
            char buf[1000]{ 0 };
            size_t buf_len = 1000;

            std::string result;
            utl::ftos(val, &result, precision, utl::UFF_HEX);
            TEST_TRUE(utl::ftos(val, buf, &buf_len, precision, utl::UFF_HEX));
#ifdef USING_CHARCONV
            char buf2[1000]{ 0 };
            std::to_chars(std::begin(buf2), std::end(buf2), val, std::chars_format::hex, precision);
#endif
            TEST_E(std::string(buf, buf_len), r);
            TEST_E(result, r);
            return true;
        };

        TEST_TRUE(_TEST_FUNC(18.0, 2, "12.00"));
        TEST_TRUE(_TEST_FUNC(0.0, 0, "0"));
        TEST_TRUE(_TEST_FUNC(-0.0, 0, "-0"));
        TEST_TRUE(_TEST_FUNC(8.1, 0, "8"));
        TEST_TRUE(_TEST_FUNC(1.0, 0, "1"));
        TEST_TRUE(_TEST_FUNC(0.34578, 5, "0.58851"));
        TEST_TRUE(_TEST_FUNC(-0.34578, 5, "-0.58851"));
        TEST_TRUE(_TEST_FUNC(999.999, 3, "3e7.ffc"));
        TEST_TRUE(_TEST_FUNC(1234567978598848646898790.3, 2, "1056e1070fa2ab0000000.00"));
        TEST_TRUE(_TEST_FUNC(8945745684576845845.45764856784578456, 2, "7c25ac4db42fc000.00"));
        TEST_TRUE(_TEST_FUNC(1.23456789e-6, 5, "0.00001"));
        TEST_TRUE(_TEST_FUNC(1234567.0, 2, "12d687.00"));
        TEST_TRUE(_TEST_FUNC(std::numeric_limits<double>::quiet_NaN(), 0, "nan"));

        TEST_TRUE(_TEST_FUNC(
            (std::numeric_limits<double>::min)(),
            360,
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
        TEST_TRUE(_TEST_FUNC(
            (std::numeric_limits<double>::max)(),
            360,
            "fffffffffffff800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "00.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));

        if constexpr (std::numeric_limits<float>::has_denorm == std::denorm_present) {
            TEST_TRUE(_TEST_FUNC(
                std::numeric_limits<double>::denorm_min(),
                360,
                "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "000000000000000040000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
        }

        {
            double t = -0.65852354;
            for (size_t i = 0; i < 10; ++i) {
                size_t buf_size = i;
                bool ret1 = utl::ftos(t, static_cast<char*>(nullptr), &buf_size, 7, utl::UFF_HEX | utl::UFF_UPP);
                TEST_E(buf_size, 10);
                TEST_FALSE(ret1);
            }
        }

        return true;
    };

    TEST_DEF("Double -> String. (Hex, Sci)") {
        auto _TEST_FUNC = [](double val, int precision, const std::string& r) -> bool {
            char buf[1000]{ 0 };
            size_t buf_len = 1000;

            std::string result;
            utl::ftos(val, &result, precision, utl::UFF_SCI | utl::UFF_HEX);
            TEST_TRUE(utl::ftos(val, buf, &buf_len, precision, utl::UFF_SCI | utl::UFF_HEX));
#ifdef USING_CHARCONV
            char buf2[1000]{ 0 };
            std::to_chars(std::begin(buf2), std::end(buf2), val, std::chars_format::hex, precision);
#endif
            TEST_E(std::string(buf, buf_len), r);
            TEST_E(result, r);
            return true;
        };
        auto _TEST_FUNC2 = [](double val, int precision, const std::string& r) -> bool {
            char buf[1000]{ 0 };
            std::string result;
            size_t buf_len = 1000;

            utl::ftos(val, &result, precision, utl::UFF_HEX2);
            TEST_TRUE(utl::ftos(val, buf, &buf_len, precision, utl::UFF_HEX2));
#ifdef USING_CHARCONV
            char buf2[1000]{ 0 };
            std::to_chars(std::begin(buf2), std::end(buf2), val, std::chars_format::hex, precision);
#endif
            TEST_E(std::string(buf, buf_len), r);
            TEST_E(result, r);
            return true;
        };

        TEST_TRUE(_TEST_FUNC(8.1, 2, "8.1as+00"));
        TEST_TRUE(_TEST_FUNC2(8.1, 2, "1.03p+03"));
        TEST_TRUE(_TEST_FUNC(0.0, 4, "0.0000s+00"));
        TEST_TRUE(_TEST_FUNC2(0.0, 2, "0.00p+00"));
        TEST_TRUE(_TEST_FUNC(-0.0, 4, "-0.0000s+00"));
        TEST_TRUE(_TEST_FUNC2(-0.0, 4, "-0.0000p+00"));
        TEST_TRUE(_TEST_FUNC(1.0, 4, "1.0000s+00"));
        TEST_TRUE(_TEST_FUNC2(1.0, 4, "1.0000p+00"));
        TEST_TRUE(_TEST_FUNC(0.34578, 4, "5.8851s-01"));
        TEST_TRUE(_TEST_FUNC2(0.34578, 4, "1.6214p-02"));
        TEST_TRUE(_TEST_FUNC(999.999, 3, "3.e80s+02"));
        TEST_TRUE(_TEST_FUNC2(999.999, 3, "1.f40p+09"));
        TEST_TRUE(_TEST_FUNC(-999.999, 3, "-3.e80s+02"));
        TEST_TRUE(_TEST_FUNC2(-999.999, 3, "-1.f40p+09"));
        TEST_TRUE(_TEST_FUNC(1234567978598848646898790.3, 10, "1.056e1070fas+20"));
        TEST_TRUE(_TEST_FUNC2(1234567978598848646898790.3, 10, "1.056e1070fap+80"));
        TEST_TRUE(_TEST_FUNC(8945745684576845845.45764856784578456, 10, "7.c25ac4db43s+15"));
        TEST_TRUE(_TEST_FUNC(1.23456789e-6, 10, "1.4b66dc015es-05"));
        TEST_TRUE(_TEST_FUNC(std::nan(""), 2, "nan"));

        TEST_TRUE(_TEST_FUNC(
            (std::numeric_limits<double>::min)(),
            360,
            "4.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s-256"));
        TEST_TRUE(_TEST_FUNC(
            (std::numeric_limits<double>::max)(),
            360,
            "f.ffffffffffff80000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s+255"));

        if constexpr (std::numeric_limits<float>::has_denorm == std::denorm_present) {
            TEST_TRUE(_TEST_FUNC(
                std::numeric_limits<double>::denorm_min(),
                360,
                "4.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s-269"));
        }

        {
            double t = -0.65852354;
            for (size_t i = 0; i < 10; ++i) {
                size_t buf_size = i;
                bool ret1 = utl::ftos(t, static_cast<char*>(nullptr), &buf_size, 3, utl::UFF_HEX | utl::UFF_SCI);
                TEST_E(buf_size, 10);
                TEST_FALSE(ret1);
            }
        }

        return true;
    };

    TEST_DEF("String -> Double. (Hex, Nor)") {
        auto _TEST_FUNC = [](const std::string& text, double r) -> bool {
            double result1;
            utl::stof(text, &result1, utl::UFF_FIX | utl::UFF_HEX);
#ifdef USING_CHARCONV
            double result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::hex);
#endif
            TEST_E(result1, r);
            return true;
        };
        auto _TEST_FUNC_NAN = [](const std::string& text) -> bool {
            double result1;
            utl::stof(text, &result1, utl::UFF_FIX | utl::UFF_HEX);
#ifdef USING_CHARCONV
            double result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::hex);
#endif
            return std::isnan(result1);
        };

        TEST_TRUE(_TEST_FUNC("8.1", 8.0625));
        TEST_TRUE(_TEST_FUNC("0", 0.0));
        TEST_TRUE(_TEST_FUNC("-0", -0.0));
        TEST_TRUE(_TEST_FUNC("0.0", 0.0));
        TEST_TRUE(_TEST_FUNC(".1", 0.0625));
        TEST_TRUE(_TEST_FUNC("1.", 1.0));
        TEST_TRUE(_TEST_FUNC("1234567.0", 19088743.0));
        TEST_TRUE(_TEST_FUNC("-1234567.0", -19088743.0));
        TEST_TRUE(_TEST_FUNC("457686785756874490000000123", 8.8054800143969061e+31));
        TEST_TRUE(_TEST_FUNC("1234567978598848646898790", 9.0144042938397500e+28));
        TEST_TRUE(_TEST_FUNC("1234567978598848646898790.3", 9.0144042938397500e+28));
        TEST_TRUE(_TEST_FUNC("8945745684576845845.45764856784578456", 4.0515338532968418e+22));
        TEST_TRUE(_TEST_FUNC("-8945745684576845845.45764856784578456", -4.0515338532968418e+22));
        TEST_TRUE(_TEST_FUNC("0.123456", 0.071111083030700684));
        TEST_TRUE(_TEST_FUNC(
            "FFFFFFFFFFFFF800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "00.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
            (std::numeric_limits<double>::max)()));
        TEST_TRUE(_TEST_FUNC(
            "FFFFFFFFFFFFFC00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "00.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
            std::numeric_limits<double>::infinity()));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
            (std::numeric_limits<double>::min)()));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
            DENORM_VAL(0x1p-1023)));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000040000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
            DENORM_VAL(std::numeric_limits<double>::denorm_min())));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
            0.0));

        TEST_TRUE(_TEST_FUNC_NAN("nan"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan"));
        TEST_TRUE(_TEST_FUNC_NAN("nan()"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(ind)"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(snan)"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan(snan)"));

        return true;
    };

    TEST_DEF("String -> Double. (Hex, Sci)") {
        auto _TEST_FUNC = [](const std::string& text, double r) -> bool {
            double result1;
            utl::stof(text, &result1, utl::UFF_SCI | utl::UFF_HEX);
#ifdef USING_CHARCONV
            double result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::hex);
#endif
            TEST_E(result1, r);
            return true;
        };
        auto _TEST_FUNC2 = [](const std::string& text, double r) -> bool {
            double result1;
            utl::stof(text, &result1, utl::UFF_HEX2);
#ifdef USING_CHARCONV
            double result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::hex);
#endif
            TEST_E(result1, r);
            return true;
        };
        auto _TEST_FUNC_NAN = [](const std::string& text) -> bool {
            double result1;
            utl::stof(text, &result1, utl::UFF_SCI);
#ifdef USING_CHARCONV
            double result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::hex);
#endif
            return std::isnan(result1);
        };

        TEST_TRUE(_TEST_FUNC("8.1", 8.0625));
        TEST_TRUE(_TEST_FUNC2("8.1", 8.0625));
        TEST_TRUE(_TEST_FUNC("0.0s0", 0.0));
        TEST_TRUE(_TEST_FUNC2("0.0p0", 0.0));
        TEST_TRUE(_TEST_FUNC("1", 1.0));
        TEST_TRUE(_TEST_FUNC2("1", 1.0));
        TEST_TRUE(_TEST_FUNC("800s-1", 128.0));
        TEST_TRUE(_TEST_FUNC2("800p-1", 1024.0));
        TEST_TRUE(_TEST_FUNC("-800s-1", -128.0));
        TEST_TRUE(_TEST_FUNC2("-800p-1", -1024.0));
        TEST_TRUE(_TEST_FUNC("800s-12", 7.2759576141834259e-12));
        TEST_TRUE(_TEST_FUNC2("800p-12", 0.5));
        TEST_TRUE(_TEST_FUNC2("12345p+17", 9773383680.0));
        TEST_TRUE(_TEST_FUNC("1234567.0", 19088743.0));
        TEST_TRUE(_TEST_FUNC2("1234567.0", 19088743.0));
        TEST_TRUE(_TEST_FUNC("-1234567.0", -19088743.0));
        TEST_TRUE(_TEST_FUNC("1234567.0s10", 2.0988294888127726e+19));
        TEST_TRUE(_TEST_FUNC("457686785756874490000000123", 8.8054800143969061e+31));
        TEST_TRUE(_TEST_FUNC("1234567978598848646898790", 9.0144042938397500e+28));
        TEST_TRUE(_TEST_FUNC("1234567978598848646898790.3", 9.0144042938397500e+28));
        TEST_TRUE(_TEST_FUNC("8945745684576845845.45764856784578456", 4.0515338532968418e+22));
        TEST_TRUE(_TEST_FUNC("0.123456", 0.071111083030700684));
        TEST_TRUE(_TEST_FUNC("0.000123456789s-2", 6.7816840268464240e-08));
        TEST_TRUE(_TEST_FUNC("1.600123456789s+308", std::numeric_limits<double>::infinity()));
        TEST_TRUE(_TEST_FUNC("-1.600123456789s+308", -std::numeric_limits<double>::infinity()));
        TEST_TRUE(_TEST_FUNC("2.470328s-324", 0.0));
        TEST_TRUE(_TEST_FUNC(
            "FFFFFFFFFFFFF800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s0",
            (std::numeric_limits<double>::max)()));
        TEST_TRUE(_TEST_FUNC(
            "FFFFFFFFFFFFFC00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s0",
            std::numeric_limits<double>::infinity()));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s0",
            (std::numeric_limits<double>::min)()));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s0",
            DENORM_VAL(0x1p-1023)));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000040000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s0",
            DENORM_VAL(std::numeric_limits<double>::denorm_min())));
        TEST_TRUE(_TEST_FUNC(
            "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s0",
            0.0));

        TEST_TRUE(_TEST_FUNC_NAN("nan"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan"));
        TEST_TRUE(_TEST_FUNC_NAN("nan()"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(ind)"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(snan)"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan(snan)"));

        return true;
    };

}

TEST_CASE(FloatingConv_LongDouble_Dec) {

    TEST_DEF("LongDouble -> String.     (Dec, Nor)") {
        auto _TEST_FUNC = [](long double val, int precision, const std::string& r) -> bool {
            char buf[17000]{ 0 };
            size_t buf_len = 17000;

            std::string result;
            utl::ftos(val, &result, precision);

            TEST_TRUE(utl::ftos(val, buf, &buf_len, precision));
#ifdef USING_CHARCONV
            char buf2[17000]{ 0 };
            std::to_chars(std::begin(buf2), std::end(buf2), val, std::chars_format::fixed, precision);
#endif
            TEST_E(std::string(buf, buf_len), r);
            TEST_E(result, r);
            return true;
        };

        TEST_TRUE(_TEST_FUNC(0.0l, 0, "0"));
        TEST_TRUE(_TEST_FUNC(-0.0l, 0, "-0"));
        TEST_TRUE(_TEST_FUNC(0.1l, 0, "0"));
        TEST_TRUE(_TEST_FUNC(0.1l, 1, "0.1"));
        TEST_TRUE(_TEST_FUNC(0.34578l, 5, "0.34578"));
        TEST_TRUE(_TEST_FUNC(1.0l, 3, "1.000"));
        TEST_TRUE(_TEST_FUNC(1.666666666l, 0, "2"));
        TEST_TRUE(_TEST_FUNC(1.666666666l, 2, "1.67"));
        TEST_TRUE(_TEST_FUNC(8.1l, 0, "8"));
        TEST_TRUE(_TEST_FUNC(8.1l, 2, "8.10"));
        TEST_TRUE(_TEST_FUNC(-8.1l, 2, "-8.10"));
        TEST_TRUE(_TEST_FUNC(999.999l, 3, "999.999"));
        TEST_TRUE(_TEST_FUNC(1234567.0l, 2, "1234567.00"));
        TEST_TRUE(_TEST_FUNC(1.0l / (uint64_t(1) << 32), 40, "0.0000000002328306436538696289062500000000"));
        TEST_TRUE(_TEST_FUNC(std::numeric_limits<long double>::quiet_NaN(), 0, "nan"));

        // 80-bits
        if constexpr (std::numeric_limits<long double>::digits == 64u) {
            TEST_TRUE(_TEST_FUNC(1234567978598848646898790.0l, 2, "1234567978598848646873088.00"));
            TEST_TRUE(_TEST_FUNC(1234567978598848646898790.3l, 2, "1234567978598848646873088.00"));
            TEST_TRUE(_TEST_FUNC(-1234567978598848646898790.3l, 2, "-1234567978598848646873088.00"));
            TEST_TRUE(_TEST_FUNC(8945745684576845845.45764856784578456l, 2, "8945745684576845845.50"));
            TEST_TRUE(_TEST_FUNC(0.000000000000000000000000000000000003l, 10, "0.0000000000"));
            TEST_TRUE(_TEST_FUNC(1.23456789e-6l, 5, "0.00000"));
            TEST_TRUE(_TEST_FUNC(-1.23456789e-6l, 5, "-0.00000"));

            TEST_TRUE(_TEST_FUNC(
                0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000666666666l,
                322,
                "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000067"));
            TEST_TRUE(_TEST_FUNC(
                0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001l,
                360,
                "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "000000000000000000000000000000000000000000000000000000000000000099999999999999999997483074219097253146556679"));
            TEST_TRUE(_TEST_FUNC(
                0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001l,
                360,
                "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "000000000000000000000000000000000000000000000000000000000000000000000009999999999999999999517626050839611172"));
            TEST_TRUE(_TEST_FUNC(
                99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0l,
                100,
                "1000000000000000000045301447821692569427117266982028554582089677088340706631722240914929734533955682091355458311743091988974850"
                "4612186811150277101581170598207939392103757888345876138166419678301184055203599651901429415217367746046796865452857846169236073"
                "8455044576656079697364356306664979658906271744.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
            TEST_TRUE(_TEST_FUNC(
                1.7e308l, 2,
                "1700000000000000000031367780251975967952702138896941337330435857632024560512058209940618695889003003369785238418856317126614034"
                "9891800079348072863741001669654745065860000681240130543625762290706238027146278224443827044369133545276131906870950190258918202"
                "8026014367280506332405764915517562011679309777401282560.00"));

            TEST_TRUE(_TEST_FUNC(
                (std::numeric_limits<long double>::max)(), 100,
                "1189731495357231765021263853030970205169063322294624200440323733891737005522970722616410290336528882853545697807495577314427443"
                "1536702884341981255738537436786735932007069732632019159182829615243655295106467910866143117906321697788388961347865606003991487"
                "5343321145491116008867984515486651285234014977303760000912547939396622315138362241783854274391783813871780588948754057516822634"
                "7659235576974805113725649020884855222494791399377585026011773549180099796226026859508558883608159846900235645132346594476384939"
                "8592764562845796617729304078066092291027150460853880879593277816229868275478307680800401506949423034117289577771003357140105597"
                "7524212405734700738625166011082837911962300846927720096515350020847447079244384854591288672300061908512647211195136146752763351"
                "9562927597957250278002980795904193139603021470997035276467445530922022679656280991498232083329641241038509239184734786121921697"
                "2105434842870483534081130425730022164213489173471742348007148807510020643905172342476560047217680964861079949434157034763206435"
                "5862420744350442438056613601760883747816538902780957697597728686007148702828795556714140463261583262360276289631617397848425448"
                "6860609948270867968048078702511858930838546584223040908805996294594586201903766048446790926002225410530775901065760671347200125"
                "8464069570302571389609837579989269545530523685607586831792231136395194688508807718721047052039575874800131431314442549439199401"
                "7575316933939236688185618912993172910425292123683515992232205099800167710278403536014082929639811512287776813570604578934353545"
                "1696539561254048846447169786893211671087229088082778350518228857646062218739702851655083720992349483334435228984751232753726636"
                "0662139022812647062340753520717240586650795182173034637826313533937067749019501978416904418247380631628285868577414325811653640"
                "4021840272491339332094921949842244273042701987304453662035026238695780468200360144729199712309553005720614186697485284685618651"
                "4832715974481203121946751686379343096189615107330065552421485195201762858595091051839472502863871632494167613804996319791441870"
                "2543027067584951920088379151694015817400467114778772014596444611752040594535047647218079757611117208462736392796003396704700376"
                "1337450955318415007379641260504792325166135484129188421134082301547330475406707281876350361733290800595189632520707167390454777"
                "7129682265206225651439919376804400292380903112437912614776255964694221981375146967079446870358004392507659451618379811859392049"
                "5440361149153107822510726914869798092409467721427270124043771874092167566136349389004512323516681460893224006979931760178053381"
                "9184998193300841098599393876029260139091141452600372028487213241195542428210183120421610446740462163533690058366460659115629876"
                "4745525068145003932941404131495400677602951005962253022823003631473824681059648442441324864573137437595096416168048024129351876"
                "2046681356368775328146755387988717718365128939471953350618850032676073543886733680020743878496570145760903498575712430451020387"
                "3049485425670247933932280911052604153852899484920399109194612991249163328991799809438033787952209313146694614970593966415237594"
                "9285890960489916121944989986384837022486672249148924678410206183364627416969576307632480235587975245253737035433882960862753427"
                "7400163334340550835370485073745448197547222289752810830208986826330202852599230841680545396879114182976299889645764827652875045"
                "6285492426516521775079951625966922911497778896235667095662713848201819134832168799586365263762097828507009933729439678463987902"
                "4914514222742527006363942327998483976739987154418554201562244154926653014515504685489258620276085761837129763358761215382565129"
                "6335381416639495165560002641591865548500570526114319529199188079545223946496276356301785808966922264062353828985358675959906470"
                "0838568712381032959192649484625076899225841930548076362021508902214922052806984201835084058693849381549890944546197789302911357"
                "6516775406232278298314033473276603952231603422824717528181818844304880921321933550869873395861276073670866652375555675803171490"
                "1084773200964243187800700087973460329062789435537435644488519071916164551411557619393996907674151564028265436640267600950875239"
                "4550734155613586793306603174472092444651353236664764973540085196704077110364053815007348689179836404957060618953500508984091382"
                "6869535090066783324472578712196604415284924840041850932811908963634175739897166596000759487800619164094854338758520657116541072"
                "2609962881501231443779440087493019447443307843889957018427100048083050121771235606228950762690428568000477188931580893585155938"
                "6317665294808903126774702966254511086154895839508779675546413794489596052797520987481383976257859210575628440175934932416214833"
                "9565350189196811389091843795734703269406342890087805846940352453479398080674273236297887100867175802531561302356064878709259865"
                "2884163509725295370911143172048877474055390540094253754241193179441751370646896438615177188498670103415325423859110896247108853"
                "85808688837777258648564145934262121086647588489260031762345960769508849149662444156604419552086811989770240.0000000000000000000"
                "000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
            TEST_TRUE(_TEST_FUNC(
                (std::numeric_limits<long double>::min)(), 16382,
                "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000033621031431120935062"
                "6267781732175260259807934484647124010882722980874269939072896704309270636505622286250190666882347322709011147172767814074749419"
                "5190631729166726391484998586218894493068740932312583186699859827484977154471718472731360102097109969846313544087646920079723924"
                "1480026897930318750147982625342102921480784734670183910844877343074218035130735386727359319586864183119594415952342021643481184"
                "7249184459743427980577174328754015453870392084141992070169512456041502456927564940669953890024759524769531732176506757310318159"
                "7138531216934946440714750725182818343250953021656567211954373202121429118506595945796555352311604444874792768392761485657965232"
                "2162887905681587885252302898430038391645430675013190288255385619643217130762140229031293434828367361764900794271003752441808130"
                "4009226106252594987092473513582063658505473311237452395518656729064541950664136519968310168014268728856897845692120508105233761"
                "5660310204777901450849407460913217202483761020953437447153656595362436183663394608242491106138007936702952778061013254187212440"
                "6677743897837558036437063651102355419200149640559363170081609821123344001893115860424454490218589209093120314587857551282477234"
                "1617267997314475124456224143302443285933691191491232146594540201981952341273388293197705152664394455565513330251535462199489621"
                "2630196487369422339458567624858134743395948945553201187683023172924999395654043357602989174799539683828730885077297612105022593"
                "3374858273582635161875702668268432647422400903439025451769081518836647226939357684503518718183054484676737812595299034991637942"
                "4190994484313857358673936834482891757174767747152950244828441870302655094290837683767167908309028899171498519810566883886295381"
                "7535081307392485024176198297106907993409312624858931103395860580700633537606824506231109296558670247441899304029060915482947415"
                "4510460824968302961645987941990102591723853180596353429115311751767505418252966606172611909085632279182382622037182038155917151"
                "5365570643239300655278021242593159886882380083899414770055610324366815753207716580020144833879672405691208601962176702215731669"
                "2546964139026311226211000027492605212462700980980242177030389734720351730651125799398322035309000329057761433918445399685674978"
                "9316419544155611174617068433199534034312862039238485154808854294314480750772190677535268703495401108303218457449600872279237329"
                "2756559330889882920161004318858242295922268433408674114910815292188572485365272725998018250350254845431651363704271356561532329"
                "3748636063731684372126294343996528780824867890518209142817854449591731906081321062782993695196160760170233481620289508327097719"
                "2985906171053867383536271149507801466868345084242373660483846435627042818388973527686755441778052280482900079416768703666853404"
                "2210421687773014161377089928320877054040742278920795201766430051689175719302891173419534235817449156616209202728656699765742570"
                "9594631799776592799276503815605221432313978551609725985127745836342430730704370033453736506046184054785151818814511506003446404"
                "7378911979153573920156634959557193964056300890745487778857970967030566288578865571927315905931649230971366752107722556294095349"
                "8448620582709588185239500972140323814900661790191286189320947634608958274014659791763546437035680512762687166617180745685252373"
                "3978910082750774720037752659936382432766788384909274210255390027775284192435156740949021930094609005335309659156257113720999685"
                "2946815969078995970594929976894723669521489132557581200997278049046121007054585700999160104236759623210762404445772701180239471"
                "6446137916582942679011549306162185470981830311545910361580637054918898403742184999378624602197990363760166097432171649962423813"
                "3717398244710503918479899767480069200108228249063603280607087417029493089375636827784090379116081127229460597625002813462540535"
                "0714883573121957679661230473916403545696225655841886336222732765707425848227004142069438565178683114701984811575479557156096919"
                "7496769555523219877533853863631356416674010059689010609149993663071163812251942156159822952680303461050263577588738352939426376"
                "3455946995819500365689870405652441068534579382118549314999817445930704024694879021268975338942192527154023942672041330548526429"
                "2912299797816598659167936945698055539633819110189337027760304541890589937712432549365871482823294982799271853547153211935653208"
                "9347433291387487038147922655782526653468152809890540661225714023747990051992106739802423318738768437113038700758728297559560329"
                "5689679737461989219641760789264565547696836454614499387250864698898719296769935276710904277724845005363481385838570687309285168"
                "0049322796107822710087642801368636578792912874613824004812160094370330973716311886175782802144938321649375288281434150156973654"
                "5684005081682943827642862422975117242784106571486341944290271020636317831339492660801341126388248334073419829399192897339224399"
                "5125694918141416948137339221197370926559186107301529336050136882081931981659262888595329115840098074011293505980430898541950516"
                "1209649374791684594260846929883965983701312884295598281794038506993708290342397624544850473778748425081571890270689172932802628"
                "0006822529839720781410082266878039320875944566943607668367471203575239880199900265804218524549093929842427816938521845658549573"
                "6366737236919564770741819432542063007512913392853534488774471706147299785909952133048682056154847336161612822155755960882066853"
                "4041826092566286300678554483072044113387601302344977544511549909450020270840108560916018969048199801049727015679399321112363226"
                "0106968042398530199776290246328505669272663051575589745992093736919678467590327059136137276786454029038126684969467121924952098"
                "9511727801989065680551864978457399815364290155364649182657250171987638126664505614496150822444264174350070489971410379870262599"
                "7249541914410568736623372965211580644698338408602804846783310626742392853900194863599419650447175791564001237942813875266868702"
                "4303919653886935969636870598043870833203521101669159199248767249156167442820138018774558164642570231859420551099382893322953281"
                "4077146627552170227939895676889308570731141138230512134790347023500626695330178149033363836386015067611963397090819849624356888"
                "1917283839449703316444581247562052139720916211015165629106830807139702202622042424098224662909540036600063506332675792493698734"
                "8062945460496242822865005811447754044351808857013220860941707966043312237757278742297682108051651369515860076839745199137024544"
                "4729710559757707900919331645254022069651331809954508019275098719701946850609725696288501882395325686764898293812016854882517326"
                "6603483825406111849847183994675328745526683158950219253693231117393584743306929631258973902090678269834180874144142148067562577"
                "7657929824255212671483196137501981632629149683399066125826552816024946130678855668982234016541673963930823520525702229417411371"
                "2330252863445167497261049198320381964755768627498577307645810218396165396871023655717876804567280802124278530265026749087040064"
                "1809455495238456488059149161028806792775306655222505308057797431875351611096506724046036277567128792952552053021568361701663808"
                "0918361613965453347217603211860669108494130308359011380690956981498770480168433347639273902114733975020781258031574394221876294"
                "7677382673973309844052930584225905253262374680051806422577195259177585737453997864170307072657261330368740618998032103933249938"
                "9223116881344721840238943584801398944396613713830251005454339327364217734169583114617969345357347620722945279314805392529171308"
                "1040676821476934254869649923833855179025232506799076767842647878980820528100868925504529689933583634942581944805878842608015319"
                "2539828796025342691271766073802395300927554338336591737983642174978857042507693063759143652821546757777333373969951170818372569"
                "1943880516846488305814985026852813679611615053755441532136513490625965925117961210156835124408644052153703499307205683864287019"
                "4134988090369104672318735404133603899848531350176228922532769032490744103365624439677126733757482969810458620193976212440964519"
                "3865157471275626378180670718115039901090785863452014979012752060917743162533131452116057189272012266294586497154886548909874734"
                "3545555207961604483896579595613007973073691350864102431230735806348202744391929640213514906462625373207079219259589327368783816"
                "4004607478541614200432932411757785859200470196565337878291720059679850148655702617534213742555058450750602372272953133454701117"
                "0908761430013200348692768944843628985731859019126746894641069529305378934736665320123678386350759159468404493037799150029052065"
                "4043141493792685904913078251576428544682776887020568596143398488107710806552701460697091365341091926056375248307040211374913925"
                "5519453383120566405982378239243780985421921556476417800869760125926281867288505400507358378238272915875960073813969309717316951"
                "8163942320270447230422772142635428964045229198757237313339870560351516282287443585486069666622085958066384515026854134806309423"
                "2019895154468843552230764280755462626895805082634605713734245299163110923321120423533792717254305999019550744155136388971639764"
                "9922610989996129570015292107962149788611793709047614972371779594267723654418111884649777931396568544883094934667923855067063674"
                "8782409246423512182135206258179671183153424086902046205108919437614947185686025272809306661409268153858178502212338810704166545"
                "1252951129840128659027527319598435340189972185873428762477296711231536006719920708512531287414118039206456403487704596160497123"
                "2438685497514401269143240245964495208104271857691938148155578393430489808950340103855237907616669085863280926371327344848323338"
                "7605732584534529192734503767384730241492197218171031081586047000826151898669714473639663825952324325327477328122596101570389261"
                "2668314611884902185938400468027559945281973949911586330305416503546771640142337823246432654336769848713462296690356358980341790"
                "5296623744946707256477350735396804838890362701865831310926488169017546405599389735924173216530938114048679108305337451795902008"
                "2729921707741498372983669467724354288737499225390345324268257838993364174520399446746674678185445165137557352309945077518764904"
                "6525528316663889407986122805626376768549440005356641573657311703178141972951914813700095541596488516554602889210209559325116482"
                "1492747304223462570929196940794718022085472114432570350661578845789143191441799599522070190285489982322436763369677531171922132"
                "1290396657532807952138514590455718875120828636515701251987443075891198524198443459602111972309617133867234201781946260963774863"
                "4113762945440155972489666268487560685328799795996212832159726195845448439353526585047229096333830269102915840123966374638717755"
                "6337641393939526550335133543832791504182823715242766453612061884607794866399578838038643602178378762651743109271199328257130360"
                "4629907903319672820128921943856887039637736236612552210723345901113700167441968452048346008834664843691114259051547492478315569"
                "5900070268259803685968845339090609204805992552225847799546885381457333848011241786039217749921908767557162673222580922678928085"
                "3129600806301776288076955439553245550939936574320577924416645810222506985543690393213724629037354768531761161632629829640913598"
                "5045263093571039949497480443373657850680798470388913924501728787820483865102841793413476517384410140818691273989852307479535683"
                "9427006487951447113483536344464378430707114154413838525043657991134313459908415148690527128082306909459267068745812866123751472"
                "7583849802806492246622579307875202174160652513472158284048870429657975600095757302323169139791207028844490077877267413894452840"
                "7365965774855880267667375518446950911797881009092700983888884824924347218253285208615336432952408403710334925996533073038979195"
                "34429164151739164937548797150871646953669007591739657589464465654688645899794195598419589698924170306781888939440250396728515625"));

            if constexpr (std::numeric_limits<float>::has_denorm == std::denorm_present) {
                TEST_TRUE(_TEST_FUNC(
                    std::numeric_limits<long double>::denorm_min(), 16445,
                    "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                    "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003"
                    "6451995318824746025284059336194198163990508156935633437209804870283716886333977368095607086258272051972473473472035311019669856"
                    "3262203216997350807558980900548382258117793167856922526380563355975662156256598341072894031979355352726859124079995417281101551"
                    "8538383046054768715422449295305859718268214262622067532355460844068079259753739688226338971902813354664211957293812000216762672"
                    "2920322774336390308456055297955188552992122553211711634876291384629300355138527501603132845785873465668580833490531237051939887"
                    "0398330858783994632250146860245946637658687045642363471024239719465886035756806713447798276421973629281775129556053684576126784"
                    "6647908517463250923736039512722781415953366096399444403402117053488448021977763604437908687623249034116909294550532283554604255"
                    "0164710485109901552496948203761750727745215979248292641646728293792248343785221488475244616533404531408985349389010799994660849"
                    "9579294086708222516789809262633228413363353723036571060667271101610169698690854552681609344678557582354528645667046490928706306"
                    "2587561930219192839574686079732400741932670792586584247903252601714502511652018733705556477669856403061671860988301022089008752"
                    "4042382938121214157585271795472816282327108859944585747150338528775954260248007917199819553441519165990035038288222003556659679"
                    "5877109093768840923105076374488209445632048427271612279814983636166058959520908357688725423253295522974989264055076295943824127"
                    "8496193819382077545905461172547961340469817691165676094261320725727312607482937653140421619686103059529199421431937449566099337"
                    "3960630752183022438454753416779724620381266269034081670865175453863821550089560659554103929149526239278808471595331845354866664"
                    "8048056340753142838532930052247768421705095126474745463575486621478494154231511103863068439142521243930419058412237422297228460"
                    "6537485052464648655369719099026760518287560996997210778473066980129427427439517032390964791367678221749630975419713438139162678"
                    "8785559991775284508827480465385881774472703067382348985029769028530013645504052272870939400583257885854573686549919562658630500"
                    "8347208762306602090337891948992097519082942160201038203947777208340066151915497269543658230502191090857669204817353495889417097"
                    "6763084807837402813849227540942073315401115262334876815962087441302256558438912500607230205782013083674113023497612445214795170"
                    "9838486879883482032894412569087555652160818151512056388931125624153290844569340951461844692909011661106348755901648356069416294"
                    "0748977437979273980467476424348391208469187176922544650628431056530919087247728625592538352854503636055085422691957124636113322"
                    "5904820301916588317084377712259367894846411294822587075835486985645510829880528578782698412556249274522235061092454342386202327"
                    "1204078275403339146131921891743158841366880194359662736634704314404430321364117756050306014368370639726903304174751873302009119"
                    "6750049676161812261479645293712407039870389086891552705149046573674539776494109487947964943816257661434694865208530802469635825"
                    "6756585794568493319042053882843808180358787110774418075478801186519577416271896383169835708225636584654568643702618602433548938"
                    "1637207327725383454545130650115406757655106569705004135372053700679248802092173634073236733684480049044084139777417069654942470"
                    "4079978645186182218847357125755681943328408748602367624946786948363138047385648623954701560331832261278365206869979853940040663"
                    "7311682731115340677841660240710431551140370410375585572939303084073843540702120551963050514959945741359169911676575478403279380"
                    "1120203473482925381507983204527948424275265017968529118631081782766384723202378700828622717633961951395137123031361943660215408"
                    "3053145691058404837254602517776514046020711292315192317640214675286822196688991538159648125316069682226072319425952181305430345"
                    "7222367967929268763072892355278854714278243863110783067060124459039315528176840304931522813148745824350892130595015198002557204"
                    "3324832155514470975125100762233648012028880876356506546349468872327276239064590766702271086341623192103570072733693546571875690"
                    "4639488489508379410353183595961399457537789777552546197746474751816297501009146356192503274264559464580058160069834781702319437"
                    "9921397347681150492687237349021564206463096138703201250277757980410772647014391579144424375252519766764670458008515297647624144"
                    "8144534960696851274398006295520756926486086815068790443764289774631751752051815479584595151590747012858384936940340076624101001"
                    "8353314151668418546075817506648156101890113303195104209905329711883744898444323085664273760647878788028529990352185502561241347"
                    "1034466624171559209077538498475106811294029060334271803320566493935024621719717684804443351866946690525266060758332190531807232"
                    "5655018902937542595909474331403843353669733988168992536983021199161428916310053493353721580057799445291767358871939804092884746"
                    "2169459030309855238237696944236518602313098550782361767005119188901535147965977867613244967164201505954043823730392290305607906"
                    "8344635440276772565425944329927654906262952475555750912023879424636021462043321631428365675851865654901693474343411406220907550"
                    "0900456895883239368146751261034644012237280581850483204752512668828752991205822530900331778836744323438127341239220429265549085"
                    "4823147328534957221096228988980099402653362932146470055202939151729305519621806566725127623749012920609841433890321094248890279"
                    "2253924667941082273267563528653863343443486495762400409978995985052082310730277076965242846278402568285187365588876653245423047"
                    "2059434786129319642112934893741195657514867872048845023075123729239731191911256140687560945268927833216880303109005580256834554"
                    "5189848740113931529074347746192102598790533223629939885944893874208748731628873071106875276344939236689723625899709117699625328"
                    "4306687635454361862179407191652155243650619571720232128624130785404930566805029233271375546253313287446285812219961565022654510"
                    "3738705816482147911391708709705090741587219676334254715505158014981911072162776206923470142349135273858121648344750020070994021"
                    "2299143954278398198492718130066960329019049510619578424853129974026270610679833857352397736524094129369521418867563271544708137"
                    "2252551022981921251125675131839210074132286029621443262127145872022404314258219255606415280218445074654034790115075558513374422"
                    "7536171881983859311648101828170027941722798030282020865670812674533493413454963892393797764944190223010420679866005010454669500"
                    "0405713472693414769167803198669884537029998706738440731033743382811651483074537449076017167215297859504035140557252652302605121"
                    "5093709900567873568385242574546385008567738263951532676549294356417565797771595638496719149577804216291892125541055147173723744"
                    "9917566696724590213751112887541253729183944179762430930282147033293860317993139495351613438927204090460650099589809789479976625"
                    "4551466419426209766529724261236796876600062917465588709082436358365384365898027756603128642962847631857291504582389580582601013"
                    "2712977834418613468288832988547377174550796802573148684253294172285056777826927377158058647145403557721233512002495437339413998"
                    "8427825997101862157234379581542753405098110723490410464838062846088733942757188785904186928092883334235227812406989744418298793"
                    "0379424145585277671332135487064725109472067557232278844790877877889211623413161760841673228098749314133188492786025932439278125"
                    "5786150891859203497212615916404999097067778739914857961305884934678762043515521190581898256624635957830927811421154992139986999"
                    "9362036311498523728025643148961517493264044287720082504285693383296981618501712654114267081137235583250590628715067128384609860"
                    "3114706035149150631601253253879335449954566700952757381089204800465245245213968878939291293825188834129812223523286113873390315"
                    "3560967292104241190946350516769769305030762386908399592555043192236069279491224465826684203312081255431851756341234566913972589"
                    "3906015586508304374020455952076899961595975457118082036883181129189476616621158029195786364034303230416100180636967072212498263"
                    "8371526338885030741846942397348674700923394503622090151818576694273383684612681663815834504878272824125127982031019322351295911"
                    "1029884002230571688216854282257873218631744450950552327428952538149545262779889824399028513132208638633070602135522752076661567"
                    "8724723251256742912688818491014163827036474514282328479933321571807940150363653616458881463557493661502090854571891872327863995"
                    "8587994168037034814110540474229845249789536928719397449614044567730127140358966582291109683295202118134357113695760781631834275"
                    "5770560406569920261633966403484121679869542684843686908354373688437968683525723666121485947946747887572716360800696442816474113"
                    "5934661060892041795546771453745824379626646188830186286875100795112525560022417883462840197508639922839065135723259180534640806"
                    "4352153215482507482023907799077900706958803402961956219041674735971288408432452159867817889018640945148068678173317682492027683"
                    "5002477523486093586963733155424491897575495851549527034538337694314953057164019215026670773910997443279760005290641511584707193"
                    "2263512716704397751410765526327361661176113949343256737438823655183185612089415529316291251367997115745789167410235872514289989"
                    "4696318241165055440420641633648741865610533303854577396019094960285009620856908523022870889487457752351255828197339296126392165"
                    "6163963369388411763476388625952406901518273413037068643383550282962237729195901788599392876651086599849223994259145259240479384"
                    "3806578034980587190625290073394283162299977568998577121169227586830717074729355744992449784197881713384684717511253038609115892"
                    "8940080663720292042948991987026341595166088897543482730429745136232505269348200152484317122907846791053791313811545862661202497"
                    "0730899515114301915281613929239329351501049859225274564107846818112938339161308943153205117651716380468462296193882444293370484"
                    "3685300968988127789731782849127985502452460634119184826382616940292452732916467073315946752134281587552738824383579602450640510"
                    "1588891854180113977161974322140503779728566948145581065332000890421599759797590739151202621402001201013470753677422763647758091"
                    "8167507850485197065326391091171451763751237612739498468510009055480025096015985581769370179656781151306565952590101977398335185"
                    "0426376819823064484137221778956652903027962230041985260791431040700078745405482215823473895787419076942400727481338608233691306"
                    "5180762043610096742215792907190406198587625325429274810050149137657645115761394214315416046766354090215865587971523274190246521"
                    "9061204466069399543537623745326925730135196847583310240300136900823696661112059148949251590372109078165306324458117672967596006"
                    "8856111942740131146840740541811184282840103004930033587050301900280487212275329820719538229025584586853416012880693465359149039"
                    "1896962723895629773939885347115510193339543746992885836460532499710634537302635647288567601861103119770642122438331343572082562"
                    "2216064552242714756266896133512774251470450806606535049288847727598012415011763970898942275983109862678035860018356698699364590"
                    "0959533847966610129802053717740681727113599896621245073184945019268969821281470919288094309836935086280049529210899513358900892"
                    "9460978456492342354477527692748258647612706134286907094211368202811748701552410025711996741039577101126706111412302594237206187"
                    "7500794162568746943742563185008052636577742906686227471051144473792567278171200235377902630705883221100892843504523748306846486"
                    "4135552613382236283996029971083220897986305897891731966709434578109722499468955961728119019245987541050221232627731661646957854"
                    "9250017916884327287309988768982693182906675999880680205645091326631900364116821609754579872655963361409538424118835379300437743"
                    "5736362099860250607867195655941525459057539451297992235991789369568086380421061323718966215706924417928398780179351346855306386"
                    "2900533945505275994496303948455562249364447779953479766845703125"));
            }
        }

        return true;
    };

    TEST_DEF("String     -> LongDouble. (Dec, Nor)") {
        auto _TEST_FUNC = [](const std::string& text, long double r) -> bool {
            long double result1;
            utl::stof(text, &result1);
#ifdef USING_CHARCONV
            long double result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::fixed);
#endif
            TEST_E(result1, r);
            return true;
        };
        auto _TEST_FUNC_NAN = [](const std::string& text) -> bool {
            long double result1;
            utl::stof(text, &result1);
#ifdef USING_CHARCONV
            long double result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::fixed);
#endif
            return std::isnan(result1);
        };

        TEST_TRUE(_TEST_FUNC("0", 0.0l));
        TEST_TRUE(_TEST_FUNC("-0", -0.0l));
        TEST_TRUE(_TEST_FUNC("8.1", 8.1l));
        TEST_TRUE(_TEST_FUNC("1.", 1.0l));
        TEST_TRUE(_TEST_FUNC("0.0", 0.0l));
        TEST_TRUE(_TEST_FUNC("1234567.0", 1234567.0l));
        TEST_TRUE(_TEST_FUNC("-1234567.0", -1234567.0l));
        TEST_TRUE(_TEST_FUNC("457686785756874490000000123", 457686785756874490000000123.0l));
        TEST_TRUE(_TEST_FUNC("1234567978598848646898790", 1234567978598848646898790.0l));
        TEST_TRUE(_TEST_FUNC("1234567978598848646898790.3", 1234567978598848646898790.3l));
        TEST_TRUE(_TEST_FUNC("8945745684576845845.45764856784578456", 8945745684576845845.45764856784578456l));
        TEST_TRUE(_TEST_FUNC("0.123456", 0.123456l));
        TEST_TRUE(_TEST_FUNC("0.000123456789e-2", 0.000123456789l));
        TEST_TRUE(_TEST_FUNC(
            "1700000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
            "0000000000000000000000000000000000000000000000000000000",
            170000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.0l));

        // 80-bits
        if constexpr (std::numeric_limits<long double>::digits == 64u) {
            TEST_TRUE(_TEST_FUNC("1234567978598848646873088.00", 1234567978598848646898790.0l));
            TEST_TRUE(_TEST_FUNC("1234567978598848646873088.00", 1234567978598848646898790.3l));
            TEST_TRUE(_TEST_FUNC("-1234567978598848646873088.00", -1234567978598848646898790.3l));
            TEST_TRUE(_TEST_FUNC("8945745684576845845.50", 8945745684576845845.45764856784578456l));

            TEST_TRUE(_TEST_FUNC(
                "0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "00000000000000000000000000000000000000000000000000000000000000000000000099999999999999999997483074219097253146556679",
                0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001l));
            TEST_TRUE(_TEST_FUNC(
                "0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "00000000000000000000000000000000000000000000000000000000000000000000000000000009999999999999999999517626050839611172",
                0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001l));
            TEST_TRUE(_TEST_FUNC(
                "100000000000000000004530144782169256942711726698202855458208967708834070663172224091492973453395568209135545831174309198897"
                "485046121868111502771015811705982079393921037578883458761381664196783011840552035996519014294152173677460467968654528578461"
                "692360738455044576656079697364356306664979658906271744.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000",
                99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0l));
            TEST_TRUE(_TEST_FUNC(
                "170000000000000000003136778025197596795270213889694133733043585763202456051205820994061869588900300336978523841885631712661"
                "403498918000793480728637410016696547450658600006812401305436257622907062380271462782244438270443691335452761319068709501902"
                "589182028026014367280506332405764915517562011679309777401282560.00",
                1.7e308l));

            TEST_TRUE(_TEST_FUNC(
                "1189731495357231765021263853030970205169063322294624200440323733891737005522970722616410290336528882853545697807495577314427443"
                "1536702884341981255738537436786735932007069732632019159182829615243655295106467910866143117906321697788388961347865606003991487"
                "5343321145491116008867984515486651285234014977303760000912547939396622315138362241783854274391783813871780588948754057516822634"
                "7659235576974805113725649020884855222494791399377585026011773549180099796226026859508558883608159846900235645132346594476384939"
                "8592764562845796617729304078066092291027150460853880879593277816229868275478307680800401506949423034117289577771003357140105597"
                "7524212405734700738625166011082837911962300846927720096515350020847447079244384854591288672300061908512647211195136146752763351"
                "9562927597957250278002980795904193139603021470997035276467445530922022679656280991498232083329641241038509239184734786121921697"
                "2105434842870483534081130425730022164213489173471742348007148807510020643905172342476560047217680964861079949434157034763206435"
                "5862420744350442438056613601760883747816538902780957697597728686007148702828795556714140463261583262360276289631617397848425448"
                "6860609948270867968048078702511858930838546584223040908805996294594586201903766048446790926002225410530775901065760671347200125"
                "8464069570302571389609837579989269545530523685607586831792231136395194688508807718721047052039575874800131431314442549439199401"
                "7575316933939236688185618912993172910425292123683515992232205099800167710278403536014082929639811512287776813570604578934353545"
                "1696539561254048846447169786893211671087229088082778350518228857646062218739702851655083720992349483334435228984751232753726636"
                "0662139022812647062340753520717240586650795182173034637826313533937067749019501978416904418247380631628285868577414325811653640"
                "4021840272491339332094921949842244273042701987304453662035026238695780468200360144729199712309553005720614186697485284685618651"
                "4832715974481203121946751686379343096189615107330065552421485195201762858595091051839472502863871632494167613804996319791441870"
                "2543027067584951920088379151694015817400467114778772014596444611752040594535047647218079757611117208462736392796003396704700376"
                "1337450955318415007379641260504792325166135484129188421134082301547330475406707281876350361733290800595189632520707167390454777"
                "7129682265206225651439919376804400292380903112437912614776255964694221981375146967079446870358004392507659451618379811859392049"
                "5440361149153107822510726914869798092409467721427270124043771874092167566136349389004512323516681460893224006979931760178053381"
                "9184998193300841098599393876029260139091141452600372028487213241195542428210183120421610446740462163533690058366460659115629876"
                "4745525068145003932941404131495400677602951005962253022823003631473824681059648442441324864573137437595096416168048024129351876"
                "2046681356368775328146755387988717718365128939471953350618850032676073543886733680020743878496570145760903498575712430451020387"
                "3049485425670247933932280911052604153852899484920399109194612991249163328991799809438033787952209313146694614970593966415237594"
                "9285890960489916121944989986384837022486672249148924678410206183364627416969576307632480235587975245253737035433882960862753427"
                "7400163334340550835370485073745448197547222289752810830208986826330202852599230841680545396879114182976299889645764827652875045"
                "6285492426516521775079951625966922911497778896235667095662713848201819134832168799586365263762097828507009933729439678463987902"
                "4914514222742527006363942327998483976739987154418554201562244154926653014515504685489258620276085761837129763358761215382565129"
                "6335381416639495165560002641591865548500570526114319529199188079545223946496276356301785808966922264062353828985358675959906470"
                "0838568712381032959192649484625076899225841930548076362021508902214922052806984201835084058693849381549890944546197789302911357"
                "6516775406232278298314033473276603952231603422824717528181818844304880921321933550869873395861276073670866652375555675803171490"
                "1084773200964243187800700087973460329062789435537435644488519071916164551411557619393996907674151564028265436640267600950875239"
                "4550734155613586793306603174472092444651353236664764973540085196704077110364053815007348689179836404957060618953500508984091382"
                "6869535090066783324472578712196604415284924840041850932811908963634175739897166596000759487800619164094854338758520657116541072"
                "2609962881501231443779440087493019447443307843889957018427100048083050121771235606228950762690428568000477188931580893585155938"
                "6317665294808903126774702966254511086154895839508779675546413794489596052797520987481383976257859210575628440175934932416214833"
                "9565350189196811389091843795734703269406342890087805846940352453479398080674273236297887100867175802531561302356064878709259865"
                "2884163509725295370911143172048877474055390540094253754241193179441751370646896438615177188498670103415325423859110896247108853"
                "85808688837777258648564145934262121086647588489260031762345960769508849149662444156604419552086811989770240.0000000000000000000"
                "000000000000000000000000000000000000000000000000000000000000000000000000000000000",
                (std::numeric_limits<long double>::max)()));
            TEST_TRUE(_TEST_FUNC(
                "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000033621031431120935062"
                "6267781732175260259807934484647124010882722980874269939072896704309270636505622286250190666882347322709011147172767814074749419"
                "5190631729166726391484998586218894493068740932312583186699859827484977154471718472731360102097109969846313544087646920079723924"
                "1480026897930318750147982625342102921480784734670183910844877343074218035130735386727359319586864183119594415952342021643481184"
                "7249184459743427980577174328754015453870392084141992070169512456041502456927564940669953890024759524769531732176506757310318159"
                "7138531216934946440714750725182818343250953021656567211954373202121429118506595945796555352311604444874792768392761485657965232"
                "2162887905681587885252302898430038391645430675013190288255385619643217130762140229031293434828367361764900794271003752441808130"
                "4009226106252594987092473513582063658505473311237452395518656729064541950664136519968310168014268728856897845692120508105233761"
                "5660310204777901450849407460913217202483761020953437447153656595362436183663394608242491106138007936702952778061013254187212440"
                "6677743897837558036437063651102355419200149640559363170081609821123344001893115860424454490218589209093120314587857551282477234"
                "1617267997314475124456224143302443285933691191491232146594540201981952341273388293197705152664394455565513330251535462199489621"
                "2630196487369422339458567624858134743395948945553201187683023172924999395654043357602989174799539683828730885077297612105022593"
                "3374858273582635161875702668268432647422400903439025451769081518836647226939357684503518718183054484676737812595299034991637942"
                "4190994484313857358673936834482891757174767747152950244828441870302655094290837683767167908309028899171498519810566883886295381"
                "7535081307392485024176198297106907993409312624858931103395860580700633537606824506231109296558670247441899304029060915482947415"
                "4510460824968302961645987941990102591723853180596353429115311751767505418252966606172611909085632279182382622037182038155917151"
                "5365570643239300655278021242593159886882380083899414770055610324366815753207716580020144833879672405691208601962176702215731669"
                "2546964139026311226211000027492605212462700980980242177030389734720351730651125799398322035309000329057761433918445399685674978"
                "9316419544155611174617068433199534034312862039238485154808854294314480750772190677535268703495401108303218457449600872279237329"
                "2756559330889882920161004318858242295922268433408674114910815292188572485365272725998018250350254845431651363704271356561532329"
                "3748636063731684372126294343996528780824867890518209142817854449591731906081321062782993695196160760170233481620289508327097719"
                "2985906171053867383536271149507801466868345084242373660483846435627042818388973527686755441778052280482900079416768703666853404"
                "2210421687773014161377089928320877054040742278920795201766430051689175719302891173419534235817449156616209202728656699765742570"
                "9594631799776592799276503815605221432313978551609725985127745836342430730704370033453736506046184054785151818814511506003446404"
                "7378911979153573920156634959557193964056300890745487778857970967030566288578865571927315905931649230971366752107722556294095349"
                "8448620582709588185239500972140323814900661790191286189320947634608958274014659791763546437035680512762687166617180745685252373"
                "3978910082750774720037752659936382432766788384909274210255390027775284192435156740949021930094609005335309659156257113720999685"
                "2946815969078995970594929976894723669521489132557581200997278049046121007054585700999160104236759623210762404445772701180239471"
                "6446137916582942679011549306162185470981830311545910361580637054918898403742184999378624602197990363760166097432171649962423813"
                "3717398244710503918479899767480069200108228249063603280607087417029493089375636827784090379116081127229460597625002813462540535"
                "0714883573121957679661230473916403545696225655841886336222732765707425848227004142069438565178683114701984811575479557156096919"
                "7496769555523219877533853863631356416674010059689010609149993663071163812251942156159822952680303461050263577588738352939426376"
                "3455946995819500365689870405652441068534579382118549314999817445930704024694879021268975338942192527154023942672041330548526429"
                "2912299797816598659167936945698055539633819110189337027760304541890589937712432549365871482823294982799271853547153211935653208"
                "9347433291387487038147922655782526653468152809890540661225714023747990051992106739802423318738768437113038700758728297559560329"
                "5689679737461989219641760789264565547696836454614499387250864698898719296769935276710904277724845005363481385838570687309285168"
                "0049322796107822710087642801368636578792912874613824004812160094370330973716311886175782802144938321649375288281434150156973654"
                "5684005081682943827642862422975117242784106571486341944290271020636317831339492660801341126388248334073419829399192897339224399"
                "5125694918141416948137339221197370926559186107301529336050136882081931981659262888595329115840098074011293505980430898541950516"
                "1209649374791684594260846929883965983701312884295598281794038506993708290342397624544850473778748425081571890270689172932802628"
                "0006822529839720781410082266878039320875944566943607668367471203575239880199900265804218524549093929842427816938521845658549573"
                "6366737236919564770741819432542063007512913392853534488774471706147299785909952133048682056154847336161612822155755960882066853"
                "4041826092566286300678554483072044113387601302344977544511549909450020270840108560916018969048199801049727015679399321112363226"
                "0106968042398530199776290246328505669272663051575589745992093736919678467590327059136137276786454029038126684969467121924952098"
                "9511727801989065680551864978457399815364290155364649182657250171987638126664505614496150822444264174350070489971410379870262599"
                "7249541914410568736623372965211580644698338408602804846783310626742392853900194863599419650447175791564001237942813875266868702"
                "4303919653886935969636870598043870833203521101669159199248767249156167442820138018774558164642570231859420551099382893322953281"
                "4077146627552170227939895676889308570731141138230512134790347023500626695330178149033363836386015067611963397090819849624356888"
                "1917283839449703316444581247562052139720916211015165629106830807139702202622042424098224662909540036600063506332675792493698734"
                "8062945460496242822865005811447754044351808857013220860941707966043312237757278742297682108051651369515860076839745199137024544"
                "4729710559757707900919331645254022069651331809954508019275098719701946850609725696288501882395325686764898293812016854882517326"
                "6603483825406111849847183994675328745526683158950219253693231117393584743306929631258973902090678269834180874144142148067562577"
                "7657929824255212671483196137501981632629149683399066125826552816024946130678855668982234016541673963930823520525702229417411371"
                "2330252863445167497261049198320381964755768627498577307645810218396165396871023655717876804567280802124278530265026749087040064"
                "1809455495238456488059149161028806792775306655222505308057797431875351611096506724046036277567128792952552053021568361701663808"
                "0918361613965453347217603211860669108494130308359011380690956981498770480168433347639273902114733975020781258031574394221876294"
                "7677382673973309844052930584225905253262374680051806422577195259177585737453997864170307072657261330368740618998032103933249938"
                "9223116881344721840238943584801398944396613713830251005454339327364217734169583114617969345357347620722945279314805392529171308"
                "1040676821476934254869649923833855179025232506799076767842647878980820528100868925504529689933583634942581944805878842608015319"
                "2539828796025342691271766073802395300927554338336591737983642174978857042507693063759143652821546757777333373969951170818372569"
                "1943880516846488305814985026852813679611615053755441532136513490625965925117961210156835124408644052153703499307205683864287019"
                "4134988090369104672318735404133603899848531350176228922532769032490744103365624439677126733757482969810458620193976212440964519"
                "3865157471275626378180670718115039901090785863452014979012752060917743162533131452116057189272012266294586497154886548909874734"
                "3545555207961604483896579595613007973073691350864102431230735806348202744391929640213514906462625373207079219259589327368783816"
                "4004607478541614200432932411757785859200470196565337878291720059679850148655702617534213742555058450750602372272953133454701117"
                "0908761430013200348692768944843628985731859019126746894641069529305378934736665320123678386350759159468404493037799150029052065"
                "4043141493792685904913078251576428544682776887020568596143398488107710806552701460697091365341091926056375248307040211374913925"
                "5519453383120566405982378239243780985421921556476417800869760125926281867288505400507358378238272915875960073813969309717316951"
                "8163942320270447230422772142635428964045229198757237313339870560351516282287443585486069666622085958066384515026854134806309423"
                "2019895154468843552230764280755462626895805082634605713734245299163110923321120423533792717254305999019550744155136388971639764"
                "9922610989996129570015292107962149788611793709047614972371779594267723654418111884649777931396568544883094934667923855067063674"
                "8782409246423512182135206258179671183153424086902046205108919437614947185686025272809306661409268153858178502212338810704166545"
                "1252951129840128659027527319598435340189972185873428762477296711231536006719920708512531287414118039206456403487704596160497123"
                "2438685497514401269143240245964495208104271857691938148155578393430489808950340103855237907616669085863280926371327344848323338"
                "7605732584534529192734503767384730241492197218171031081586047000826151898669714473639663825952324325327477328122596101570389261"
                "2668314611884902185938400468027559945281973949911586330305416503546771640142337823246432654336769848713462296690356358980341790"
                "5296623744946707256477350735396804838890362701865831310926488169017546405599389735924173216530938114048679108305337451795902008"
                "2729921707741498372983669467724354288737499225390345324268257838993364174520399446746674678185445165137557352309945077518764904"
                "6525528316663889407986122805626376768549440005356641573657311703178141972951914813700095541596488516554602889210209559325116482"
                "1492747304223462570929196940794718022085472114432570350661578845789143191441799599522070190285489982322436763369677531171922132"
                "1290396657532807952138514590455718875120828636515701251987443075891198524198443459602111972309617133867234201781946260963774863"
                "4113762945440155972489666268487560685328799795996212832159726195845448439353526585047229096333830269102915840123966374638717755"
                "6337641393939526550335133543832791504182823715242766453612061884607794866399578838038643602178378762651743109271199328257130360"
                "4629907903319672820128921943856887039637736236612552210723345901113700167441968452048346008834664843691114259051547492478315569"
                "5900070268259803685968845339090609204805992552225847799546885381457333848011241786039217749921908767557162673222580922678928085"
                "3129600806301776288076955439553245550939936574320577924416645810222506985543690393213724629037354768531761161632629829640913598"
                "5045263093571039949497480443373657850680798470388913924501728787820483865102841793413476517384410140818691273989852307479535683"
                "9427006487951447113483536344464378430707114154413838525043657991134313459908415148690527128082306909459267068745812866123751472"
                "7583849802806492246622579307875202174160652513472158284048870429657975600095757302323169139791207028844490077877267413894452840"
                "7365965774855880267667375518446950911797881009092700983888884824924347218253285208615336432952408403710334925996533073038979195"
                "34429164151739164937548797150871646953669007591739657589464465654688645899794195598419589698924170306781888939440250396728515625",
                (std::numeric_limits<long double>::min)()));
            TEST_TRUE(_TEST_FUNC(
                "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003"
                "6451995318824746025284059336194198163990508156935633437209804870283716886333977368095607086258272051972473473472035311019669856"
                "3262203216997350807558980900548382258117793167856922526380563355975662156256598341072894031979355352726859124079995417281101551"
                "8538383046054768715422449295305859718268214262622067532355460844068079259753739688226338971902813354664211957293812000216762672"
                "2920322774336390308456055297955188552992122553211711634876291384629300355138527501603132845785873465668580833490531237051939887"
                "0398330858783994632250146860245946637658687045642363471024239719465886035756806713447798276421973629281775129556053684576126784"
                "6647908517463250923736039512722781415953366096399444403402117053488448021977763604437908687623249034116909294550532283554604255"
                "0164710485109901552496948203761750727745215979248292641646728293792248343785221488475244616533404531408985349389010799994660849"
                "9579294086708222516789809262633228413363353723036571060667271101610169698690854552681609344678557582354528645667046490928706306"
                "2587561930219192839574686079732400741932670792586584247903252601714502511652018733705556477669856403061671860988301022089008752"
                "4042382938121214157585271795472816282327108859944585747150338528775954260248007917199819553441519165990035038288222003556659679"
                "5877109093768840923105076374488209445632048427271612279814983636166058959520908357688725423253295522974989264055076295943824127"
                "8496193819382077545905461172547961340469817691165676094261320725727312607482937653140421619686103059529199421431937449566099337"
                "3960630752183022438454753416779724620381266269034081670865175453863821550089560659554103929149526239278808471595331845354866664"
                "8048056340753142838532930052247768421705095126474745463575486621478494154231511103863068439142521243930419058412237422297228460"
                "6537485052464648655369719099026760518287560996997210778473066980129427427439517032390964791367678221749630975419713438139162678"
                "8785559991775284508827480465385881774472703067382348985029769028530013645504052272870939400583257885854573686549919562658630500"
                "8347208762306602090337891948992097519082942160201038203947777208340066151915497269543658230502191090857669204817353495889417097"
                "6763084807837402813849227540942073315401115262334876815962087441302256558438912500607230205782013083674113023497612445214795170"
                "9838486879883482032894412569087555652160818151512056388931125624153290844569340951461844692909011661106348755901648356069416294"
                "0748977437979273980467476424348391208469187176922544650628431056530919087247728625592538352854503636055085422691957124636113322"
                "5904820301916588317084377712259367894846411294822587075835486985645510829880528578782698412556249274522235061092454342386202327"
                "1204078275403339146131921891743158841366880194359662736634704314404430321364117756050306014368370639726903304174751873302009119"
                "6750049676161812261479645293712407039870389086891552705149046573674539776494109487947964943816257661434694865208530802469635825"
                "6756585794568493319042053882843808180358787110774418075478801186519577416271896383169835708225636584654568643702618602433548938"
                "1637207327725383454545130650115406757655106569705004135372053700679248802092173634073236733684480049044084139777417069654942470"
                "4079978645186182218847357125755681943328408748602367624946786948363138047385648623954701560331832261278365206869979853940040663"
                "7311682731115340677841660240710431551140370410375585572939303084073843540702120551963050514959945741359169911676575478403279380"
                "1120203473482925381507983204527948424275265017968529118631081782766384723202378700828622717633961951395137123031361943660215408"
                "3053145691058404837254602517776514046020711292315192317640214675286822196688991538159648125316069682226072319425952181305430345"
                "7222367967929268763072892355278854714278243863110783067060124459039315528176840304931522813148745824350892130595015198002557204"
                "3324832155514470975125100762233648012028880876356506546349468872327276239064590766702271086341623192103570072733693546571875690"
                "4639488489508379410353183595961399457537789777552546197746474751816297501009146356192503274264559464580058160069834781702319437"
                "9921397347681150492687237349021564206463096138703201250277757980410772647014391579144424375252519766764670458008515297647624144"
                "8144534960696851274398006295520756926486086815068790443764289774631751752051815479584595151590747012858384936940340076624101001"
                "8353314151668418546075817506648156101890113303195104209905329711883744898444323085664273760647878788028529990352185502561241347"
                "1034466624171559209077538498475106811294029060334271803320566493935024621719717684804443351866946690525266060758332190531807232"
                "5655018902937542595909474331403843353669733988168992536983021199161428916310053493353721580057799445291767358871939804092884746"
                "2169459030309855238237696944236518602313098550782361767005119188901535147965977867613244967164201505954043823730392290305607906"
                "8344635440276772565425944329927654906262952475555750912023879424636021462043321631428365675851865654901693474343411406220907550"
                "0900456895883239368146751261034644012237280581850483204752512668828752991205822530900331778836744323438127341239220429265549085"
                "4823147328534957221096228988980099402653362932146470055202939151729305519621806566725127623749012920609841433890321094248890279"
                "2253924667941082273267563528653863343443486495762400409978995985052082310730277076965242846278402568285187365588876653245423047"
                "2059434786129319642112934893741195657514867872048845023075123729239731191911256140687560945268927833216880303109005580256834554"
                "5189848740113931529074347746192102598790533223629939885944893874208748731628873071106875276344939236689723625899709117699625328"
                "4306687635454361862179407191652155243650619571720232128624130785404930566805029233271375546253313287446285812219961565022654510"
                "3738705816482147911391708709705090741587219676334254715505158014981911072162776206923470142349135273858121648344750020070994021"
                "2299143954278398198492718130066960329019049510619578424853129974026270610679833857352397736524094129369521418867563271544708137"
                "2252551022981921251125675131839210074132286029621443262127145872022404314258219255606415280218445074654034790115075558513374422"
                "7536171881983859311648101828170027941722798030282020865670812674533493413454963892393797764944190223010420679866005010454669500"
                "0405713472693414769167803198669884537029998706738440731033743382811651483074537449076017167215297859504035140557252652302605121"
                "5093709900567873568385242574546385008567738263951532676549294356417565797771595638496719149577804216291892125541055147173723744"
                "9917566696724590213751112887541253729183944179762430930282147033293860317993139495351613438927204090460650099589809789479976625"
                "4551466419426209766529724261236796876600062917465588709082436358365384365898027756603128642962847631857291504582389580582601013"
                "2712977834418613468288832988547377174550796802573148684253294172285056777826927377158058647145403557721233512002495437339413998"
                "8427825997101862157234379581542753405098110723490410464838062846088733942757188785904186928092883334235227812406989744418298793"
                "0379424145585277671332135487064725109472067557232278844790877877889211623413161760841673228098749314133188492786025932439278125"
                "5786150891859203497212615916404999097067778739914857961305884934678762043515521190581898256624635957830927811421154992139986999"
                "9362036311498523728025643148961517493264044287720082504285693383296981618501712654114267081137235583250590628715067128384609860"
                "3114706035149150631601253253879335449954566700952757381089204800465245245213968878939291293825188834129812223523286113873390315"
                "3560967292104241190946350516769769305030762386908399592555043192236069279491224465826684203312081255431851756341234566913972589"
                "3906015586508304374020455952076899961595975457118082036883181129189476616621158029195786364034303230416100180636967072212498263"
                "8371526338885030741846942397348674700923394503622090151818576694273383684612681663815834504878272824125127982031019322351295911"
                "1029884002230571688216854282257873218631744450950552327428952538149545262779889824399028513132208638633070602135522752076661567"
                "8724723251256742912688818491014163827036474514282328479933321571807940150363653616458881463557493661502090854571891872327863995"
                "8587994168037034814110540474229845249789536928719397449614044567730127140358966582291109683295202118134357113695760781631834275"
                "5770560406569920261633966403484121679869542684843686908354373688437968683525723666121485947946747887572716360800696442816474113"
                "5934661060892041795546771453745824379626646188830186286875100795112525560022417883462840197508639922839065135723259180534640806"
                "4352153215482507482023907799077900706958803402961956219041674735971288408432452159867817889018640945148068678173317682492027683"
                "5002477523486093586963733155424491897575495851549527034538337694314953057164019215026670773910997443279760005290641511584707193"
                "2263512716704397751410765526327361661176113949343256737438823655183185612089415529316291251367997115745789167410235872514289989"
                "4696318241165055440420641633648741865610533303854577396019094960285009620856908523022870889487457752351255828197339296126392165"
                "6163963369388411763476388625952406901518273413037068643383550282962237729195901788599392876651086599849223994259145259240479384"
                "3806578034980587190625290073394283162299977568998577121169227586830717074729355744992449784197881713384684717511253038609115892"
                "8940080663720292042948991987026341595166088897543482730429745136232505269348200152484317122907846791053791313811545862661202497"
                "0730899515114301915281613929239329351501049859225274564107846818112938339161308943153205117651716380468462296193882444293370484"
                "3685300968988127789731782849127985502452460634119184826382616940292452732916467073315946752134281587552738824383579602450640510"
                "1588891854180113977161974322140503779728566948145581065332000890421599759797590739151202621402001201013470753677422763647758091"
                "8167507850485197065326391091171451763751237612739498468510009055480025096015985581769370179656781151306565952590101977398335185"
                "0426376819823064484137221778956652903027962230041985260791431040700078745405482215823473895787419076942400727481338608233691306"
                "5180762043610096742215792907190406198587625325429274810050149137657645115761394214315416046766354090215865587971523274190246521"
                "9061204466069399543537623745326925730135196847583310240300136900823696661112059148949251590372109078165306324458117672967596006"
                "8856111942740131146840740541811184282840103004930033587050301900280487212275329820719538229025584586853416012880693465359149039"
                "1896962723895629773939885347115510193339543746992885836460532499710634537302635647288567601861103119770642122438331343572082562"
                "2216064552242714756266896133512774251470450806606535049288847727598012415011763970898942275983109862678035860018356698699364590"
                "0959533847966610129802053717740681727113599896621245073184945019268969821281470919288094309836935086280049529210899513358900892"
                "9460978456492342354477527692748258647612706134286907094211368202811748701552410025711996741039577101126706111412302594237206187"
                "7500794162568746943742563185008052636577742906686227471051144473792567278171200235377902630705883221100892843504523748306846486"
                "4135552613382236283996029971083220897986305897891731966709434578109722499468955961728119019245987541050221232627731661646957854"
                "9250017916884327287309988768982693182906675999880680205645091326631900364116821609754579872655963361409538424118835379300437743"
                "5736362099860250607867195655941525459057539451297992235991789369568086380421061323718966215706924417928398780179351346855306386"
                "2900533945505275994496303948455562249364447779953479766845703125",
                DENORM_VAL(std::numeric_limits<long double>::denorm_min())));
        }

        TEST_TRUE(_TEST_FUNC_NAN("nan"));
        TEST_TRUE(_TEST_FUNC_NAN("nan()"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(ind)"));
        TEST_TRUE(_TEST_FUNC_NAN("nan(snan)"));
        TEST_TRUE(_TEST_FUNC_NAN("-nan(snan)"));

        return true;
    };

}
