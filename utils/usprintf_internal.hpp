// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_USPRINTF_INTERNAL_HPP_
#define UTILS_USPRINTF_INTERNAL_HPP_

#include <cassert>
#include <cmath>
#include <cstdarg>

#include "utils/int_conv.hpp"
#include "utils/float_conv.h"
#include "utils/type_utils.hpp"
#include "utils/unicode_conv.h"


namespace utl {
namespace internal {

    enum Flags {
        FLAG_LEFT_JUST = 1 << 0,
        FLAG_SIGN      = 1 << 1,
        FLAG_SPACE     = 1 << 2,
        FLAG_ALTER     = 1 << 3,
        FLAG_ZERO      = 1 << 4,
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

    struct vlw {
        va_list args;
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

    inline bool extract_c(
        std::string& r,
        int flags, int modifier, int width, vlw* args)
    {
        char buf[4];
        size_t len;
        if (modifier == MOD_ll) {
            utf32_to_utf8(va_arg(args->args, utl::upromote<char32_t>::type), buf, &len);
        } else if (modifier == MOD_l) {
            if (!utf16_to_utf8(va_arg(args->args, utl::upromote<char16_t>::type), buf, &len)) {
                return false;
            }
        } else {
            buf[0] = char(va_arg(args->args, int));
            len = 1;
        }

        fill_c(buf, len, r, flags, width);
        return true;
    }

    inline bool extract_c(
        std::u16string& r,
        int flags, int modifier, int width, vlw* args)
    {
        char16_t buf[2];
        size_t len;
        if (modifier == MOD_ll) {
            utf32_to_utf16(va_arg(args->args, utl::upromote<char32_t>::type), buf, &len);
        } else if (modifier == MOD_l) {
            buf[0] = va_arg(args->args, utl::upromote<char16_t>::type);
            len = 1;
        } else {
            if (!utf8_to_utf16(
                char(va_arg(args->args, int)), buf))
            {
                return false;
            }
            len = 1;
        }

        fill_c(buf, len, r, flags, width);
        return true;
    }

    inline bool extract_c(
        std::u32string& r,
        int flags, int modifier, int width, vlw* args)
    {
        char32_t ch;
        if (modifier == MOD_ll) {
            ch = va_arg(args->args, utl::upromote<char32_t>::type);
        } else if (modifier == MOD_l) {
            if (!utf16_to_utf32(
                va_arg(args->args, utl::upromote<char16_t>::type), &ch))
            {
                return false;
            }
        } else {
            if (!utf8_to_utf32(
                char(va_arg(args->args, int)), &ch))
            {
                return false;
            }
        }

        fill_c(&ch, 1, r, flags, width);
        return true;
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

    inline int extract_c(
        char* r, size_t* len,
        int flags, int modifier, int width, vlw* args)
    {
        char buf[4];
        size_t _len;
        if (modifier == MOD_ll) {
            utf32_to_utf8(va_arg(args->args, utl::upromote<char32_t>::type), buf, &_len);
        } else if (modifier == MOD_l) {
            if (!utf16_to_utf8(va_arg(args->args, utl::upromote<char16_t>::type), buf, &_len)) {
                return UCR_FAILED;
            }
        } else {
            buf[0] = char(va_arg(args->args, int));
            _len = 1;
        }

        auto s = r;
        auto se = s ? s + *len : nullptr;
        fill_c(buf, _len, s, se, flags, width);

        *len = _len;
        if (!s || s - r != _len) {
            return UCR_BUFFER;
        }
        return UCR_OK;
    }

    inline int extract_c(
        char16_t* r, size_t* len,
        int flags, int modifier, int width, vlw* args)
    {
        char16_t buf[2];
        size_t _len;
        if (modifier == MOD_ll) {
            utf32_to_utf16(va_arg(args->args, utl::upromote<char32_t>::type), buf, &_len);
        } else if (modifier == MOD_l) {
            buf[0] = va_arg(args->args, utl::upromote<char16_t>::type);
            _len = 1;
        } else {
            if (!utf8_to_utf16(
                char(va_arg(args->args, int)), buf))
            {
                return false;
            }
            _len = 1;
        }

        auto s = r;
        auto se = s ? s + *len : nullptr;
        fill_c(buf, _len, s, se, flags, width);

        *len = _len;
        if (!s || s - r != _len) {
            return UCR_BUFFER;
        }
        return UCR_OK;
    }

    inline int extract_c(
        char32_t* r, size_t* len,
        int flags, int modifier, int width, vlw* args)
    {
        char32_t ch;
        if (modifier == MOD_ll) {
            ch = va_arg(args->args, utl::upromote<char32_t>::type);
        } else if (modifier == MOD_l) {
            if (!utf16_to_utf32(
                va_arg(args->args, utl::upromote<char16_t>::type), &ch))
            {
                return false;
            }
        } else {
            if (!utf8_to_utf32(
                char(va_arg(args->args, int)), &ch))
            {
                return false;
            }
        }

        auto s = r;
        auto se = s ? s + *len : nullptr;
        size_t _len = 1;
        fill_c(&ch, _len, s, se, flags, width);

        *len = _len;
        if (!s || s - r != _len) {
            return UCR_BUFFER;
        }
        return UCR_OK;
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

    inline bool extract_s(
        std::string& r,
        int flags, int precision, int modifier, int width, vlw* args)
    {
        std::string u8_str;
        std::string_view sv;
        if (modifier == MOD_ll) {
            std::u32string_view u32_s(va_arg(args->args, char32_t*));
            utf32_to_utf8(u32_s, &u8_str);
            sv = u8_str;
        } else if (modifier == MOD_l) {
            std::u16string_view u16_s(va_arg(args->args, char16_t*));
            if (!utf16_to_utf8(u16_s, &u8_str)) {
                return false;
            }
            sv = u8_str;
        } else {
            sv = va_arg(args->args, char*);
        }

        if (precision != -2) {
            sv = sv.substr(0, size_t(precision));
        }

        fill_s(r, sv, flags, width);
        return true;
    }

    inline bool extract_s(
        std::u16string& r,
        int flags, int precision, int modifier, int width, vlw* args)
    {
        std::u16string u16_str;
        std::u16string_view sv;
        if (modifier == MOD_ll) {
            std::u32string_view u32_s(va_arg(args->args, char32_t*));
            utf32_to_utf16(u32_s, &u16_str);
            sv = u16_str;
        } else if (modifier == MOD_l) {
            sv = va_arg(args->args, char16_t*);
        } else {
            std::string_view u8_s(va_arg(args->args, char*));
            if (!utf8_to_utf16(u8_s, &u16_str)) {
                return false;
            }
            sv = u16_str;
        }

        if (precision != -2) {
            sv = sv.substr(0, size_t(precision));
        }

        fill_s(r, sv, flags, width);
        return true;
    }

    inline bool extract_s(
        std::u32string& r,
        int flags, int precision, int modifier, int width, vlw* args)
    {
        std::u32string u32_str;
        std::u32string_view sv;
        if (modifier == MOD_ll) {
            sv = va_arg(args->args, char32_t*);
        } else if (modifier == MOD_l) {
            std::u16string_view u16_s(va_arg(args->args, char16_t*));
            if (!utf16_to_utf32(u16_s, &u32_str)) {
                return false;
            }
            sv = u32_str;
        } else {
            std::string_view u8_s(va_arg(args->args, char*));
            if (!utf8_to_utf32(u8_s, &u32_str)) {
                return false;
            }
            sv = u32_str;
        }

        if (precision != -2) {
            sv = sv.substr(0, size_t(precision));
        }

        fill_s(r, sv, flags, width);
        return true;
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

    inline int extract_s(
        char* r, size_t* len,
        int flags, int precision, int modifier, int width, vlw* args)
    {
        char* dig;
        size_t _len = *len;
        if (modifier == MOD_ll) {
            std::u32string_view u32_s(va_arg(args->args, char32_t*));
            if (utf32_to_utf8(u32_s, r, &_len)) {
                dig = r;
            } else {
                dig = nullptr;
            }
        } else if (modifier == MOD_l) {
            std::u16string_view u16_s(va_arg(args->args, char16_t*));
            int ret = utf16_to_utf8(u16_s, r, &_len);
            if (ret == UCR_FAILED) {
                return UCR_FAILED;
            }
            if (ret == UCR_OK) {
                dig = r;
            } else {
                dig = nullptr;
            }
        } else {
            dig = va_arg(args->args, char*);
            _len = std::char_traits<char>::length(dig);
        }

        if (precision != -2 && _len > size_t(precision)) {
            _len = size_t(precision);
        }

        auto s = r;
        auto se = s ? s + *len : nullptr;

        fill_s(s, se, dig, _len, *len, flags, width);
        if (!s || s - r != *len) {
            return UCR_BUFFER;
        }
        return UCR_OK;
    }

    inline int extract_s(
        char16_t* r, size_t* len,
        int flags, int precision, int modifier, int width, vlw* args)
    {
        char16_t* dig;
        size_t _len = *len;
        if (modifier == MOD_ll) {
            std::u32string_view u32_s(va_arg(args->args, char32_t*));
            if (utf32_to_utf16(u32_s, r, &_len)) {
                dig = r;
            } else {
                dig = nullptr;
            }
        } else if (modifier == MOD_l) {
            dig = va_arg(args->args, char16_t*);
            _len = std::char_traits<char16_t>::length(dig);
        } else {
            std::string_view u8_s(va_arg(args->args, char*));
            int ret = utf8_to_utf16(u8_s, r, &_len);
            if (ret == UCR_FAILED) {
                return UCR_FAILED;
            }
            if (ret == UCR_OK) {
                dig = r;
            } else {
                dig = nullptr;
            }
        }

        if (precision != -2 && _len > size_t(precision)) {
            _len = size_t(precision);
        }

        auto s = r;
        auto se = s ? s + *len : nullptr;

        fill_s(s, se, dig, _len, *len, flags, width);
        if (!s || s - r != *len) {
            return UCR_BUFFER;
        }
        return UCR_OK;
    }

    inline int extract_s(
        char32_t* r, size_t* len,
        int flags, int precision, int modifier, int width, vlw* args)
    {
        char32_t* dig;
        size_t _len = *len;
        if (modifier == MOD_ll) {
            dig = va_arg(args->args, char32_t*);
            _len = std::char_traits<char32_t>::length(dig);
        } else if (modifier == MOD_l) {
            std::u16string_view u16_s(va_arg(args->args, char16_t*));
            int ret = utf16_to_utf32(u16_s, r, &_len);
            if (ret == UCR_FAILED) {
                return UCR_FAILED;
            }
            if (ret == UCR_OK) {
                dig = r;
            } else {
                dig = nullptr;
            }
        } else {
            std::string_view u8_s(va_arg(args->args, char*));
            int ret = utf8_to_utf32(u8_s, r, &_len);
            if (ret == UCR_FAILED) {
                return UCR_FAILED;
            }
            if (ret == UCR_OK) {
                dig = r;
            } else {
                dig = nullptr;
            }
        }

        if (precision != -2 && _len > size_t(precision)) {
            _len = size_t(precision);
        }

        auto s = r;
        auto se = s ? s + *len : nullptr;

        fill_s(s, se, dig, _len, *len, flags, width);
        if (!s || s - r != *len) {
            return UCR_BUFFER;
        }
        return UCR_OK;
    }


    template <typename Cy>
    bool va_itos(
        int modifier, Cy* buf, size_t* len, bool* minus, bool* zero, vlw* vars)
    {
        switch (modifier) {
        case MOD_hh:
        {
            auto val = va_arg(vars->args, int);
            *zero = val == 0; *minus = val < 0;
            return itos(val, buf, len);
        }
        case MOD_h:
        {
            auto val = va_arg(vars->args, int);
            *zero = val == 0; *minus = val < 0;
            return itos(val, buf, len);
        }
        case MOD_l:
        {
            auto val = va_arg(vars->args, long);
            *zero = val == 0; *minus = val < 0;
            return itos(val, buf, len);
        }
        case MOD_ll:
        {
            auto val = va_arg(vars->args, long long);
            *zero = val == 0; *minus = val < 0;
            return itos(val, buf, len);
        }
        case MOD_j:
        {
            auto val = va_arg(vars->args, intmax_t);
            *zero = val == 0; *minus = val < 0;
            return itos(val, buf, len);
        }
        case MOD_z:
        {
            auto val = va_arg(vars->args, std::make_signed<size_t>::type);
            *zero = val == 0; *minus = val < 0;
            return itos(val, buf, len);
        }
        case MOD_t:
        {
            auto val = va_arg(vars->args, ptrdiff_t);
            *zero = val == 0; *minus = val < 0;
            return itos(val, buf, len);
        }
        default:
        {
            auto val = va_arg(vars->args, int);
            *zero = val == 0; *minus = val < 0;
            return itos(val, buf, len);
        }
        }
    }

    template <typename Cy>
    bool va_uitos(
        int modifier, int radix, bool upper,
        Cy* buf, size_t* len, bool* minus, bool* zero, vlw* vars)
    {
        switch (modifier) {
        case MOD_hh:
        {
            auto val = va_arg(vars->args, unsigned int);
            *zero = val == 0; *minus = val < 0;
            return itos(val, buf, len, radix, upper);
        }
        case MOD_h:
        {
            auto val = va_arg(vars->args, unsigned int);
            *zero = val == 0; *minus = val < 0;
            return itos(val, buf, len, radix, upper);
        }
        case MOD_l:
        {
            auto val = va_arg(vars->args, unsigned long);
            *zero = val == 0; *minus = val < 0;
            return itos(val, buf, len, radix, upper);
        }
        case MOD_ll:
        {
            auto val = va_arg(vars->args, unsigned long long);
            *zero = val == 0; *minus = val < 0;
            return itos(val, buf, len, radix, upper);
        }
        case MOD_j:
        {
            auto val = va_arg(vars->args, uintmax_t);
            *zero = val == 0; *minus = val < 0;
            return itos(val, buf, len, radix, upper);
        }
        case MOD_z:
        {
            auto val = va_arg(vars->args, size_t);
            *zero = val == 0; *minus = val < 0;
            return itos(val, buf, len, radix, upper);
        }
        case MOD_t:
        {
            auto val = va_arg(vars->args, std::make_unsigned<ptrdiff_t>::type);
            *zero = val == 0; *minus = val < 0;
            return itos(val, buf, len, radix, upper);
        }
        default:
        {
            auto val = va_arg(vars->args, unsigned int);
            *zero = val == 0; *minus = val < 0;
            return itos(val, buf, len, radix, upper);
        }
        }
    }

    template <typename Cy>
    void extract_diu(
        std::basic_string<Cy>& str, bool us, int flags, int precision, int width)
    {
        if (str.size() == 1 && str[0] == Cy('0') && precision == 0) {
            str.clear();
            return;
        }

        // sign
        Cy sign = 0;
        size_t sign_len = 0;
        std::basic_string_view<Cy> sv = str;
        if (!us) {
            if (sv[0] != Cy('-')) {
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
            zero_count = dc - sv.size();
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
        bool us, bool minus, bool zero, int flags, int precision, int width)
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
        if (!us) {
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
            zero_count = dc - d_len;
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
        std::basic_string<Cy>& str, int flags, int precision, int width)
    {
        if (str.size() == 1 && str[0] == Cy('0') && precision == 0) {
            if (!(flags & FLAG_ALTER)) {
                str.clear();
            }
            return;
        }

        // sign
        Cy sign = 0;
        size_t sign_len = 0;

        size_t zero_count = 0;
        if ((flags & FLAG_ALTER) && str[0] != Cy('0')) {
            zero_count += 1;
        }

        // precision
        size_t dc = precision == -2 ? 1u : size_t(precision);
        if (str.size() < dc) {
            zero_count += dc - str.size();
        }

        // width
        size_t space_count = 0;
        if ((flags & FLAG_ZERO) && precision == -2 && !(flags & FLAG_LEFT_JUST)) {
            if (str.size() + sign_len + zero_count < size_t(width)) {
                zero_count += size_t(width) - str.size() - sign_len - zero_count;
            }
        } else {
            if (str.size() + sign_len + zero_count < size_t(width)) {
                space_count = size_t(width) - str.size() - sign_len - zero_count;
            }
        }

        std::basic_string<Cy> _str;
        _str.reserve(zero_count + space_count + 1 + str.size());

        if (flags & FLAG_LEFT_JUST) {
            if (sign) {
                _str.push_back(sign);
            }
            _str.append(zero_count, Cy('0'));
            _str.append(str);
            _str.append(space_count, Cy(' '));
        } else {
            _str.append(space_count, Cy(' '));
            if (sign) {
                _str.push_back(sign);
            }
            _str.append(zero_count, Cy('0'));
            _str.append(str);
        }

        str = std::move(_str);
    }

    template <typename Cy>
    bool extract_o(
        Cy* str, size_t* len, size_t d_len,
        bool zero, int flags, int precision, int width)
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
            auto diff = zero_count + sign_len;
            if (diff > 0) {
                auto rs = s + d_len + diff;
                auto rds = s + d_len;
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
            auto diff = space_count + zero_count + sign_len;
            if (diff > 0) {
                auto rs = s + d_len + diff;
                auto rds = s + d_len;
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
        std::basic_string<Cy>& str, int flags, int precision, int width, Cy cp)
    {
        if (str.size() == 1 && str[0] == Cy('0') && precision == 0) {
            str.clear();
            return;
        }

        // sign
        Cy sign = 0;
        size_t sign_len = 0;

        size_t zero_count = 0;
        size_t pref_count = 0;
        if (flags & FLAG_ALTER) {
            zero_count = 2;
            pref_count = 2;
        }

        // precision
        size_t dc = precision == -2 ? 1u : size_t(precision);
        if (str.size() < dc) {
            zero_count += dc - str.size();
        }

        // width
        size_t space_count = 0;
        if (str.size() + sign_len + zero_count < size_t(width)) {
            if ((flags & FLAG_ZERO) && precision == -2 && !(flags & FLAG_LEFT_JUST)) {
                zero_count += size_t(width) - str.size() - sign_len - zero_count;
            } else {
                space_count = size_t(width) - str.size() - sign_len - zero_count;
            }
        }

        std::basic_string<Cy> _str;
        _str.reserve(zero_count + space_count + 1 + str.size());

        if (flags & FLAG_LEFT_JUST) {
            if (sign) {
                _str.push_back(sign);
            }
            if (flags & FLAG_ALTER) {
                _str.push_back(Cy('0'));
                _str.push_back(cp);
            }
            _str.append(zero_count - pref_count, Cy('0'));
            _str.append(str);
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
            _str.append(str);
        }

        str = std::move(_str);
    }

    template <typename Cy>
    bool extract_xX(
        Cy* str, size_t* len, size_t d_len,
        bool zero, int flags, int precision, int width, Cy cp)
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
            auto diff = zero_count + sign_len;
            if (diff > 0) {
                auto rs = s + d_len + diff;
                auto rds = s + d_len;
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
            auto diff = space_count + zero_count + sign_len;
            if (diff > 0) {
                auto rs = s + d_len + diff;
                auto rds = s + d_len;
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
        int flags, int precision, int modifier, int width, int fmt, vlw* args)
    {
        if (precision == -2) {
            precision = 6;
        }

        if (flags & FLAG_ALTER) {
            fmt |= FF_DIG;
        }

        std::basic_string<Cy> _str;
        if (modifier == MOD_L) {
            ftos(va_arg(args->args, long double), &_str, precision, fmt);
        } else {
            ftos(va_arg(args->args, double), &_str, precision, fmt);
        }

        // sign
        Cy sign = 0;
        size_t sign_len = 0;
        std::basic_string_view<Cy> sv = _str;
        if (sv[0] != Cy('-')) {
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
        int flags, int precision, int modifier, int width, int fmt, vlw* args)
    {
        if (precision == -2) {
            precision = 6;
        }

        if (flags & FLAG_ALTER) {
            fmt |= FF_DIG;
        }

        bool minus;
        size_t d_len = *len;
        if (modifier == MOD_L) {
            auto val = va_arg(args->args, long double);
            minus = std::signbit(val);
            if (!ftos(val, str, &d_len, precision, fmt)) {
                str = nullptr;
            }
        } else {
            auto val = va_arg(args->args, double);
            minus = std::signbit(val);
            if (!ftos(val, str, &d_len, precision, fmt)) {
                str = nullptr;
            }
        }

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
        int flags, int precision, int modifier, int width, int fmt, Cy cp, vlw* args)
    {
        size_t zero_count = 2;
        size_t pref_count = 2;
        fmt |= (FF_HEX2 | FF_ENZ);
        if (flags & FLAG_ALTER) {
            fmt |= FF_DIG;
        }
        if (precision == -2) {
            precision = 0;
            fmt |= FF_EXA;
        }

        std::basic_string<Cy> _str;
        if (modifier == MOD_L) {
            ftos(va_arg(args->args, long double), &_str, precision, fmt);
        } else {
            ftos(va_arg(args->args, double), &_str, precision, fmt);
        }

        // sign
        Cy sign = 0;
        size_t sign_len = 0;
        std::basic_string_view<Cy> sv = _str;
        if (sv[0] != Cy('-')) {
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
        int flags, int precision, int modifier, int width, int fmt, Cy cp, vlw* args)
    {
        size_t zero_count = 2;
        size_t pref_count = 2;
        fmt |= (FF_HEX2 | FF_ENZ);
        if (flags & FLAG_ALTER) {
            fmt |= FF_DIG;
        }
        if (precision == -2) {
            precision = 0;
            fmt |= FF_EXA;
        }

        bool minus;
        size_t d_len = *len;
        if (modifier == MOD_L) {
            auto val = va_arg(args->args, long double);
            minus = std::signbit(val);
            if (!ftos(val, str, &d_len, precision, fmt)) {
                str = nullptr;
            }
        } else {
            auto val = va_arg(args->args, double);
            minus = std::signbit(val);
            if (!ftos(val, str, &d_len, precision, fmt)) {
                str = nullptr;
            }
        }

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
    bool usprintf_base(
        const Cy* format, size_t len,
        std::basic_string<Cy>* out, vlw* vars) {
        int flags;
        int width;
        int precision;
        int modifier;
        std::basic_string<Cy> result;

        auto s = format;
        auto se = format + len;

        const Cy* prev_pos = s;
        for (; s < se; ++s) {
            if (*s == Cy('%')) {
                flags = 0;
                width = 0;
                precision = 0;
                modifier = 0;
                result.append(prev_pos, s - prev_pos);

                ++s;

                // flags
                parseFlags(&s, se - s, &flags);
                if (s >= se) {
                    prev_pos = s;
                    continue;
                }

                // width
                parseWidth(&s, se - s, &width);
                if (s >= se) {
                    prev_pos = s;
                    continue;
                }
                if (width == -1) {
                    width = va_arg(vars->args, int);
                    if (width < 0) {
                        flags |= FLAG_LEFT_JUST;
                        // 防止未定义行为
                        if (width == INT_MIN) {
                            width = INT_MAX;
                        } else {
                            width = -width;
                        }
                    }
                } else if (width == -2) {
                    width = 0;
                }

                // precision
                parsePrecision(&s, se - s, &precision);
                if (s >= se) {
                    prev_pos = s;
                    continue;
                }
                if (precision == -1) {
                    precision = va_arg(vars->args, int);
                    if (precision < 0) {
                        precision = -2;
                    }
                }

                // length modifier
                parseLengthModifier(&s, se - s, &modifier);
                if (s >= se) {
                    prev_pos = s;
                    continue;
                }

                // specifier
                switch (*s) {
                case Cy('%'):
                    result.push_back(Cy('%'));
                    break;

                case Cy('c'):
                    if (!extract_c(result, flags, modifier, width, vars)) {
                        return false;
                    }
                    break;

                case Cy('s'):
                    if (!extract_s(result, flags, precision, modifier, width, vars)) {
                        return false;
                    }
                    break;

                case Cy('d'):
                case Cy('i'):
                {
                    std::basic_string<Cy> _str;
                    switch (modifier) {
                    case MOD_hh: itos(va_arg(vars->args, int), &_str); break;
                    case MOD_h:  itos(va_arg(vars->args, int), &_str); break;
                    case MOD_l:  itos(va_arg(vars->args, long), &_str); break;
                    case MOD_ll: itos(va_arg(vars->args, long long), &_str); break;
                    case MOD_j:  itos(va_arg(vars->args, intmax_t), &_str); break;
                    case MOD_z:  itos(va_arg(vars->args, std::make_signed<size_t>::type), &_str); break;
                    case MOD_t:  itos(va_arg(vars->args, ptrdiff_t), &_str); break;
                    default:     itos(va_arg(vars->args, int), &_str); break;
                    }
                    extract_diu(_str, false, flags, precision, width);
                    result.append(_str);
                    break;
                }

                case Cy('o'):
                {
                    std::basic_string<Cy> _str;
                    switch (modifier) {
                    case MOD_hh: itos(va_arg(vars->args, unsigned int), &_str, 8); break;
                    case MOD_h:  itos(va_arg(vars->args, unsigned int), &_str, 8); break;
                    case MOD_l:  itos(va_arg(vars->args, unsigned long), &_str, 8); break;
                    case MOD_ll: itos(va_arg(vars->args, unsigned long long), &_str, 8); break;
                    case MOD_j:  itos(va_arg(vars->args, uintmax_t), &_str, 8); break;
                    case MOD_z:  itos(va_arg(vars->args, size_t), &_str, 8); break;
                    case MOD_t:  itos(va_arg(vars->args, std::make_unsigned<ptrdiff_t>::type), &_str, 8); break;
                    default:     itos(va_arg(vars->args, unsigned int), &_str, 8); break;
                    }

                    extract_o(_str, flags, precision, width);
                    result.append(_str);
                    break;
                }

                case Cy('x'):
                {
                    std::basic_string<Cy> _str;
                    switch (modifier) {
                    case MOD_hh: itos(va_arg(vars->args, unsigned int), &_str, 16); break;
                    case MOD_h:  itos(va_arg(vars->args, unsigned int), &_str, 16); break;
                    case MOD_l:  itos(va_arg(vars->args, unsigned long), &_str, 16); break;
                    case MOD_ll: itos(va_arg(vars->args, unsigned long long), &_str, 16); break;
                    case MOD_j:  itos(va_arg(vars->args, uintmax_t), &_str, 16); break;
                    case MOD_z:  itos(va_arg(vars->args, size_t), &_str, 16); break;
                    case MOD_t:  itos(va_arg(vars->args, std::make_unsigned<ptrdiff_t>::type), &_str, 16); break;
                    default:     itos(va_arg(vars->args, unsigned int), &_str, 16); break;
                    }

                    extract_xX(_str, flags, precision, width, Cy('x'));
                    result.append(_str);
                    break;
                }

                case Cy('X'):
                {
                    std::basic_string<Cy> _str;
                    switch (modifier) {
                    case MOD_hh: itos(va_arg(vars->args, unsigned int), &_str, 16, true); break;
                    case MOD_h:  itos(va_arg(vars->args, unsigned int), &_str, 16, true); break;
                    case MOD_l:  itos(va_arg(vars->args, unsigned long), &_str, 16, true); break;
                    case MOD_ll: itos(va_arg(vars->args, unsigned long long), &_str, 16, true); break;
                    case MOD_j:  itos(va_arg(vars->args, uintmax_t), &_str, 16, true); break;
                    case MOD_z:  itos(va_arg(vars->args, size_t), &_str, 16, true); break;
                    case MOD_t:  itos(va_arg(vars->args, std::make_unsigned<ptrdiff_t>::type), &_str, 16, true); break;
                    default:     itos(va_arg(vars->args, unsigned int), &_str, 16, true); break;
                    }

                    extract_xX(_str, flags, precision, width, Cy('X'));
                    result.append(_str);
                    break;
                }

                case Cy('u'):
                {
                    std::basic_string<Cy> _str;
                    switch (modifier) {
                    case MOD_hh: itos(va_arg(vars->args, unsigned int), &_str); break;
                    case MOD_h:  itos(va_arg(vars->args, unsigned int), &_str); break;
                    case MOD_l:  itos(va_arg(vars->args, unsigned long), &_str); break;
                    case MOD_ll: itos(va_arg(vars->args, unsigned long long), &_str); break;
                    case MOD_j:  itos(va_arg(vars->args, uintmax_t), &_str); break;
                    case MOD_z:  itos(va_arg(vars->args, size_t), &_str); break;
                    case MOD_t:  itos(va_arg(vars->args, std::make_unsigned<ptrdiff_t>::type), &_str); break;
                    default:     itos(va_arg(vars->args, unsigned int), &_str); break;
                    }

                    extract_diu(_str, true, flags, precision, width);
                    result.append(_str);
                    break;
                }

                case Cy('f'):
                {
                    extract_fFeEgG(
                        &result, flags, precision, modifier, width, 0, vars);
                    break;
                }

                case Cy('F'):
                {
                    extract_fFeEgG(
                        &result, flags, precision, modifier, width, FF_UPP, vars);
                    break;
                }

                case Cy('e'):
                {
                    extract_fFeEgG(
                        &result, flags, precision, modifier, width, FF_SCI, vars);
                    break;
                }

                case Cy('E'):
                {
                    extract_fFeEgG(
                        &result, flags, precision, modifier, width, FF_SCI | FF_UPP, vars);
                    break;
                }

                case Cy('a'):
                {
                    extract_aA(
                        &result, flags, precision, modifier, width, 0, Cy('x'), vars);
                    break;
                }

                case Cy('A'):
                {
                    extract_aA(
                        &result, flags, precision, modifier, width, FF_UPP, Cy('X'), vars);
                    break;
                }

                case Cy('g'):
                {
                    int fmt = 0;
                    if (!(flags & FLAG_ALTER)) {
                        fmt |= FF_NTZ;
                    }

                    extract_fFeEgG(
                        &result, flags, precision, modifier, width, fmt, vars);
                    break;
                }
                case Cy('G'):
                {
                    int fmt = FF_UPP;
                    if (!(flags & FLAG_ALTER)) {
                        fmt |= FF_NTZ;
                    }

                    extract_fFeEgG(
                        &result, flags, precision, modifier, width, fmt, vars);
                    break;
                }
                case Cy('n'): return false;
                case Cy('p'):
                {
                    std::basic_string<Cy> _str;
                    auto p = uintptr_t(va_arg(vars->args, void*));
                    itos(p, &_str, 16, true);
                    result.append(_str);
                    break;
                }
                default:break;
                }

                prev_pos = s + 1;
            }
        }

        result.append(prev_pos, se - prev_pos);
        *out = std::move(result);
        return true;
    }

    template <typename Cy>
    int usprintf_base(
        const Cy* format, size_t len,
        Cy* buf, size_t* buf_len, vlw* vars)
    {
        int flags;
        int width;
        int precision;
        int modifier;

        auto s = format;
        auto se = format + len;

        auto rs = buf;
        auto rse = rs ? rs + *buf_len : nullptr;
        size_t act_len = 0;

        const Cy* prev_pos = s;
        for (; s < se; ++s) {
            if (*s == Cy('%')) {
                flags = 0;
                width = 0;
                precision = 0;
                modifier = 0;

                if (rs && rse - rs >= s - prev_pos) {
                    for (auto p = prev_pos; p < s;) {
                        *rs++ = *p++;
                    }
                } else {
                    rs = rse;
                }
                act_len += s - prev_pos;

                ++s;

                // flags
                parseFlags(&s, se - s, &flags);
                if (s >= se) {
                    prev_pos = s;
                    continue;
                }

                // width
                parseWidth(&s, se - s, &width);
                if (s >= se) {
                    prev_pos = s;
                    continue;
                }
                if (width == -1) {
                    width = va_arg(vars->args, int);
                    if (width < 0) {
                        flags |= FLAG_LEFT_JUST;
                        // 防止未定义行为
                        if (width == INT_MIN) {
                            width = INT_MAX;
                        } else {
                            width = -width;
                        }
                    }
                } else if (width == -2) {
                    width = 0;
                }

                // precision
                parsePrecision(&s, se - s, &precision);
                if (s >= se) {
                    prev_pos = s;
                    continue;
                }
                if (precision == -1) {
                    precision = va_arg(vars->args, int);
                    if (precision < 0) {
                        precision = -2;
                    }
                }

                // length modifier
                parseLengthModifier(&s, se - s, &modifier);
                if (s >= se) {
                    prev_pos = s;
                    continue;
                }

                // specifier
                switch (*s) {
                case Cy('%'):
                    if (rs && rs < rse) {
                        *rs++ = Cy('%');
                    }
                    ++act_len;
                    break;

                case Cy('c'):
                {
                    size_t _len = rs ? rse - rs : 0u;
                    int ret = extract_c(rs, &_len, flags, modifier, width, vars);
                    if (ret == UCR_FAILED) {
                        return ret;
                    }
                    if (ret == UCR_BUFFER) {
                        rs = nullptr;
                    }
                    act_len += _len;
                    if (rs) rs += _len;
                    break;
                }

                case Cy('s'):
                {
                    size_t _len = rs ? rse - rs : 0u;
                    int ret = extract_s(rs, &_len, flags, precision, modifier, width, vars);
                    if (ret == UCR_FAILED) {
                        return ret;
                    }
                    if (ret == UCR_BUFFER) {
                        rs = nullptr;
                    }
                    act_len += _len;
                    if (rs) rs += _len;
                    break;
                }

                case Cy('d'):
                case Cy('i'):
                {
                    bool zero, minus;
                    size_t d_len = rs ? rse - rs : 0u;
                    if (!va_itos(modifier, rs, &d_len, &minus, &zero, vars)) {
                        rs = nullptr;
                    }
                    size_t _len = rs ? rse - rs : 0u;
                    if (!extract_diu(rs, &_len, d_len, false, minus, zero, flags, precision, width)) {
                        rs = nullptr;
                    }
                    act_len += _len;
                    if (rs) rs += _len;
                    break;
                }

                case Cy('o'):
                {
                    bool zero, minus;
                    size_t d_len = rs ? rse - rs : 0u;
                    if (!va_uitos(modifier, 8, false, rs, &d_len, &minus, &zero, vars)) {
                        rs = nullptr;
                    }
                    size_t _len = rs ? rse - rs : 0u;
                    if (!extract_o(rs, &_len, d_len, zero, flags, precision, width)) {
                        rs = nullptr;
                    }
                    act_len += _len;
                    if (rs) rs += _len;
                    break;
                }

                case Cy('x'):
                {
                    bool zero, minus;
                    size_t d_len = rs ? rse - rs : 0u;
                    if (!va_uitos(modifier, 16, false, rs, &d_len, &minus, &zero, vars)) {
                        rs = nullptr;
                    }
                    size_t _len = rs ? rse - rs : 0u;
                    if (!extract_xX(rs, &_len, d_len, zero, flags, precision, width, Cy('x'))) {
                        rs = nullptr;
                    }
                    act_len += _len;
                    if (rs) rs += _len;
                    break;
                }

                case Cy('X'):
                {
                    bool zero, minus;
                    size_t d_len = rs ? rse - rs : 0u;
                    if (!va_uitos(modifier, 16, true, rs, &d_len, &minus, &zero, vars)) {
                        rs = nullptr;
                    }

                    size_t _len = rs ? rse - rs : 0u;
                    if (!extract_xX(rs, &_len, d_len, zero, flags, precision, width, Cy('X'))) {
                        rs = nullptr;
                    }
                    act_len += _len;
                    if (rs) rs += _len;
                    break;
                }

                case Cy('u'):
                {
                    bool zero, minus;
                    size_t d_len = rs ? rse - rs : 0u;
                    if (!va_uitos(modifier, 10, false, rs, &d_len, &minus, &zero, vars)) {
                        rs = nullptr;
                    }

                    size_t _len = rs ? rse - rs : 0u;
                    if (!extract_diu(rs, &_len, d_len, true, minus, zero, flags, precision, width)) {
                        rs = nullptr;
                    }
                    act_len += _len;
                    if (rs) rs += _len;
                    break;
                }

                case Cy('f'):
                {
                    size_t _len = rs ? rse - rs : 0u;
                    if (!extract_fFeEgG(
                        rs, &_len, flags, precision, modifier, width, 0, vars))
                    {
                        rs = nullptr;
                    }
                    act_len += _len;
                    if (rs) rs += _len;
                    break;
                }

                case Cy('F'):
                {
                    size_t _len = rs ? rse - rs : 0u;
                    if (!extract_fFeEgG(
                        rs, &_len, flags, precision, modifier, width, FF_UPP, vars))
                    {
                        rs = nullptr;
                    }
                    act_len += _len;
                    if (rs) rs += _len;
                    break;
                }

                case Cy('e'):
                {
                    size_t _len = rs ? rse - rs : 0u;
                    if (!extract_fFeEgG(
                        rs, &_len, flags, precision, modifier, width, FF_SCI, vars))
                    {
                        rs = nullptr;
                    }
                    act_len += _len;
                    if (rs) rs += _len;
                    break;
                }

                case Cy('E'):
                {
                    size_t _len = rs ? rse - rs : 0u;
                    if (!extract_fFeEgG(
                        rs, &_len, flags, precision, modifier, width, FF_SCI | FF_UPP, vars))
                    {
                        rs = nullptr;
                    }
                    act_len += _len;
                    if (rs) rs += _len;
                    break;
                }

                case Cy('a'):
                {
                    size_t _len = rs ? rse - rs : 0u;
                    if (!extract_aA(
                        rs, &_len, flags, precision, modifier, width, 0, Cy('x'), vars))
                    {
                        rs = nullptr;
                    }
                    act_len += _len;
                    if (rs) rs += _len;
                    break;
                }

                case Cy('A'):
                {
                    size_t _len = rs ? rse - rs : 0u;
                    if (!extract_aA(
                        rs, &_len, flags, precision, modifier, width, FF_UPP, Cy('X'), vars))
                    {
                        rs = nullptr;
                    }
                    act_len += _len;
                    if (rs) rs += _len;
                    break;
                }

                case Cy('g'):
                {
                    int fmt = 0;
                    if (!(flags & FLAG_ALTER)) {
                        fmt |= FF_NTZ;
                    }

                    size_t _len = rs ? rse - rs : 0u;
                    if (!extract_fFeEgG(
                        rs, &_len, flags, precision, modifier, width, fmt, vars))
                    {
                        rs = nullptr;
                    }
                    act_len += _len;
                    if (rs) rs += _len;
                    break;
                }
                case Cy('G'):
                {
                    int fmt = FF_UPP;
                    if (!(flags & FLAG_ALTER)) {
                        fmt |= FF_NTZ;
                    }

                    size_t _len = rs ? rse - rs : 0u;
                    if (!extract_fFeEgG(
                        rs, &_len, flags, precision, modifier, width, fmt, vars))
                    {
                        rs = nullptr;
                    }
                    act_len += _len;
                    if (rs) rs += _len;
                    break;
                }
                case Cy('n'): return UCR_FAILED;
                case Cy('p'):
                {
                    size_t _len = rs ? rse - rs : 0u;
                    auto p = uintptr_t(va_arg(vars->args, void*));
                    if (!itos(p, rs, &_len, 16, true)) {
                        rs = nullptr;
                    }
                    act_len += _len;
                    if (rs) rs += _len;
                    break;
                }
                default:break;
                }

                prev_pos = s + 1;
            }
        }

        if (rs && rse - rs >= se - prev_pos) {
            for (auto p = prev_pos; p < se;) {
                *rs++ = *p++;
            }
        } else {
            rs = rse;
        }
        act_len += se - prev_pos;

        *buf_len = act_len;
        if (!rs || rs - buf != act_len) {
            return UCR_BUFFER;
        }
        return UCR_OK;
    }

}
}

#endif  // UTILS_USPRINTF_INTERNAL_HPP_
