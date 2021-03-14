// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils-test/endian_unit_test.h"

#include "utils/endian.hpp"
#include "utils/endian_ml.hpp"
#include "utils/log.h"


namespace utl {
namespace test {

    void testEndian() {
        // int 16
        DCHECK(swapBytes(uint16_t(0x0000)) == uint16_t(0x0000));
        DCHECK(swapBytes(uint16_t(0xABCD)) == uint16_t(0xCDAB));
        DCHECK(swapBytes(uint16_t(0xFFFF)) == uint16_t(0xFFFF));

        DCHECK(swapBytes(int16_t(0x0000)) == int16_t(0x0000));
        DCHECK(swapBytes(int16_t(0xABCD)) == int16_t(0xCDAB));
        DCHECK(swapBytes(int16_t(0xFFFF)) == int16_t(0xFFFF));

        // int 32
        DCHECK(swapBytes(uint32_t(0x00000000)) == uint32_t(0x00000000));
        DCHECK(swapBytes(uint32_t(0x89ABCDEF)) == uint32_t(0xEFCDAB89));
        DCHECK(swapBytes(uint32_t(0xFFFFFFFF)) == uint32_t(0xFFFFFFFF));

        DCHECK(swapBytes(int32_t(0x00000000)) == int32_t(0x00000000));
        DCHECK(swapBytes(int32_t(0x89ABCDEF)) == int32_t(0xEFCDAB89));
        DCHECK(swapBytes(int32_t(0xFFFFFFFF)) == int32_t(0xFFFFFFFF));

        // int 64
        DCHECK(swapBytes(uint64_t(0x0000000000000000)) == uint64_t(0x0000000000000000));
        DCHECK(swapBytes(uint64_t(0x0123456789ABCDEF)) == uint64_t(0xEFCDAB8967452301));
        DCHECK(swapBytes(uint64_t(0xFFFFFFFFFFFFFFFF)) == uint64_t(0xFFFFFFFFFFFFFFFF));

        DCHECK(swapBytes(int64_t(0x0000000000000000)) == int64_t(0x0000000000000000));
        DCHECK(swapBytes(int64_t(0x0123456789ABCDEF)) == int64_t(0xEFCDAB8967452301));
        DCHECK(swapBytes(int64_t(0xFFFFFFFFFFFFFFFF)) == int64_t(0xFFFFFFFFFFFFFFFF));
    }

    void testEndianMLBExLE() {
        // uint 32
        {
            uint32_t mem = 0;
            uint8_t* buf = reinterpret_cast<uint8_t*>(&mem);

            buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0x11;
            DCHECK(MLBExLE<3>(mem) == uint32_t(0x00000000));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0xFE;
            DCHECK(MLBExLE<3>(mem) == uint32_t(0x0089ABCD));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0x12;
            DCHECK(MLBExLE<3>(mem) == uint32_t(0x00FFFFFF));
        }

        // int 32
        {
            int32_t mem = 0;
            uint8_t* buf = reinterpret_cast<uint8_t*>(&mem);

            buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0x11;
            DCHECK(MLBExLE<3>(mem) == int32_t(0x00000000));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0xCC;
            DCHECK(MLBExLE<3>(mem) == int32_t(0xFF89ABCD));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0x23;
            DCHECK(MLBExLE<3>(mem) == int32_t(0xFFFFFFFF));
        }

        // uint 64
        {
            uint64_t mem = 0;
            uint8_t* buf = reinterpret_cast<uint8_t*>(&mem);

            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            DCHECK(MLBExLE<4>(mem) == uint64_t(0x00000000));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            DCHECK(MLBExLE<4>(mem) == uint64_t(0x01234567));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            DCHECK(MLBExLE<4>(mem) == uint64_t(0xFFFFFFFF));
        }

        // int 64
        {
            int64_t mem = 0;
            uint8_t* buf = reinterpret_cast<uint8_t*>(&mem);

            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            DCHECK(MLBExLE<4>(mem) == int64_t(0x00000000));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            DCHECK(MLBExLE<4>(mem) == int64_t(0x01234567));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            DCHECK(MLBExLE<4>(mem) == int64_t(0xFFFFFFFFFFFFFFFF));
        }
    }

    void testEndianMLLExBE() {
        // uint 32
        {
            uint32_t mem = 0;
            uint8_t* buf = reinterpret_cast<uint8_t*>(&mem);

            buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0x11;
            DCHECK(MLLExBE<3>(mem) == uint32_t(0x00000000));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0xFE;
            DCHECK(MLLExBE<3>(mem) == uint32_t(0x89ABCD00));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0x12;
            DCHECK(MLLExBE<3>(mem) == uint32_t(0xFFFFFF00));
        }

        // int 32
        {
            int32_t mem = 0;
            uint8_t* buf = reinterpret_cast<uint8_t*>(&mem);

            buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0x11;
            DCHECK(MLLExBE<3>(mem) == int32_t(0x00000000));

            buf[0] = 0x89; buf[1] = 0xAB; buf[2] = 0xCD; buf[3] = 0xCC;
            DCHECK(MLLExBE<3>(mem) == int32_t(0x89ABCDFF));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0x23;
            DCHECK(MLLExBE<3>(mem) == int32_t(0xFFFFFFFF));
        }

        // uint 64
        {
            uint64_t mem = 0;
            uint8_t* buf = reinterpret_cast<uint8_t*>(&mem);

            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            DCHECK(MLLExBE<4>(mem) == uint64_t(0x00000000));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            DCHECK(MLLExBE<4>(mem) == uint64_t(0x0123456700000000));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            DCHECK(MLLExBE<4>(mem) == uint64_t(0xFFFFFFFF00000000));
        }

        // int 64
        {
            int64_t mem = 0;
            uint8_t* buf = reinterpret_cast<uint8_t*>(&mem);

            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x11; buf[5] = 0x22; buf[6] = 0xEE; buf[7] = 0xFF;
            DCHECK(MLLExBE<4>(mem) == int64_t(0x00000000));

            buf[0] = 0x01; buf[1] = 0x23; buf[2] = 0x45; buf[3] = 0x67; buf[4] = 0x89; buf[5] = 0xAB; buf[6] = 0xCD; buf[7] = 0xEF;
            DCHECK(MLLExBE<4>(mem) == int64_t(0x0123456700000000));

            buf[0] = 0xFF; buf[1] = 0xFF; buf[2] = 0xFF; buf[3] = 0xFF; buf[4] = 0xCC; buf[5] = 0xCC; buf[6] = 0xCC; buf[7] = 0xCC;
            DCHECK(MLLExBE<4>(mem) == int64_t(0xFFFFFFFFFFFFFFFF));
        }
    }

    void TEST_ENDIAN() {
        testEndian();
        testEndianMLBExLE();
        testEndianMLLExBE();
    }

}
}