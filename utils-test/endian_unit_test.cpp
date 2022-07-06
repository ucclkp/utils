// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/endian.hpp"
#include "utils/unit_test/test_collector.h"


using namespace utl;

TEST_CASE(EndianUnitTest) {

    TEST_DEF("Endian tests.") {
        // int 16
        TEST_TRUE(swapBytes(uint16_t(0x0000)) == uint16_t(0x0000));
        TEST_TRUE(swapBytes(uint16_t(0xABCD)) == uint16_t(0xCDAB));
        TEST_TRUE(swapBytes(uint16_t(0xFFFF)) == uint16_t(0xFFFF));

        TEST_TRUE(swapBytes(int16_t(0x0000)) == int16_t(0x0000));
        TEST_TRUE(swapBytes(int16_t(0xABCD)) == int16_t(0xCDAB));
        TEST_TRUE(swapBytes(int16_t(0xFFFF)) == int16_t(0xFFFF));

        // int 32
        TEST_TRUE(swapBytes(uint32_t(0x0000'0000)) == uint32_t(0x0000'0000));
        TEST_TRUE(swapBytes(uint32_t(0x89AB'CDEF)) == uint32_t(0xEFCD'AB89));
        TEST_TRUE(swapBytes(uint32_t(0xFFFF'FFFF)) == uint32_t(0xFFFF'FFFF));

        TEST_TRUE(swapBytes(int32_t(0x0000'0000)) == int32_t(0x0000'0000));
        TEST_TRUE(swapBytes(int32_t(0x89AB'CDEF)) == int32_t(0xEFCD'AB89));
        TEST_TRUE(swapBytes(int32_t(0xFFFF'FFFF)) == int32_t(0xFFFF'FFFF));

        // int 64
        TEST_TRUE(swapBytes(uint64_t(0x0000'0000'0000'0000)) == uint64_t(0x0000'0000'0000'0000));
        TEST_TRUE(swapBytes(uint64_t(0x0123'4567'89AB'CDEF)) == uint64_t(0xEFCD'AB89'6745'2301));
        TEST_TRUE(swapBytes(uint64_t(0xFFFF'FFFF'FFFF'FFFF)) == uint64_t(0xFFFF'FFFF'FFFF'FFFF));

        TEST_TRUE(swapBytes(int64_t(0x0000'0000'0000'0000)) == int64_t(0x0000'0000'0000'0000));
        TEST_TRUE(swapBytes(int64_t(0x0123'4567'89AB'CDEF)) == int64_t(0xEFCD'AB89'6745'2301));
        TEST_TRUE(swapBytes(int64_t(0xFFFF'FFFF'FFFF'FFFF)) == int64_t(0xFFFF'FFFF'FFFF'FFFF));

        return true;
    };

    TEST_DEF("fromToBE tests.") {
#ifdef ENDIAN_LE
        TEST_E(fromToBE(int16_t(0x1122)), int16_t(0x2211));
        TEST_E(fromToBE(int16_t(0x0000)), int16_t(0x0000));
        TEST_E(fromToBE(int16_t(0xFFFF)), int16_t(0xFFFF));
        TEST_E(fromToBE(uint16_t(0x1122u)), uint16_t(0x2211u));
        TEST_E(fromToBE(uint16_t(0x0000u)), uint16_t(0x0000u));
        TEST_E(fromToBE(uint16_t(0xFFFFu)), uint16_t(0xFFFFu));

        TEST_E(fromToBE(int32_t(0x1122'3344)), int32_t(0x4433'2211));
        TEST_E(fromToBE(int32_t(0x0000'0000)), int32_t(0x0000'0000));
        TEST_E(fromToBE(int32_t(0xFFFF'FFFF)), int32_t(0xFFFF'FFFF));
        TEST_E(fromToBE(uint32_t(0x1122'3344u)), uint32_t(0x4433'2211u));
        TEST_E(fromToBE(uint32_t(0x0000'0000u)), uint32_t(0x0000'0000u));
        TEST_E(fromToBE(uint32_t(0xFFFF'FFFFu)), uint32_t(0xFFFF'FFFFu));

        TEST_E(fromToBE(int64_t(0x1122'3344'5566'7788)), int64_t(0x8877'6655'4433'2211));
        TEST_E(fromToBE(int64_t(0x0000'0000'0000'0000)), int64_t(0x0000'0000'0000'0000));
        TEST_E(fromToBE(int64_t(0xFFFF'FFFF'FFFF'FFFF)), int64_t(0xFFFF'FFFF'FFFF'FFFF));
        TEST_E(fromToBE(uint64_t(0x1122'3344'5566'7788u)), uint64_t(0x8877'6655'4433'2211u));
        TEST_E(fromToBE(uint64_t(0x0000'0000'0000'0000u)), uint64_t(0x0000'0000'0000'0000u));
        TEST_E(fromToBE(uint64_t(0xFFFF'FFFF'FFFF'FFFFu)), uint64_t(0xFFFF'FFFF'FFFF'FFFFu));
#else
        TEST_E(fromToLE(int16_t(0x1122)), int16_t(0x1122));
        TEST_E(fromToLE(int16_t(0x0000)), int16_t(0x0000));
        TEST_E(fromToLE(int16_t(0xFFFF)), int16_t(0xFFFF));
        TEST_E(fromToLE(uint16_t(0x1122u)), uint16_t(0x1122u));
        TEST_E(fromToLE(uint16_t(0x0000u)), uint16_t(0x0000u));
        TEST_E(fromToLE(uint16_t(0xFFFFu)), uint16_t(0xFFFFu));

        TEST_E(fromToLE(int32_t(0x1122'3344)), int32_t(0x1122'3344));
        TEST_E(fromToLE(int32_t(0x0000'0000)), int32_t(0x0000'0000));
        TEST_E(fromToLE(int32_t(0xFFFF'FFFF)), int32_t(0xFFFF'FFFF));
        TEST_E(fromToLE(uint32_t(0x1122'3344u)), uint32_t(0x1122'3344u));
        TEST_E(fromToLE(uint32_t(0x0000'0000u)), uint32_t(0x0000'0000u));
        TEST_E(fromToLE(uint32_t(0xFFFF'FFFFu)), uint32_t(0xFFFF'FFFFu));

        TEST_E(fromToLE(int64_t(0x1122'3344'5566'7788)), int64_t(0x1122'3344'5566'7788));
        TEST_E(fromToLE(int64_t(0x0000'0000'0000'0000)), int64_t(0x0000'0000'0000'0000));
        TEST_E(fromToLE(int64_t(0xFFFF'FFFF'FFFF'FFFF)), int64_t(0xFFFF'FFFF'FFFF'FFFF));
        TEST_E(fromToLE(uint64_t(0x1122'3344'5566'7788u)), uint64_t(0x1122'3344'5566'7788u));
        TEST_E(fromToLE(uint64_t(0x0000'0000'0000'0000u)), uint64_t(0x0000'0000'0000'0000u));
        TEST_E(fromToLE(uint64_t(0xFFFF'FFFF'FFFF'FFFFu)), uint64_t(0xFFFF'FFFF'FFFF'FFFFu));
#endif
        return true;
    };

    TEST_DEF("fromToLE tests.") {
#ifdef ENDIAN_LE
        TEST_E(fromToLE(int16_t(0x1122)), int16_t(0x1122));
        TEST_E(fromToLE(int16_t(0x0000)), int16_t(0x0000));
        TEST_E(fromToLE(int16_t(0xFFFF)), int16_t(0xFFFF));
        TEST_E(fromToLE(uint16_t(0x1122u)), uint16_t(0x1122u));
        TEST_E(fromToLE(uint16_t(0x0000u)), uint16_t(0x0000u));
        TEST_E(fromToLE(uint16_t(0xFFFFu)), uint16_t(0xFFFFu));

        TEST_E(fromToLE(int32_t(0x1122'3344)), int32_t(0x1122'3344));
        TEST_E(fromToLE(int32_t(0x0000'0000)), int32_t(0x0000'0000));
        TEST_E(fromToLE(int32_t(0xFFFF'FFFF)), int32_t(0xFFFF'FFFF));
        TEST_E(fromToLE(uint32_t(0x1122'3344u)), uint32_t(0x1122'3344u));
        TEST_E(fromToLE(uint32_t(0x0000'0000u)), uint32_t(0x0000'0000u));
        TEST_E(fromToLE(uint32_t(0xFFFF'FFFFu)), uint32_t(0xFFFF'FFFFu));

        TEST_E(fromToLE(int64_t(0x1122'3344'5566'7788)), int64_t(0x1122'3344'5566'7788));
        TEST_E(fromToLE(int64_t(0x0000'0000'0000'0000)), int64_t(0x0000'0000'0000'0000));
        TEST_E(fromToLE(int64_t(0xFFFF'FFFF'FFFF'FFFF)), int64_t(0xFFFF'FFFF'FFFF'FFFF));
        TEST_E(fromToLE(uint64_t(0x1122'3344'5566'7788u)), uint64_t(0x1122'3344'5566'7788u));
        TEST_E(fromToLE(uint64_t(0x0000'0000'0000'0000u)), uint64_t(0x0000'0000'0000'0000u));
        TEST_E(fromToLE(uint64_t(0xFFFF'FFFF'FFFF'FFFFu)), uint64_t(0xFFFF'FFFF'FFFF'FFFFu));
#else
        TEST_E(fromToBE(int16_t(0x1122)), int16_t(0x2211));
        TEST_E(fromToBE(int16_t(0x0000)), int16_t(0x0000));
        TEST_E(fromToBE(int16_t(0xFFFF)), int16_t(0xFFFF));
        TEST_E(fromToBE(uint16_t(0x1122u)), uint16_t(0x2211u));
        TEST_E(fromToBE(uint16_t(0x0000u)), uint16_t(0x0000u));
        TEST_E(fromToBE(uint16_t(0xFFFFu)), uint16_t(0xFFFFu));

        TEST_E(fromToBE(int32_t(0x1122'3344)), int32_t(0x4433'2211));
        TEST_E(fromToBE(int32_t(0x0000'0000)), int32_t(0x0000'0000));
        TEST_E(fromToBE(int32_t(0xFFFF'FFFF)), int32_t(0xFFFF'FFFF));
        TEST_E(fromToBE(uint32_t(0x1122'3344u)), uint32_t(0x4433'2211u));
        TEST_E(fromToBE(uint32_t(0x0000'0000u)), uint32_t(0x0000'0000u));
        TEST_E(fromToBE(uint32_t(0xFFFF'FFFFu)), uint32_t(0xFFFF'FFFFu));

        TEST_E(fromToBE(int64_t(0x1122'3344'5566'7788)), int64_t(0x8877'6655'4433'2211));
        TEST_E(fromToBE(int64_t(0x0000'0000'0000'0000)), int64_t(0x0000'0000'0000'0000));
        TEST_E(fromToBE(int64_t(0xFFFF'FFFF'FFFF'FFFF)), int64_t(0xFFFF'FFFF'FFFF'FFFF));
        TEST_E(fromToBE(uint64_t(0x1122'3344'5566'7788u)), uint64_t(0x8877'6655'4433'2211u));
        TEST_E(fromToBE(uint64_t(0x0000'0000'0000'0000u)), uint64_t(0x0000'0000'0000'0000u));
        TEST_E(fromToBE(uint64_t(0xFFFF'FFFF'FFFF'FFFFu)), uint64_t(0xFFFF'FFFF'FFFF'FFFFu));
#endif

        return true;
    };

}