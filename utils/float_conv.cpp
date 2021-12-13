// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "float_conv.h"

#include <cassert>
#include <charconv>


namespace {

    uint16_t u16_clut[]
    {
        //0, 1, 2,  3,    4,
        1, 10, 100, 1000, 10000
    };

    uint32_t u32_clut[]
    {
        //0, 1, 2,  3,    4,     5,      6,       7,
        1, 10, 100, 1000, 10000, 100000, 1000000, 10000000,
        //8,       9,
        100000000, 1000000000
    };

    uint64_t u64_clut[]
    {
        //0, 1, 2,  3,    4,     5,      6,       7,
        1, 10, 100, 1000, 10000, 100000, 1000000, 10000000,
        //8,       9,          10,          11,           12,
        100000000, 1000000000, 10000000000, 100000000000, 1000000000000,
        //13,           14,              15,               16,
        10000000000000, 100000000000000, 1000000000000000, 10000000000000000,
        //17,               18,                  19
        100000000000000000, 1000000000000000000, 10000000000000000000,
    };

    uint8_t u64_dlut[]
    {
         1,  1,  1,  1,  2,  2,  2,  3,
         3,  3,  4,  4,  4,  4,  5,  5,
         5,  6,  6,  6,  7,  7,  7,  7,
         8,  8,  8,  9,  9,  9, 10, 10,
        10, 10, 11, 11, 11, 12, 12, 12,
        13, 13, 13, 13, 14, 14, 14, 15,
        15, 15, 16, 16, 16, 16, 17, 17,
        17, 18, 18, 18, 19, 19, 19, 19
    };

    class BigFloat {
    public:
        static const size_t uic = 10;
        static const size_t dig = 18;

        explicit BigFloat(uint8_t top) {
            this->top = top;
            std::memset(raw, 0, sizeof(uint64_t) * uic);
        }

        void add(const BigFloat& rhs) {
            uint8_t r = 0;
            for (size_t i = uic; i-- > 0;) {
                auto tmp = raw[i] + rhs.raw[i] + r;
                if (tmp >= u64_clut[dig]) {
                    r = 1;
                    raw[i] = tmp - u64_clut[dig];
                } else {
                    r = 0;
                    raw[i] = tmp;
                }
            }
            top = r;
        }

        void div2() {
            uint8_t r = top & 0x1;
            top /= 2;

            for (size_t i = 0; i < uic; ++i) {
                if (r) {
                    raw[i] += u64_clut[dig];
                }
                r = raw[i] & 0x1;
                raw[i] /= 2;
            }
            assert(r == 0);
        }

        void divd(uint8_t d) {
            uint8_t r = top % d;
            top /= d;

            for (size_t i = 0; i < uic; ++i) {
                if (r) {
                    raw[i] += u64_clut[dig] * r;
                }
                r = raw[i] % d;
                raw[i] /= d;
            }
            assert(r == 0);
        }

    private:
        static int8_t getHPos(uint64_t v) {
            for (int8_t i = 64; i-- > 0;) {
                if (v & (uint64_t(1) << i)) {
                    return i;
                }
            }
            return -1;
        }

        uint8_t top;
        uint64_t raw[uic];
    };

}

namespace utl {

    void testBigFloat() {
        BigFloat bf1(1);
        bf1.divd(8);

        BigFloat bf2(1);
        bf2.div2();
        bf2.div2();
        bf2.div2();

        char buf[64]{ 0 };
        std::to_chars(
            std::begin(buf), std::end(buf),
            1.0 / (uint64_t(1) << 52),
            std::chars_format::fixed, 63);

        BigFloat bf(1);
        for (size_t i = 0; i < 52 / 3; ++i) {
            bf.divd(8);
        }
        for (size_t i = 0; i < 52 % 3; ++i) {
            bf.div2();
        }

        return;
    }

    void floatToString(double val, std::string* out) {
        uint64_t uv;
        std::memcpy(&uv, &val, 8);

        uint64_t fra = uv & 0xFFFFFFFFFFFFF;
        uint16_t exp_shift = (uv >> 52) & 0x7FF;
        int16_t exp = exp_shift - 1023;
        uint8_t sign = uv >> 63;

        if (fra == 0 && exp_shift == 0) {
            *out = "0";
            return;
        }
        if (fra == 0 && exp_shift == 0x7FF) {
            *out = "INF";
            return;
        }
        if (fra != 0 && exp_shift == 0x7FF) {
            *out = "NaN";
            return;
        }
        if (exp_shift != 0) {
            BigFloat bf(1);
            BigFloat res(0);
            for (size_t i = 52 - exp; i-- > 0;) {
                bf.div2();
                if (i > 51 && i != 52) {
                    continue;
                }
                if (i == 52 || (fra & (uint64_t(1) << i))) {
                    res.add(bf);
                }
            }
        }
    }

}