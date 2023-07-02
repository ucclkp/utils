// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STRINGS_FLOAT_CONV_BIG_NUM_HPP_
#define UTILS_STRINGS_FLOAT_CONV_BIG_NUM_HPP_

#include "utils/strings/int_conv.hpp"
#include "utils/strings/float_conv_types.hpp"


namespace utl {
namespace internal {

    constexpr uint16_t u16_clut[]
    {
        //0, 1,  2,    3,     4,
        1u, 10u, 100u, 1000u, 1000'0u
    };

    constexpr uint32_t u32_clut[]
    {
        //0, 1,  2,    3,     4,       5,        6,         7,
        1u, 10u, 100u, 1000u, 1000'0u, 1000'00u, 1000'000u, 1000'0000u,
        //8,          9,
        1000'0000'0u, 1000'0000'00u
    };

    constexpr uint64_t u64_clut[]
    {
        //0, 1,  2,    3,     4,       5,        6,         7,
        1u, 10u, 100u, 1000u, 1000'0u, 1000'00u, 1000'000u, 1000'0000u,
        //8,          9,             10,             11,              12,
        1000'0000'0u, 1000'0000'00u, 1000'0000'000u, 1000'0000'0000u, 1000'0000'0000'0u,
        //13,               14,                  15,                   16,
        1000'0000'0000'00u, 1000'0000'0000'000u, 1000'0000'0000'0000u, 1000'0000'0000'0000'0u,
        //17,                    18,                       19
        1000'0000'0000'0000'00u, 1000'0000'0000'0000'000u, 1000'0000'0000'0000'0000u,
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
        static constexpr int mine =
            std::numeric_limits<FTy>::min_exponent - 1;
        static constexpr bool has_denorm = !FORCE_NO_DENORM &&
            std::numeric_limits<FTy>::has_denorm == std::denorm_present;

        static constexpr bool hb_present() {
            auto total = mant + bexp() + 1u;
            return total == 79u;
        }

        static constexpr size_t mine_c() {
            return mant + maxe - 2u;
        }

        static constexpr size_t bexp() {
            size_t r = 0;
            auto max_exp = maxe;
            while (max_exp) {
                max_exp /= 2;
                ++r;
            }
            return r;
        }

        static constexpr size_t sexp() {
            if (hb_present()) {
                return mant + 1u;
            }
            return mant;
        }

        static constexpr size_t ssign() {
            return sexp() + bexp();
        }
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

        static constexpr size_t mine_adc() {
            if constexpr (Base::has_denorm) {
                return (mant + maxe - 2u) * 10u / 33u;
            } else {
                return (maxe - 1u) * 10u / 33u;
            }
        }

    };

    template <typename FTy>
    struct FloatTraits<FTy, 16> {
        using Base = FloatTraits_base<FTy>;

        static constexpr size_t oful =
            std::numeric_limits<FTy>::max_exponent / 4u;
        static constexpr size_t mant = Base::mant;
        static constexpr size_t maxe = Base::maxe;

        static constexpr size_t mine_adc() {
            if constexpr (Base::has_denorm) {
                return (mant + maxe + 3u) / 4u;
            } else {
                return (maxe + 3u) / 4u;
            }
        }

    };


    /**
     * 单元格数据类型的属性。
     * UTy 表示单元格的数据类型；
     * bN 表示单元格中的数位基于几进制进行分割，即实际基数。
     *
     * 成员：
     * udig 表示单元格在与进制中的一位数进行运算时，能确保不溢出的最大可盛放数位数；
     * uceil 表示单元格中可盛放的的最大数 + 1，超过该数就要进位，也叫单元格基数；
     * bpow(e) 计算 bN^e ；
     * dcount(bc) 保守估计一个具有 bc 个二进制位的数需要几个数位来容纳；
     * ucount(bc) 保守估计一个具有 bc 个二进制位的数需要几个单元格来容纳。
     */
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

    /**
     * 模拟较长的小数位。
     * FTy 表示关联的浮点数类型；
     * UTy 表示单元格的数据类型；
     * bN 表示单元格中的数位基于几进制进行分割；
     * Unit 表示单元格的总数。
     */
    template <typename FTy, typename UTy, size_t bN, size_t Unit>
    class BigFloat_bN {
    public:
        using FTraits = FloatTraits<FTy, bN>;
        using UCTraits = UCeilTraits<UTy, bN>;
        static constexpr size_t uic = Unit;
        static constexpr size_t dig = UCTraits::udig;
        static constexpr UTy uceil = UCTraits::uceil;

        explicit BigFloat_bN(uint_fast8_t top)
            : top(top),
              effect(0) {}

        void add(const BigFloat_bN& rhs) {
            uint_fast8_t r = 0;
            size_t i = rhs.effect;
            for (; i-- > 0;) {
                if (i >= effect) {
                    raw[i] = 0;
                }
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

            if (rhs.effect > effect) {
                effect = rhs.effect;
            } else if (effect == rhs.effect) {
                for (i = effect; i-- > 0;) {
                    if (!raw[i]) {
                        --effect;
                    } else {
                        break;
                    }
                }
            }
        }

        void mul2() {
            uint_fast8_t r = 0;
            for (size_t i = effect; i-- > 0;) {
                raw[i] *= 2;
                if (r) {
                    raw[i] += r;
                }
                r = uint_fast8_t(raw[i] / uceil);
                raw[i] -= uceil * r;
            }
            top = r;

            if (effect && !raw[effect - 1]) {
                --effect;
            }
        }

        void round(size_t d_pos, uint_fast8_t sign, int frm) {
            auto i = d_pos / dig;
            if (i >= effect)
                return;

            if (frm == FCR_CEIL) {
                if (sign) {
                    return;
                }
                if (isZeroAfter(d_pos)) {
                    return;
                }
            } else if (frm == FCR_FLOOR) {
                if (!sign) {
                    return;
                }
                if (isZeroAfter(d_pos)) {
                    return;
                }
            } else if (frm == FCR_ZERO) {
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

            for (size_t i = 0; i < effect; ++i) {
                if (r) {
                    raw[i] += uceil;
                }
                r = raw[i] & 0x1;
                raw[i] /= 2;
            }

            if (r && effect < uic) {
                raw[effect] = uceil / 2;
                ++effect;
            }
        }

        void divd(uint_fast8_t d) {
            uint_fast8_t r = top % d;
            top /= d;

            for (size_t i = 0; i < effect; ++i) {
                if (r) {
                    raw[i] += uceil * r;
                }
                r = raw[i] % d;
                raw[i] /= d;
            }

            if (r && effect < uic) {
                raw[effect] = uceil * r;
                r = raw[effect] % d;
                raw[effect] /= d;
                ++effect;
            }
            assert(r == 0);
        }

        uint_fast8_t getDigit(size_t d_pos) const {
            auto i = d_pos / dig;
            if (i >= effect)
                return 0;

            auto d = raw[i] / UCTraits::bpow(dig - 1 - (d_pos % dig)) % bN;
            return uint_fast8_t(d);
        }

        size_t getSignDigitPos() const {
            for (size_t i = 0; i < effect; ++i) {
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
                        if (c >= bN) {
                            if (_s != s) {
                                *n = _s;
                                raw[i] = result * UCTraits::bpow(uint_fast8_t(dig - j));
                                effect = i + 1;
                                return SCR_OK;
                            }
                            return SCR_FAIL;
                        }
                        result = result * bN + c;
                        ++_s;
                    } else {
                        result *= bN;
                    }
                }
                raw[i] = result;
                effect = i + 1;
            }

            assert(_s == _se);
            if (_s != s) {
                *n = _s;
                return SCR_OK;
            }
            return SCR_FAIL;
        }

        template <typename Cy>
        int fromChars(
            const Cy* s, size_t len,
            size_t d_pos, bool has_dp, const Cy** n)
        {
            auto _s = s;
            auto _se = s + len;
            auto j = d_pos % dig;

            size_t i;
            for (i = 0; i < d_pos / dig; ++i) {
                raw[i] = 0;
            }

            for (; i < uic; ++i) {
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
                        if (c >= bN) {
                            if (_s != s) {
                                *n = _s;
                                raw[i] = result * UCTraits::bpow(uint_fast8_t(dig - j));
                                effect = i + 1;
                                return SCR_OK;
                            }
                            return SCR_FAIL;
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
                effect = i + 1;
            }

            assert(_s == _se);
            if (_s != s) {
                *n = _s;
                return SCR_OK;
            }
            return SCR_FAIL;
        }

        template <bool Upp, typename Cy>
        void toChars(Cy* s, size_t* len) const {
            size_t ep = 0;
            for (size_t i = 0; i < effect; ++i) {
                if (raw[i]) {
                    ep = i;
                }
            }

            auto _s = s;
            size_t _len = 0;
            for (size_t i = 0; i < effect; ++i) {
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
            for (size_t i = 0; i < effect; ++i) {
                if (raw[i]) {
                    ep = i;
                }
            }

            auto _s = s;
            size_t _len = 0;
            uint_fast8_t j = d_pos % dig;
            for (size_t i = d_pos / dig; i < effect; ++i) {
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
            for (size_t i = 0; i < effect; ++i) {
                if (raw[i]) {
                    return false;
                }
            }
            return true;
        }

        bool isZeroAfter(size_t pos) const {
            auto i = pos / dig;
            if (i >= effect)
                return true;

            auto r = raw[i] % UCTraits::bpow(dig - 1 - (pos % dig));
            if (r) {
                return false;
            }

            ++i;
            for (; i < effect; ++i) {
                if (raw[i]) {
                    return false;
                }
            }
            return true;
        }

        uint_fast8_t top;
        UTy raw[uic];
        size_t effect;
    };

    template <typename FTy, typename UTy, size_t bN, size_t Unit>
    class BigUInt_bN {
    public:
        using FTraits = FloatTraits<FTy, bN>;
        using UCTraits = UCeilTraits<UTy, bN>;
        static constexpr size_t uic = Unit;
        static constexpr size_t dig = UCTraits::udig;
        static constexpr UTy uceil = UCTraits::uceil;

        explicit BigUInt_bN(uint_fast8_t bottom)
            : bottom(bottom),
              effect(0) {}

        void add(uint_fast8_t val) {
            if (val == 0) {
                return;
            }

            uint_fast8_t r = val;
            for (size_t i = uic; i-- > uic - effect;) {
                auto tmp = raw[i] + r;
                if (tmp >= uceil) {
                    r = 1;
                    raw[i] = tmp - uceil;
                } else {
                    raw[i] = tmp;
                    return;
                }
            }

            if (r && effect < uic) {
                raw[uic - effect - 1] = r;
                r = 0;
                ++effect;
            }

            assert(r == 0);
        }

        void add(const BigUInt_bN& rhs) {
            uint_fast8_t r = 0;

            size_t max_u, min_u;
            const UTy* max_raw;
            if (effect >= rhs.effect) {
                max_u = effect;
                max_raw = raw;
                min_u = rhs.effect;
            } else {
                max_u = rhs.effect;
                max_raw = rhs.raw;
                min_u = effect;
                for (size_t i = min_u; i < max_u; ++i) {
                    raw[uic - i - 1] = 0;
                }
            }

            size_t i = uic;
            for (; i-- > uic - min_u;) {
                auto tmp = raw[i] + rhs.raw[i] + r;
                if (tmp >= uceil) {
                    r = 1;
                    raw[i] = tmp - uceil;
                } else {
                    r = 0;
                    raw[i] = tmp;
                }
            }

            for (++i; i-- > uic - max_u;) {
                auto tmp = max_raw[i] + r;
                if (tmp >= uceil) {
                    r = 1;
                    raw[i] = tmp - uceil;
                } else {
                    r = 0;
                    raw[i] = tmp;
                }
            }

            if (rhs.effect > effect) {
                effect = rhs.effect;
            }
            if (r && effect < uic) {
                raw[uic - effect - 1] = r;
                r = 0;
                ++effect;
            }

            assert(r == 0);
        }

        void mul2() {
            UTy r;
            bottom *= 2;
            r = bottom / 10;
            bottom -= uint_fast8_t(10 * r);

            for (size_t i = uic; i-- > uic - effect;) {
                raw[i] *= 2;
                if (r) {
                    raw[i] += r;
                }
                r = raw[i] / uceil;
                raw[i] -= uceil * r;
            }

            if (r && effect < uic) {
                raw[uic - effect - 1] = r;
                r = 0;
                ++effect;
            }

            assert(r == 0);
        }

        void muld(uint_fast8_t d) {
            UTy r;
            bottom *= d;
            r = bottom / 10;
            bottom -= uint_fast8_t(10 * r);

            for (size_t i = uic; i-- > uic - effect;) {
                raw[i] = raw[i] * d;
                if (r) {
                    raw[i] += r;
                }
                r = raw[i] / uceil;
                raw[i] -= uceil * r;
            }

            if (r && effect < uic) {
                raw[uic - effect - 1] = r;
                r = 0;
                ++effect;
            }

            assert(r == 0);
        }

        void div2() {
            uint_fast8_t r = 0;
            for (size_t i = uic - effect; i < uic; ++i) {
                if (r) {
                    raw[i] += uceil;
                }
                r = raw[i] & 0x1;
                raw[i] /= 2;
            }
            bottom = r;

            if (effect && !raw[uic - effect]) {
                --effect;
            }
        }

        void round(size_t d_pos, uint_fast8_t sign, bool f_zero, int frm) {
            if (d_pos == 0) {
                return;
            }

            auto i = d_pos / dig;
            if (i >= effect)
                return;
            i = uic - i - 1;

            if (frm == FCR_CEIL) {
                if (sign) {
                    return;
                }
                if (isZeroBefore(d_pos)) {
                    return;
                }
            } else if (frm == FCR_FLOOR) {
                if (!sign) {
                    return;
                }
                if (isZeroBefore(d_pos)) {
                    return;
                }
            } else if (frm == FCR_ZERO) {
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

            auto tar = uic - effect;
            for (; i-- > tar;) {
                tmp = raw[i] + r;
                if (tmp >= uceil) {
                    r = 1;
                    raw[i] = tmp - uceil;
                } else {
                    raw[i] = tmp;
                    return;
                }
            }

            if (r == 0) {
                return;
            }

            assert(tar != 0);
            if (tar > 0) {
                raw[i] = r;
                ++effect;
            }
        }

        void moveUR() {
            size_t i;
            UTy mr = 0u;
            for (i = uic; i-- > uic - effect;) {
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
            size_t target = uic - effect + diff;
            for (; j-- > target;) {
                raw[j] = raw[j - diff];
            }
            ++j;
            if (j >= uic - effect) {
                effect = j - uic - effect;
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
            for (size_t j = uic; j-- > 1 + uic - effect;) {
                raw[j] /= bo;
                raw[j] += (raw[j - 1] % bo) * bo_1;
            }
            if (effect) {
                raw[uic - effect] /= bo;
                if (!raw[uic - effect]) {
                    --effect;
                }
            }
        }

        void movdr(size_t d_len) {
            auto ul = d_len / dig;
            auto dl = d_len % dig;

            if (ul) {
                size_t j = uic;
                size_t target = uic - effect + ul;
                for (; j-- > target;) {
                    raw[j] = raw[j - ul];
                }
                ++j;
                if (j >= uic - effect) {
                    effect = j - uic - effect;
                }
            }

            if (dl) {
                auto bo = UCTraits::bpow(uint_fast8_t(dl));
                auto bo_1 = UCTraits::bpow(uint_fast8_t(dig - dl));
                for (size_t j = uic; j-- > 1 + uic - effect;) {
                    raw[j] /= bo;
                    raw[j] += (raw[j - 1] % bo) * bo_1;
                }
                if (effect) {
                    raw[uic - effect] /= bo;
                    if (!raw[uic - effect]) {
                        --effect;
                    }
                }
            }
        }

        uint_fast8_t getDigit(size_t d_pos) const {
            auto i = d_pos / dig;
            if (i >= effect)
                return 0;
            i = uic - i - 1;

            auto d = raw[i] / UCTraits::bpow(d_pos % dig) % bN;
            return uint_fast8_t(d);
        }

        size_t getSignDigitPos() const {
            for (size_t i = uic - effect; i < uic; ++i) {
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
                return SCR_FAIL;
            }

            auto su = (len + dig - 1) / dig;
            if (su > uic) {
                su = uic;
            }

            effect = su;
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
                    if (c >= bN) {
                        if (_s != s) {
                            *n = _s;
                            raw[i] = result * UCTraits::bpow(uint_fast8_t(dig - j));
                            movdr(len - (_s - s));
                            return SCR_OK;
                        }
                        return SCR_FAIL;
                    }
                    result = result * bN + c;
                    ++_s;

                    // 此时必定溢出
                    if (_s - s > FTraits::oful) {
                        *n = s;
                        return SCR_OF;
                    }
                }
                raw[i] = result;
                sd = 0;
            }

            assert(_s == _se);
            if (_s != s) {
                *n = _s;
                return SCR_OK;
            }
            return SCR_FAIL;
        }

        template <typename Cy>
        int fromChars(
            const Cy* s, size_t len,
            size_t d_pos, bool has_dp, const Cy** n)
        {
            auto actual_len = len + d_pos - (has_dp ? 1 : 0);
            if (!actual_len) {
                return SCR_FAIL;
            }

            auto su = (actual_len + dig - 1) / dig;
            if (su > uic) {
                su = uic;
            }

            size_t cd = d_pos;
            auto _s = s;
            auto _se = s + len;
            auto sd = (actual_len - 1) % dig + 1;

            size_t i;
            size_t skip_u = (d_pos + dig - 1) / dig;
            for (i = uic; i-- > uic - skip_u;) {
                raw[i] = 0;
            }

            effect = su;
            for (i = uic - su; i < uic; ++i) {
                if (_s >= _se) {
                    break;
                }

                UTy result = 0;
                auto tdig = (std::min<size_t>)(_se - _s, sd);
                for (size_t j = 0; j < tdig;) {
                    if (_s < _se) {
                        if (has_dp && *_s == Cy('.')) {
                            ++_s;
                            has_dp = false;
                            continue;
                        }

                        auto c = ctoi(*_s);
                        if (c >= bN) {
                            if (_s != s) {
                                *n = _s;
                                raw[i] = result * UCTraits::bpow(uint_fast8_t(sd - j));
                                movdr(actual_len - cd);
                                return SCR_OK;
                            }
                            return SCR_FAIL;
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
                        return SCR_OF;
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
                return SCR_OK;
            }
            return SCR_FAIL;
        }

        template <bool Upp, typename Cy>
        void toChars(Cy* s, size_t* len) const {
            size_t ep = uic - 1;
            for (size_t i = uic; i-- > uic - effect;) {
                if (raw[i]) {
                    ep = i;
                }
            }

            auto _s = s + (uic - ep) * dig;
            size_t _len = 0;
            for (size_t i = uic; i-- > uic - effect;) {
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
            if (sud >= effect) {
                *len = 0;
                return;
            }

            size_t ep = uic - 1;
            for (size_t i = uic; i-- > uic - effect;) {
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
            for (size_t i = uic - sud; i-- > uic - effect;) {
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
            for (size_t i = uic - effect; i < uic; ++i) {
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
            if (i >= effect)
                return true;
            i = uic - i - 1;

            auto r = raw[i] % UCTraits::bpow(pos % dig);
            if (r) {
                return false;
            }

            if (i == 0) {
                return true;
            }

            for (; i < uic; ++i) {
                if (raw[i]) {
                    return false;
                }
            }
            return true;
        }

        UTy raw[uic];
        uint_fast8_t bottom;
        size_t effect;
    };

}
}

#endif  // UTILS_STRINGS_FLOAT_CONV_BIG_NUM_HPP_
