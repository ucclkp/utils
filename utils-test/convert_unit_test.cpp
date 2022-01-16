// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include <cmath>

#include "utils/float_conv.hpp"
#include "utils/int_conv.hpp"
#include "utils/uint_exp.hpp"
#include "utils/unit_test/test_collector.h"

//#define USING_CHARCONV

#ifdef USING_CHARCONV
#include <charconv>
#endif


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

            bi.round(15, 0, true, utl::FR_NEAR);
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

            bi.round(6, 0, true, utl::FR_NEAR);
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

            bfn.round(3, 0, utl::FR_NEAR);
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

    TEST_DEF("Float -> String. (Dec, Nor)") {
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
        TEST_TRUE(_TEST_FUNC(1.0 / (uint64_t(1) << 32), 40, "0.0000000002328306436538696289062500000000"));

        TEST_TRUE(_TEST_FUNC(1234567978598848646898790.0f, 2, "1234567946798590058299392.00"));
        TEST_TRUE(_TEST_FUNC(1234567978598848646898790.3f, 2, "1234567946798590058299392.00"));
        TEST_TRUE(_TEST_FUNC(-1234567978598848646898790.3f, 2, "-1234567946798590058299392.00"));
        TEST_TRUE(_TEST_FUNC(8945745684576845845.45764856784578456f, 2, "8945745900597149696.00"));
        TEST_TRUE(_TEST_FUNC(0.000000000000000000000000000000000003f, 10, "0.0000000000"));
        TEST_TRUE(_TEST_FUNC(1.23456789e-6f, 5, "0.00000"));
        TEST_TRUE(_TEST_FUNC(-1.23456789e-6f, 5, "-0.00000"));
        TEST_TRUE(_TEST_FUNC(std::numeric_limits<float>::quiet_NaN(), 0, "nan"));

        TEST_TRUE(_TEST_FUNC(0.000000000000000000000000000000000000666666666f, 100, "0.0000000000000000000000000000000000006666666457845356908353807450461843001623376951223899472250601785"));
        TEST_TRUE(_TEST_FUNC(0.000000000000000000000000000000000000000000001f, 100, "0.0000000000000000000000000000000000000000000014012984643248170709237295832899161312802619418765157718"));
        TEST_TRUE(_TEST_FUNC(0.000000000000000000000000000001f, 200, "0.00000000000000000000000000000100000000317107685097105134713526475381475147564611094530562240834115073084831237792968750000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
        TEST_TRUE(_TEST_FUNC(99999999999999999999999999999999999999.0f, 100, "99999996802856924650656260769173209088.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
        TEST_TRUE(_TEST_FUNC(3.4e38f, 2, "339999995214436424907732413799364296704.00"));
        TEST_TRUE(_TEST_FUNC((std::numeric_limits<float>::min)(), 200, "0.00000000000000000000000000000000000001175494350822287507968736537222245677818665556772087521508751706278417259454727172851562500000000000000000000000000000000000000000000000000000000000000000000000000"));
        TEST_TRUE(_TEST_FUNC((std::numeric_limits<float>::denorm_min)(), 200, "0.00000000000000000000000000000000000000000000140129846432481707092372958328991613128026194187651577175706828388979108268586060148663818836212158203125000000000000000000000000000000000000000000000000000"));
        TEST_TRUE(_TEST_FUNC((std::numeric_limits<float>::max)(), 200, "340282346638528859811704183484516925440.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));

        {
            float t = -0.65852354f;
            for (size_t i = 0; i < 10; ++i) {
                size_t buf_size = i;
                bool ret1 = utl::ftos<float, char16_t>(t, nullptr, &buf_size, 3, utl::FF_SCI | utl::FF_UPP, utl::FR_FLOOR);
                TEST_E(buf_size, 10);
                TEST_FALSE(ret1);
            }
        }

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

        TEST_TRUE(_TEST_FUNC(0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000666666666, 322, "0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000067"));
        TEST_TRUE(_TEST_FUNC(0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001, 360, "0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000099999998365971443346061920956311959264775925"));
        TEST_TRUE(_TEST_FUNC(0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001, 360, "0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000009881312916824930883531375857364427447"));
        TEST_TRUE(_TEST_FUNC(99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0, 100, "100000000000000005250476025520442024870446858110815915491585411551180245798890819578637137508044786404370444383288387817694252323536043057564479218478670698284838720092657580373783023379478809005936895323497079994508111903896764088007465274278014249457925878882005684283811566947219638686545940054016.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
        TEST_TRUE(_TEST_FUNC(1.7e308, 2, "169999999999999993883079578865998174333346074304075874502773119193537729178160565864330091787584707988572262467983188919169916105593357174268369962062473635296474636515660464935663040684957844303524367815028553272712298986386310828644513212353921123253311675499856875650512437415429217994623324794855339589632.00"));
        TEST_TRUE(_TEST_FUNC((std::numeric_limits<double>::min)(), 360, "0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000022250738585072013830902327173324040642192159804623318"));
        TEST_TRUE(_TEST_FUNC((std::numeric_limits<double>::denorm_min)(), 360, "0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004940656458412465441765687928682213724"));
        TEST_TRUE(_TEST_FUNC((std::numeric_limits<double>::max)(), 360, "179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));

        {
            double t = -0.65852354;
            for (size_t i = 0; i < 10; ++i) {
                size_t buf_size = i;
                bool ret1 = utl::ftos<double, char16_t>(t, nullptr, &buf_size, 3, utl::FF_SCI | utl::FF_UPP, utl::FR_FLOOR);
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
            utl::ftos(val, &result, precision, utl::FF_SCI);
            TEST_TRUE(utl::ftos(val, buf, &buf_len, precision, utl::FF_SCI));
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
            utl::ftos(val, &result, precision, utl::FF_SCI | utl::FF_NOR);
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
        TEST_TRUE(_TEST_FUNC(0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001, 360, "9.999999836597144334606192095631195926477592543369521455045849970592234919138160934722838380422693853865367923662877802160444990315364051565565391595587327639198904852632370647709618104786126163799632995155486767134548944815532598435214836120691606867323339473597648426536418734881746971242559593050185515442628522784588118513181984697915381667591534186401310452e-317"));
        TEST_TRUE(_TEST_FUNC(0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001, 360, "9.881312916824930883531375857364427447301196052286495288511713650013510145404175037305996727232719847595931293908914354618533134207118795927975495920215637562526014263806228090556916343356979642073774372721139974614461000127748183071299687746249467945463392302800634307707961482524771311823420533171133735363740791206212498638905431829849106586109130888022549603e-324"));
        TEST_TRUE(_TEST_FUNC(99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0, 360, "1.000000000000000052504760255204420248704468581108159154915854115511802457988908195786371375080447864043704443832883878176942523235360430575644792184786706982848387200926575803737830233794788090059368953234970799945081119038967640880074652742780142494579258788820056842838115669472196386865459400540160000000000000000000000000000000000000000000000000000000000000e+299"));
        TEST_TRUE(_TEST_FUNC(1234567.0, 360, "1.234567000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000e+06"));
        TEST_TRUE(_TEST_FUNC(1.7e308, 10, "1.7000000000e+308"));
        TEST_TRUE(_TEST_FUNC((std::numeric_limits<double>::min)(), 360, "2.225073858507201383090232717332404064219215980462331830553327416887204434813918195854283159012511020564067339731035811005152434161553460108856012385377718821130777993532002330479610147442583636071921565046942503734208375250806650616658158948720491179968591639648500635908770118304874799780887753749949451580451605050915399856582470818645113537935804992115981086e-308"));
        TEST_TRUE(_TEST_FUNC((std::numeric_limits<double>::denorm_min)(), 360, "4.940656458412465441765687928682213723650598026143247644255856825006755072702087518652998363616359923797965646954457177309266567103559397963987747960107818781263007131903114045278458171678489821036887186360569987307230500063874091535649843873124733972731696151400317153853980741262385655911710266585566867681870395603106249319452715914924553293054565444011274801e-324"));
        TEST_TRUE(_TEST_FUNC((std::numeric_limits<double>::max)(), 360, "1.797693134862315708145274237317043567980705675258449965989174768031572607800285387605895586327668781715404589535143824642343213268894641827684675467035375169860499105765512820762454900903893289440758685084551339423045832369032229481658085593321233482747978262041447231687381771809192998812504040261841248583680000000000000000000000000000000000000000000000000000e+308"));
        TEST_TRUE(_TEST_FUNC(std::nan(""), 0, "nan"));

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
        TEST_TRUE(_TEST_FUNC("0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001", 0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001));
        TEST_TRUE(_TEST_FUNC("99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999", std::numeric_limits<double>::infinity()));
        TEST_TRUE(_TEST_FUNC("99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999", 99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0));
        TEST_TRUE(_TEST_FUNC("0.123456", 0.123456));
        TEST_TRUE(_TEST_FUNC("0.000123456789e-2", 0.000123456789));
        TEST_TRUE(_TEST_FUNC("170000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", 170000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.0));
        TEST_TRUE(_TEST_FUNC("179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", (std::numeric_limits<double>::max)()));
        //                  |
        TEST_TRUE(_TEST_FUNC("179769313486231580814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", std::numeric_limits<double>::infinity()));
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000022250738585072013830902327173324040642192159804623318", (std::numeric_limits<double>::min)()));
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004940656458412465441765687928682213724", (std::numeric_limits<double>::denorm_min)()));
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001940656458412465441765687928682213724", 0));

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
            utl::stof(text, &result1, utl::FF_SCI);
#ifdef USING_CHARCONV
            double result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::scientific);
#endif
            TEST_E(result1, r);
            return true;
        };
        auto _TEST_FUNC_NAN = [](const std::string& text) -> bool {
            double result1;
            utl::stof(text, &result1, utl::FF_SCI);
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
        TEST_TRUE(_TEST_FUNC("0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001", 0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001));
        TEST_TRUE(_TEST_FUNC("99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999", std::numeric_limits<double>::infinity()));
        TEST_TRUE(_TEST_FUNC("99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999", 99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0));

        TEST_TRUE(_TEST_FUNC("0.123456", 0.123456));
        TEST_TRUE(_TEST_FUNC("0.000123456789e-2", 0.000123456789e-2));
        TEST_TRUE(_TEST_FUNC("1.700123456789e308", 1.700123456789e308));
        TEST_TRUE(_TEST_FUNC("1.600123456789e+308", 1.600123456789e+308));
        TEST_TRUE(_TEST_FUNC("2.470328e-324", 2.470328e-324));
        TEST_TRUE(_TEST_FUNC("2.470328e-326", 2.470328e-326));
        TEST_TRUE(_TEST_FUNC("179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000e0", (std::numeric_limits<double>::max)()));
        //                  |
        TEST_TRUE(_TEST_FUNC("179769313486231580814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000e0", std::numeric_limits<double>::infinity()));
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000022250738585072013830902327173324040642192159804623318e0", (std::numeric_limits<double>::min)()));
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004940656458412465441765687928682213724e0", (std::numeric_limits<double>::denorm_min)()));
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001940656458412465441765687928682213724e0", 0.0));

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
            utl::ftos(val, &result, precision, utl::FF_HEX);
            TEST_TRUE(utl::ftos(val, buf, &buf_len, precision, utl::FF_HEX));
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
        TEST_TRUE(_TEST_FUNC((std::numeric_limits<double>::min)(), 360, "0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
        TEST_TRUE(_TEST_FUNC((std::numeric_limits<double>::denorm_min)(), 360, "0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
        TEST_TRUE(_TEST_FUNC((std::numeric_limits<double>::max)(), 360, "fffffffffffff800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
        TEST_TRUE(_TEST_FUNC(std::numeric_limits<double>::quiet_NaN(), 0, "nan"));

        return true;
    };

    TEST_DEF("Double -> String. (Hex, Sci)") {
        auto _TEST_FUNC = [](double val, int precision, const std::string& r) -> bool {
            char buf[1000]{ 0 };
            size_t buf_len = 1000;

            std::string result;
            utl::ftos(val, &result, precision, utl::FF_SCI | utl::FF_HEX);
            TEST_TRUE(utl::ftos(val, buf, &buf_len, precision, utl::FF_SCI | utl::FF_HEX));
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

            utl::ftos(val, &result, precision, utl::FF_HEX2);
            TEST_TRUE(utl::ftos(val, buf, &buf_len, precision, utl::FF_HEX2));
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
        TEST_TRUE(_TEST_FUNC((std::numeric_limits<double>::min)(), 360, "4.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s-256"));
        TEST_TRUE(_TEST_FUNC((std::numeric_limits<double>::denorm_min)(), 360, "4.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s-269"));
        TEST_TRUE(_TEST_FUNC((std::numeric_limits<double>::max)(), 360, "f.ffffffffffff800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s+255"));
        TEST_TRUE(_TEST_FUNC(std::nan(""), 2, "nan"));

        return true;
    };

    TEST_DEF("String -> Double. (Hex, Nor)") {
        auto _TEST_FUNC = [](const std::string& text, double r) -> bool {
            double result1;
            utl::stof(text, &result1, utl::FF_NOR | utl::FF_HEX);
#ifdef USING_CHARCONV
            double result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::hex);
#endif
            TEST_E(result1, r);
            return true;
        };
        auto _TEST_FUNC_NAN = [](const std::string& text) -> bool {
            double result1;
            utl::stof(text, &result1, utl::FF_NOR | utl::FF_HEX);
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
        TEST_TRUE(_TEST_FUNC("FFFFFFFFFFFFF800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", (std::numeric_limits<double>::max)()));
        TEST_TRUE(_TEST_FUNC("FFFFFFFFFFFFFC00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", std::numeric_limits<double>::infinity()));
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", (std::numeric_limits<double>::min)()));
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", (std::numeric_limits<double>::denorm_min)()));
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000", 0.0));

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
            utl::stof(text, &result1, utl::FF_SCI | utl::FF_HEX);
#ifdef USING_CHARCONV
            double result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::hex);
#endif
            TEST_E(result1, r);
            return true;
        };
        auto _TEST_FUNC2 = [](const std::string& text, double r) -> bool {
            double result1;
            utl::stof(text, &result1, utl::FF_HEX2);
#ifdef USING_CHARCONV
            double result2;
            std::from_chars(text.data(), text.data() + text.size(), result2, std::chars_format::hex);
#endif
            TEST_E(result1, r);
            return true;
        };
        auto _TEST_FUNC_NAN = [](const std::string& text) -> bool {
            double result1;
            utl::stof(text, &result1, utl::FF_SCI);
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
        TEST_TRUE(_TEST_FUNC("FFFFFFFFFFFFF800000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s0", (std::numeric_limits<double>::max)()));
        TEST_TRUE(_TEST_FUNC("FFFFFFFFFFFFFC00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s0", std::numeric_limits<double>::infinity()));
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s0", (std::numeric_limits<double>::min)()));
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s0", (std::numeric_limits<double>::denorm_min)()));
        TEST_TRUE(_TEST_FUNC("0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000s0", 0.0));

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

    TEST_DEF("LongDouble -> String. (Dec, Nor)") {
        auto _TEST_FUNC = [](long double val, int precision, const std::string& r) -> bool {
            char buf[16384]{ 0 };
            size_t buf_len = 16384;

            std::string result;
            utl::ftos(val, &result, precision);
            TEST_TRUE(utl::ftos(val, buf, &buf_len, precision));
#ifdef USING_CHARCONV
            char buf2[16384]{ 0 };
            std::to_chars(std::begin(buf2), std::end(buf2), val, std::chars_format::fixed, precision);
#endif
            TEST_E(std::string(buf, buf_len), r);
            TEST_E(result, r);
            return true;
        };

        TEST_TRUE(_TEST_FUNC(0.0, 0, "0"));
        TEST_TRUE(_TEST_FUNC(-0.0, 0, "-0"));
        TEST_TRUE(_TEST_FUNC(0.1, 0, "0"));
        TEST_TRUE(_TEST_FUNC(0.1, 1, "0.1"));
        TEST_TRUE(_TEST_FUNC(0.34578, 5, "0.34578"));
        TEST_TRUE(_TEST_FUNC(1.0, 3, "1.000"));
        TEST_TRUE(_TEST_FUNC(1.666666666, 0, "2"));
        TEST_TRUE(_TEST_FUNC(1.666666666, 2, "1.67"));
        TEST_TRUE(_TEST_FUNC(8.1, 0, "8"));
        TEST_TRUE(_TEST_FUNC(8.1, 2, "8.10"));
        TEST_TRUE(_TEST_FUNC(-8.1, 2, "-8.10"));
        TEST_TRUE(_TEST_FUNC(999.999, 3, "999.999"));
        TEST_TRUE(_TEST_FUNC(1234567.0, 2, "1234567.00"));
        TEST_TRUE(_TEST_FUNC(1.0 / (uint64_t(1) << 32), 40, "0.0000000002328306436538696289062500000000"));

        TEST_TRUE(_TEST_FUNC(1234567978598848646898790.0, 2, "1234567978598848698253312.00"));
        TEST_TRUE(_TEST_FUNC(1234567978598848646898790.3, 2, "1234567978598848698253312.00"));
        TEST_TRUE(_TEST_FUNC(-1234567978598848646898790.3, 2, "-1234567978598848698253312.00"));
        TEST_TRUE(_TEST_FUNC(8945745684576845845.45764856784578456, 2, "8945745684576845824.00"));
        TEST_TRUE(_TEST_FUNC(0.000000000000000000000000000000000003, 10, "0.0000000000"));
        TEST_TRUE(_TEST_FUNC(1.23456789e-6, 5, "0.00000"));
        TEST_TRUE(_TEST_FUNC(-1.23456789e-6, 5, "-0.00000"));
        TEST_TRUE(_TEST_FUNC(std::numeric_limits<long double>::quiet_NaN(), 0, "nan"));

        TEST_TRUE(_TEST_FUNC(0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000666666666, 322, "0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000067"));
        TEST_TRUE(_TEST_FUNC(0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001, 360, "0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000099999998365971443346061920956311959264775925"));
        TEST_TRUE(_TEST_FUNC(0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001, 360, "0.000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000009881312916824930883531375857364427447"));
        TEST_TRUE(_TEST_FUNC(99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.0, 100, "100000000000000005250476025520442024870446858110815915491585411551180245798890819578637137508044786404370444383288387817694252323536043057564479218478670698284838720092657580373783023379478809005936895323497079994508111903896764088007465274278014249457925878882005684283811566947219638686545940054016.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"));
        TEST_TRUE(_TEST_FUNC(1.7e308, 2, "169999999999999993883079578865998174333346074304075874502773119193537729178160565864330091787584707988572262467983188919169916105593357174268369962062473635296474636515660464935663040684957844303524367815028553272712298986386310828644513212353921123253311675499856875650512437415429217994623324794855339589632.00"));

        std::string result;
        utl::ftos((std::numeric_limits<long double>::max)(), &result, 5000);
        utl::ftos((std::numeric_limits<long double>::min)(), &result, 5000);
        utl::ftos(std::numeric_limits<long double>::denorm_min(), &result, 5000);

        return true;
    };

}
