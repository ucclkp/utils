// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_FLOAT_CONV_INTERNAL_HPP_
#define UTILS_FLOAT_CONV_INTERNAL_HPP_

#include "utils/float_conv_big_num.hpp"


namespace utl {
namespace internal {

    template <typename FTy>
    void bit_conv(uint_fast64_t v, FTy* out) {
        if constexpr (sizeof(FTy) == 2) {
            auto _v = uint16_t(v);
            std::memcpy(out, &_v, sizeof(FTy));
            return;
        }
        if constexpr (sizeof(FTy) == 4) {
            auto _v = uint32_t(v);
            std::memcpy(out, &_v, sizeof(FTy));
            return;
        }
        if constexpr (sizeof(FTy) == 8) {
            auto _v = uint64_t(v);
            std::memcpy(out, &_v, sizeof(FTy));
            return;
        }

        static_assert(
            sizeof(FTy) == 2 || sizeof(FTy) == 4 || sizeof(FTy) == 8,
            "unavailable type!");
    }

    template <typename FTy>
    void bit_conv(FTy v, uint_fast64_t* out) {
        if constexpr (sizeof(FTy) == 2) {
            uint16_t _v;
            std::memcpy(&_v, &v, sizeof(FTy));
            *out = _v;
            return;
        }
        if constexpr (sizeof(FTy) == 4) {
            uint32_t _v;
            std::memcpy(&_v, &v, sizeof(FTy));
            *out = _v;
            return;
        }
        if constexpr (sizeof(FTy) == 8) {
            uint64_t _v;
            std::memcpy(&_v, &v, sizeof(FTy));
            *out = _v;
            return;
        }

        static_assert(
            sizeof(FTy) == 2 || sizeof(FTy) == 4 || sizeof(FTy) == 8,
            "unavailable type!");
    }

    template <typename FTy>
    void infd(uint_fast8_t sign, FTy* out) {
        *out = sign ? -std::numeric_limits<FTy>::infinity() :
            std::numeric_limits<FTy>::infinity();
    }

    template <typename FTy>
    void zerd(uint_fast8_t sign, FTy* out) {
        *out = sign ? -FTy(0) : FTy(0);
    }

    template <typename FTy>
    void nand(uint_fast8_t sign, int type, FTy* out) {
        // 0|1 << 51:     nan
        // 1|1 << 51:     -nan(ind)
        // 0|1 << 51 - 1: nan(snan)
        // 1|1 << 51 - 1: -nan(snan)
        using FT = FloatTraits_base<FTy>;
        constexpr auto mant_1 = FT::mant - 1u;
        constexpr auto bexp = sizeof(FTy) * CHAR_BIT - FT::mant - 1u;
        constexpr auto exp_mask = (uint_fast64_t(1u) << bexp) - 1u;

        uint_fast64_t v;
        if (type == 2) {
            // nan(snan)/-nan(snan)
            v = uint_fast64_t(exp_mask) << FT::mant;
            if (sign) {
                v |= uint_fast64_t(1u) << (sizeof(FTy) * CHAR_BIT - 1);
            }
            v |= 1u;
            bit_conv(v, out);
        } else if (type == 1 || sign) {
            // -nan(ind)
            v = uint_fast64_t(exp_mask) << FT::mant;
            v |= uint_fast64_t(1u) << mant_1;
            v |= uint_fast64_t(1u) << (sizeof(FTy) * CHAR_BIT - 1);
            bit_conv(v, out);
        } else {
            // nan
            v = uint_fast64_t(exp_mask) << FT::mant;
            v |= uint_fast64_t(1u) << mant_1;
            bit_conv(v, out);
        }
    }

    template <typename FTy, typename UTy, size_t bN, size_t UUnit, size_t FUnit>
    void bn_nor_round(
        BigUInt_bN<FTy, UTy, bN, UUnit>* bu,
        BigFloat_bN<FTy, UTy, bN, FUnit>* bf,
        uint_fast8_t sign, int round, int precision)
    {
        if (bu) {
            if (precision > 0) {
                bf->round(precision - 1, sign, round);
                bu->add(bf->getTop());
            } else {
                if (round == FR_CEIL) {
                    if (!sign && !bf->isZero()) {
                        bu->add(1);
                    }
                } else if (round == FR_FLOOR) {
                    if (sign && !bf->isZero()) {
                        bu->add(1);
                    }
                } else if (round == FR_ZERO) {
                } else {
                    auto d_1 = bf->getDigit(0);
                    if (d_1 > bN / 2) {
                        bu->add(1);
                    } else if (d_1 == bN / 2) {
                        auto d = bu->getDigit(0);
                        if (!bf->isZeroAfter(0) || (d % 2) != 0) {
                            bu->add(1);
                        }
                    }
                }
            }
        } else {
            if (precision > 0) {
                bf->round(precision - 1, sign, round);
            }
        }
    }

    template <typename FTy, typename UTy, size_t bN, size_t UUnit, size_t FUnit>
    void bn_sci_round(
        BigUInt_bN<FTy, UTy, bN, UUnit>* bu,
        BigFloat_bN<FTy, UTy, bN, FUnit>* bf,
        uint_fast8_t sign, int round, int precision)
    {
        size_t exp;
        if (bu && !bu->isZero()) {
            exp = bu->getSignDigitPos();

            if (uint_fast32_t(precision) < exp) {
                bu->round(exp - precision, sign, bf->isZero(), round);
            } else if (precision == exp) {
                if (round == FR_CEIL) {
                    if (!sign && !bf->isZero()) {
                        bu->add(1);
                    }
                } else if (round == FR_FLOOR) {
                    if (sign && !bf->isZero()) {
                        bu->add(1);
                    }
                } else if (round == FR_ZERO) {
                } else {
                    auto d_1 = bf->getDigit(0);
                    if (d_1 > bN / 2) {
                        bu->add(1);
                    } else if (d_1 == bN / 2) {
                        auto d = bu->getDigit(0);
                        if (!bf->isZeroAfter(0) || (d % 2) != 0) {
                            bu->add(1);
                        }
                    }
                }
            } else {
                bf->round(precision - exp - 1, sign, round);
                bu->add(bf->getTop());
            }
        } else {
            exp = bf->getSignDigitPos();
            bf->round(exp + precision, sign, round);
        }
    }

    template <typename FTy, typename UTy, size_t bN, size_t UUnit, size_t FUnit>
    void bn_round(
        uint_fast64_t fra, int_fast32_t exp_shf, uint_fast8_t sign,
        int& fmt, int round, int& precision,
        BigUInt_bN<FTy, UTy, bN, UUnit>& bi,
        BigFloat_bN<FTy, UTy, bN, FUnit>& bf)
    {
        using BigUInt = BigUInt_bN<FTy, UTy, bN, UUnit>;
        using BigFloat = BigFloat_bN<FTy, UTy, bN, FUnit>;
        using FT = FloatTraits_base<FTy>;
        constexpr auto maxe_1 = FT::maxe - 1u;

        if (exp_shf != 0) {
            if (fmt & FF_SCI) {
                if (fmt & FF_NOR) {
                    int P = precision;
                    if (P == 0) P = 1;

                    BigUInt bi2 = bi;
                    BigFloat bf2 = bf;
                    bn_sci_round(&bi2, &bf2, sign, round, precision);

                    int X;
                    bool nor = false;
                    if (!bi2.isZero()) {
                        X = int(bi2.getSignDigitPos());
                        if (P > X) {
                            nor = true;
                        }
                    } else {
                        X = -(int(bf2.getSignDigitPos()) + 1);
                        if (X >= -4) {
                            nor = true;
                        }
                    }

                    if (nor) {
                        fmt &= ~FF_SCI;
                        precision = P - 1 - X;
                        bn_nor_round(&bi, &bf, sign, round, precision);
                    } else {
                        fmt &= ~FF_NOR;
                        precision = P - 1;
                        bn_sci_round(&bi, &bf, sign, round, precision);
                    }
                } else {
                    bn_sci_round(&bi, &bf, sign, round, precision);
                }
            } else {
                bn_nor_round(&bi, &bf, sign, round, precision);
            }
        } else {
            if (fmt & FF_SCI) {
                if (fmt & FF_NOR) {
                    int P = precision;
                    if (P == 0) P = 1;

                    BigFloat bf2 = bf;
                    bn_sci_round<FTy, UTy, bN, UUnit, FUnit>(
                        nullptr, &bf2, sign, round, precision);

                    int X = -(int(bf2.getSignDigitPos()) + 1);
                    if (X >= -4) {
                        fmt &= ~FF_SCI;
                        precision = P - 1 - X;
                        bn_nor_round<FTy, UTy, bN, UUnit, FUnit>(
                            nullptr, &bf, sign, round, precision);
                    } else {
                        fmt &= ~FF_NOR;
                        precision = P - 1;
                        bn_sci_round<FTy, UTy, bN, UUnit, FUnit>(
                            nullptr, &bf, sign, round, precision);
                    }
                } else {
                    bn_sci_round<FTy, UTy, bN, UUnit, FUnit>(
                        nullptr, &bf, sign, round, precision);
                }
            } else {
                bn_nor_round<FTy, UTy, bN, UUnit, FUnit>(
                    nullptr, &bf, sign, round, precision);
            }
        }
    }

    template <typename FTy, typename UTy, size_t bN, size_t UUnit, size_t FUnit>
    void dftobn(
        uint_fast64_t fra, int_fast32_t exp_shf,
        BigUInt_bN<FTy, UTy, bN, UUnit>& bi,
        BigFloat_bN<FTy, UTy, bN, FUnit>& bf,
        int* bi_available)
    {
        using BigUInt = BigUInt_bN<FTy, UTy, bN, UUnit>;
        using BigFloat = BigFloat_bN<FTy, UTy, bN, FUnit>;
        using FT = FloatTraits_base<FTy>;
        constexpr auto maxe_1 = FT::maxe - 1u;

        if (exp_shf != 0) {
            int_fast32_t exp = exp_shf - int_fast32_t(maxe_1);

            BigUInt bi_tmp(5);
            for (int_fast32_t i = exp; i >= 0; --i) {
                bi_tmp.mul2();
                if (i == 0 || (i <= FT::mant && (fra & (uint_fast64_t(1u) << (FT::mant - i))))) {
                    bi.add(bi_tmp);
                }
            }

            BigFloat bf_tmp(1);
            for (int_fast32_t i = exp + 1; i <= int_fast32_t(FT::mant); ++i) {
                bf_tmp.div2();
                if (i == 0 || (i > 0 && (fra & (uint_fast64_t(1u) << (FT::mant - i))))) {
                    bf.add(bf_tmp);
                }
            }
            *bi_available = 1;
        } else {
            int_fast32_t exp = 1 - int_fast32_t(maxe_1);

            BigFloat bf_tmp(1);
            for (int_fast32_t i = exp + 1; i <= int_fast32_t(FT::mant); ++i) {
                bf_tmp.div2();
                if (i > 0 && (fra & (uint_fast64_t(1) << (FT::mant - i)))) {
                    bf.add(bf_tmp);
                }
            }
            *bi_available = 0;
        }
    }

    template <typename FTy, typename UTy, typename Cy, size_t bN, size_t UUnit, size_t FUnit>
    void bntocp(
        int fmt, int precision, int bi_available,
        BigUInt_bN<FTy, UTy, bN, UUnit>& bi,
        BigFloat_bN<FTy, UTy, bN, FUnit>& bf,
        Cy* ubuf, size_t* u_len, Cy* fbuf, size_t* f_len, size_t* r_exp)
    {
        using UCTraits = UCeilTraits<UTy, bN>;
        using BigUInt = BigUInt_bN<FTy, UTy, bN, UUnit>;
        using BigFloat = BigFloat_bN<FTy, UTy, bN, FUnit>;

        if (fmt & FF_SCI) {
            if (bi_available && !bi.isZero()) {
                auto exp = bi.getSignDigitPos();
                size_t len = *u_len;
                if (fmt & FF_UPP) {
                    bi.toChars<true>(ubuf, &len, exp);
                } else {
                    bi.toChars<false>(ubuf, &len, exp);
                }
                *u_len = len;

                size_t i = 0;
                size_t _len;
                if (fmt & FF_EXA) {
                    _len = exp;
                } else {
                    _len = exp >= size_t(precision) ? size_t(precision) : exp;
                }

                len = _len >= *f_len ? *f_len : _len;
                if (len) {
                    if (fmt & FF_UPP) {
                        bi.toChars<true>(fbuf, &len, exp - len);
                    } else {
                        bi.toChars<false>(fbuf, &len, exp - len);
                    }
                    i += len;
                }

                size_t buf_rem = *f_len - len;
                if (fmt & FF_EXA) {
                    len = buf_rem;
                } else {
                    size_t target_rem = size_t(precision) - len;
                    len = buf_rem >= target_rem ? target_rem : buf_rem;
                }

                if (len) {
                    if (fmt & FF_UPP) {
                        bf.toChars<true>(fbuf + i, &len, 0);
                    } else {
                        bf.toChars<false>(fbuf + i, &len, 0);
                    }
                    i += len;
                }
                *f_len = i;
                *r_exp = exp;
            } else {
                auto exp = bf.getSignDigitPos();
                size_t len = *u_len;
                if (fmt & FF_UPP) {
                    bf.toChars<true>(ubuf, &len, exp);
                } else {
                    bf.toChars<false>(ubuf, &len, exp);
                }
                *u_len = len;

                size_t i = 0;
                if (fmt & FF_EXA) {
                    len = *f_len;
                } else {
                    len = size_t(precision) > *f_len ? *f_len : size_t(precision);
                }

                if (fmt & FF_UPP) {
                    bf.toChars<true>(fbuf, &len, exp + 1);
                } else {
                    bf.toChars<false>(fbuf, &len, exp + 1);
                }
                i += len;
                *f_len = i;
                *r_exp = exp + 1;
            }
        } else {
            if (bi_available) {
                size_t i_cs_len = *u_len;
                if (fmt & FF_UPP) {
                    bi.toChars<true>(ubuf, &i_cs_len, 0);
                } else {
                    bi.toChars<false>(ubuf, &i_cs_len, 0);
                }
                *u_len = i_cs_len;

                size_t f_cs_len;
                if (fmt & FF_EXA) {
                    f_cs_len = *f_len;
                } else {
                    f_cs_len = size_t(precision) > *f_len ? *f_len : size_t(precision);
                }

                if (fmt & FF_UPP) {
                    bf.toChars<true>(fbuf, &f_cs_len, 0);
                } else {
                    bf.toChars<false>(fbuf, &f_cs_len, 0);
                }
                *f_len = f_cs_len;
            } else {
                if (!(fmt & FF_EXA) && precision == 0) {
                    if (*u_len > 0) {
                        *ubuf = Cy('0');
                        *u_len = 1;
                        *f_len = 0;
                    }
                    return;
                }

                *u_len = 0;

                size_t f_cs_len;
                if (fmt & FF_EXA) {
                    f_cs_len = *f_len;
                } else {
                    f_cs_len = size_t(precision) > *f_len ? *f_len : size_t(precision);
                }

                if (fmt & FF_UPP) {
                    bf.toChars<true>(fbuf, &f_cs_len, 0);
                } else {
                    bf.toChars<false>(fbuf, &f_cs_len, 0);
                }
                *f_len = f_cs_len;
            }
        }
    }

    template <typename Cy>
    void cpnos(
        uint_fast8_t sign,
        const Cy* su, size_t su_len,
        const Cy* sf, size_t sf_len, int precision, int fmt, std::basic_string<Cy>* out)
    {
        if (sign) {
            out->push_back(Cy('-'));
        }

        if (!su_len) {
            out->push_back(Cy('0'));
        } else {
            auto _su = su;
            while (_su < (su + su_len - 1) && *_su == Cy('0')) {
                ++_su;
            }
            out->append(_su, su_len - (_su - su));
        }

        if (!(fmt & FF_EXA) && precision == 0) {
            if (fmt & FF_DIG) {
                out->push_back(Cy('.'));
            }
            return;
        }

        out->push_back(Cy('.'));

        if ((fmt & FF_EXA) || (fmt & FF_NTZ)) {
            if (!sf_len) {
                out->push_back(Cy('0'));
            } else {
                auto _sf = sf + sf_len;
                while (_sf > sf && *--_sf == Cy('0')) {}
                out->append(sf, _sf - sf + 1);
            }
        } else {
            if (!sf_len) {
                out->append(precision, Cy('0'));
            } else {
                if (sf_len > size_t(precision)) {
                    out->append(sf, precision);
                } else {
                    out->append(sf, sf_len);
                    out->append(precision - sf_len, Cy('0'));
                }
            }
        }
    }

    template <typename Cy>
    bool cpnos(
        uint_fast8_t sign,
        const Cy* su, size_t su_len,
        const Cy* sf, size_t sf_len, int precision, int fmt, Cy* buf, size_t* len)
    {
        auto s = buf;
        auto se = s ? (buf + *len) : nullptr;
        size_t act_len = 0;

        if (sign) {
            if (s && s < se) {
                *s++ = Cy('-');
            }
            ++act_len;
        }

        if (!su_len) {
            if (s && s < se) {
                *s++ = Cy('0');
            }
            ++act_len;
        } else {
            auto _su = su;
            while (_su < (su + su_len - 1) && *_su == Cy('0')) {
                ++_su;
            }

            auto _len = su_len - size_t(_su - su);
            if (s && _len <= size_t(se - s)) {
                if (_len) {
                    std::memcpy(s, _su, _len * sizeof(Cy));
                    s += _len;
                }
            } else {
                s = se;
            }
            act_len += _len;
        }

        if (!(fmt & FF_EXA) && precision == 0) {
            if (fmt & FF_DIG) {
                if (s && s < se) {
                    *s++ = Cy('.');
                }
                ++act_len;
            }
            *len = act_len;
            return s && act_len == s - buf;
        }

        if (s && s < se) {
            *s++ = Cy('.');
        }
        ++act_len;

        if ((fmt & FF_EXA) || (fmt & FF_NTZ)) {
            if (!sf_len) {
                if (s && se - s >= 1) {
                    *s++ = Cy('0');
                }
                ++act_len;
            } else {
                auto _sf = sf + sf_len;
                while (_sf > sf && *--_sf == Cy('0')) {}

                auto _len = _sf - sf + 1;
                if (s && _len <= se - s) {
                    std::memcpy(s, sf, _len * sizeof(Cy));
                    s += _len;
                } else {
                    s = se;
                }
                act_len += _len;
            }
        } else {
            if (!sf_len) {
                if (s && precision <= se - s) {
                    for (int i = 0; i < precision; ++i) {
                        *s++ = Cy('0');
                    }
                } else {
                    s = se;
                }
                act_len += precision;
            } else {
                if (sf_len > size_t(precision)) {
                    if (s && precision <= se - s) {
                        std::memcpy(s, sf, precision * sizeof(Cy));
                        s += precision;
                    } else {
                        s = se;
                    }
                    act_len += precision;
                } else {
                    if (s && sf_len <= size_t(se - s)) {
                        std::memcpy(s, sf, sf_len * sizeof(Cy));
                        s += sf_len;
                    } else {
                        s = se;
                    }
                    act_len += sf_len;

                    auto _len = size_t(precision) - sf_len;
                    if (s && _len <= size_t(se - s)) {
                        for (size_t i = 0; i < _len; ++i) {
                            *s++ = Cy('0');
                        }
                    } else {
                        s = se;
                    }
                    act_len += _len;
                }
            }
        }

        *len = act_len;
        return s && act_len == s - buf;
    }

    template <typename FTy, typename UTy, size_t bN, size_t UUnit, size_t FUnit>
    void normalize(
        int bi_available,
        BigUInt_bN<FTy, UTy, bN, UUnit>& bi,
        BigFloat_bN<FTy, UTy, bN, FUnit>& bf,
        int* r_exp)
    {
        *r_exp = 0;
        if (bi_available && !bi.isZero()) {
            auto d = bi.getDigit(bi.getSignDigitPos());
            assert(d != 0);
            while (d > 1) {
                bi.div2();
                bf.setTop(bi.getBottom());
                bf.div2();
                bi.setBottom(0);
                d /= 2;
                ++*r_exp;
            }
        } else {
            auto d = bf.getDigit(bf.getSignDigitPos());
            while (d > 1) {
                bf.div2();
                d /= 2;
                ++*r_exp;
            }
        }
    }

    template <typename FTy, typename UTy, typename Cy, size_t bN, size_t UUnit, size_t FUnit>
    void dftos(
        uint_fast64_t fra, int_fast32_t exp_shf, uint_fast8_t sign,
        int fmt, int round, int precision, std::basic_string<Cy>* out)
    {
        using UCTraits = UCeilTraits<UTy, bN>;
        using BigUInt = BigUInt_bN<FTy, UTy, bN, UUnit>;
        using BigFloat = BigFloat_bN<FTy, UTy, bN, FUnit>;

        BigUInt bi(0);
        BigFloat bf(0);
        int bi_available;
        dftobn(
            fra, exp_shf,
            bi, bf, &bi_available);

        int rem_exp = 0;
        if (fmt & FF_HEX2) {
            normalize(bi_available, bi, bf, &rem_exp);
        }

        if (!(fmt & FF_EXA)) {
            bn_round(
                fra, exp_shf, sign,
                fmt, round, precision,
                bi, bf);
        }

        if (fmt & FF_SCI) {
            Cy ubuf[1];
            size_t u_len = 1u;

            constexpr size_t fb_size = (UUnit + FUnit) * UCTraits::udig;
            Cy fbuf[fb_size];
            size_t f_len = fb_size;

            size_t exp;
            bntocp(fmt, precision, bi_available, bi, bf, ubuf, &u_len, fbuf, &f_len, &exp);
            cpnos(sign, ubuf, u_len, fbuf, f_len, precision, fmt, out);

            bool upp = !!(fmt & FF_UPP);
            if (fmt & FF_HEX2) {
                out->push_back(Cy(upp ? 'P' : 'p'));
            } else if (fmt & FF_HEX) {
                out->push_back(Cy(upp ? 'S' : 's'));
            } else {
                out->push_back(Cy(upp ? 'E' : 'e'));
            }

            if (bi_available && !bi.isZero()) {
                out->push_back(Cy('+'));
            } else {
                out->push_back(Cy('-'));
                rem_exp = -rem_exp;
            }

            std::basic_string<Cy> _str;
            if (fmt & FF_HEX2) {
                exp = exp * 4 + rem_exp;
            }

            itos(exp, &_str);
            if (!(fmt & FF_ENZ) && exp < 10) {
                out->push_back(Cy('0'));
            }

            out->append(_str);
        } else {
            constexpr size_t ub_size = BigUInt::uic * BigUInt::dig;
            Cy ubuf[ub_size];
            size_t u_len = ub_size;

            constexpr size_t fb_size = BigFloat::uic * BigFloat::dig;
            Cy fbuf[fb_size];
            size_t f_len = fb_size;

            bntocp(fmt, precision, bi_available, bi, bf, ubuf, &u_len, fbuf, &f_len, nullptr);
            cpnos(sign, ubuf, u_len, fbuf, f_len, precision, fmt, out);
        }
    }

    template <typename FTy, typename UTy, typename Cy, size_t bN, size_t UUnit, size_t FUnit>
    bool dftos(
        uint_fast64_t fra, int_fast32_t exp_shf, uint_fast8_t sign,
        int fmt, int round, int precision, Cy* buf, size_t* len)
    {
        using UCTraits = UCeilTraits<UTy, bN>;
        using BigUInt = BigUInt_bN<FTy, UTy, bN, UUnit>;
        using BigFloat = BigFloat_bN<FTy, UTy, bN, FUnit>;

        BigUInt bi(0);
        BigFloat bf(0);
        int bi_available;
        dftobn(
            fra, exp_shf,
            bi, bf, &bi_available);

        int rem_exp = 0;
        if (fmt & FF_HEX2) {
            normalize(bi_available, bi, bf, &rem_exp);
        }

        if (!(fmt & FF_EXA)) {
            bn_round(
                fra, exp_shf, sign,
                fmt, round, precision,
                bi, bf);
        }

        if (fmt & FF_SCI) {
            Cy ubuf[1];
            size_t u_len = 1u;

            constexpr size_t fb_size = (UUnit + FUnit) * UCTraits::udig;
            Cy fbuf[fb_size];
            size_t f_len = fb_size;

            size_t exp;
            auto _len = *len;
            size_t act_len = 0;
            bntocp(fmt, precision, bi_available, bi, bf, ubuf, &u_len, fbuf, &f_len, &exp);
            bool ret = cpnos(sign, ubuf, u_len, fbuf, f_len, precision, fmt, buf, &_len);

            Cy* s, *se;
            if (buf) {
                se = buf + *len;
                if (ret) {
                    s = buf + _len;
                } else {
                    s = se;
                }
            } else {
                s = nullptr;
                se = nullptr;
            }
            act_len += _len;

            if (s && se - s >= 2) {
                bool upp = !!(fmt & FF_UPP);
                if (fmt & FF_HEX2) {
                    *s++ = Cy(upp ? 'P' : 'p');
                } else if (fmt & FF_HEX) {
                    *s++ = Cy(upp ? 'S' : 's');
                } else {
                    *s++ = Cy(upp ? 'E' : 'e');
                }

                if (bi_available && !bi.isZero()) {
                    *s++ = Cy('+');
                } else {
                    *s++ = Cy('-');
                    rem_exp = -rem_exp;
                }
            } else {
                s = se;
                if (!bi_available || bi.isZero()) {
                    rem_exp = -rem_exp;
                }
            }
            act_len += 2;

            if (fmt & FF_HEX2) {
                exp = exp * 4 + rem_exp;
            }

            if (!(fmt & FF_ENZ) && exp < 10) {
                if (s && s < se) {
                    *s++ = Cy('0');
                }
                ++act_len;
            }

            size_t is = se - s;
            if (itos(exp, s, &is)) {
                s += is;
            } else {
                s = se;
            }
            act_len += is;

            *len = act_len;
            return s && act_len == s - buf;
        }

        constexpr size_t ub_size = BigUInt::uic * BigUInt::dig;
        Cy ubuf[ub_size];
        size_t u_len = ub_size;

        constexpr size_t fb_size = BigFloat::uic * BigFloat::dig;
        Cy fbuf[fb_size];
        size_t f_len = fb_size;

        bntocp(fmt, precision, bi_available, bi, bf, ubuf, &u_len, fbuf, &f_len, nullptr);
        bool ret = cpnos(sign, ubuf, u_len, fbuf, f_len, precision, fmt, buf, len);
        if (!ret) {
            return false;
        }
        return true;
    }

    template <typename FTy, typename UTy, size_t bN, size_t UUnit, size_t FUnit>
    bool btof(
        BigUInt_bN<FTy, UTy, bN, UUnit>& bu,
        BigFloat_bN<FTy, UTy, bN, FUnit>& bf,
        uint_fast8_t sign, int round, FTy* out)
    {
        using FT = FloatTraits_base<FTy>;
        constexpr size_t kSpace = 8u;
        constexpr auto mant_1 = FT::mant - 1u;
        constexpr auto maxe_1 = FT::maxe - 1u;
        constexpr auto mine_b = mant_1 + maxe_1;

        if (bu.isZero() && bf.isZero()) {
            zerd(sign, out);
            return true;
        }

        int_fast32_t exp = 0;
        uint_fast64_t fra = 0;
        for (size_t j = 0; j < FT::maxe; ++j) {
            if (bu.isZero()) {
                break;
            }

            bu.div2();
            auto b = bu.getBottom();
            if (b) {
                bu.setBottom(0);
                if (bu.isZero()) {
                    break;
                }
                fra >>= 1u;
                fra |= uint_fast64_t(1u) << (mant_1 + kSpace);
            } else {
                fra >>= 1u;
            }

            ++exp;
        }

        // 上溢
        if (!bu.isZero()) {
            infd(sign, out);
            return false;
        }

        bool update_exp = exp == 0;
        uint_fast32_t b_shf = exp;
        for (size_t j = 0; j < mine_b + kSpace; ++j) {
            if (bf.isZero()) {
                break;
            }

            if (update_exp) {
                --exp;
            }

            bf.mul2();
            auto top = bf.getTop();
            if (top) {
                if (update_exp) {
                    if (exp <= -int_fast32_t(maxe_1)) {
                        b_shf = -int_fast32_t(maxe_1) - exp;
                        exp = -int_fast32_t(maxe_1);
                        update_exp = false;
                    }
                }

                if (b_shf <= mant_1 + kSpace && !update_exp) {
                    fra |= uint_fast64_t(1u) << (mant_1 + kSpace - b_shf);
                }

                bf.setTop(0);
            }

            if (!update_exp) {
                ++b_shf;
            }

            if (top && update_exp) {
                update_exp = false;
            }
        }

        if (exp < -int_fast32_t(mine_b)) {
            exp = 0;
        }

        if (round == FR_CEIL) {
            if (!sign) {
                if (fra & (uint_fast64_t(1u) << kSpace) - 1u) {
                    fra += uint_fast64_t(1u) << kSpace;
                }
            }
        } else if (round == FR_FLOOR) {
            if (sign) {
                if (fra & (uint_fast64_t(1u) << kSpace) - 1u) {
                    fra += uint_fast64_t(1u) << kSpace;
                }
            }
        } else if (round == FR_ZERO) {
        } else {
            if (fra & (uint_fast64_t(1u) << (kSpace - 1))) {
                fra += uint_fast64_t(1u) << kSpace;
            }
        }
        fra >>= kSpace;

        uint_fast32_t exp_shift = exp + int_fast32_t(maxe_1);

        fra |= uint_fast64_t(exp_shift) << FT::mant;
        if (sign) {
            fra |= uint_fast64_t(1u) << (sizeof(FTy) * CHAR_BIT - 1);
        }
        bit_conv(fra, out);
        return true;
    }

    template <typename FTy, typename UTy, typename Cy, size_t bN, size_t UUnit, size_t FUnit>
    int dnorstof(
        const Cy* s, size_t len, const Cy* di,
        uint_fast8_t sign, int round, FTy* out, const Cy** p)
    {
        using BigUInt = BigUInt_bN<FTy, UTy, bN, UUnit>;
        using BigFloat = BigFloat_bN<FTy, UTy, bN, FUnit>;
        using FT = FloatTraits<FTy, bN>;

        BigUInt bu(0);
        BigFloat bf(0);
        const Cy* _p;

        int ret;
        if (!di) {
            ret = bu.fromChars(s, len, &_p);
            if (ret != UCR_OK) {
                if (ret == UCR_OVERFLOWED) {
                    infd(sign, out);
                }
                return ret;
            }
        } else if (di == s) {
            ret = bf.fromChars(s + 1, len - 1, &_p);
            if (ret != UCR_OK) {
                return ret;
            }

            if (bf.getSignDigitPos() > FT::mine_b) {
                zerd(sign, out);
                return UCR_OVERFLOWED;
            }
        } else {
            ret = bu.fromChars(s, di - s, &_p);
            if (ret != UCR_OK) {
                if (ret == UCR_OVERFLOWED) {
                    infd(sign, out);
                }
                return ret;
            }
            if (_p == di) {
                bf.fromChars(di + 1, (s + len - di) - 1, &_p);
            }
        }

        if (!btof(bu, bf, sign, round, out)) {
            infd(sign, out);
            return UCR_OVERFLOWED;
        }

        *p = _p;
        return UCR_OK;
    }

    template <typename FTy, typename UTy, typename Cy, size_t bN, size_t UUnit, size_t FUnit>
    int dscistof(
        const Cy* str, size_t len, const Cy* di, const Cy* de,
        uint_fast8_t sign, int_fast32_t exp, int fmt, int round, FTy* out, const Cy** p)
    {
        using BigUInt = BigUInt_bN<FTy, UTy, bN, UUnit>;
        using BigFloat = BigFloat_bN<FTy, UTy, bN, FUnit>;
        using FT = FloatTraits<FTy, bN>;

        BigUInt bu(0);
        BigFloat bf(0);
        const Cy* _p;

        int exp_d;
        int exp_rem;
        if (fmt & FF_HEX2) {
            exp_d = exp / 4;
            exp_rem = exp % 4;
        } else {
            exp_d = exp;
            exp_rem = 0;
        }

        int ret;
        intptr_t fl, ul;
        if (di) {
            fl = de - di - 1;
            ul = di - str;

            if (exp_d >= fl) {
                ret = bu.fromChars(str, de - str, exp_d - fl, true, &_p);
                if (ret != UCR_OK) {
                    if (ret == UCR_OVERFLOWED) {
                        infd(sign, out);
                    }
                    return ret;
                }
            } else if (-exp_d < ul) {
                auto s = di + 1 + exp_d;
                ret = bu.fromChars(str, s - str, 0, s > di, &_p);
                if (ret != UCR_OK) {
                    if (ret == UCR_OVERFLOWED) {
                        infd(sign, out);
                    }
                    return ret;
                }
                if (_p == s) {
                    bf.fromChars(s, de - s, 0, s <= di, &_p);
                }
            } else {
                ret = bf.fromChars(str, de - str, -(exp_d + ul), true, &_p);
                if (ret != UCR_OK) {
                    return ret;
                }

                if (bf.getSignDigitPos() > FT::mine_b) {
                    zerd(sign, out);
                    return UCR_OVERFLOWED;
                }
            }
        } else {
            fl = 0;
            ul = de - str;

            if (exp_d >= fl) {
                ret = bu.fromChars(str, de - str, exp_d - fl, false, &_p);
                if (ret != UCR_OK) {
                    if (ret == UCR_OVERFLOWED) {
                        infd(sign, out);
                    }
                    return ret;
                }
            } else if (-exp_d < ul) {
                auto s = de + exp_d;
                ret = bu.fromChars(str, s - str, 0, false, &_p);
                if (ret != UCR_OK) {
                    if (ret == UCR_OVERFLOWED) {
                        infd(sign, out);
                    }
                    return ret;
                }
                if (_p == s) {
                    bf.fromChars(s, de - s, 0, false, &_p);
                }
            } else {
                ret = bf.fromChars(str, de - str, -(exp_d + ul), false, &_p);
                if (ret != UCR_OK) {
                    return ret;
                }

                if (bf.getSignDigitPos() > FT::mine_b) {
                    zerd(sign, out);
                    return UCR_OVERFLOWED;
                }
            }
        }

        if (exp_rem > 0) {
            for (int i = 0; i < exp_rem; ++i) {
                bf.mul2();
                bu.setBottom(bf.getTop());
                bu.mul2();
            }
        } else if (exp_rem < 0) {
            for (int i = 0; i > exp_rem; --i) {
                bu.div2();
                bf.setTop(bu.getBottom());
                bf.div2();
            }
        }

        if (!btof(bu, bf, sign, round, out)) {
            infd(sign, out);
            return UCR_OVERFLOWED;
        }

        *p = _p;
        return UCR_OK;
    }

    template <typename FTy, typename UTy, typename Cy, size_t bN>
    int norstof(
        const Cy* str, size_t len,
        uint_fast8_t sign, int round, FTy* out, const Cy** p)
    {
        using UCTraits = UCeilTraits<UTy, bN>;

        auto s = str;
        auto se = s + len;
        const Cy* di = nullptr;
        while (s < se) {
            if (*s == Cy('.')) {
                di = s;
                break;
            }
            ++s;
        }

        size_t uuc, fuc;
        if (di) {
            auto ul = di - str;
            uuc = (ul + UCTraits::udig - 1) / UCTraits::udig + 1;
            fuc = (se - di + UCTraits::udig) / UCTraits::udig + 1;
        } else {
            auto ul = se - str;
            uuc = (ul + UCTraits::udig - 1) / UCTraits::udig + 1;
            fuc = 1;
        }

        if (uuc < 1 && fuc < 1) {
            return dnorstof<FTy, UTy, Cy, bN, 1, 1>(str, len, di, sign, round, out, p);
        }
        if (uuc < 2 && fuc < 2) {
            return dnorstof<FTy, UTy, Cy, bN, 2, 2>(str, len, di, sign, round, out, p);
        }
        if (uuc < 4 && fuc < 4) {
            return dnorstof<FTy, UTy, Cy, bN, 4, 4>(str, len, di, sign, round, out, p);
        }
        if (uuc < 8 && fuc < 8) {
            return dnorstof<FTy, UTy, Cy, bN, 8, 8>(str, len, di, sign, round, out, p);
        }
        if (uuc < 16 && fuc < 16) {
            return dnorstof<FTy, UTy, Cy, bN, 16, 16>(str, len, di, sign, round, out, p);
        }
        if (uuc < 32 && fuc < 32) {
            return dnorstof<FTy, UTy, Cy, bN, 32, 32>(str, len, di, sign, round, out, p);
        }
        return dnorstof<FTy, UTy, Cy, bN, 64, 64>(str, len, di, sign, round, out, p);
    }

    template <typename FTy, typename UTy, typename Cy, size_t bN>
    int scistof(
        const Cy* str, size_t len,
        uint_fast8_t sign, int fmt, int round, FTy* out, const Cy** p)
    {
        using UCTraits = UCeilTraits<UTy, bN>;

        auto s = str;
        auto se = str + len;

        const Cy* di = nullptr;
        const Cy* dp = nullptr;
        const Cy* de = se;
        while (s < se) {
            if constexpr (bN == 10) {
                if (*s == Cy('e') || *s == Cy('E')) {
                    dp = s;
                    break;
                }
            }
            if constexpr (bN == 16) {
                if (fmt & FF_HEX2) {
                    if (*s == Cy('p') || *s == Cy('P')) {
                        dp = s;
                        break;
                    }
                } else {
                    if (*s == Cy('s') || *s == Cy('S')) {
                        dp = s;
                        break;
                    }
                }
            }

            if (*s == Cy('.')) {
                if (di) {
                    de = s;
                    break;
                }
                di = s;
                ++s;
                continue;
            }

            auto c = ctoi(*s);
            if (c == uint_fast8_t(-1) || c >= bN) {
                de = s;
                break;
            }
            ++s;
        }

        int_fast32_t exp = 0;
        if (dp) {
            s = dp + 1;
            if (s == se) {
                return UCR_FAILED;
            }
            if (*s == Cy('+')) {
                ++s;
                if (s == se) {
                    return UCR_FAILED;
                }
            }

            if (!stoi(s, se - s, &exp, 10, &s)) {
                return UCR_FAILED;
            }
            de = dp;
        }

        const Cy* di1;
        intptr_t fl, ul;
        if (di) {
            fl = de - di - 1;
            ul = di - str;
            di1 = di;
        } else {
            fl = 0;
            ul = de - str;
            di1 = de - 1;
        }

        int_fast32_t exp_d;
        if (fmt & FF_HEX2) {
            exp_d = (exp + (exp >= 0 ? 3 : -3)) / 4;
        } else {
            exp_d = exp;
        }

        size_t uuc, fuc;
        if (exp_d >= fl) {
            uuc = ((de - str) + (exp_d - fl) + UCTraits::udig - 1) / UCTraits::udig + 1;
            fuc = 1;
        } else if (-exp_d <= ul) {
            auto _s = di1 + 1 + exp_d;
            uuc = (_s - str + UCTraits::udig - 1) / UCTraits::udig + 1;
            fuc = (de - _s + UCTraits::udig - 1) / UCTraits::udig + 1;
        } else {
            uuc = 1;
            fuc = ((de - str) - (exp_d + ul) + UCTraits::udig - 1) / UCTraits::udig + 1;
        }

        if (uuc < 1 && fuc < 1) {
            return dscistof<FTy, UTy, Cy, bN, 1, 1>(
                str, len, di, de, sign, exp, fmt, round, out, p);
        }
        if (uuc < 2 && fuc < 2) {
            return dscistof<FTy, UTy, Cy, bN, 2, 2>(
                str, len, di, de, sign, exp, fmt, round, out, p);
        }
        if (uuc < 4 && fuc < 4) {
            return dscistof<FTy, UTy, Cy, bN, 4, 4>(
                str, len, di, de, sign, exp, fmt, round, out, p);
        }
        if (uuc < 8 && fuc < 8) {
            return dscistof<FTy, UTy, Cy, bN, 8, 8>(
                str, len, di, de, sign, exp, fmt, round, out, p);
        }
        if (uuc < 16 && fuc < 16) {
            return dscistof<FTy, UTy, Cy, bN, 16, 16>(
                str, len, di, de, sign, exp, fmt, round, out, p);
        }
        if (uuc < 32 && fuc < 32) {
            return dscistof<FTy, UTy, Cy, bN, 32, 32>(
                str, len, di, de, sign, exp, fmt, round, out, p);
        }
        return dscistof<FTy, UTy, Cy, bN, 64, 64>(
            str, len, di, de, sign, exp, fmt, round, out, p);
    }

    template <typename FTy>
    void ftodf(
        FTy val,
        uint_fast8_t& sign, uint_fast32_t& exp_shift, uint_fast64_t& fra)
    {
        using FT = FloatTraits_base<FTy>;
        constexpr auto bexp = sizeof(FTy) * CHAR_BIT - FT::mant - 1u;
        constexpr auto exp_mask = (uint_fast64_t(1u) << bexp) - 1u;

        uint_fast64_t uv;
        bit_conv(val, &uv);

        fra = uv & ((uint_fast64_t(1u) << FT::mant) - 1u);
        exp_shift = (uv >> FT::mant) & exp_mask;
        sign = uint_fast8_t(uv >> (sizeof(FTy) * CHAR_BIT - 1u));
    }

    template <typename FTy, typename UTy, size_t bN>
    void predict(
        uint_fast32_t exp_shift, int precision, int fmt, size_t& uuc, size_t& fuc)
    {
        using UCT = UCeilTraits<UTy, bN>;
        using FT = FloatTraits_base<FTy>;
        constexpr auto maxe_1 = FT::maxe - 1u;

        if (fmt & FF_EXA) {
            precision = int(FT::maxe + FT::mant);
        }

        int_fast32_t exp = exp_shift - maxe_1;
        if (exp >= 0) {
            uuc = UCT::ucount(exp) + 1;
            fuc = (precision + UCT::udig - 1) / UCT::udig + 1;
        } else {
            uuc = 1;
            fuc = UCT::ucount(-exp) + (precision + UCT::udig - 1) / UCT::udig + 1;
        }
    }

    template <typename FTy, typename UTy, typename Cy, size_t bN>
    void ftos_base(FTy val, int fmt, int round, int precision, std::basic_string<Cy>* out) {
        using FT = FloatTraits_base<FTy>;
        constexpr auto maxe_1 = FT::maxe - 1u;
        constexpr auto bexp = sizeof(FTy) * CHAR_BIT - FT::mant - 1u;
        constexpr auto exp_mask = (uint_fast64_t(1u) << bexp) - 1u;

        uint_fast64_t fra;
        uint_fast32_t exp_shift;
        uint_fast8_t sign;
        ftodf(val, sign, exp_shift, fra);

        bool upp = !!(fmt & FF_UPP);
        if (fra == 0 && exp_shift == 0) {
            cpnos<Cy>(sign, nullptr, 0, nullptr, 0, precision, fmt, out);

            if ((fmt & FF_SCI) && !(fmt & FF_NOR)) {
                if (fmt & FF_HEX2) {
                    out->push_back(upp ? 'P' : 'p');
                } else if (fmt & FF_HEX) {
                    out->push_back(upp ? 'S' : 's');
                } else {
                    out->push_back(upp ? 'E' : 'e');
                }
                out->push_back(Cy('+'));
                if (!(fmt & FF_ENZ)) {
                    out->push_back(Cy('0'));
                }
                out->push_back(Cy('0'));
            }
            return;
        }

        if (fra == 0 && exp_shift == exp_mask) {
            if (sign) {
                out->push_back(Cy('-'));
            }
            out->push_back(Cy(upp ? 'I' : 'i'));
            out->push_back(Cy(upp ? 'N' : 'n'));
            out->push_back(Cy(upp ? 'F' : 'f'));
            return;
        }
        if (fra != 0 && exp_shift == exp_mask) {
            if (fra & (uint_fast64_t(1u) << (FT::mant - 1u))) {
                if (sign) {
                    out->push_back(Cy('-'));
                }
                out->push_back(Cy(upp ? 'N' : 'n'));
                out->push_back(Cy(upp ? 'A' : 'a'));
                out->push_back(Cy(upp ? 'N' : 'n'));
                if (sign) {
                    out->push_back(Cy('('));
                    out->push_back(Cy(upp ? 'I' : 'i'));
                    out->push_back(Cy(upp ? 'N' : 'n'));
                    out->push_back(Cy(upp ? 'D' : 'd'));
                    out->push_back(Cy(')'));
                }
            } else {
                if (sign) {
                    out->push_back(Cy('-'));
                }
                out->push_back(Cy(upp ? 'N' : 'n'));
                out->push_back(Cy(upp ? 'A' : 'a'));
                out->push_back(Cy(upp ? 'N' : 'n'));
                out->push_back(Cy('('));
                out->push_back(Cy(upp ? 'S' : 's'));
                out->push_back(Cy(upp ? 'N' : 'n'));
                out->push_back(Cy(upp ? 'A' : 'a'));
                out->push_back(Cy(upp ? 'N' : 'n'));
                out->push_back(Cy(')'));
            }
            return;
        }

        size_t uuc;
        size_t fuc;
        predict<FTy, UTy, bN>(exp_shift, precision, fmt, uuc, fuc);

        if (uuc < 1 && fuc < 1) {
            dftos<FTy, UTy, Cy, bN, 1, 1>(fra, exp_shift, sign, fmt, round, precision, out);
        } else if (uuc < 2 && fuc < 2) {
            dftos<FTy, UTy, Cy, bN, 2, 2>(fra, exp_shift, sign, fmt, round, precision, out);
        } else if (uuc < 4 && fuc < 4) {
            dftos<FTy, UTy, Cy, bN, 4, 4>(fra, exp_shift, sign, fmt, round, precision, out);
        } else if (uuc < 8 && fuc < 8) {
            dftos<FTy, UTy, Cy, bN, 8, 8>(fra, exp_shift, sign, fmt, round, precision, out);
        } else if (uuc < 16 && fuc < 16) {
            dftos<FTy, UTy, Cy, bN, 16, 16>(fra, exp_shift, sign, fmt, round, precision, out);
        } else if (uuc < 32 && fuc < 32) {
            dftos<FTy, UTy, Cy, bN, 32, 32>(fra, exp_shift, sign, fmt, round, precision, out);
        } else {
            dftos<FTy, UTy, Cy, bN, 64, 64>(fra, exp_shift, sign, fmt, round, precision, out);
        }
    }

    template <typename FTy, typename UTy, typename Cy, size_t bN>
    bool ftos_base(
        FTy val, int fmt, int round, int precision, Cy* buf, size_t* len)
    {
        using FT = FloatTraits_base<FTy>;
        constexpr auto maxe_1 = FT::maxe - 1u;
        constexpr auto bexp = sizeof(FTy) * CHAR_BIT - FT::mant - 1u;
        constexpr auto exp_mask = (uint_fast64_t(1u) << bexp) - 1u;

        uint_fast64_t fra;
        uint_fast32_t exp_shift;
        uint_fast8_t sign;
        ftodf(val, sign, exp_shift, fra);

        bool upp = !!(fmt & FF_UPP);
        if (fra == 0 && exp_shift == 0) {
            size_t _len = *len;
            bool ret = cpnos<Cy>(sign, nullptr, 0, nullptr, 0, precision, fmt, buf, &_len);

            if (fmt & FF_SCI && !(fmt & FF_NOR)) {
                Cy* s, *se;
                if (buf) {
                    se = buf + *len;
                    if (ret) {
                        s = buf + _len;
                    } else {
                        s = se;
                    }
                } else {
                    s = nullptr;
                    se = nullptr;
                }
                if (!s || se - s < 4) {
                    *len = _len + 4u;
                    return false;
                }

                if (fmt & FF_HEX2) {
                    *s++ = (upp ? 'P' : 'p');
                } else if (fmt & FF_HEX) {
                    *s++ = (upp ? 'S' : 's');
                } else {
                    *s++ = (upp ? 'E' : 'e');
                }
                *s++ = (Cy('+'));
                if (!(fmt & FF_ENZ)) {
                    *s++ = (Cy('0'));
                }
                *s = (Cy('0'));

                *len = _len + 4;
            } else {
                *len = _len;
            }
            return ret;
        }

        if (fra == 0 && exp_shift == exp_mask) {
            auto s = buf;
            if (sign) {
                if (!s || *len < 4) { *len = 4u; return false; }
                *s++ = Cy('-');
            } else {
                if (!s || *len < 3) { *len = 3u; return false; }
            }
            *s++ = Cy(upp ? 'I' : 'i');
            *s++ = Cy(upp ? 'N' : 'n');
            *s++ = Cy(upp ? 'F' : 'f');
            *len = s - buf;
            return true;
        }
        if (fra != 0 && exp_shift == exp_mask) {
            auto s = buf;
            if (fra & (uint_fast64_t(1u) << (FT::mant - 1u))) {
                if (sign) {
                    if (!s || *len < 9) { *len = 9u; return false; }
                    *s++ = Cy('-');
                } else {
                    if (!s || *len < 3) { *len = 3u; return false; }
                }
                *s++ = Cy(upp ? 'N' : 'n');
                *s++ = Cy(upp ? 'A' : 'a');
                *s++ = Cy(upp ? 'N' : 'n');
                if (sign) {
                    *s++ = Cy('(');
                    *s++ = Cy(upp ? 'I' : 'i');
                    *s++ = Cy(upp ? 'N' : 'n');
                    *s++ = Cy(upp ? 'D' : 'd');
                    *s++ = Cy(')');
                }
            } else {
                if (sign) {
                    if (!s || *len < 10) { *len = 10u; return false; }
                    *s++ = Cy('-');
                } else {
                    if (!s || *len < 9) { *len = 9u; return false; }
                }
                *s++ = Cy(upp ? 'N' : 'n');
                *s++ = Cy(upp ? 'A' : 'a');
                *s++ = Cy(upp ? 'N' : 'n');
                *s++ = Cy('(');
                *s++ = Cy(upp ? 'S' : 's');
                *s++ = Cy(upp ? 'N' : 'n');
                *s++ = Cy(upp ? 'A' : 'a');
                *s++ = Cy(upp ? 'N' : 'n');
                *s++ = Cy(')');
            }
            *len = s - buf;
            return true;
        }

        size_t uuc;
        size_t fuc;
        predict<FTy, UTy, bN>(exp_shift, precision, fmt, uuc, fuc);

        bool ret;
        if (uuc < 1 && fuc < 1) {
            ret = dftos<FTy, UTy, Cy, bN, 1, 1>(fra, exp_shift, sign, fmt, round, precision, buf, len);
        } else if (uuc < 2 && fuc < 2) {
            ret = dftos<FTy, UTy, Cy, bN, 2, 2>(fra, exp_shift, sign, fmt, round, precision, buf, len);
        } else if (uuc < 4 && fuc < 4) {
            ret = dftos<FTy, UTy, Cy, bN, 4, 4>(fra, exp_shift, sign, fmt, round, precision, buf, len);
        } else if (uuc < 8 && fuc < 8) {
            ret = dftos<FTy, UTy, Cy, bN, 8, 8>(fra, exp_shift, sign, fmt, round, precision, buf, len);
        } else if (uuc < 16 && fuc < 16) {
            ret = dftos<FTy, UTy, Cy, bN, 16, 16>(fra, exp_shift, sign, fmt, round, precision, buf, len);
        } else if (uuc < 32 && fuc < 32) {
            ret = dftos<FTy, UTy, Cy, bN, 32, 32>(fra, exp_shift, sign, fmt, round, precision, buf, len);
        } else {
            ret = dftos<FTy, UTy, Cy, bN, 64, 64>(fra, exp_shift, sign, fmt, round, precision, buf, len);
        }
        return ret;
    }

    template <typename FTy, typename UTy, typename Cy, size_t bN>
    int stof_base(
        const Cy* str, size_t len, int fmt, int round, FTy* out, const Cy** p)
    {
        auto s = str;
        auto se = str + len;

        uint_fast8_t sign;
        if (*s == Cy('-')) {
            ++s;
            if (s >= se) {
                return UCR_FAILED;
            }
            sign = 1;
        } else {
            sign = 0;
        }

        if ((*s == Cy('n') || *s == Cy('N')) &&
            se - s >= 3 &&
            (*(s + 1) == Cy('a') || *(s + 1) == Cy('A')) &&
            (*(s + 2) == Cy('n') || *(s + 2) == Cy('N')))
        {
            s += 3;
            // nan
            if (s >= se || *s != Cy('(')) {
                *p = s;
                nand<FTy>(sign, 0, out);
                return UCR_OK;
            }

            auto ss = s;

            // nan()
            ++s;
            if (s < se) {
                if ((*s == Cy('i') || *s == Cy('I')) &&
                    se - s >= 4 &&
                    (*(s + 1) == Cy('n') || *(s + 1) == Cy('N')) &&
                    (*(s + 2) == Cy('d') || *(s + 2) == Cy('D')) &&
                    *(s + 3) == Cy(')'))
                {
                    *p = s + 4;
                    nand<FTy>(sign, 1, out);
                    return UCR_OK;
                }

                if ((*s == Cy('s') || *s == Cy('S')) &&
                    se - s >= 5 &&
                    (*(s + 1) == Cy('n') || *(s + 1) == Cy('N')) &&
                    (*(s + 2) == Cy('a') || *(s + 2) == Cy('A')) &&
                    (*(s + 3) == Cy('n') || *(s + 3) == Cy('N')) &&
                    *(s + 4) == Cy(')'))
                {
                    *p = s + 5;
                    nand<FTy>(sign, 2, out);
                    return UCR_OK;
                }

                for (; s < se; ++s) {
                    if (*s == Cy(')')) {
                        break;
                    }
                }

                if (s < se) {
                    *p = s + 1;
                    nand<FTy>(sign, 0, out);
                    return UCR_OK;
                }
            }

            *p = ss;
            nand<FTy>(sign, 0, out);
            return UCR_OK;
        }

        for (; s < se; ++s) {
            if (*s != Cy('0')) {
                break;
            }
        }
        if (s == se) {
            zerd(sign, out);
            *p = s;
            return true;
        }

        if (fmt & FF_SCI) {
            return scistof<FTy, UTy, Cy, bN>(s, se - s, sign, fmt, round, out, p);
        }
        return norstof<FTy, UTy, Cy, bN>(s, se - s, sign, round, out, p);
    }

}
}

#endif  // UTILS_FLOAT_CONV_INTERNAL_HPP_