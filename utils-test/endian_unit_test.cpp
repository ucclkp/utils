// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/endian.hpp"
#include "utils/endian_ml.hpp"
#include "utils/unit_test/test_collector.h"


using namespace utl;

TEST_CASE(EndianConverterUnitTest) {

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

    TEST_DEF("Endian ML BE -> LE 4 Bytes tests.") {
        // uint 32
        {
            uint32_t mem;
            auto buf = reinterpret_cast<unsigned char*>(&mem);

            buf[0] = 0; buf[1] = 0; buf[2] = 0x22; buf[3] = 0x11;
            TEST_TRUE(swapBytesMLB2L<2>(mem) == uint32_t(0x0000'0000));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0x00;
            TEST_TRUE(swapBytesMLB2L<2>(mem) == uint32_t(0x0000'89AB));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0x12;
            TEST_TRUE(swapBytesMLB2L<2>(mem) == uint32_t(0x0000'FFFF));


            buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0x11;
            TEST_TRUE(swapBytesMLB2L<3>(mem) == uint32_t(0x0000'0000));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0x00;
            TEST_TRUE(swapBytesMLB2L<3>(mem) == uint32_t(0x0089'ABCD));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0x12;
            TEST_TRUE(swapBytesMLB2L<3>(mem) == uint32_t(0x00FF'FFFF));
        }

        // int 32
        {
            int32_t mem = 0;
            auto buf = reinterpret_cast<unsigned char*>(&mem);

            buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0x11;
            TEST_TRUE(swapBytesMLB2L<2>(mem) == int32_t(0x00000000));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0xCC;
            TEST_TRUE(swapBytesMLB2L<2>(mem) == int32_t(0xFFFF89AB));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0x01; buf[3] = 0x23;
            TEST_TRUE(swapBytesMLB2L<2>(mem) == int32_t(0xFFFFFFFF));


            buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0x11;
            TEST_TRUE(swapBytesMLB2L<3>(mem) == int32_t(0x00000000));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0xCC;
            TEST_TRUE(swapBytesMLB2L<3>(mem) == int32_t(0xFF89ABCD));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0x23;
            TEST_TRUE(swapBytesMLB2L<3>(mem) == int32_t(0xFFFFFFFF));
        }
        return true;
    };

    TEST_DEF("Endian ML BE -> LE 8 Bytes tests.") {
        // uint 64
        {
            uint64_t mem = 0;
            auto buf = reinterpret_cast<unsigned char*>(&mem);

            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            TEST_TRUE(swapBytesMLB2L<4>(mem) == uint64_t(0x0000'0000));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            TEST_TRUE(swapBytesMLB2L<4>(mem) == uint64_t(0x0123'4567));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            TEST_TRUE(swapBytesMLB2L<4>(mem) == uint64_t(0xFFFF'FFFF));


            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            TEST_TRUE(swapBytesMLB2L<5>(mem) == uint64_t(0x0000'0000'11));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            TEST_TRUE(swapBytesMLB2L<5>(mem) == uint64_t(0x0123'4567'89));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            TEST_TRUE(swapBytesMLB2L<5>(mem) == uint64_t(0xFFFF'FFFF'CC));


            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            TEST_TRUE(swapBytesMLB2L<6>(mem) == uint64_t(0x0000'0000'1122));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            TEST_TRUE(swapBytesMLB2L<6>(mem) == uint64_t(0x0123'4567'89AB));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            TEST_TRUE(swapBytesMLB2L<6>(mem) == uint64_t(0xFFFF'FFFF'CCCC));


            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            TEST_TRUE(swapBytesMLB2L<7>(mem) == uint64_t(0x0000'0000'1122'EE));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            TEST_TRUE(swapBytesMLB2L<7>(mem) == uint64_t(0x0123'4567'89AB'CD));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            TEST_TRUE(swapBytesMLB2L<7>(mem) == uint64_t(0xFFFF'FFFF'CCCC'CC));
        }

        // int 64
        {
            int64_t mem = 0;
            auto buf = reinterpret_cast<unsigned char*>(&mem);

            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            TEST_TRUE(swapBytesMLB2L<4>(mem) == int64_t(0x0000'0000));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            TEST_TRUE(swapBytesMLB2L<4>(mem) == int64_t(0x0123'4567));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            TEST_TRUE(swapBytesMLB2L<4>(mem) == int64_t(0xFFFF'FFFF'FFFF'FFFF));


            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            TEST_TRUE(swapBytesMLB2L<5>(mem) == int64_t(0x0000'0000'11));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            TEST_TRUE(swapBytesMLB2L<5>(mem) == int64_t(0x0123'4567'89));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            TEST_TRUE(swapBytesMLB2L<5>(mem) == int64_t(0xFFFF'FFFF'FFFF'FFCC));


            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            TEST_TRUE(swapBytesMLB2L<6>(mem) == int64_t(0x0000'0000'1122));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            TEST_TRUE(swapBytesMLB2L<6>(mem) == int64_t(0x0123'4567'89AB));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            TEST_TRUE(swapBytesMLB2L<6>(mem) == int64_t(0xFFFF'FFFF'FFFF'CCCC));


            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            TEST_TRUE(swapBytesMLB2L<7>(mem) == int64_t(0x0000'0000'1122'EE));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            TEST_TRUE(swapBytesMLB2L<7>(mem) == int64_t(0x0123'4567'89AB'CD));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            TEST_TRUE(swapBytesMLB2L<7>(mem) == int64_t(0xFFFF'FFFF'FFCC'CCCC));
        }
        return true;
    };

    TEST_DEF("Endian ML LE -> BE 4 Bytes tests.") {
        // uint 32
        {
            uint32_t mem = 0;
            auto buf = reinterpret_cast<unsigned char*>(&mem);

            buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0x11;
            TEST_TRUE(swapBytesMLL2B<2>(mem) == uint32_t(0x0000'0000));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0xFE;
            TEST_TRUE(swapBytesMLL2B<2>(mem) == uint32_t(0x89AB'0000));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0x12;
            TEST_TRUE(swapBytesMLL2B<2>(mem) == uint32_t(0xFFFF'0000));


            buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0x11;
            TEST_TRUE(swapBytesMLL2B<3>(mem) == uint32_t(0x0000'0000));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0xFE;
            TEST_TRUE(swapBytesMLL2B<3>(mem) == uint32_t(0x89AB'CD00));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0x12;
            TEST_TRUE(swapBytesMLL2B<3>(mem) == uint32_t(0xFFFF'FF00));
        }

        // int 32
        {
            int32_t mem = 0;
            auto buf = reinterpret_cast<unsigned char*>(&mem);

            buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0x11;
            TEST_TRUE(swapBytesMLL2B<2>(mem) == int32_t(0x0000'0000));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0xCC;
            TEST_TRUE(swapBytesMLL2B<2>(mem) == int32_t(0x89AB'FFFF));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0x23;
            TEST_TRUE(swapBytesMLL2B<2>(mem) == int32_t(0xFFFF'FFFF));


            buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0x11;
            TEST_TRUE(swapBytesMLL2B<3>(mem) == int32_t(0x0000'0000));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0xCC;
            TEST_TRUE(swapBytesMLL2B<3>(mem) == int32_t(0x89AB'CDFF));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0x23;
            TEST_TRUE(swapBytesMLL2B<3>(mem) == int32_t(0xFFFF'FFFF));
        }

        return true;
    };

    TEST_DEF("Endian ML LE -> BE 8 Bytes tests.") {
        // uint 64
        {
            uint64_t mem = 0;
            auto buf = reinterpret_cast<unsigned char*>(&mem);

            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            TEST_TRUE(swapBytesMLL2B<4>(mem) == uint64_t(0x0000'0000));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            TEST_TRUE(swapBytesMLL2B<4>(mem) == uint64_t(0x0123'4567'0000'0000));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            TEST_TRUE(swapBytesMLL2B<4>(mem) == uint64_t(0xFFFF'FFFF'0000'0000));


            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            TEST_TRUE(swapBytesMLL2B<5>(mem) == uint64_t(0x0000'0000'1100'0000));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            TEST_TRUE(swapBytesMLL2B<5>(mem) == uint64_t(0x0123'4567'8900'0000));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            TEST_TRUE(swapBytesMLL2B<5>(mem) == uint64_t(0xFFFF'FFFF'CC00'0000));


            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            TEST_TRUE(swapBytesMLL2B<6>(mem) == uint64_t(0x0000'0000'1122'0000));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            TEST_TRUE(swapBytesMLL2B<6>(mem) == uint64_t(0x0123'4567'89AB'0000));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            TEST_TRUE(swapBytesMLL2B<6>(mem) == uint64_t(0xFFFF'FFFF'CCCC'0000));


            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            TEST_TRUE(swapBytesMLL2B<7>(mem) == uint64_t(0x0000'0000'1122'EE00));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            TEST_TRUE(swapBytesMLL2B<7>(mem) == uint64_t(0x0123'4567'89AB'CD00));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            TEST_TRUE(swapBytesMLL2B<7>(mem) == uint64_t(0xFFFF'FFFF'CCCC'CC00));
        }

        // int 64
        {
            int64_t mem = 0;
            auto buf = reinterpret_cast<unsigned char*>(&mem);

            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            TEST_TRUE(swapBytesMLL2B<4>(mem) == int64_t(0x0000'0000));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            TEST_TRUE(swapBytesMLL2B<4>(mem) == int64_t(0x0123'4567'0000'0000));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            TEST_TRUE(swapBytesMLL2B<4>(mem) == int64_t(0xFFFF'FFFF'FFFF'FFFF));


            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            TEST_TRUE(swapBytesMLL2B<5>(mem) == int64_t(0x0000'0000'1100'0000));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            TEST_TRUE(swapBytesMLL2B<5>(mem) == int64_t(0x0123'4567'89FF'FFFF));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            TEST_TRUE(swapBytesMLL2B<5>(mem) == int64_t(0xFFFF'FFFF'CCFF'FFFF));


            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            TEST_TRUE(swapBytesMLL2B<6>(mem) == int64_t(0x0000'0000'1122'0000));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            TEST_TRUE(swapBytesMLL2B<6>(mem) == int64_t(0x0123'4567'89AB'FFFF));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            TEST_TRUE(swapBytesMLL2B<6>(mem) == int64_t(0xFFFF'FFFF'CCCC'FFFF));


            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            TEST_TRUE(swapBytesMLL2B<7>(mem) == int64_t(0x0000'0000'1122'EEFF));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            TEST_TRUE(swapBytesMLL2B<7>(mem) == int64_t(0x0123'4567'89AB'CDFF));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            TEST_TRUE(swapBytesMLL2B<7>(mem) == int64_t(0xFFFF'FFFF'CCCC'CCFF));
        }
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

    TEST_DEF("MLToLE tests.") {
        uint32_t mem;
        auto buf = reinterpret_cast<unsigned char*>(&mem);

        buf[0] = 0; buf[1] = 0; buf[2] = 0x22; buf[3] = 0x11;
        TEST_E(MLToLE<2>(mem), 0x0000'0000);
        TEST_E(MLToLE<3>(mem), 0x0022'0000);

        return true;
    };

    TEST_DEF("MLToBE tests.") {
        uint32_t mem;
        auto buf = reinterpret_cast<unsigned char*>(&mem);

        buf[0] = 0; buf[1] = 0; buf[2] = 0x22; buf[3] = 0x11;
        TEST_E(MLToBE<2>(mem), 0x0000'0000);
        TEST_E(MLToBE<3>(mem), 0x0000'2200);

        return true;
    };

    /*TEST_DEF("fromMLLE tests.") {
        return true;
    };

    TEST_DEF("fromMLBE tests.") {
        return true;
    };

    TEST_DEF("toMLLE tests.") {
        return true;
    };

    TEST_DEF("toMLBE tests.") {
        return true;
    };*/

}