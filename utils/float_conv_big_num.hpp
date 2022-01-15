// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_FLOAT_CONV_BIG_NUM_HPP_
#define UTILS_FLOAT_CONV_BIG_NUM_HPP_

#include "utils/int_conv.hpp"
#include "utils/float_conv_types.hpp"


namespace utl {
namespace internal {

    constexpr uint16_t u16_clut[]
    {
        //0, 1, 2,  3,    4,
        1, 10, 100, 1000, 1000'0
    };

    constexpr uint32_t u32_clut[]
    {
        //0, 1, 2,  3,    4,      5,       6,        7,
        1, 10, 100, 1000, 1000'0, 1000'00, 1000'000, 1000'0000,
        //8,         9,
        1000'0000'0, 1000'0000'00
    };

    constexpr uint64_t u64_clut[]
    {
        //0, 1, 2,  3,    4,      5,       6,        7,
        1, 10, 100, 1000, 1000'0, 1000'00, 1000'000, 1000'0000,
        //8,         9,            10,            11,             12,
        1000'0000'0, 1000'0000'00, 1000'0000'000, 1000'0000'0000, 1000'0000'0000'0,
        //13,              14,                 15,                  16,
        1000'0000'0000'00, 1000'0000'0000'000, 1000'0000'0000'0000, 1000'0000'0000'0000'0,
        //17,                   18,                      19
        1000'0000'0000'0000'00, 1000'0000'0000'0000'000, 1000'0000'0000'0000'0000,
    };

    constexpr uint8_t u64_dlut[]
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


    template <typename FTy>
    struct FloatTraits_base {
        static_assert(
            std::is_floating_point<FTy>::value,
            "FTy must be a floating type!");

        static constexpr size_t mant =
            std::numeric_limits<FTy>::digits - 1u;
        static constexpr size_t maxe =
            std::numeric_limits<FTy>::max_exponent;
    };


    template <typename FTy, size_t bN>
    struct FloatTraits {};

    template <typename FTy>
    struct FloatTraits<FTy, 10> {
        using Base = FloatTraits_base<FTy>;

        static constexpr size_t oful =
            std::numeric_limits<FTy>::max_exponent10 + 1u;
        static constexpr size_t mant = Base::mant;
        static constexpr size_t maxe = Base::maxe;
        static constexpr size_t mine_b = (mant + maxe - 2u) * 10u / 33u;
    };

    template <typename FTy>
    struct FloatTraits<FTy, 16> {
        using Base = FloatTraits_base<FTy>;

        static constexpr size_t oful =
            std::numeric_limits<FTy>::max_exponent / 4u;
        static constexpr size_t mant = Base::mant;
        static constexpr size_t maxe = Base::maxe;
        static constexpr size_t mine_b = (mant + maxe + 3u) / 4u;
    };


    template <typename UTy, size_t bN>
    struct UCeilTraits {};

    template <>
    struct UCeilTraits<uint16_t, 10> {
        static constexpr size_t udig = 3u;
        static constexpr uint16_t uceil = u16_clut[udig];

        static uint16_t bpow(uint_fast8_t e) {
            return u16_clut[e];
        }

        static size_t dcount(size_t bit_count) {
            return (bit_count + 2u) / 3u;
        }

        static size_t ucount(size_t bit_count) {
            return (dcount(bit_count) + udig - 1u) / udig;
        }
    };

    template <>
    struct UCeilTraits<uint32_t, 10> {
        static constexpr size_t udig = 8u;
        static constexpr uint32_t uceil = u32_clut[udig];

        static uint32_t bpow(uint_fast8_t e) {
            return u32_clut[e];
        }

        static size_t dcount(size_t bit_count) {
            return (bit_count + 2u) / 3u;
        }

        static size_t ucount(size_t bit_count) {
            return (dcount(bit_count) + udig - 1u) / udig;
        }
    };

    template <>
    struct UCeilTraits<uint64_t, 10> {
        static constexpr size_t udig = 18u;
        static constexpr uint64_t uceil = u64_clut[udig];

        static uint64_t bpow(uint_fast8_t e) {
            return u64_clut[e];
        }

        static size_t dcount(size_t bit_count) {
            return (bit_count + 2u) / 3u;
        }

        static size_t ucount(size_t bit_count) {
            return (dcount(bit_count) + udig - 1u) / udig;
        }
    };

    template <>
    struct UCeilTraits<uint16_t, 16> {
        static constexpr size_t udig = 2u;
        static constexpr uint16_t uceil = uint16_t(1u) << (udig * 4u);

        static uint16_t bpow(uint_fast8_t e) {
            return uint16_t(1u) << (e * 4u);
        }

        static size_t dcount(size_t bit_count) {
            return (bit_count + 3u) / 4u;
        }

        static size_t ucount(size_t bit_count) {
            return (dcount(bit_count) + udig - 1u) / udig;
        }
    };

    template <>
    struct UCeilTraits<uint32_t, 16> {
        static constexpr size_t udig = 6u;
        static constexpr uint32_t uceil = uint32_t(1u) << (udig * 4u);

        static uint32_t bpow(uint_fast8_t e) {
            return uint32_t(1u) << (e * 4u);
        }

        static size_t dcount(size_t bit_count) {
            return (bit_count + 3u) / 4u;
        }

        static size_t ucount(size_t bit_count) {
            return (dcount(bit_count) + udig - 1u) / udig;
        }
    };

    template <>
    struct UCeilTraits<uint64_t, 16> {
        static constexpr size_t udig = 14u;
        static constexpr uint64_t uceil = uint64_t(1u) << (udig * 4u);

        static uint64_t bpow(uint_fast8_t e) {
            return uint64_t(1u) << (e * 4u);
        }

        static size_t dcount(size_t bit_count) {
            return (bit_count + 3u) / 4u;
        }

        static size_t ucount(size_t bit_count) {
            return (dcount(bit_count) + udig - 1u) / udig;
        }
    };

    template <typename FTy, typename UTy, size_t bN, size_t Unit>
    class BigFloat_bN {
    public:
        using FTraits = FloatTraits<FTy, bN>;
        using UCTraits = UCeilTraits<UTy, bN>;
        static constexpr size_t uic = Unit;
        static constexpr size_t dig = UCTraits::udig;
        static constexpr UTy uceil = UCTraits::uceil;

        explicit BigFloat_bN(uint_fast8_t top) {
            this->top = top;
            std::memset(raw, 0, sizeof(UTy) * uic);
        }

        void add(const BigFloat_bN& rhs) {
            uint_fast8_t r = 0;
            for (size_t i = uic; i-- > 0;) {
                auto tmp = raw[i] + rhs.raw[i] + r;
                if (tmp >= uceil) {
                    r = 1;
                    raw[i] = tmp - uceil;
                } else {
                    r = 0;
                    raw[i] = tmp;
                }
            }
            top = r;
        }

        void mul2() {
            uint_fast8_t r = 0;
            for (size_t i = uic; i-- > 0;) {
                raw[i] *= 2;
                if (r) {
                    raw[i] += r;
                }
                r = uint_fast8_t(raw[i] / uceil);
                raw[i] -= uceil * r;
            }
            top = r;
        }

        void round(size_t d_pos, uint_fast8_t sign, int frm) {
            auto i = d_pos / dig;
            if (i >= uic)
                return;

            if (frm == FR_CEIL) {
                if (sign) {
                    return;
                }
                if (isZeroAfter(d_pos)) {
                    return;
                }
            } else if (frm == FR_FLOOR) {
                if (!sign) {
                    return;
                }
                if (isZeroAfter(d_pos)) {
                    return;
                }
            } else if (frm == FR_ZERO) {
                return;
            } else {
                auto d_1 = getDigit(d_pos + 1);
                if (d_1 < bN / 2) {
                    return;
                }

                if (d_1 == bN / 2) {
                    auto d = getDigit(d_pos);
                    if ((d % 2) == 0 && isZeroAfter(d_pos + 1)) {
                        return;
                    }
                }
            }

            // 走到这里就是进位
            auto tmp = raw[i] + UCTraits::bpow(dig - 1 - (d_pos % dig));
            if (tmp < uceil) {
                raw[i] = tmp;
                return;
            }

            uint_fast8_t r = 1;
            raw[i] = tmp - uceil;

            for (; i-- > 0;) {
                tmp = raw[i] + r;
                if (tmp >= uceil) {
                    r = 1;
                    raw[i] = tmp - uceil;
                } else {
                    raw[i] = tmp;
                    return;
                }
            }
            top = r;
        }

        void div2() {
            uint_fast8_t r = top & 0x1;
            top /= 2;

            for (size_t i = 0; i < uic; ++i) {
                if (r) {
                    raw[i] += uceil;
                }
                r = raw[i] & 0x1;
                raw[i] /= 2;
            }
        }

        void divd(uint_fast8_t d) {
            uint_fast8_t r = top % d;
            top /= d;

            for (size_t i = 0; i < uic; ++i) {
                if (r) {
                    raw[i] += uceil * r;
                }
                r = raw[i] % d;
                raw[i] /= d;
            }
            assert(r == 0);
        }

        uint_fast8_t getDigit(size_t d_pos) const {
            auto i = d_pos / dig;
            if (i >= uic)
                return 0;

            auto d = raw[i] / UCTraits::bpow(dig - 1 - (d_pos % dig)) % bN;
            return uint_fast8_t(d);
        }

        size_t getSignDigitPos() const {
            for (size_t i = 0; i < uic; ++i) {
                auto u = raw[i];
                if (u) {
                    for (uint_fast8_t j = dig; j-- > 0;) {
                        auto d = u / UCTraits::bpow(j) % bN;
                        if (d) {
                            return i * dig + (dig - j - 1);
                        }
                    }
                    assert(false);
                    break;
                }
            }
            return 0;
        }

        template <typename Cy>
        int fromChars(const Cy* s, size_t len, const Cy** n) {
            auto _s = s;
            auto _se = s + len;
            for (size_t i = 0; i < uic; ++i) {
                if (_s >= _se) {
                    break;
                }

                UTy result = 0;
                for (size_t j = 0; j < dig; ++j) {
                    if (_s < _se) {
                        auto c = ctoi(*_s);
                        if (c == uint_fast8_t(-1) || c >= bN) {
                            if (_s != s) {
                                *n = _s;
                                raw[i] = result * UCTraits::bpow(uint_fast8_t(dig - j));
                                return UCR_OK;
                            }
                            return UCR_FAILED;
                        }
                        result = result * bN + c;
                        ++_s;
                    } else {
                        result *= bN;
                    }
                }
                raw[i] = result;
            }

            assert(_s == _se);
            if (_s != s) {
                *n = _s;
                return UCR_OK;
            }
            return UCR_FAILED;
        }

        template <typename Cy>
        int fromChars(
            const Cy* s, size_t len,
            size_t d_pos, bool has_dp, const Cy** n)
        {
            auto _s = s;
            auto _se = s + len;
            auto j = d_pos % dig;
            for (size_t i = d_pos / dig; i < uic; ++i) {
                if (_s >= _se) {
                    break;
                }

                UTy result = 0;
                for (; j < dig;) {
                    if (_s < _se) {
                        if (has_dp && *_s == Cy('.')) {
                            ++_s;
                            has_dp = false;
                            continue;
                        }

                        auto c = ctoi(*_s);
                        if (c == uint_fast8_t(-1) || c >= bN) {
                            if (_s != s) {
                                *n = _s;
                                raw[i] = result * UCTraits::bpow(uint_fast8_t(dig - j));
                                return UCR_OK;
                            }
                            return UCR_FAILED;
                        }
                        result = result * bN + c;
                        ++_s;
                    } else {
                        result *= bN;
                    }
                    ++j;
                }
                j = 0;
                raw[i] = result;
            }

            assert(_s == _se);
            if (_s != s) {
                *n = _s;
                return UCR_OK;
            }
            return UCR_FAILED;
        }

        template <bool Upp, typename Cy>
        void toChars(Cy* s, size_t* len) const {
            size_t ep = 0;
            for (size_t i = 0; i < uic; ++i) {
                if (raw[i]) {
                    ep = i;
                }
            }

            auto _s = s;
            size_t _len = 0;
            for (size_t i = 0; i < uic; ++i) {
                auto ss = _s + dig;
                auto val = raw[i];

                for (size_t j = 0; j < dig; ++j) {
                    --ss;
                    auto v = uint_fast8_t(val % bN);
                    if constexpr (Upp) {
                        *ss = itocu<Cy>(v);
                    } else {
                        *ss = itocl<Cy>(v);
                    }
                    val /= bN;
                }

                _s += dig;
                _len += dig;
                if (i >= ep) {
                    break;
                }
            }

            *len = _len;
        }

        template <bool Upp, typename Cy>
        void toChars(Cy* s, size_t* len, size_t d_pos) const {
            size_t ep = 0;
            for (size_t i = 0; i < uic; ++i) {
                if (raw[i]) {
                    ep = i;
                }
            }

            auto _s = s;
            size_t _len = 0;
            uint_fast8_t j = d_pos % dig;
            for (size_t i = d_pos / dig; i < uic; ++i) {
                auto ul = *len - _len;
                size_t ude;
                if (ul > dig - j) {
                    ude = dig;
                    ul = dig - j;
                } else {
                    ude = j + ul;
                }

                auto ss = _s + ul;
                auto val = raw[i];
                val /= UCTraits::bpow(uint_fast8_t(dig - ude));

                auto j0 = j;
                for (; j < dig && _len < *len; ++j) {
                    --ss;
                    auto v = uint_fast8_t(val % bN);
                    if constexpr (Upp) {
                        *ss = itocu<Cy>(v);
                    } else {
                        *ss = itocl<Cy>(v);
                    }
                    val /= bN;
                    ++_len;
                }

                _s += dig - j0;
                if (i >= ep) {
                    break;
                }
                j = 0;
            }

            *len = _len;
        }

        void setTop(uint_fast8_t top) {
            this->top = top;
        }

        uint_fast8_t getTop() const {
            return top;
        }

        bool isZero() const {
            if (top) return false;
            for (size_t i = 0; i < uic; ++i) {
                if (raw[i]) {
                    return false;
                }
            }
            return true;
        }

        bool isZeroAfter(size_t pos) const {
            auto i = pos / dig;
            if (i >= uic)
                return true;

            auto r = raw[i] % UCTraits::bpow(dig - 1 - (pos % dig));
            if (r) {
                return false;
            }

            ++i;
            for (; i < uic; ++i) {
                if (raw[i]) {
                    return false;
                }
            }
            return true;
        }

        uint_fast8_t top;
        UTy raw[uic];
    };

    template <typename FTy, typename UTy, size_t bN, size_t Unit>
    class BigUInt_bN {
    public:
        using FTraits = FloatTraits<FTy, bN>;
        using UCTraits = UCeilTraits<UTy, bN>;
        static constexpr size_t uic = Unit;
        static constexpr size_t dig = UCTraits::udig;
        static constexpr UTy uceil = UCTraits::uceil;

        explicit BigUInt_bN(uint_fast8_t bottom) {
            this->bottom = bottom;
            std::memset(raw, 0, sizeof(UTy) * uic);
        }

        void add(uint_fast8_t val) {
            if (val == 0) {
                return;
            }

            auto tmp = raw[uic - 1] + val;
            if (tmp < uceil) {
                raw[uic - 1] = tmp;
                return;
            }

            uint_fast8_t r = 1;
            raw[uic - 1] = tmp - uceil;

            for (size_t i = uic - 1; i-- > 0;) {
                tmp = raw[i] + r;
                if (tmp >= uceil) {
                    r = 1;
                    raw[i] = tmp - uceil;
                } else {
                    raw[i] = tmp;
                    return;
                }
            }
            assert(r == 0);
        }

        void add(const BigUInt_bN& rhs) {
            uint_fast8_t r = 0;
            for (size_t i = uic; i-- > 0;) {
                auto tmp = raw[i] + rhs.raw[i] + r;
                if (tmp >= uceil) {
                    r = 1;
                    raw[i] = tmp - uceil;
                } else {
                    r = 0;
                    raw[i] = tmp;
                }
            }
            assert(r == 0);
        }

        void mul2() {
            UTy r;
            bottom *= 2;
            r = bottom / 10;
            bottom -= uint_fast8_t(10 * r);

            for (size_t i = uic; i-- > 0;) {
                raw[i] *= 2;
                if (r) {
                    raw[i] += r;
                }
                r = raw[i] / uceil;
                raw[i] -= uceil * r;
            }
            assert(r == 0);
        }

        void muld(uint_fast8_t d) {
            UTy r;
            bottom *= d;
            r = bottom / 10;
            bottom -= uint_fast8_t(10 * r);

            for (size_t i = uic; i-- > 0;) {
                raw[i] = raw[i] * d;
                if (r) {
                    raw[i] += r;
                }
                r = raw[i] / uceil;
                raw[i] -= uceil * r;
            }
            assert(r == 0);
        }

        void div2() {
            uint_fast8_t r = 0;
            for (size_t i = 0; i < uic; ++i) {
                if (r) {
                    raw[i] += uceil;
                }
                r = raw[i] & 0x1;
                raw[i] /= 2;
            }
            bottom = r;
        }

        void round(size_t d_pos, uint_fast8_t sign, bool f_zero, int frm) {
            if (d_pos == 0) {
                return;
            }

            auto i = d_pos / dig;
            if (i >= uic)
                return;
            i = uic - i - 1;

            if (frm == FR_CEIL) {
                if (sign) {
                    return;
                }
                if (isZeroBefore(d_pos)) {
                    return;
                }
            } else if (frm == FR_FLOOR) {
                if (!sign) {
                    return;
                }
                if (isZeroBefore(d_pos)) {
                    return;
                }
            } else if (frm == FR_ZERO) {
                return;
            } else {
                auto d_1 = getDigit(d_pos - 1);
                if (d_1 < bN / 2) {
                    return;
                }

                if (d_1 == bN / 2) {
                    auto d = getDigit(d_pos);
                    if ((d % 2) == 0 && isZeroBefore(d_pos - 1) && f_zero) {
                        return;
                    }
                }
            }

            // 走到这里就是进位
            auto tmp = raw[i] + UCTraits::bpow(d_pos % dig);
            if (tmp < uceil) {
                raw[i] = tmp;
                return;
            }

            uint_fast8_t r = 1;
            raw[i] = tmp - uceil;

            for (; i-- > 0;) {
                tmp = raw[i] + r;
                if (tmp >= uceil) {
                    r = 1;
                    raw[i] = tmp - uceil;
                } else {
                    raw[i] = tmp;
                    return;
                }
            }
            assert(r == 0);
        }

        void moveUR() {
            size_t i;
            UTy mr = 0u;
            for (i = uic; i-- > 0;) {
                mr = raw[i];
                if (mr) {
                    break;
                }
            }

            if (!mr) {
                return;
            }

            size_t diff = uic - 1u - i;
            if (!diff) {
                return;
            }

            size_t j = uic;
            for (; j-- > diff;) {
                raw[j] = raw[j - diff];
            }
            for (++j; j-- > 0u;) {
                raw[j] = 0u;
            }
        }

        void moveDR() {
            auto mr = raw[uic - 1];
            if (!mr) {
                return;
            }

            size_t i = 0;
            for (; i < dig; ++i) {
                auto t = mr % bN;
                if (t) {
                    break;
                }
                mr /= bN;
            }

            if (!i) {
                return;
            }

            auto bo = UCTraits::bpow(uint_fast8_t(i));
            auto bo_1 = UCTraits::bpow(uint_fast8_t(dig - i));
            for (size_t j = uic; j-- > 1;) {
                raw[j] /= bo;
                raw[j] += (raw[j - 1] % bo) * bo_1;
            }
            raw[0] /= bo;
        }

        void movdr(size_t d_len) {
            auto ul = d_len / dig;
            auto dl = d_len % dig;

            if (ul) {
                size_t j = uic;
                for (; j-- > ul;) {
                    raw[j] = raw[j - ul];
                }
                for (++j; j-- > 0u;) {
                    raw[j] = 0u;
                }
            }

            if (dl) {
                auto bo = UCTraits::bpow(uint_fast8_t(dl));
                auto bo_1 = UCTraits::bpow(uint_fast8_t(dig - dl));
                for (size_t j = uic; j-- > 1;) {
                    raw[j] /= bo;
                    raw[j] += (raw[j - 1] % bo) * bo_1;
                }
                raw[0] /= bo;
            }
        }

        uint_fast8_t getDigit(size_t d_pos) const {
            auto i = d_pos / dig;
            if (i >= uic)
                return 0;
            i = uic - i - 1;

            auto d = raw[i] / UCTraits::bpow(d_pos % dig) % bN;
            return uint_fast8_t(d);
        }

        size_t getSignDigitPos() const {
            for (size_t i = 0; i < uic; ++i) {
                auto u = raw[i];
                if (u) {
                    for (uint_fast8_t j = dig; j-- > 0;) {
                        auto d = u / UCTraits::bpow(j) % bN;
                        if (d) {
                            return (uic - 1 - i) * dig + j;
                        }
                    }
                    assert(false);
                    break;
                }
            }
            return 0u;
        }

        template <typename Cy>
        int fromChars(const Cy* s, size_t len, const Cy** n) {
            if (!len) {
                return UCR_FAILED;
            }

            auto su = (len + dig - 1) / dig;
            if (su > uic) {
                su = uic;
            }

            auto _s = s;
            auto _se = s + len;
            auto sd = dig - 1 - (len - 1) % dig;
            for (size_t i = uic - su; i < uic; ++i) {
                if (_s >= _se) {
                    break;
                }

                UTy result = 0;
                for (size_t j = sd; j < dig; ++j) {
                    auto c = ctoi(*_s);
                    if (c == uint_fast8_t(-1) || c >= bN) {
                        if (_s != s) {
                            *n = _s;
                            raw[i] = result * UCTraits::bpow(uint_fast8_t(dig - j));
                            movdr(len - (_s - s));
                            return UCR_OK;
                        }
                        return UCR_FAILED;
                    }
                    result = result * bN + c;
                    ++_s;

                    // 此时必定溢出
                    if (_s - s > FTraits::oful) {
                        *n = s;
                        return UCR_OVERFLOWED;
                    }
                }
                raw[i] = result;
                sd = 0;
            }

            assert(_s == _se);
            if (_s != s) {
                *n = _s;
                return UCR_OK;
            }
            return UCR_FAILED;
        }

        template <typename Cy>
        int fromChars(
            const Cy* s, size_t len,
            size_t d_pos, bool has_dp, const Cy** n)
        {
            auto actual_len = len + d_pos - (has_dp ? 1 : 0);
            if (!actual_len) {
                return UCR_FAILED;
            }

            auto su = (actual_len + dig - 1) / dig;
            if (su > uic) {
                su = uic;
            }

            size_t cd = d_pos;
            auto _s = s;
            auto _se = s + len;
            auto sd = (actual_len - 1) % dig + 1;
            for (size_t i = uic - su; i < uic; ++i) {
                if (_s >= _se) {
                    break;
                }

                UTy result = 0;
                auto tdig = std::min<size_t>(_se - _s, sd);
                for (size_t j = 0; j < tdig;) {
                    if (_s < _se) {
                        if (has_dp && *_s == Cy('.')) {
                            ++_s;
                            has_dp = false;
                            continue;
                        }

                        auto c = ctoi(*_s);
                        if (c == uint_fast8_t(-1) || c >= bN) {
                            if (_s != s) {
                                *n = _s;
                                raw[i] = result * UCTraits::bpow(uint_fast8_t(sd - j));
                                movdr(actual_len - cd);
                                return UCR_OK;
                            }
                            return UCR_FAILED;
                        }
                        result = result * bN + c;
                        ++_s;
                    } else {
                        result *= bN;
                    }
                    ++cd;
                    ++j;

                    // 此时必定溢出
                    if (cd > FTraits::oful) {
                        return UCR_OVERFLOWED;
                    }
                }
                raw[i] = result * UCTraits::bpow(uint_fast8_t(sd - tdig));
                sd = dig;
            }

            if (has_dp && _s < _se && *_s == Cy('.')) {
                ++_s;
            }

            assert(_s == _se);
            if (_s != s) {
                *n = _s;
                return UCR_OK;
            }
            return UCR_FAILED;
        }

        template <bool Upp, typename Cy>
        void toChars(Cy* s, size_t* len) const {
            size_t ep = uic - 1;
            for (size_t i = uic; i-- > 0;) {
                if (raw[i]) {
                    ep = i;
                }
            }

            auto _s = s + (uic - ep) * dig;
            size_t _len = 0;
            for (size_t i = uic; i-- > 0;) {
                auto val = raw[i];

                for (size_t j = 0; j < dig; ++j) {
                    --_s;
                    auto v = uint_fast8_t(val % bN);
                    if constexpr (Upp) {
                        *_s = itocu<Cy>(v);
                    } else {
                        *_s = itocl<Cy>(v);
                    }
                    val /= bN;
                }

                _len += dig;
                if (i <= ep) {
                    break;
                }
            }

            *len = _len;
        }

        template <bool Upp, typename Cy>
        void toChars(Cy* s, size_t* len, size_t d_pos) const {
            auto sud = d_pos / dig;
            if (sud >= uic) {
                *len = 0;
                return;
            }

            size_t ep = uic - 1;
            for (size_t i = uic; i-- > 0;) {
                if (raw[i]) {
                    ep = i;
                }
            }

            auto tl = (uic - ep) * dig;
            if (tl > *len) {
                tl = *len;
            }

            auto _s = s + tl;
            size_t _len = 0;
            uint_fast8_t j = d_pos % dig;
            for (size_t i = uic - sud; i-- > 0;) {
                auto val = raw[i];
                val /= UCTraits::bpow(j);

                for (; j < dig && _len < *len; ++j) {
                    --_s;
                    auto v = uint_fast8_t(val % bN);
                    if constexpr (Upp) {
                        *_s = itocu<Cy>(v);
                    } else {
                        *_s = itocl<Cy>(v);
                    }
                    val /= bN;
                    ++_len;
                }

                if (i <= ep) {
                    break;
                }
                j = 0;
            }

            *len = _len;
        }

        void setBottom(uint_fast8_t bottom) {
            this->bottom = bottom;
        }

        uint_fast8_t getBottom() const {
            return bottom;
        }

        bool isZero() const {
            if (bottom) return false;
            for (size_t i = 0; i < uic; ++i) {
                if (raw[i]) {
                    return false;
                }
            }
            return true;
        }

        bool isZeroBefore(size_t pos) const {
            if (pos == 0) {
                return true;
            }

            auto i = pos / dig;
            if (i >= uic)
                return true;
            i = uic - i - 1;

            auto r = raw[i] % UCTraits::bpow(pos % dig);
            if (r) {
                return false;
            }

            if (i == 0) {
                return true;
            }

            for (; i-- > uic;) {
                if (raw[i]) {
                    return false;
                }
            }
            return true;
        }

        UTy raw[uic];
        uint_fast8_t bottom;
    };

}
}

#endif  // UTILS_FLOAT_CONV_BIG_NUM_HPP_