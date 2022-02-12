// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STRINGS_USPRINTF_INTERNAL_HPP_
#define UTILS_STRINGS_USPRINTF_INTERNAL_HPP_

#include <cassert>
#include <cmath>
#include <cstdarg>

#include "utils/strings/int_conv.hpp"
#include "utils/strings/float_conv.h"
#include "utils/strings/unicode_conv.h"
#include "utils/strings/ustring_print_internal.hpp"
#include "utils/type_utils.hpp"

#define VA_ARG_ITOSB(type)  \
    {auto val = va_arg(vars->args, type);  \
    *zero = val == 0; *minus = val < 0;    \
    return itos(val, buf, len, radix, upper);}

#define VA_ARG_ITOSS(type)  \
    {auto val = va_arg(vars->args, type);  \
    *zero = val == 0; *minus = val < 0;    \
    itos(val, str, radix, upper); return;}


namespace utl {
namespace internal {

    struct vlw {
        va_list args;
    };

    inline int extract_c(
        char* r, size_t* len,
        int flags, int modifier, int width, vlw* args)
    {
        char buf[4];
        size_t _len = 4;
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
        size_t _len = 2;
        if (modifier == MOD_ll) {
            utf32_to_utf16(va_arg(args->args, utl::upromote<char32_t>::type), buf, &_len);
        } else if (modifier == MOD_l) {
            buf[0] = va_arg(args->args, utl::upromote<char16_t>::type);
            _len = 1;
        } else {
            if (!utf8_to_utf16(
                char(va_arg(args->args, int)), buf))
            {
                return UCR_FAILED;
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
                return UCR_FAILED;
            }
        } else {
            if (!utf8_to_utf32(
                char(va_arg(args->args, int)), &ch))
            {
                return UCR_FAILED;
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

    inline bool extract_c(
        std::string& r,
        int flags, int modifier, int width, vlw* args)
    {
        char buf[4];
        size_t len = 4;
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
        size_t len = 2;
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

    inline int extract_s(
        char* r, size_t* len,
        int flags, int precision, int modifier, int width, vlw* args)
    {
        const char* dig;
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
            if (_len > *len) {
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
        char16_t* r, size_t* len,
        int flags, int precision, int modifier, int width, vlw* args)
    {
        const char16_t* dig;
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
            if (_len > *len) {
                dig = nullptr;
            }
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
        const char32_t* dig;
        size_t _len = *len;
        if (modifier == MOD_ll) {
            dig = va_arg(args->args, char32_t*);
            _len = std::char_traits<char32_t>::length(dig);
            if (_len > *len) {
                dig = nullptr;
            }
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
    bool va_itos(
        int modifier, int radix, bool upper,
        Cy* buf, size_t* len, bool* minus, bool* zero, vlw* vars)
    {
        switch (modifier) {
        case MOD_hh: VA_ARG_ITOSB(int);
        case MOD_h:  VA_ARG_ITOSB(int);
        case MOD_l:  VA_ARG_ITOSB(long);
        case MOD_ll: VA_ARG_ITOSB(long long);
        case MOD_j:  VA_ARG_ITOSB(intmax_t);
        case MOD_z:  VA_ARG_ITOSB(std::make_signed<size_t>::type);
        case MOD_t:  VA_ARG_ITOSB(ptrdiff_t);
        default:     VA_ARG_ITOSB(int);
        }
    }

    template <typename Cy>
    void va_itos(
        int modifier, int radix, bool upper,
        std::basic_string<Cy>* str, bool* minus, bool* zero, vlw* vars)
    {
        switch (modifier) {
        case MOD_hh: VA_ARG_ITOSS(int);
        case MOD_h:  VA_ARG_ITOSS(int);
        case MOD_l:  VA_ARG_ITOSS(long);
        case MOD_ll: VA_ARG_ITOSS(long long);
        case MOD_j:  VA_ARG_ITOSS(intmax_t);
        case MOD_z:  VA_ARG_ITOSS(std::make_signed<size_t>::type);
        case MOD_t:  VA_ARG_ITOSS(ptrdiff_t);
        default:     VA_ARG_ITOSS(int);
        }
    }

    template <typename Cy>
    bool va_uitos(
        int modifier, int radix, bool upper,
        Cy* buf, size_t* len, bool* minus, bool* zero, vlw* vars)
    {
        switch (modifier) {
        case MOD_hh: VA_ARG_ITOSB(unsigned int);
        case MOD_h:  VA_ARG_ITOSB(unsigned int);
        case MOD_l:  VA_ARG_ITOSB(unsigned long);
        case MOD_ll: VA_ARG_ITOSB(unsigned long long);
        case MOD_j:  VA_ARG_ITOSB(uintmax_t);
        case MOD_z:  VA_ARG_ITOSB(size_t);
        case MOD_t:  VA_ARG_ITOSB(std::make_unsigned<ptrdiff_t>::type);
        default:     VA_ARG_ITOSB(unsigned int);
        }
    }

    template <typename Cy>
    void va_uitos(
        int modifier, int radix, bool upper,
        std::basic_string<Cy>* str, bool* minus, bool* zero, vlw* vars)
    {
        switch (modifier) {
        case MOD_hh: VA_ARG_ITOSS(unsigned int);
        case MOD_h:  VA_ARG_ITOSS(unsigned int);
        case MOD_l:  VA_ARG_ITOSS(unsigned long);
        case MOD_ll: VA_ARG_ITOSS(unsigned long long);
        case MOD_j:  VA_ARG_ITOSS(uintmax_t);
        case MOD_z:  VA_ARG_ITOSS(size_t);
        case MOD_t:  VA_ARG_ITOSS(std::make_unsigned<ptrdiff_t>::type);
        default:     VA_ARG_ITOSS(unsigned int);
        }
    }

    template <typename Cy>
    bool va_ftos_fFeEgG(
        int flags, int precision, int modifier, int fmt,
        Cy* buf, size_t* len, bool* minus, vlw* args)
    {
        if (precision == -2) {
            precision = 6;
        }
        if (flags & FLAG_ALTER) {
            fmt |= UFF_DIG;
        }

        if (modifier == MOD_L) {
            auto val = va_arg(args->args, long double);
            *minus = std::signbit(val);
            if (!ftos(val, buf, len, precision, fmt)) {
                return false;
            }
        } else {
            auto val = va_arg(args->args, double);
            *minus = std::signbit(val);
            if (!ftos(val, buf, len, precision, fmt)) {
                return false;
            }
        }
        return true;
    }

    template <typename Cy>
    void va_ftos_fFeEgG(
        int flags, int precision, int modifier, int fmt,
        std::basic_string<Cy>* str, bool* minus, vlw* args)
    {
        if (precision == -2) {
            precision = 6;
        }
        if (flags & FLAG_ALTER) {
            fmt |= UFF_DIG;
        }

        if (modifier == MOD_L) {
            auto val = va_arg(args->args, long double);
            *minus = std::signbit(val);
            ftos(val, str, precision, fmt);
        } else {
            auto val = va_arg(args->args, double);
            *minus = std::signbit(val);
            ftos(val, str, precision, fmt);
        }
    }

    template <typename Cy>
    bool va_ftos_aA(
        int flags, int precision, int modifier, int fmt,
        Cy* buf, size_t* len, bool* minus, vlw* args)
    {
        fmt |= (UFF_HEX2 | UFF_ENZ);
        if (flags & FLAG_ALTER) {
            fmt |= UFF_DIG;
        }
        if (precision == -2) {
            precision = 0;
            fmt |= UFF_EXA;
        }

        if (modifier == MOD_L) {
            auto val = va_arg(args->args, long double);
            *minus = std::signbit(val);
            if (!ftos(val, buf, len, precision, fmt)) {
                return false;
            }
        } else {
            auto val = va_arg(args->args, double);
            *minus = std::signbit(val);
            if (!ftos(val, buf, len, precision, fmt)) {
                return false;
            }
        }
        return true;
    }

    template <typename Cy>
    void va_ftos_aA(
        int flags, int precision, int modifier, int fmt,
        std::basic_string<Cy>* str, bool* minus, vlw* args)
    {
        fmt |= (UFF_HEX2 | UFF_ENZ);
        if (flags & FLAG_ALTER) {
            fmt |= UFF_DIG;
        }
        if (precision == -2) {
            precision = 0;
            fmt |= UFF_EXA;
        }
        if (modifier == MOD_L) {
            auto val = va_arg(args->args, long double);
            *minus = std::signbit(val);
            ftos(val, str, precision, fmt);
        } else {
            auto val = va_arg(args->args, double);
            *minus = std::signbit(val);
            ftos(val, str, precision, fmt);
        }
    }

    template <typename Cy>
    bool usprintf_base(
        const Cy* format, size_t len,
        std::basic_string<Cy>* out, vlw* vars)
    {
        int flags;
        int width;
        int precision;
        int modifier;
        //PRIType pri;
        std::basic_string<Cy> result;

        auto s = format;
        auto se = format + len;
        auto prev_pos = s;

        for (; s < se; ++s) {
            if (*s != Cy('%')) {
                continue;
            }

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

            /*auto pri_s = s;
            parsePRI(&pri_s, se - s, &pri);
            if (s >= se) {
                prev_pos = s;
                continue;
            }*/

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
                bool zero, minus;
                std::basic_string<Cy> _str;
                va_itos(modifier, 10, false, &_str, &minus, &zero, vars);
                extract_diu(_str, false, minus, zero, flags, precision, width);
                result.append(_str);
                break;
            }

            case Cy('o'):
            {
                bool zero, minus;
                std::basic_string<Cy> _str;
                va_uitos(modifier, 8, false, &_str, &minus, &zero, vars);
                extract_o(_str, true, minus, zero, flags, precision, width);
                result.append(_str);
                break;
            }

            case Cy('x'):
            {
                bool zero, minus;
                std::basic_string<Cy> _str;
                va_uitos(modifier, 16, false, &_str, &minus, &zero, vars);
                extract_xX(_str, true, minus, zero, flags, precision, width, Cy('x'));
                result.append(_str);
                break;
            }

            case Cy('X'):
            {
                bool zero, minus;
                std::basic_string<Cy> _str;
                va_uitos(modifier, 16, true, &_str, &minus, &zero, vars);
                extract_xX(_str, true, minus, zero, flags, precision, width, Cy('X'));
                result.append(_str);
                break;
            }

            case Cy('u'):
            {
                bool zero, minus;
                std::basic_string<Cy> _str;
                va_uitos(modifier, 10, false, &_str, &minus, &zero, vars);
                extract_diu(_str, true, minus, zero, flags, precision, width);
                result.append(_str);
                break;
            }

            case Cy('f'):
            {
                bool minus;
                std::basic_string<Cy> _str;
                va_ftos_fFeEgG(flags, precision, modifier, 0, &_str, &minus, vars);
                extract_fFeEgG(&result, flags, width, minus, _str);
                break;
            }

            case Cy('F'):
            {
                bool minus;
                std::basic_string<Cy> _str;
                va_ftos_fFeEgG(flags, precision, modifier, UFF_UPP, &_str, &minus, vars);
                extract_fFeEgG(&result, flags, width, minus, _str);
                break;
            }

            case Cy('e'):
            {
                bool minus;
                std::basic_string<Cy> _str;
                va_ftos_fFeEgG(flags, precision, modifier, UFF_SCI, &_str, &minus, vars);
                extract_fFeEgG(&result, flags, width, minus, _str);
                break;
            }

            case Cy('E'):
            {
                bool minus;
                std::basic_string<Cy> _str;
                va_ftos_fFeEgG(flags, precision, modifier, UFF_SCI | UFF_UPP, &_str, &minus, vars);
                extract_fFeEgG(&result, flags, width, minus, _str);
                break;
            }

            case Cy('a'):
            {
                bool minus;
                std::basic_string<Cy> _str;
                va_ftos_aA(flags, precision, modifier, 0, &_str, &minus, vars);
                extract_aA(&result, flags, width, Cy('x'), minus, _str);
                break;
            }

            case Cy('A'):
            {
                bool minus;
                std::basic_string<Cy> _str;
                va_ftos_aA(flags, precision, modifier, UFF_UPP, &_str, &minus, vars);
                extract_aA(&result, flags, width, Cy('X'), minus, _str);
                break;
            }

            case Cy('g'):
            {
                int fmt = 0;
                if (!(flags & FLAG_ALTER)) {
                    fmt |= UFF_NTZ;
                }

                bool minus;
                std::basic_string<Cy> _str;
                va_ftos_fFeEgG(flags, precision, modifier, fmt, &_str, &minus, vars);
                extract_fFeEgG(&result, flags, width, minus, _str);
                break;
            }
            case Cy('G'):
            {
                int fmt = UFF_UPP;
                if (!(flags & FLAG_ALTER)) {
                    fmt |= UFF_NTZ;
                }

                bool minus;
                std::basic_string<Cy> _str;
                va_ftos_fFeEgG(flags, precision, modifier, fmt, &_str, &minus, vars);
                extract_fFeEgG(&result, flags, width, minus, _str);
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
        auto prev_pos = s;

        auto rs = buf;
        auto rse = rs ? rs + *buf_len : nullptr;
        size_t act_len = 0;

        for (; s < se; ++s) {
            if (*s != Cy('%')) {
                continue;
            }

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
                if (!va_itos(modifier, 10, false, rs, &d_len, &minus, &zero, vars)) {
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
                if (!extract_o(rs, &_len, d_len, true, minus, zero, flags, precision, width)) {
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
                if (!extract_xX(rs, &_len, d_len, true, minus, zero, flags, precision, width, Cy('x'))) {
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
                if (!extract_xX(rs, &_len, d_len, true, minus, zero, flags, precision, width, Cy('X'))) {
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
                bool minus;
                size_t d_len = rs ? rse - rs : 0u;
                if (!va_ftos_fFeEgG(
                    flags, precision, modifier, 0, rs, &d_len, &minus, vars))
                {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_fFeEgG(
                    rs, &_len, flags, width, minus, d_len))
                {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                break;
            }

            case Cy('F'):
            {
                bool minus;
                size_t d_len = rs ? rse - rs : 0u;
                if (!va_ftos_fFeEgG(
                    flags, precision, modifier, UFF_UPP, rs, &d_len, &minus, vars))
                {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_fFeEgG(
                    rs, &_len, flags, width, minus, d_len))
                {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                break;
            }

            case Cy('e'):
            {
                bool minus;
                size_t d_len = rs ? rse - rs : 0u;
                if (!va_ftos_fFeEgG(
                    flags, precision, modifier, UFF_SCI, rs, &d_len, &minus, vars))
                {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_fFeEgG(
                    rs, &_len, flags, width, minus, d_len))
                {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                break;
            }

            case Cy('E'):
            {
                bool minus;
                size_t d_len = rs ? rse - rs : 0u;
                if (!va_ftos_fFeEgG(
                    flags, precision, modifier, UFF_SCI | UFF_UPP, rs, &d_len, &minus, vars))
                {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_fFeEgG(
                    rs, &_len, flags, width, minus, d_len))
                {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                break;
            }

            case Cy('a'):
            {
                bool minus;
                size_t d_len = rs ? rse - rs : 0u;
                if (!va_ftos_aA(
                    flags, precision, modifier, 0, rs, &d_len, &minus, vars))
                {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_aA(
                    rs, &_len, flags, width, Cy('x'), minus, d_len))
                {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                break;
            }

            case Cy('A'):
            {
                bool minus;
                size_t d_len = rs ? rse - rs : 0u;
                if (!va_ftos_aA(
                    flags, precision, modifier, UFF_UPP, rs, &d_len, &minus, vars))
                {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_aA(
                    rs, &_len, flags, width, Cy('X'), minus, d_len))
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
                    fmt |= UFF_NTZ;
                }

                bool minus;
                size_t d_len = rs ? rse - rs : 0u;
                if (!va_ftos_fFeEgG(
                    flags, precision, modifier, fmt, rs, &d_len, &minus, vars))
                {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_fFeEgG(
                    rs, &_len, flags, width, minus, d_len))
                {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                break;
            }
            case Cy('G'):
            {
                int fmt = UFF_UPP;
                if (!(flags & FLAG_ALTER)) {
                    fmt |= UFF_NTZ;
                }

                bool minus;
                size_t d_len = rs ? rse - rs : 0u;
                if (!va_ftos_fFeEgG(
                    flags, precision, modifier, fmt, rs, &d_len, &minus, vars))
                {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_fFeEgG(
                    rs, &_len, flags, width, minus, d_len))
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

        if (rs && rse - rs >= se - prev_pos) {
            for (auto p = prev_pos; p < se;) {
                *rs++ = *p++;
            }
        } else {
            rs = rse;
        }
        act_len += se - prev_pos;

        *buf_len = act_len;
        return rs && rs - buf == act_len ? UCR_OK : UCR_BUFFER;
    }

}
}

#endif  // UTILS_STRINGS_USPRINTF_INTERNAL_HPP_
