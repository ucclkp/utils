// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_USTRING_PRINT_INTERNAL_HPP_
#define UTILS_USTRING_PRINT_INTERNAL_HPP_

#include <cassert>
#include <string>

#include "utils/int_conv.hpp"


namespace utl {
namespace internal {

    enum Flags {
        FLAG_LEFT_JUST = 1 << 0,
        FLAG_SIGN = 1 << 1,
        FLAG_SPACE = 1 << 2,
        FLAG_ALTER = 1 << 3,
        FLAG_ZERO = 1 << 4,
    };

    enum Modifier {
        MOD_NONE = 0,
        MOD_hh,
        MOD_h,
        MOD_l,
        MOD_ll,
        MOD_j,
        MOD_z,
        MOD_t,
        MOD_L,
    };

    enum PRIType {
        PRI_NONE,
        PRI_8,
        PRI_16,
        PRI_32,
        PRI_64,
        PRI_L8,
        PRI_L16,
        PRI_L32,
        PRI_L64,
        PRI_F8,
        PRI_F16,
        PRI_F32,
        PRI_F64,
        PRI_MAX,
        PRI_PTR,
    };

    template <typename Cy>
    void fill_c(
        const Cy* buf, size_t len,
        std::basic_string<Cy>& r, int flags, int width)
    {
        if (len < size_t(width)) {
            if (flags & FLAG_LEFT_JUST) {
                r.append(buf, len);
                r.append(size_t(width) - len, Cy(' '));
            } else {
                r.append(size_t(width) - len, Cy(' '));
                r.append(buf, len);
            }
        } else {
            r.append(buf, len);
        }
    }


    template <typename Cy>
    void fill_c(
        const Cy* buf, size_t& len,
        Cy*& s, Cy* se, int flags, int width)
    {
        if (len < size_t(width)) {
            auto c_len = len;
            len = size_t(width);
            if (!s || se - s < width) {
                return;
            }

            if (flags & FLAG_LEFT_JUST) {
                for (size_t i = 0; i < c_len; ++i) {
                    *s++ = buf[i];
                }

                auto _len = size_t(width) - c_len;
                for (size_t i = 0; i < _len; ++i) {
                    *s++ = ' ';
                }
            } else {
                auto _len = size_t(width) - c_len;
                for (size_t i = 0; i < _len; ++i) {
                    *s++ = ' ';
                }

                for (size_t i = 0; i < c_len; ++i) {
                    *s++ = buf[i];
                }
            }
        } else {
            if (s && size_t(se - s) >= len) {
                for (size_t i = 0; i < len; ++i) {
                    *s++ = buf[i];
                }
            }
        }
    }

    template <typename Cy>
    void fill_s(
        std::basic_string<Cy>& r,
        const std::basic_string_view<Cy>& sv,
        int flags, int width)
    {
        if (sv.size() < size_t(width)) {
            if (flags & FLAG_LEFT_JUST) {
                r.append(sv);
                r.append(size_t(width) - sv.size(), Cy(' '));
            } else {
                r.append(size_t(width) - sv.size(), Cy(' '));
                r.append(sv);
            }
        } else {
            r.append(sv);
        }
    }

    template <typename Cy>
    void fill_s(
        Cy*& s, Cy* se,
        const Cy* ds, size_t d_len, size_t& len,
        int flags, int width)
    {
        if (d_len < size_t(width)) {
            len = size_t(width);
            if (!ds || !s || se - s < width) {
                return;
            }

            if (flags & FLAG_LEFT_JUST) {
                if (ds != s) {
                    for (size_t i = 0; i < d_len; ++i) {
                        *s++ = *ds++;
                    }
                } else {
                    s += d_len;
                }

                auto _len = size_t(width) - d_len;
                for (size_t i = 0; i < _len; ++i) {
                    *s++ = Cy(' ');
                }
            } else {
                auto rs = s + d_len;
                auto rds = ds + d_len;
                for (size_t i = 0; i < d_len; ++i) {
                    *--rs = *--rds;
                }

                auto _len = size_t(width) - d_len;
                for (size_t i = 0; i < _len; ++i) {
                    *s++ = Cy(' ');
                }
                s += d_len;
            }
        } else {
            if (ds && s) {
                if (ds != s && size_t(se - s) >= d_len) {
                    for (size_t i = 0; i < d_len; ++i) {
                        *s++ = *ds++;
                    }
                }
                if (ds == s) {
                    s += d_len;
                }
            }
            len = d_len;
        }
    }

    template <typename Cy>
    void extract_diu(
        std::basic_string<Cy>& str,
        bool no_sign, bool minus, bool zero, int flags, int precision, int width)
    {
        if (zero && precision == 0) {
            str.clear();
            return;
        }

        // sign
        Cy sign = 0;
        size_t sign_len = 0;
        std::basic_string_view<Cy> sv = str;
        if (!no_sign) {
            if (!minus) {
                if (flags & FLAG_SIGN) {
                    sign = Cy('+');
                    sign_len = 1;
                } else if (flags & FLAG_SPACE) {
                    sign = Cy(' ');
                    sign_len = 1;
                }
            } else {
                sign = Cy('-');
                sign_len = 1;
                sv = sv.substr(1);
            }
        }

        // precision
        size_t zero_count = 0;
        size_t dc = precision == -2 ? 1u : size_t(precision);
        if (sv.size() < dc) {
            zero_count += dc - sv.size();
        }

        // width
        size_t space_count = 0;
        if (sv.size() + sign_len + zero_count < size_t(width)) {
            if ((flags & FLAG_ZERO) && precision == -2 && !(flags & FLAG_LEFT_JUST)) {
                zero_count += size_t(width) - sv.size() - sign_len - zero_count;
            } else {
                space_count = size_t(width) - sv.size() - sign_len - zero_count;
            }
        }

        std::basic_string<Cy> _str;
        _str.reserve(zero_count + space_count + 1 + sv.size());

        if (flags & FLAG_LEFT_JUST) {
            if (sign) {
                _str.push_back(sign);
            }
            _str.append(zero_count, Cy('0'));
            _str.append(sv);
            _str.append(space_count, Cy(' '));
        } else {
            _str.append(space_count, Cy(' '));
            if (sign) {
                _str.push_back(sign);
            }
            _str.append(zero_count, Cy('0'));
            _str.append(sv);
        }

        str = std::move(_str);
    }

    template <typename Cy>
    bool extract_diu(
        Cy* str, size_t* len, size_t d_len,
        bool no_sign, bool minus, bool zero, int flags, int precision, int width)
    {
        if (zero && precision == 0) {
            *len = 0;
            return true;
        }

        auto s = str;
        auto se = s ? s + *len : nullptr;

        // sign
        Cy sign = 0;
        size_t sign_len = 0;
        if (!no_sign) {
            if (!minus) {
                if (flags & FLAG_SIGN) {
                    sign = Cy('+');
                    sign_len = 1;
                } else if (flags & FLAG_SPACE) {
                    sign = Cy(' ');
                    sign_len = 1;
                }
            } else {
                sign = Cy('-');
                sign_len = 1;
                assert(d_len > 0);
                --d_len;
            }
        }

        // precision
        size_t zero_count = 0;
        size_t dc = precision == -2 ? 1u : size_t(precision);
        if (d_len < dc) {
            zero_count += dc - d_len;
        }

        // width
        size_t space_count = 0;
        if (d_len + sign_len + zero_count < size_t(width)) {
            if ((flags & FLAG_ZERO) && precision == -2 && !(flags & FLAG_LEFT_JUST)) {
                zero_count += size_t(width) - d_len - sign_len - zero_count;
            } else {
                space_count = size_t(width) - d_len - sign_len - zero_count;
            }
        }

        auto total = zero_count + space_count + sign_len + d_len;
        *len = total;
        if (!s || total > *len) {
            return false;
        }

        if (flags & FLAG_LEFT_JUST) {
            auto diff = zero_count + (minus ? 0 : sign_len);
            if (diff > 0) {
                auto rs = s + d_len + diff;
                auto rds = s + d_len;
                if (minus) { ++rs; ++rds; }
                for (; rds > s;) {
                    *--rs = *--rds;
                }
            }

            if (sign_len) {
                *s++ = sign;
            }
            for (size_t i = 0; i < zero_count; ++i) {
                *s++ = Cy('0');
            }
            s += d_len;
            for (size_t i = 0; i < space_count; ++i) {
                *s++ = Cy(' ');
            }
        } else {
            auto diff = space_count + zero_count + (minus ? 0 : sign_len);
            if (diff > 0) {
                auto rs = s + d_len + diff;
                auto rds = s + d_len;
                if (minus) { ++rs; ++rds; }
                for (; rds > s;) {
                    *--rs = *--rds;
                }
            }
            for (size_t i = 0; i < space_count; ++i) {
                *s++ = Cy(' ');
            }
            if (sign_len) {
                *s++ = sign;
            }
            for (size_t i = 0; i < zero_count; ++i) {
                *s++ = Cy('0');
            }
            s += d_len;
        }
        return true;
    }

    template <typename Cy>
    void extract_o(
        std::basic_string<Cy>& str,
        bool no_sign, bool minus, bool zero, int flags, int precision, int width)
    {
        if (zero && precision == 0) {
            if (!(flags & FLAG_ALTER)) {
                str.clear();
            }
            return;
        }

        // sign
        Cy sign = 0;
        size_t sign_len = 0;
        std::basic_string_view<Cy> sv = str;
        if (!no_sign) {
            if (!minus) {
                if (flags & FLAG_SIGN) {
                    sign = Cy('+');
                    sign_len = 1;
                } else if (flags & FLAG_SPACE) {
                    sign = Cy(' ');
                    sign_len = 1;
                }
            } else {
                sign = Cy('-');
                sign_len = 1;
                sv = sv.substr(1);
            }
        }

        size_t zero_count = 0;
        if ((flags & FLAG_ALTER) && !zero) {
            zero_count += 1;
        }

        // precision
        size_t dc = precision == -2 ? 1u : size_t(precision);
        if (sv.size() < dc) {
            zero_count += dc - sv.size();
        }

        // width
        size_t space_count = 0;
        if (sv.size() + sign_len + zero_count < size_t(width)) {
            if ((flags & FLAG_ZERO) && precision == -2 && !(flags & FLAG_LEFT_JUST)) {
                zero_count += size_t(width) - sv.size() - sign_len - zero_count;
            } else {
                space_count = size_t(width) - sv.size() - sign_len - zero_count;
            }
        }

        std::basic_string<Cy> _str;
        _str.reserve(zero_count + space_count + 1 + sv.size());

        if (flags & FLAG_LEFT_JUST) {
            if (sign) {
                _str.push_back(sign);
            }
            _str.append(zero_count, Cy('0'));
            _str.append(sv);
            _str.append(space_count, Cy(' '));
        } else {
            _str.append(space_count, Cy(' '));
            if (sign) {
                _str.push_back(sign);
            }
            _str.append(zero_count, Cy('0'));
            _str.append(sv);
        }

        str = std::move(_str);
    }

    template <typename Cy>
    bool extract_o(
        Cy* str, size_t* len, size_t d_len,
        bool no_sign, bool minus, bool zero, int flags, int precision, int width)
    {
        if (zero && precision == 0) {
            if (!(flags & FLAG_ALTER)) {
                *len = 0;
            } else {
                *len = 1;
            }
            return true;
        }

        auto s = str;
        auto se = s ? s + *len : nullptr;

        // sign
        Cy sign = 0;
        size_t sign_len = 0;
        if (!no_sign) {
            if (!minus) {
                if (flags & FLAG_SIGN) {
                    sign = Cy('+');
                    sign_len = 1;
                } else if (flags & FLAG_SPACE) {
                    sign = Cy(' ');
                    sign_len = 1;
                }
            } else {
                sign = Cy('-');
                sign_len = 1;
                assert(d_len > 0);
                --d_len;
            }
        }

        size_t zero_count = 0;
        if ((flags & FLAG_ALTER) && !zero) {
            zero_count += 1;
        }

        // precision
        size_t dc = precision == -2 ? 1u : size_t(precision);
        if (d_len < dc) {
            zero_count += dc - d_len;
        }

        // width
        size_t space_count = 0;
        if (d_len + sign_len + zero_count < size_t(width)) {
            if ((flags & FLAG_ZERO) && precision == -2 && !(flags & FLAG_LEFT_JUST)) {
                zero_count += size_t(width) - d_len - sign_len - zero_count;
            } else {
                space_count = size_t(width) - d_len - sign_len - zero_count;
            }
        }

        auto total = zero_count + space_count + sign_len + d_len;
        *len = total;
        if (!s || total > *len) {
            return false;
        }

        if (flags & FLAG_LEFT_JUST) {
            auto diff = zero_count + (minus ? 0 : sign_len);
            if (diff > 0) {
                auto rs = s + d_len + diff;
                auto rds = s + d_len;
                if (minus) { ++rs; ++rds; }
                for (; rds > s;) {
                    *--rs = *--rds;
                }
            }

            if (sign_len) {
                *s++ = sign;
            }
            for (size_t i = 0; i < zero_count; ++i) {
                *s++ = Cy('0');
            }
            s += d_len;
            for (size_t i = 0; i < space_count; ++i) {
                *s++ = Cy(' ');
            }
        } else {
            auto diff = space_count + zero_count + (minus ? 0 : sign_len);
            if (diff > 0) {
                auto rs = s + d_len + diff;
                auto rds = s + d_len;
                if (minus) { ++rs; ++rds; }
                for (; rds > s;) {
                    *--rs = *--rds;
                }
            }
            for (size_t i = 0; i < space_count; ++i) {
                *s++ = Cy(' ');
            }
            if (sign_len) {
                *s++ = sign;
            }
            for (size_t i = 0; i < zero_count; ++i) {
                *s++ = Cy('0');
            }
            s += d_len;
        }
        return true;
    }

    template <typename Cy>
    void extract_xX(
        std::basic_string<Cy>& str,
        bool no_sign, bool minus, bool zero, int flags, int precision, int width, Cy cp)
    {
        if (zero && precision == 0) {
            str.clear();
            return;
        }

        // sign
        Cy sign = 0;
        size_t sign_len = 0;
        std::basic_string_view<Cy> sv = str;
        if (!no_sign) {
            if (!minus) {
                if (flags & FLAG_SIGN) {
                    sign = Cy('+');
                    sign_len = 1;
                } else if (flags & FLAG_SPACE) {
                    sign = Cy(' ');
                    sign_len = 1;
                }
            } else {
                sign = Cy('-');
                sign_len = 1;
                sv = sv.substr(1);
            }
        }

        size_t zero_count = 0;
        size_t pref_count = 0;
        if (flags & FLAG_ALTER) {
            zero_count = 2;
            pref_count = 2;
        }

        // precision
        size_t dc = precision == -2 ? 1u : size_t(precision);
        if (sv.size() < dc) {
            zero_count += dc - sv.size();
        }

        // width
        size_t space_count = 0;
        if (sv.size() + sign_len + zero_count < size_t(width)) {
            if ((flags & FLAG_ZERO) && precision == -2 && !(flags & FLAG_LEFT_JUST)) {
                zero_count += size_t(width) - sv.size() - sign_len - zero_count;
            } else {
                space_count = size_t(width) - sv.size() - sign_len - zero_count;
            }
        }

        std::basic_string<Cy> _str;
        _str.reserve(zero_count + space_count + 1 + sv.size());

        if (flags & FLAG_LEFT_JUST) {
            if (sign) {
                _str.push_back(sign);
            }
            if (flags & FLAG_ALTER) {
                _str.push_back(Cy('0'));
                _str.push_back(cp);
            }
            _str.append(zero_count - pref_count, Cy('0'));
            _str.append(sv);
            _str.append(space_count, Cy(' '));
        } else {
            _str.append(space_count, Cy(' '));
            if (sign) {
                _str.push_back(sign);
            }
            if (flags & FLAG_ALTER) {
                _str.push_back(Cy('0'));
                _str.push_back(cp);
            }
            _str.append(zero_count - pref_count, Cy('0'));
            _str.append(sv);
        }

        str = std::move(_str);
    }

    template <typename Cy>
    bool extract_xX(
        Cy* str, size_t* len, size_t d_len,
        bool no_sign, bool minus, bool zero, int flags, int precision, int width, Cy cp)
    {
        if (zero && precision == 0) {
            *len = 0;
            return true;
        }

        auto s = str;
        auto se = s ? s + *len : nullptr;

        // sign
        Cy sign = 0;
        size_t sign_len = 0;
        if (!no_sign) {
            if (!minus) {
                if (flags & FLAG_SIGN) {
                    sign = Cy('+');
                    sign_len = 1;
                } else if (flags & FLAG_SPACE) {
                    sign = Cy(' ');
                    sign_len = 1;
                }
            } else {
                sign = Cy('-');
                sign_len = 1;
                assert(d_len > 0);
                --d_len;
            }
        }

        size_t zero_count = 0;
        size_t pref_count = 0;
        if (flags & FLAG_ALTER) {
            zero_count = 2;
            pref_count = 2;
        }

        // precision
        size_t dc = precision == -2 ? 1u : size_t(precision);
        if (d_len < dc) {
            zero_count += dc - d_len;
        }

        // width
        size_t space_count = 0;
        if (d_len + sign_len + zero_count < size_t(width)) {
            if ((flags & FLAG_ZERO) && precision == -2 && !(flags & FLAG_LEFT_JUST)) {
                zero_count += size_t(width) - d_len - sign_len - zero_count;
            } else {
                space_count = size_t(width) - d_len - sign_len - zero_count;
            }
        }

        auto total = zero_count + space_count + sign_len + d_len;
        *len = total;
        if (!s || total > *len) {
            return false;
        }

        if (flags & FLAG_LEFT_JUST) {
            auto diff = zero_count + (minus ? 0 : sign_len);
            if (diff > 0) {
                auto rs = s + d_len + diff;
                auto rds = s + d_len;
                if (minus) { ++rs; ++rds; }
                for (; rds > s;) {
                    *--rs = *--rds;
                }
            }

            if (sign_len) {
                *s++ = sign;
            }
            if (flags & FLAG_ALTER) {
                *s++ = Cy('0');
                *s++ = cp;
            }
            for (size_t i = 0; i < zero_count - pref_count; ++i) {
                *s++ = Cy('0');
            }
            s += d_len;
            for (size_t i = 0; i < space_count; ++i) {
                *s++ = Cy(' ');
            }
        } else {
            auto diff = space_count + zero_count + (minus ? 0 : sign_len);
            if (diff > 0) {
                auto rs = s + d_len + diff;
                auto rds = s + d_len;
                if (minus) { ++rs; ++rds; }
                for (; rds > s;) {
                    *--rs = *--rds;
                }
            }
            for (size_t i = 0; i < space_count; ++i) {
                *s++ = Cy(' ');
            }
            if (sign_len) {
                *s++ = sign;
            }
            if (flags & FLAG_ALTER) {
                *s++ = Cy('0');
                *s++ = cp;
            }
            for (size_t i = 0; i < zero_count - pref_count; ++i) {
                *s++ = Cy('0');
            }
            s += d_len;
        }
        return true;
    }

    template <typename Cy>
    void extract_fFeEgG(
        std::basic_string<Cy>* r,
        int flags, int width, bool minus, const std::basic_string<Cy>& str)
    {
        // sign
        Cy sign = 0;
        size_t sign_len = 0;
        std::basic_string_view<Cy> sv = str;
        if (!minus) {
            if (flags & FLAG_SIGN) {
                sign = Cy('+');
                sign_len = 1;
            } else if (flags & FLAG_SPACE) {
                sign = Cy(' ');
                sign_len = 1;
            }
        } else {
            sign = Cy('-');
            sign_len = 1;
            sv = sv.substr(1);
        }

        // width
        size_t zero_count = 0;
        size_t space_count = 0;
        if (sv.size() + sign_len + zero_count < size_t(width)) {
            if ((flags & FLAG_ZERO) && !(flags & FLAG_LEFT_JUST)) {
                zero_count += size_t(width) - sv.size() - sign_len - zero_count;
            } else {
                space_count = size_t(width) - sv.size() - sign_len - zero_count;
            }
        }

        std::basic_string<Cy> __str;
        __str.reserve(zero_count + space_count + 1 + sv.size());

        if (flags & FLAG_LEFT_JUST) {
            if (sign) {
                __str.push_back(sign);
            }
            __str.append(zero_count, Cy('0'));
            __str.append(sv);
            __str.append(space_count, Cy(' '));
        } else {
            __str.append(space_count, Cy(' '));
            if (sign) {
                __str.push_back(sign);
            }
            __str.append(zero_count, Cy('0'));
            __str.append(sv);
        }

        r->append(__str);
    }

    template <typename Cy>
    bool extract_fFeEgG(
        Cy* str, size_t* len,
        int flags, int width, bool minus, size_t d_len)
    {
        auto s = str;
        auto se = s ? s + *len : nullptr;

        // sign
        Cy sign = 0;
        size_t sign_len = 0;
        if (!minus) {
            if (flags & FLAG_SIGN) {
                sign = Cy('+');
                sign_len = 1;
            } else if (flags & FLAG_SPACE) {
                sign = Cy(' ');
                sign_len = 1;
            }
        } else {
            sign = Cy('-');
            sign_len = 1;
            assert(d_len > 0);
            --d_len;
        }

        // width
        size_t zero_count = 0;
        size_t space_count = 0;
        if (d_len + sign_len + zero_count < size_t(width)) {
            if ((flags & FLAG_ZERO) && !(flags & FLAG_LEFT_JUST)) {
                zero_count += size_t(width) - d_len - sign_len - zero_count;
            } else {
                space_count = size_t(width) - d_len - sign_len - zero_count;
            }
        }

        auto total = zero_count + space_count + sign_len + d_len;
        *len = total;
        if (!s || total > *len) {
            return false;
        }

        if (flags & FLAG_LEFT_JUST) {
            auto diff = zero_count + (minus ? 0 : sign_len);
            if (diff > 0) {
                auto rs = s + d_len + diff;
                auto rds = s + d_len;
                if (minus) { ++rs; ++rds; }
                for (; rds > s;) {
                    *--rs = *--rds;
                }
            }

            if (sign_len) {
                *s++ = sign;
            }
            for (size_t i = 0; i < zero_count; ++i) {
                *s++ = Cy('0');
            }
            s += d_len;
            for (size_t i = 0; i < space_count; ++i) {
                *s++ = Cy(' ');
            }
        } else {
            auto diff = space_count + zero_count + (minus ? 0 : sign_len);
            if (diff > 0) {
                auto rs = s + d_len + diff;
                auto rds = s + d_len;
                if (minus) { ++rs; ++rds; }
                for (; rds > s;) {
                    *--rs = *--rds;
                }
            }
            for (size_t i = 0; i < space_count; ++i) {
                *s++ = Cy(' ');
            }
            if (sign_len) {
                *s++ = sign;
            }
            for (size_t i = 0; i < zero_count; ++i) {
                *s++ = Cy('0');
            }
            s += d_len;
        }
        return true;
    }

    template <typename Cy>
    void extract_aA(
        std::basic_string<Cy>* r,
        int flags, int width, Cy cp, bool minus, const std::basic_string<Cy>& str)
    {
        size_t zero_count = 2;
        size_t pref_count = 2;

        // sign
        Cy sign = 0;
        size_t sign_len = 0;
        std::basic_string_view<Cy> sv = str;
        if (!minus) {
            if (flags & FLAG_SIGN) {
                sign = Cy('+');
                sign_len = 1;
            } else if (flags & FLAG_SPACE) {
                sign = Cy(' ');
                sign_len = 1;
            }
        } else {
            sign = Cy('-');
            sign_len = 1;
            sv = sv.substr(1);
        }

        // width
        size_t space_count = 0;
        if (sv.size() + sign_len + zero_count < size_t(width)) {
            if ((flags & FLAG_ZERO) && !(flags & FLAG_LEFT_JUST)) {
                zero_count += size_t(width) - sv.size() - sign_len - zero_count;
            } else {
                space_count = size_t(width) - sv.size() - sign_len - zero_count;
            }
        }

        std::basic_string<Cy> __str;
        __str.reserve(zero_count + space_count + 1 + sv.size());

        if (flags & FLAG_LEFT_JUST) {
            if (sign) {
                __str.push_back(sign);
            }
            __str.push_back(Cy('0'));
            __str.push_back(cp);
            __str.append(zero_count - pref_count, Cy('0'));
            __str.append(sv);
            __str.append(space_count, Cy(' '));
        } else {
            __str.append(space_count, Cy(' '));
            if (sign) {
                __str.push_back(sign);
            }
            __str.push_back(Cy('0'));
            __str.push_back(cp);
            __str.append(zero_count - pref_count, Cy('0'));
            __str.append(sv);
        }

        r->append(__str);
    }

    template <typename Cy>
    bool extract_aA(
        Cy* str, size_t* len,
        int flags, int width, Cy cp, bool minus, size_t d_len)
    {
        size_t zero_count = 2;
        size_t pref_count = 2;

        auto s = str;
        auto se = s ? s + *len : nullptr;

        // sign
        Cy sign = 0;
        size_t sign_len = 0;
        if (!minus) {
            if (flags & FLAG_SIGN) {
                sign = Cy('+');
                sign_len = 1;
            } else if (flags & FLAG_SPACE) {
                sign = Cy(' ');
                sign_len = 1;
            }
        } else {
            sign = Cy('-');
            sign_len = 1;
            assert(d_len > 0);
            --d_len;
        }

        // width
        size_t space_count = 0;
        if (d_len + sign_len + zero_count < size_t(width)) {
            if ((flags & FLAG_ZERO) && !(flags & FLAG_LEFT_JUST)) {
                zero_count += size_t(width) - d_len - sign_len - zero_count;
            } else {
                space_count = size_t(width) - d_len - sign_len - zero_count;
            }
        }

        auto total = zero_count + space_count + sign_len + d_len;
        *len = total;
        if (!s || total > *len) {
            return false;
        }

        if (flags & FLAG_LEFT_JUST) {
            auto diff = zero_count + (minus ? 0 : sign_len);
            if (diff > 0) {
                auto rs = s + d_len + diff;
                auto rds = s + d_len;
                if (minus) { ++rs; ++rds; }
                for (; rds > s;) {
                    *--rs = *--rds;
                }
            }

            if (sign_len) {
                *s++ = sign;
            }
            *s++ = Cy('0');
            *s++ = cp;
            for (size_t i = 0; i < zero_count - pref_count; ++i) {
                *s++ = Cy('0');
            }
            s += d_len;
            for (size_t i = 0; i < space_count; ++i) {
                *s++ = Cy(' ');
            }
        } else {
            auto diff = space_count + zero_count + (minus ? 0 : sign_len);
            if (diff > 0) {
                auto rs = s + d_len + diff;
                auto rds = s + d_len;
                if (minus) { ++rs; ++rds; }
                for (; rds > s;) {
                    *--rs = *--rds;
                }
            }
            for (size_t i = 0; i < space_count; ++i) {
                *s++ = Cy(' ');
            }
            if (sign_len) {
                *s++ = sign;
            }
            *s++ = Cy('0');
            *s++ = cp;
            for (size_t i = 0; i < zero_count - pref_count; ++i) {
                *s++ = Cy('0');
            }
            s += d_len;
        }
        return true;
    }


    template <typename Cy>
    void parseFlags(
        const Cy** format, size_t len, int* flags)
    {
        auto s = *format;
        auto se = s + len;

        for (; s < se; ++s) {
            switch (*s) {
            case Cy('-'): *flags |= FLAG_LEFT_JUST; break;
            case Cy('+'): *flags |= FLAG_SIGN;      break;
            case Cy(' '): *flags |= FLAG_SPACE;     break;
            case Cy('#'): *flags |= FLAG_ALTER;     break;
            case Cy('0'): *flags |= FLAG_ZERO;      break;
            default: *format = s; return;
            }
        }
        *format = s;
    }

    template <typename Cy>
    void parseWidth(
        const Cy** format, size_t len, int* width_mode)
    {
        auto s = *format;
        auto se = s + len;

        bool has_r = false;
        for (; s < se; ++s) {
            auto next = *s;
            if (next == Cy('*')) {
                if (has_r) { break; }
                *width_mode = -1;
                has_r = true;
                break;
            }

            auto num = ctoi(next);
            if (num != uint_fast8_t(-1) && num >= 0 && num <= 9) {
                *width_mode = *width_mode * 10 + num;
            } else {
                break;
            }
            has_r = true;
        }
        if (!has_r) {
            *width_mode = -2;
        }
        *format = s;
    }

    template <typename Cy>
    void parsePrecision(
        const Cy** format, size_t len, int* precision_mode)
    {
        auto s = *format;
        auto se = s + len;

        if (*s == Cy('.')) {
            ++s;
            if (s >= se) {
                *format = s;
                return;
            }
            len -= 1;
            parseWidth(&s, len, precision_mode);
            if (*precision_mode == -2) {
                *precision_mode = 0;
            }
        } else {
            *precision_mode = -2;
        }

        *format = s;
    }

    template <typename Cy>
    void parseLengthModifier(
        const Cy** format, size_t len, int* modifier)
    {
        auto s = *format;
        auto se = s + len;

        switch (*s) {
        case Cy('h'):
            ++s;
            if (s < se && *s == Cy('h')) {
                ++s;
                *modifier = MOD_hh;
            } else {
                *modifier = MOD_h;
            }
            break;
        case Cy('l'):
            ++s;
            if (s < se && *s == Cy('l')) {
                ++s;
                *modifier = MOD_ll;
            } else {
                *modifier = MOD_l;
            }
            break;
        case Cy('j'): ++s; *modifier = MOD_j; break;
        case Cy('z'): ++s; *modifier = MOD_z; break;
        case Cy('t'): ++s; *modifier = MOD_t; break;
        case Cy('L'): ++s; *modifier = MOD_L; break;
        default: break;
        }

        *format = s;
    }

    template <typename Cy>
    void parsePRI(
        const Cy** format, size_t len, PRIType* pri)
    {
        auto s = *format;
        auto se = s + len;
        if (*s != Cy('P')) {
            *pri = PRI_NONE;
            return;
        }
        ++s;

        Cy prefix = 0;
        for (; s < se;) {
            switch (*s) {
            case Cy('L'):
                prefix = Cy('L');
                break;

            case Cy('F'):
                prefix = Cy('F');
                break;

            case Cy('M'):
                *pri = PRI_MAX;
                *format = ++s;
                return;

            case Cy('P'):
                *pri = PRI_PTR;
                *format = ++s;
                return;

            case Cy('8'):
                switch (prefix) {
                case Cy('L'):
                    *pri = PRI_L8;
                    *format = ++s;
                    break;
                case Cy('F'):
                    *pri = PRI_F8;
                    *format = ++s;
                    break;
                default:
                    *pri = PRI_8;
                    *format = ++s;
                    break;
                }
                return;

            case Cy('1'):
                if (++s >= se || *s != Cy('6')) {
                    *pri = PRI_NONE;
                    return;
                }
                switch (prefix) {
                case Cy('L'):
                    *pri = PRI_L16;
                    *format = ++s;
                    break;
                case Cy('F'):
                    *pri = PRI_F16;
                    *format = ++s;
                    break;
                default:
                    *pri = PRI_16;
                    *format = ++s;
                    break;
                }
                return;

            case Cy('3'):
                if (++s >= se || *s != Cy('2')) {
                    *pri = PRI_NONE;
                    return;
                }
                switch (prefix) {
                case Cy('L'):
                    *pri = PRI_L32;
                    *format = ++s;
                    break;
                case Cy('F'):
                    *pri = PRI_F32;
                    *format = ++s;
                    break;
                default:
                    *pri = PRI_32;
                    *format = ++s;
                    break;
                }
                return;

            case Cy('6'):
                if (++s >= se || *s != Cy('4')) {
                    *pri = PRI_NONE;
                    return;
                }
                switch (prefix) {
                case Cy('L'):
                    *pri = PRI_L64;
                    *format = ++s;
                    break;
                case Cy('F'):
                    *pri = PRI_F64;
                    *format = ++s;
                    break;
                default:
                    *pri = PRI_64;
                    *format = ++s;
                    break;
                }
                return;

            default:
                *pri = PRI_NONE;
                return;
            }
        }

        *pri = PRI_NONE;
    }

}
}

#endif  // UTILS_USTRING_PRINT_INTERNAL_HPP_