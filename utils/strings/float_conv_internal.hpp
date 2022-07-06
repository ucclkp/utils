// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STRINGS_FLOAT_CONV_INTERNAL_HPP_
#define UTILS_STRINGS_FLOAT_CONV_INTERNAL_HPP_

#include "utils/strings/float_conv_big_num.hpp"
#include "utils/strings/uint_exp.hpp"

#define SELECT_IF(u, f)  \
    if (uuc <= u && fuc <= f)

#define dftos_buf_INVOKE(u, f)  \
    return dftos<FTy, UTy, Cy, bN, u, f>(fra, exp_shift, sign, fmt, round, precision, buf, len);

#define dftos_str_INVOKE(u, f)  \
    {dftos<FTy, UTy, Cy, bN, u, f>(fra, exp_shift, sign, fmt, round, precision, out); return;}

#define dscistof_INVOKE(u, f)  \
    return dscistof<FTy, UTy, Cy, bN, u, f>(str, len, di, de, sign, exp, fmt, round, out);

#define dnorstof_INVOKE(u, f)  \
    return dnorstof<FTy, UTy, Cy, bN, u, f>(str, len, di, sign, round, out, p);

#define dftos_buf_IF_INVOKE(u, f)  SELECT_IF(u, f) dftos_buf_INVOKE(u, f)
#define dftos_str_IF_INVOKE(u, f)  SELECT_IF(u, f) dftos_str_INVOKE(u, f)
#define dscistof_IF_INVOKE(u, f)   SELECT_IF(u, f) dscistof_INVOKE(u, f)
#define dnorstof_IF_INVOKE(u, f)   SELECT_IF(u, f) dnorstof_INVOKE(u, f)


namespace utl {
namespace internal {

    template <typename FTy>
    void ftodf(
        FTy val,
        uint_fast8_t& sign, uint_fast32_t& exp_shift, uint_e<FTy>& fra)
    {
        using FT = FloatTraits_base<FTy>;
        using uint_e = uint_e<FTy>;
        constexpr auto bexp = FT::bexp();
        // 根据 [expr.const]/2，移位运算符不能出现在 constexpr 表达式中
        const auto exp_mask = (uint_fast32_t(1u) << bexp) - 1u;

        uint_e uv;
        uv.from(val);

        fra = uv & (uint_e::bitAt(FT::mant) - 1u);
        exp_shift = ((uv >> FT::sexp()) & exp_mask).uf32();
        sign = uv.getBit(FT::ssign());
    }

    template <typename FTy>
    void dftof(
        uint_fast8_t sign, uint_fast32_t exp_shift, const uint_e<FTy>& fra,
        uint_fast8_t hidden_bit, FTy* val)
    {
        using FT = FloatTraits_base<FTy>;
        using uint_e = uint_e<FTy>;

        uint_e uv(fra);
        uv |= uint_e(exp_shift) << FT::sexp();
        if (FT::hb_present() && hidden_bit) {
            uv.setBit(FT::mant);
        }
        if (sign) {
            uv.setBit(FT::ssign());
        }

        uv.to(val);
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
        using uint_e = uint_e<FTy>;
        constexpr auto mant_1 = FT::mant - 1u;
        constexpr auto bexp = FT::bexp();
        const auto exp_mask = (uint_fast32_t(1u) << bexp) - 1u;

        if (type == 2) {
            // nan(snan)/-nan(snan)
            dftof(sign, exp_mask, uint_e(1u), 0, out);
        } else if (type == 1 || sign) {
            // -nan(ind)
            dftof(sign, exp_mask, uint_e::bitAt(mant_1), 0, out);
        } else {
            // nan
            dftof(0, exp_mask, uint_e::bitAt(mant_1), 0, out);
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
                if (round == FCR_CEIL) {
                    if (!sign && !bf->isZero()) {
                        bu->add(1);
                    }
                } else if (round == FCR_FLOOR) {
                    if (sign && !bf->isZero()) {
                        bu->add(1);
                    }
                } else if (round == FCR_ZERO) {
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
                if (round == FCR_CEIL) {
                    if (!sign && !bf->isZero()) {
                        bu->add(1);
                    }
                } else if (round == FCR_FLOOR) {
                    if (sign && !bf->isZero()) {
                        bu->add(1);
                    }
                } else if (round == FCR_ZERO) {
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
        int_fast32_t exp_shf, uint_fast8_t sign,
        int& fmt, int round, int& precision,
        BigUInt_bN<FTy, UTy, bN, UUnit>& bi,
        BigFloat_bN<FTy, UTy, bN, FUnit>& bf)
    {
        using BigUInt = BigUInt_bN<FTy, UTy, bN, UUnit>;
        using BigFloat = BigFloat_bN<FTy, UTy, bN, FUnit>;
        using FT = FloatTraits_base<FTy>;

        if (exp_shf != 0) {
            if (fmt & FCF_SCI) {
                if (fmt & FCF_FIX) {
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
                        fmt &= ~FCF_SCI;
                        precision = P - 1 - X;
                        bn_nor_round(&bi, &bf, sign, round, precision);
                    } else {
                        fmt &= ~FCF_FIX;
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
            if (fmt & FCF_SCI) {
                if (fmt & FCF_FIX) {
                    int P = precision;
                    if (P == 0) P = 1;

                    BigFloat bf2 = bf;
                    bn_sci_round<FTy, UTy, bN, UUnit, FUnit>(
                        nullptr, &bf2, sign, round, precision);

                    int X = -(int(bf2.getSignDigitPos()) + 1);
                    if (X >= -4) {
                        fmt &= ~FCF_SCI;
                        precision = P - 1 - X;
                        bn_nor_round<FTy, UTy, bN, UUnit, FUnit>(
                            nullptr, &bf, sign, round, precision);
                    } else {
                        fmt &= ~FCF_FIX;
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
        const uint_e<FTy>& fra, int_fast32_t exp_shf,
        BigUInt_bN<FTy, UTy, bN, UUnit>& bi,
        BigFloat_bN<FTy, UTy, bN, FUnit>& bf,
        int* bi_available)
    {
        using BigUInt = BigUInt_bN<FTy, UTy, bN, UUnit>;
        using BigFloat = BigFloat_bN<FTy, UTy, bN, FUnit>;
        using FT = FloatTraits_base<FTy>;
        using uint_e = uint_e<FTy>;
        constexpr auto maxe_1 = FT::maxe - 1u;

        if (exp_shf != 0) {
            int_fast32_t exp = exp_shf - int_fast32_t(maxe_1);

            BigUInt bi_tmp(5);
            for (int_fast32_t i = exp; i >= 0; --i) {
                bi_tmp.mul2();
                if (i == 0 || (i <= FT::mant && fra.getBit(FT::mant - i))) {
                    bi.add(bi_tmp);
                }
            }

            BigFloat bf_tmp(1);
            for (int_fast32_t i = exp + 1; i <= int_fast32_t(FT::mant); ++i) {
                bf_tmp.div2();
                if (i == 0 || (i > 0 && fra.getBit(FT::mant - i))) {
                    bf.add(bf_tmp);
                }
            }
            *bi_available = 1;
        } else {
            int_fast32_t exp = 1 - int_fast32_t(maxe_1);

            BigFloat bf_tmp(1);
            for (int_fast32_t i = exp + 1; i <= int_fast32_t(FT::mant); ++i) {
                bf_tmp.div2();
                if (i > 0 && fra.getBit(FT::mant - i)) {
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

        if (fmt & FCF_SCI) {
            if (bi_available && !bi.isZero()) {
                auto exp = bi.getSignDigitPos();
                size_t len = *u_len;
                if (fmt & FCF_UPP) {
                    bi.template toChars<true>(ubuf, &len, exp);
                } else {
                    bi.template toChars<false>(ubuf, &len, exp);
                }
                *u_len = len;

                size_t i = 0;
                size_t _len;
                if (fmt & FCF_EXA) {
                    _len = exp;
                } else {
                    _len = exp >= size_t(precision) ? size_t(precision) : exp;
                }

                len = _len >= *f_len ? *f_len : _len;
                if (len) {
                    if (fmt & FCF_UPP) {
                        bi.template toChars<true>(fbuf, &len, exp - len);
                    } else {
                        bi.template toChars<false>(fbuf, &len, exp - len);
                    }
                    i += len;
                }

                size_t buf_rem = *f_len - len;
                if (fmt & FCF_EXA) {
                    len = buf_rem;
                } else {
                    size_t target_rem = size_t(precision) - len;
                    len = buf_rem >= target_rem ? target_rem : buf_rem;
                }

                if (len) {
                    if (fmt & FCF_UPP) {
                        bf.template toChars<true>(fbuf + i, &len, 0);
                    } else {
                        bf.template toChars<false>(fbuf + i, &len, 0);
                    }
                    i += len;
                }
                *f_len = i;
                *r_exp = exp;
            } else {
                auto exp = bf.getSignDigitPos();
                size_t len = *u_len;
                if (fmt & FCF_UPP) {
                    bf.template toChars<true>(ubuf, &len, exp);
                } else {
                    bf.template toChars<false>(ubuf, &len, exp);
                }
                *u_len = len;

                size_t i = 0;
                if (fmt & FCF_EXA) {
                    len = *f_len;
                } else {
                    len = size_t(precision) > *f_len ? *f_len : size_t(precision);
                }

                if (fmt & FCF_UPP) {
                    bf.template toChars<true>(fbuf, &len, exp + 1);
                } else {
                    bf.template toChars<false>(fbuf, &len, exp + 1);
                }
                i += len;
                *f_len = i;
                *r_exp = exp + 1;
            }
        } else {
            if (bi_available) {
                size_t i_cs_len = *u_len;
                if (fmt & FCF_UPP) {
                    bi.template toChars<true>(ubuf, &i_cs_len, 0);
                } else {
                    bi.template toChars<false>(ubuf, &i_cs_len, 0);
                }
                *u_len = i_cs_len;

                size_t f_cs_len;
                if (fmt & FCF_EXA) {
                    f_cs_len = *f_len;
                } else {
                    f_cs_len = size_t(precision) > *f_len ? *f_len : size_t(precision);
                }

                if (fmt & FCF_UPP) {
                    bf.template toChars<true>(fbuf, &f_cs_len, 0);
                } else {
                    bf.template toChars<false>(fbuf, &f_cs_len, 0);
                }
                *f_len = f_cs_len;
            } else {
                if (!(fmt & FCF_EXA) && precision == 0) {
                    if (*u_len > 0) {
                        *ubuf = Cy('0');
                        *u_len = 1;
                        *f_len = 0;
                    }
                    return;
                }

                *u_len = 0;

                size_t f_cs_len;
                if (fmt & FCF_EXA) {
                    f_cs_len = *f_len;
                } else {
                    f_cs_len = size_t(precision) > *f_len ? *f_len : size_t(precision);
                }

                if (fmt & FCF_UPP) {
                    bf.template toChars<true>(fbuf, &f_cs_len, 0);
                } else {
                    bf.template toChars<false>(fbuf, &f_cs_len, 0);
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

        if (!(fmt & FCF_EXA) && precision == 0) {
            if (fmt & FCF_DIG) {
                out->push_back(Cy('.'));
            }
            return;
        }

        out->push_back(Cy('.'));

        if ((fmt & FCF_EXA) || (fmt & FCF_NTZ)) {
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

        if (!(fmt & FCF_EXA) && precision == 0) {
            if (fmt & FCF_DIG) {
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

        if ((fmt & FCF_EXA) || (fmt & FCF_NTZ)) {
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

    template <typename FTy, typename Cy>
    bool to_special_s(
        const uint_e<FTy>& fra, int_fast32_t exp_shf, uint_fast8_t sign,
        int fmt, int precision, std::basic_string<Cy>* out)
    {
        using FT = FloatTraits_base<FTy>;
        constexpr auto bexp = FT::bexp();
        // 根据 [expr.const]/2，移位运算符不能出现在 constexpr 表达式中
        const auto exp_mask = (uint_fast32_t(1u) << bexp) - 1u;

        bool upp = !!(fmt & FCF_UPP);
        if (fra == 0 && exp_shf == 0) {
            cpnos<Cy>(sign, nullptr, 0, nullptr, 0, precision, fmt, out);

            if ((fmt & FCF_SCI) && !(fmt & FCF_FIX)) {
                if (fmt & FCF_HEX2) {
                    out->push_back(upp ? 'P' : 'p');
                } else if (fmt & FCF_HEX) {
                    out->push_back(upp ? 'S' : 's');
                } else {
                    out->push_back(upp ? 'E' : 'e');
                }
                out->push_back(Cy('+'));
                if (!(fmt & FCF_ENZ)) {
                    out->push_back(Cy('0'));
                }
                out->push_back(Cy('0'));
            }
            return true;
        }

        if (fra == 0 && exp_shf == exp_mask) {
            if (sign) {
                out->push_back(Cy('-'));
            }
            out->push_back(Cy(upp ? 'I' : 'i'));
            out->push_back(Cy(upp ? 'N' : 'n'));
            out->push_back(Cy(upp ? 'F' : 'f'));
            return true;
        }
        if (fra != 0 && exp_shf == exp_mask) {
            if (fra.getBit(FT::mant - 1u)) {
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
            return true;
        }

        return false;
    }

    template <typename FTy, typename Cy>
    bool to_special_s(
        const uint_e<FTy>& fra, int_fast32_t exp_shf, uint_fast8_t sign,
        int fmt, int precision, Cy* buf, size_t* len, bool* ret_val)
    {
        using FT = FloatTraits_base<FTy>;
        constexpr auto bexp = FT::bexp();
        // 根据 [expr.const]/2，移位运算符不能出现在 constexpr 表达式中
        const auto exp_mask = (uint_fast32_t(1u) << bexp) - 1u;

        bool upp = !!(fmt & FCF_UPP);
        if (fra == 0 && exp_shf == 0) {
            size_t _len = *len;
            bool ret = cpnos<Cy>(sign, nullptr, 0, nullptr, 0, precision, fmt, buf, &_len);

            if (fmt & FCF_SCI && !(fmt & FCF_FIX)) {
                Cy* s, * se;
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
                    *ret_val = false;
                    return true;
                }

                if (fmt & FCF_HEX2) {
                    *s++ = (upp ? 'P' : 'p');
                } else if (fmt & FCF_HEX) {
                    *s++ = (upp ? 'S' : 's');
                } else {
                    *s++ = (upp ? 'E' : 'e');
                }
                *s++ = (Cy('+'));
                if (!(fmt & FCF_ENZ)) {
                    *s++ = (Cy('0'));
                }
                *s = (Cy('0'));

                *len = _len + 4;
            } else {
                *len = _len;
            }

            *ret_val = ret;
            return true;
        }

        if (fra == 0 && exp_shf == exp_mask) {
            auto s = buf;
            if (sign) {
                if (!s || *len < 4) { *len = 4u; *ret_val = false; return true; }
                *s++ = Cy('-');
            } else {
                if (!s || *len < 3) { *len = 3u; *ret_val = false; return true; }
            }
            *s++ = Cy(upp ? 'I' : 'i');
            *s++ = Cy(upp ? 'N' : 'n');
            *s++ = Cy(upp ? 'F' : 'f');
            *len = s - buf;
            *ret_val = true;
            return true;
        }
        if (fra != 0 && exp_shf == exp_mask) {
            auto s = buf;
            if (fra.getBit(FT::mant - 1u)) {
                if (sign) {
                    if (!s || *len < 9) { *len = 9u; *ret_val = false; return true; }
                    *s++ = Cy('-');
                } else {
                    if (!s || *len < 3) { *len = 3u; *ret_val = false; return true; }
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
                    if (!s || *len < 10) { *len = 10u; *ret_val = false; return true; }
                    *s++ = Cy('-');
                } else {
                    if (!s || *len < 9) { *len = 9u; *ret_val = false; return true; }
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
            *ret_val = true;
            return true;
        }

        return false;
    }

    template <typename FTy, typename Cy>
    bool from_special_s(
        uint_fast8_t sign, const Cy*& s, const Cy* se,
        FTy* out, const Cy** p, int* ret_val)
    {
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
                *ret_val = SCR_OK;
                return true;
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
                    *ret_val = SCR_OK;
                    return true;
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
                    *ret_val = SCR_OK;
                    return true;
                }

                for (; s < se; ++s) {
                    if (*s == Cy(')')) {
                        break;
                    }
                }

                if (s < se) {
                    *p = s + 1;
                    nand<FTy>(sign, 0, out);
                    *ret_val = SCR_OK;
                    return true;
                }
            }

            *p = ss;
            nand<FTy>(sign, 0, out);
            *ret_val = SCR_OK;
            return true;
        }

        for (; s < se; ++s) {
            if (*s != Cy('0')) {
                break;
            }
        }
        if (s == se) {
            zerd(sign, out);
            *p = s;
            *ret_val = SCR_OK;
            return true;
        }

        return false;
    }

    template <typename FTy, typename UTy, typename Cy, size_t bN, size_t UUnit, size_t FUnit>
    void dftos(
        const uint_e<FTy>& fra, int_fast32_t exp_shf, uint_fast8_t sign,
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

        if (!(fmt & FCF_EXA)) {
            bn_round(
                exp_shf, sign,
                fmt, round, precision,
                bi, bf);
        }

        if (fmt & FCF_SCI) {
            Cy ubuf[1];
            size_t u_len = 1u;

            constexpr size_t fb_size = (UUnit + FUnit) * UCTraits::udig;
            Cy fbuf[fb_size];
            size_t f_len = fb_size;

            size_t exp;
            bntocp(fmt, precision, bi_available, bi, bf, ubuf, &u_len, fbuf, &f_len, &exp);
            cpnos(sign, ubuf, u_len, fbuf, f_len, precision, fmt, out);

            bool upp = !!(fmt & FCF_UPP);
            if (fmt & FCF_HEX) {
                out->push_back(Cy(upp ? 'S' : 's'));
            } else {
                out->push_back(Cy(upp ? 'E' : 'e'));
            }

            if (bi_available && !bi.isZero()) {
                out->push_back(Cy('+'));
            } else {
                out->push_back(Cy('-'));
            }

            std::basic_string<Cy> _str;
            itos(exp, &_str);
            if (!(fmt & FCF_ENZ) && exp < 10) {
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
        const uint_e<FTy>& fra, int_fast32_t exp_shf, uint_fast8_t sign,
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

        if (!(fmt & FCF_EXA)) {
            bn_round(
                exp_shf, sign,
                fmt, round, precision,
                bi, bf);
        }

        if (fmt & FCF_SCI) {
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
                s = ret ? buf + _len : se;
            } else {
                s = nullptr;
                se = nullptr;
            }
            act_len += _len;

            if (s && se - s >= 2) {
                bool upp = !!(fmt & FCF_UPP);
                if (fmt & FCF_HEX) {
                    *s++ = Cy(upp ? 'S' : 's');
                } else {
                    *s++ = Cy(upp ? 'E' : 'e');
                }

                if (bi_available && !bi.isZero()) {
                    *s++ = Cy('+');
                } else {
                    *s++ = Cy('-');
                }
            } else {
                s = se;
            }
            act_len += 2;

            if (!(fmt & FCF_ENZ) && exp < 10) {
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
        using uint_e = uint_e<FTy>;
        using FT = FloatTraits_base<FTy>;
        constexpr size_t kSpace = 8u;
        constexpr auto mant_1 = FT::mant - 1u;
        constexpr auto maxe_1 = FT::maxe - 1u;
        constexpr auto mine_c = FT::mine_c();

        int_fast32_t exp;
        if (bu.isZero() && bf.isZero()) {
            exp = -int_fast32_t(maxe_1);
        } else {
            exp = 0;
        }

        uint_e fra = 0;
        bool update_exp = bu.isZero();
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
                fra.setBit(mant_1 + kSpace);
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

        uint_fast32_t b_shf = exp;
        for (size_t j = 0; j < mine_c + kSpace; ++j) {
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
                        // dnorm
                        b_shf = -int_fast32_t(maxe_1) - exp;
                        exp = -int_fast32_t(maxe_1);
                        update_exp = false;
                    }
                }

                if (b_shf <= mant_1 + kSpace && !update_exp) {
                    fra.setBit(mant_1 + kSpace - b_shf);
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

        if (exp < -int_fast32_t(maxe_1)) {
            exp = -int_fast32_t(maxe_1);
        }

        if (round == FCR_CEIL) {
            if (!sign) {
                if (fra & (uint_e::bitAt(kSpace) - 1u)) {
                    fra += uint_e::bitAt(kSpace);
                }
            }
        } else if (round == FCR_FLOOR) {
            if (sign) {
                if (fra & (uint_e::bitAt(kSpace) - 1u)) {
                    fra += uint_e::bitAt(kSpace);
                }
            }
        } else if (round == FCR_ZERO) {
        } else {
            if (fra.getBit(kSpace - 1)) {
                fra += uint_e::bitAt(kSpace);
            }
        }
        fra >>= kSpace;
        if (fra.getBit(FT::mant)) {
            fra.clearBit(FT::mant);
            ++exp;
        }

        uint_fast32_t exp_shift = exp + int_fast32_t(maxe_1);
        if (!FT::has_denorm && exp_shift == 0) {
            fra.zero();
        }

        dftof(sign, exp_shift, fra, exp_shift != 0 ? 1 : 0, out);
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
            if (ret != SCR_OK) {
                if (ret == SCR_OF) {
                    infd(sign, out);
                }
                return ret;
            }
        } else if (di == s) {
            ret = bf.fromChars(s + 1, len - 1, &_p);
            if (ret != SCR_OK) {
                return ret;
            }

            if (bf.getSignDigitPos() > FT::mine_adc()) {
                zerd(sign, out);
                return SCR_OF;
            }
        } else {
            ret = bu.fromChars(s, di - s, &_p);
            if (ret != SCR_OK) {
                if (ret == SCR_OF) {
                    infd(sign, out);
                }
                return ret;
            }
            if (_p == di) {
                auto _len = (s + len - di) - 1;
                if (_len) {
                    bf.fromChars(di + 1, _len, &_p);
                } else {
                    _p = di + 1;
                }
            }
        }

        if (!btof(bu, bf, sign, round, out)) {
            infd(sign, out);
            return SCR_OF;
        }

        *p = _p;
        return SCR_OK;
    }

    template <typename FTy, typename UTy, typename Cy, size_t bN, size_t UUnit, size_t FUnit>
    int dscistof(
        const Cy* str, size_t len, const Cy* di, const Cy* de,
        uint_fast8_t sign, int_fast32_t exp, int fmt, int round, FTy* out)
    {
        using BigUInt = BigUInt_bN<FTy, UTy, bN, UUnit>;
        using BigFloat = BigFloat_bN<FTy, UTy, bN, FUnit>;
        using FT = FloatTraits<FTy, bN>;

        BigUInt bu(0);
        BigFloat bf(0);
        const Cy* _p;

        int ret;
        intptr_t fl, ul;
        if (di) {
            fl = de - di - 1;
            ul = di - str;

            if (exp >= fl) {
                ret = bu.fromChars(str, de - str, exp - fl, true, &_p);
                if (ret != SCR_OK) {
                    if (ret == SCR_OF) {
                        infd(sign, out);
                    }
                    return ret;
                }
            } else if (-exp < ul) {
                auto s = di + 1 + exp;
                ret = bu.fromChars(str, s - str, 0, s > di, &_p);
                if (ret != SCR_OK) {
                    if (ret == SCR_OF) {
                        infd(sign, out);
                    }
                    return ret;
                }
                if (_p == s) {
                    bf.fromChars(s, de - s, 0, s <= di, &_p);
                }
            } else {
                ret = bf.fromChars(str, de - str, -(exp + ul), true, &_p);
                if (ret != SCR_OK) {
                    return ret;
                }

                if (bf.getSignDigitPos() > FT::mine_adc()) {
                    zerd(sign, out);
                    return SCR_OF;
                }
            }
        } else {
            fl = 0;
            ul = de - str;

            if (exp >= fl) {
                ret = bu.fromChars(str, de - str, exp - fl, false, &_p);
                if (ret != SCR_OK) {
                    if (ret == SCR_OF) {
                        infd(sign, out);
                    }
                    return ret;
                }
            } else if (-exp < ul) {
                auto s = de + exp;
                ret = bu.fromChars(str, s - str, 0, false, &_p);
                if (ret != SCR_OK) {
                    if (ret == SCR_OF) {
                        infd(sign, out);
                    }
                    return ret;
                }
                if (_p == s) {
                    bf.fromChars(s, de - s, 0, false, &_p);
                }
            } else {
                ret = bf.fromChars(str, de - str, -(exp + ul), false, &_p);
                if (ret != SCR_OK) {
                    return ret;
                }

                if (bf.getSignDigitPos() > FT::mine_adc()) {
                    zerd(sign, out);
                    return SCR_OF;
                }
            }
        }

        if (!btof(bu, bf, sign, round, out)) {
            infd(sign, out);
            return SCR_OF;
        }

        return SCR_OK;
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

        dnorstof_IF_INVOKE(1, 1);
        dnorstof_IF_INVOKE(2, 2);
        dnorstof_IF_INVOKE(4, 4);
        dnorstof_IF_INVOKE(8, 8);
        dnorstof_IF_INVOKE(16, 16);
        dnorstof_IF_INVOKE(32, 32);
        dnorstof_IF_INVOKE(64, 64);
        dnorstof_IF_INVOKE(128, 128);
        dnorstof_IF_INVOKE(256, 256);
        dnorstof_IF_INVOKE(512, 512);
        dnorstof_INVOKE(1024, 1024);
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
            } else if constexpr (bN == 16) {
                if (*s == Cy('s') || *s == Cy('S')) {
                    dp = s;
                    break;
                }
            } else {
                return SCR_FAIL;
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
            if (s >= se) {
                return SCR_FAIL;
            }
            if (*s == Cy('+')) {
                ++s;
                if (s >= se) {
                    return SCR_FAIL;
                }
            }

            if (!stoi(s, se - s, &exp, 10, &s)) {
                return SCR_FAIL;
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

        size_t uuc, fuc;
        if (exp >= fl) {
            uuc = ((de - str) + (exp - fl) + UCTraits::udig - 1) / UCTraits::udig + 1;
            fuc = 1;
        } else if (-exp <= ul) {
            auto _s = di1 + 1 + exp;
            uuc = (_s - str + UCTraits::udig - 1) / UCTraits::udig + 1;
            fuc = (de - _s + UCTraits::udig - 1) / UCTraits::udig + 1;
        } else {
            uuc = 1;
            fuc = ((de - str) - (exp + ul) + UCTraits::udig - 1) / UCTraits::udig + 1;
        }

        *p = s;

        dscistof_IF_INVOKE(1, 1);
        dscistof_IF_INVOKE(2, 2);
        dscistof_IF_INVOKE(4, 4);
        dscistof_IF_INVOKE(8, 8);
        dscistof_IF_INVOKE(16, 16);
        dscistof_IF_INVOKE(32, 32);
        dscistof_IF_INVOKE(64, 64);
        dscistof_IF_INVOKE(128, 128);
        dscistof_IF_INVOKE(256, 256);
        dscistof_IF_INVOKE(512, 512);
        dscistof_INVOKE(1024, 1024);
    }

    template <typename FTy, typename UTy, size_t bN>
    void predict(
        uint_fast32_t exp_shift, int precision, int fmt, size_t& uuc, size_t& fuc)
    {
        using UCT = UCeilTraits<UTy, bN>;
        using FT = FloatTraits_base<FTy>;
        constexpr auto maxe_1 = FT::maxe - 1u;

        if (fmt & FCF_EXA) {
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
        using uint_e = uint_e<FTy>;

        uint_e fra;
        uint_fast32_t exp_shift;
        uint_fast8_t sign;
        ftodf(val, sign, exp_shift, fra);

        if (to_special_s(fra, exp_shift, sign, fmt, precision, out)) {
            return;
        }

        size_t uuc;
        size_t fuc;
        predict<FTy, UTy, bN>(exp_shift, precision, fmt, uuc, fuc);

        dftos_str_IF_INVOKE(1, 1);
        dftos_str_IF_INVOKE(2, 2);
        dftos_str_IF_INVOKE(4, 4);
        dftos_str_IF_INVOKE(8, 8);
        dftos_str_IF_INVOKE(16, 16);
        dftos_str_IF_INVOKE(32, 32);
        dftos_str_IF_INVOKE(64, 64);
        dftos_str_IF_INVOKE(128, 128);
        dftos_str_IF_INVOKE(256, 256);
        dftos_str_IF_INVOKE(512, 512);
        dftos_str_INVOKE(1024, 1024);
    }

    template <typename FTy, typename UTy, typename Cy, size_t bN>
    bool ftos_base(
        FTy val, int fmt, int round, int precision, Cy* buf, size_t* len)
    {
        using FT = FloatTraits_base<FTy>;
        using uint_e = uint_e<FTy>;

        uint_e fra;
        uint_fast32_t exp_shift;
        uint_fast8_t sign;
        ftodf(val, sign, exp_shift, fra);

        bool ret_val;
        if (to_special_s(fra, exp_shift, sign, fmt, precision, buf, len, &ret_val)) {
            return ret_val;
        }

        size_t uuc;
        size_t fuc;
        predict<FTy, UTy, bN>(exp_shift, precision, fmt, uuc, fuc);

        dftos_buf_IF_INVOKE(1, 1);
        dftos_buf_IF_INVOKE(2, 2);
        dftos_buf_IF_INVOKE(4, 4);
        dftos_buf_IF_INVOKE(8, 8);
        dftos_buf_IF_INVOKE(16, 16);
        dftos_buf_IF_INVOKE(32, 32);
        dftos_buf_IF_INVOKE(64, 64);
        dftos_buf_IF_INVOKE(128, 128);
        dftos_buf_IF_INVOKE(256, 256);
        dftos_buf_IF_INVOKE(512, 512);
        dftos_buf_INVOKE(1024, 1024);
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
                return SCR_FAIL;
            }
            sign = 1;
        } else {
            sign = 0;
        }

        int ret_val;
        if (from_special_s(sign, s, se, out, p, &ret_val)) {
            return ret_val;
        }

        if (fmt & FCF_SCI) {
            return scistof<FTy, UTy, Cy, bN>(s, se - s, sign, fmt, round, out, p);
        }
        return norstof<FTy, UTy, Cy, bN>(s, se - s, sign, round, out, p);
    }

    template <typename FTy>
    void fra_round(
        uint_e<FTy>& fra, uint_fast32_t exp_shift, uint_fast8_t sign,
        int precision, int round, uint_fast8_t* fu)
    {
        using FT = FloatTraits_base<FTy>;

        uint_fast8_t u1 = exp_shift == 0 ? 0 : 1;
        if (FT::mant > precision * 4) {
            if (precision > 0) {
                if (round == FCR_CEIL) {
                    if (!sign && !fra.isZeroAfter(FT::mant - precision * 4)) {
                        fra.incAt(FT::mant - precision * 4);
                        u1 += fra.getBit(FT::mant);
                    }
                } else if (round == FCR_FLOOR) {
                    if (sign && !fra.isZeroAfter(FT::mant - precision * 4)) {
                        fra.incAt(FT::mant - precision * 4);
                        u1 += fra.getBit(FT::mant);
                    }
                } else if (round == FCR_ZERO) {
                } else {
                    uint_fast8_t d_1;
                    bool has_next = FT::mant >= (precision + 1) * 4;
                    if (has_next) {
                        d_1 = (fra >> (FT::mant - (precision + 1) * 4)).uf8() & 0xFu;
                    } else {
                        d_1 = (fra << ((precision + 1) * 4) - FT::mant).uf8() & 0xFu;
                    }
                    if (d_1 > 8u) {
                        fra.incAt(FT::mant - precision * 4);
                        u1 += fra.getBit(FT::mant);
                    } else if (d_1 == 8u) {
                        auto d = (fra >> (FT::mant - precision * 4)).uf8() & 0xFu;
                        if (!fra.isZeroAfter(has_next ? (FT::mant - (precision + 1) * 4) : 0) || d % 2 != 0) {
                            fra.incAt(FT::mant - precision * 4);
                            u1 += fra.getBit(FT::mant);
                        }
                    }
                }
            } else {
                if (round == FCR_CEIL) {
                    if (!sign && fra) {
                        ++u1;
                    }
                } else if (round == FCR_FLOOR) {
                    if (sign && fra) {
                        ++u1;
                    }
                } else if (round == FCR_ZERO) {
                } else {
                    uint_fast8_t d_1;
                    bool has_next = FT::mant >= 4;
                    if (has_next) {
                        d_1 = (fra >> (FT::mant - 4)).uf8() & 0xFu;
                    } else {
                        d_1 = (fra << (4 - FT::mant)).uf8() & 0xFu;
                    }
                    if (d_1 > 8u) {
                        ++u1;
                    } else if (d_1 == 8u) {
                        auto d = u1;
                        if (!fra.isZeroAfter(has_next ? FT::mant - 4 : 0) || d % 2 != 0) {
                            ++u1;
                        }
                    }
                }
            }
        }

        *fu = u1;
    }

    template <typename FTy, typename Cy>
    void ftos_hex2_base(
        FTy val, int fmt, int round, int precision, std::basic_string<Cy>* out)
    {
        using FT = FloatTraits_base<FTy>;
        using uint_e = uint_e<FTy>;
        constexpr auto maxe_1 = FT::maxe - 1u;
        constexpr size_t bN = 16;
        constexpr size_t bbit = 4;

        uint_e fra;
        uint_fast32_t exp_shift;
        uint_fast8_t sign;
        ftodf(val, sign, exp_shift, fra);

        if (to_special_s(fra, exp_shift, sign, fmt, precision, out)) {
            return;
        }

        uint_fast8_t u1;
        fra_round(fra, exp_shift, sign, precision, round, &u1);

        bool upp = !!(fmt & FCF_UPP);
        const char* lut = upp ? kDigitCharUpper : kDigitCharLower;

        Cy ubuf[1] = { Cy(lut[u1]) };
        Cy fbuf[(FT::mant + bbit - 1u) / bbit];
        size_t bit_count = FT::mant;

        if (fmt & FCF_EXA) {
            precision = (std::numeric_limits<int>::max)();
        }

        size_t fbuf_len = 0;
        for (int i = 0; i < precision; ++i) {
            if (bit_count >= bbit) {
                fbuf[fbuf_len] = Cy(lut[(fra >> (bit_count - bbit)).uf8() & (bN - 1u)]);
                bit_count -= bbit;
                ++fbuf_len;
            } else {
                if (bit_count > 0u) {
                    fbuf[fbuf_len] = Cy(lut[(fra << (bbit - bit_count)).uf8() & (bN - 1u)]);
                    ++fbuf_len;
                }
                break;
            }
        }

        cpnos(sign, ubuf, 1, fbuf, fbuf_len, precision, fmt, out);

        out->push_back(Cy(upp ? 'P' : 'p'));

        int_fast32_t exp;
        if (exp_shift != 0) {
            exp = exp_shift - int_fast32_t(maxe_1);
        } else {
            exp = 1 - int_fast32_t(maxe_1);
        }

        if (exp >= 0) {
            out->push_back(Cy('+'));
        } else {
            out->push_back(Cy('-'));
            exp = -exp;
        }

        if (!(fmt & FCF_ENZ) && exp < 10) {
            out->push_back(Cy('0'));
        }

        std::basic_string<Cy> _str;
        itos(exp, &_str);
        out->append(_str);
    }

    template <typename FTy, typename Cy>
    bool ftos_hex2_base(
        FTy val, int fmt, int round, int precision, Cy* buf, size_t* len)
    {
        using FT = FloatTraits_base<FTy>;
        using uint_e = uint_e<FTy>;
        constexpr auto maxe_1 = FT::maxe - 1u;
        constexpr size_t bN = 16;
        constexpr size_t bbit = 4;

        uint_e fra;
        uint_fast32_t exp_shift;
        uint_fast8_t sign;
        ftodf(val, sign, exp_shift, fra);

        bool ret_val;
        if (to_special_s(fra, exp_shift, sign, fmt, precision, buf, len, &ret_val)) {
            return ret_val;
        }

        uint_fast8_t u1;
        fra_round(fra, exp_shift, sign, precision, round, &u1);

        bool upp = !!(fmt & FCF_UPP);
        const char* lut = upp ? kDigitCharUpper : kDigitCharLower;

        Cy ubuf[1] = { Cy(lut[u1]) };
        Cy fbuf[(FT::mant + bbit - 1u) / bbit];
        size_t bit_count = FT::mant;

        if (fmt & FCF_EXA) {
            precision = (std::numeric_limits<int>::max)();
        }

        size_t fbuf_len = 0;
        for (int i = 0; i < precision; ++i) {
            if (bit_count >= bbit) {
                fbuf[fbuf_len] = Cy(lut[(fra >> (bit_count - bbit)).uf8() & (bN - 1u)]);
                bit_count -= bbit;
                ++fbuf_len;
            } else {
                if (bit_count > 0u) {
                    fbuf[fbuf_len] = Cy(lut[(fra << (bbit - bit_count)).uf8() & (bN - 1u)]);
                    ++fbuf_len;
                }
                break;
            }
        }

        int_fast32_t exp;
        if (exp_shift != 0) {
            exp = exp_shift - int_fast32_t(maxe_1);
        } else {
            exp = 1 - int_fast32_t(maxe_1);
        }

        auto _len = *len;
        size_t act_len = 0;
        bool ret = cpnos(sign, ubuf, 1, fbuf, fbuf_len, precision, fmt, buf, &_len);

        Cy* s, * se;
        if (buf) {
            se = buf + *len;
            s = ret ? buf + _len : se;
        } else {
            s = nullptr;
            se = nullptr;
        }
        act_len += _len;

        if (s && se - s >= 2) {
            *s++ = Cy(upp ? 'P' : 'p');

            if (exp >= 0) {
                *s++ = Cy('+');
            } else {
                *s++ = Cy('-');
                exp = -exp;
            }
        } else {
            s = se;
            if (exp < 0) {
                exp = -exp;
            }
        }
        act_len += 2;

        if (!(fmt & FCF_ENZ) && exp < 10) {
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

    template <typename FTy, typename Cy>
    int stof_hex2_base(
        const Cy* str, size_t len, int fmt, int round, FTy* out, const Cy** p)
    {
        using FT = FloatTraits_base<FTy>;
        constexpr size_t bN = 16;
        constexpr size_t bbit = 4;
        constexpr auto maxe_1 = FT::maxe - 1u;
        constexpr size_t kSpace = 8u;

        auto s = str;
        auto se = str + len;

        uint_fast8_t sign;
        if (*s == Cy('-')) {
            ++s;
            if (s >= se) {
                return SCR_FAIL;
            }
            sign = 1;
        } else {
            sign = 0;
        }

        int ret_val;
        if (from_special_s(sign, s, se, out, p, &ret_val)) {
            return ret_val;
        }

        int_fast32_t exp = 0;
        const Cy* dig = nullptr;
        const Cy* sd = nullptr;

        uint_e<FTy> fra = 0;
        size_t bit_count = FT::mant + 1u + kSpace;

        for (; s < se; ++s) {
            if (*s == Cy('.')) {
                if (dig) {
                    break;
                }
                dig = s;
            } else {
                auto c = ctoi(*s);
                if (c == uint_fast8_t(-1) || c >= bN) {
                    break;
                }
                if (c != 0 && !sd) sd = s;
                if (sd) {
                    if (!dig) ++exp;
                } else {
                    if (dig) --exp;
                    continue;
                }

                if (bit_count >= bbit) {
                    fra <<= bbit;
                    fra |= c;
                    bit_count -= bbit;
                } else {
                    if (bit_count > 0u) {
                        fra <<= bit_count;
                        fra |= c >> (bbit - bit_count);
                        bit_count = 0u;
                    }
                    break;
                }
            }
        }

        if (!sd) {
            exp = -int_fast32_t(maxe_1);
        } else {
            exp *= bbit;

        }

        if (!bit_count) {
            for (; s < se; ++s) {
                auto c = ctoi(*s);
                if (c == uint_fast8_t(-1) || c >= bN) {
                    break;
                }
            }
        } else {
            fra <<= bit_count;
        }

        auto bc = FT::mant + 1u >= bbit ? (FT::mant + 1u - bbit) : 0u;
        auto top = (fra >> (bc + kSpace)).uf8() & (bN - 1u);
        for (size_t i = bbit; i-- > 0;) {
            if (uint_fast8_t(top) & (uint_fast8_t(1u) << i)) {
                fra <<= (bbit - 1u) - i;
                exp -= (bbit - 1u) - int_fast32_t(i) + 1u;
                break;
            }
        }

        if (s < se) {
            if (*s == Cy('p') || *s == Cy('P')) {
                ++s;
                if (s >= se) {
                    return SCR_FAIL;
                }
                if (*s == Cy('+')) {
                    ++s;
                    if (s >= se) {
                        return SCR_FAIL;
                    }
                }

                int_fast32_t _exp = 0;
                if (!stoi(s, se - s, &_exp, 10, &s)) {
                    return SCR_FAIL;
                }
                if (sd) {
                    exp += _exp;
                }
            }
        }

        if (round == FCR_CEIL) {
            if (!sign) {
                if (!fra.isZeroAfter(kSpace)) {
                    fra.incAt(kSpace);
                }
            }
        } else if (round == FCR_FLOOR) {
            if (sign) {
                if (!fra.isZeroAfter(kSpace)) {
                    fra.incAt(kSpace);
                }
            }
        } else if (round == FCR_ZERO) {
        } else {
            if (fra.getBit(kSpace - 1)) {
                fra.incAt(kSpace);
            }
        }
        fra >>= kSpace;
        if (fra.getBit(FT::mant + 1u)) {
            fra >>= 1;
            ++exp;
        }

        if (exp < FT::mine) {
            // 下溢
            if (std::numeric_limits<FTy>::has_denorm == std::denorm_present) {
                auto diff = FT::mine - exp;
                if (diff <= FT::mant) {
                    fra >>= diff;
                    exp = -int_fast32_t(maxe_1);
                } else {
                    zerd(sign, out);
                    return SCR_OF;
                }
            } else {
                zerd(sign, out);
                return SCR_OF;
            }
        } else if (exp > int(maxe_1)) {
            // 上溢
            infd(sign, out);
            return SCR_OF;
        }

        fra.clearBit(FT::mant);

        uint_fast32_t exp_shift = exp + int_fast32_t(maxe_1);
        if (!FT::has_denorm && exp_shift == 0) {
            fra.zero();
        }

        dftof(sign, exp_shift, fra, exp_shift != 0 ? 1 : 0, out);
        *p = s;
        return SCR_OK;
    }

}
}

#endif  // UTILS_STRINGS_FLOAT_CONV_INTERNAL_HPP_
