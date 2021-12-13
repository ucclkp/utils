// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "string_printf_u16.h"

#include <cstdarg>

#include "utils/convert.h"
#include "utils/unicode.h"

#define ADV_IDX  ++index; if (index >= count) return false;


namespace {

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

    bool parseFlags(
        const char16_t* format, size_t count, size_t& index, int* flags)
    {
        for (;;) {
            ADV_IDX;

            auto next = format[index];
            switch (next) {
            case u'-': *flags |= FLAG_LEFT_JUST; break;
            case u'+': *flags |= FLAG_SIGN;      break;
            case u' ': *flags |= FLAG_SPACE;     break;
            case u'#': *flags |= FLAG_ALTER;     break;
            case u'0': *flags |= FLAG_ZERO;      break;
            default: return true;
            }
        }
    }

    bool parseWidth(
        const char16_t* format, size_t count, size_t& index, int* width_mode)
    {
        for (;;) {
            auto next = format[index];
            if (next == u'*') {
                if (*width_mode != 0) {
                    return false;
                }
                *width_mode = -1;
                return true;
            }
            if (next >= u'0' && next <= u'9') {
                *width_mode = *width_mode * 10 + (next - u'0');
            } else {
                return true;
            }
            ADV_IDX;
        }
    }

    bool parsePrecision(
        const char16_t* format, size_t count, size_t& index, int* precision_mode)
    {
        auto next = format[index];
        if (next == u'.') {
            ADV_IDX;
            return parseWidth(format, count, index, precision_mode);
        }
        return true;
    }

    bool parseLengthModifier(
        const char16_t* format, size_t count, size_t& index, int* modifier)
    {
        auto next = format[index];

        switch (next) {
        case u'h':
            ADV_IDX;
            if (format[index] == u'h') {
                ++index;
                if (index >= count) return false;
                *modifier = MOD_hh;
            } else {
                *modifier = MOD_h;
            }
            break;
        case u'l':
            ADV_IDX;
            if (format[index] == u'l') {
                ++index;
                if (index >= count) return false;
                *modifier = MOD_ll;
            } else {
                *modifier = MOD_l;
            }
            break;
        case u'j': ADV_IDX; *modifier = MOD_j; break;
        case u'z': ADV_IDX; *modifier = MOD_z; break;
        case u't': ADV_IDX; *modifier = MOD_t; break;
        case u'L': ADV_IDX; *modifier = MOD_L; break;
        default: break;
        }

        return true;
    }

}

namespace utl {

    std::u16string stringPrintf_u16(const char16_t* format, ...) {
        va_list vars;
        va_start(vars, format);

        int flags;
        int width_mode;
        int precision_mode;
        int modifier;
        std::u16string result;

        size_t prev_pos = 0;
        size_t count = std::char_traits<char16_t>::length(format);
        for (size_t i = 0; i < count; ++i) {
            if (format[i] == u'%') {
                flags = 0;
                width_mode = 0;
                precision_mode = 0;
                modifier = 0;
                result.append(format + prev_pos, i - prev_pos);

                // flags
                if (!parseFlags(format, count, i, &flags)) {
                    prev_pos = i;
                    continue;
                }

                // width
                if (!parseWidth(format, count, i, &width_mode)) {
                    prev_pos = i;
                    continue;
                }

                // precision
                if (!parsePrecision(format, count, i, &precision_mode)) {
                    prev_pos = i;
                    continue;
                }

                // length modifier
                if (!parseLengthModifier(format, count, i, &modifier)) {
                    prev_pos = i;
                    continue;
                }

                // specifier
                auto next = format[i];
                switch (next) {
                case u'%':
                    result.push_back(u'%');
                    break;

                case u'c':
                    if (modifier == MOD_l) {
                        auto ch = char16_t(va_arg(vars, int));
                        result.push_back(ch);
                    } else {
                        char16_t ch;
                        if (!Unicode::UTF8ToUTF16(
                            char(va_arg(vars, wint_t)), &ch))
                        {
                            prev_pos = i + 1;
                            continue;
                        }
                        result.push_back(ch);
                    }
                    break;

                case u's':
                    if (modifier == MOD_l) {
                        auto u16_s = va_arg(vars, char16_t*);
                        result.append(u16_s);
                    } else {
                        std::u16string u16_str;
                        std::string_view u8_s(va_arg(vars, char*));
                        if (!Unicode::UTF8ToUTF16(u8_s, &u16_str)) {
                            prev_pos = i + 1;
                            continue;
                        }
                        result.append(u16_str);
                    }
                    break;

                case u'd':
                case u'i':
                    switch (modifier) {
                    case MOD_hh: result.append(to_u16string(va_arg(vars, signed char))); break;
                    case MOD_h:  result.append(to_u16string(va_arg(vars, short))); break;
                    case MOD_l:  result.append(to_u16string(va_arg(vars, long))); break;
                    case MOD_ll: result.append(to_u16string(va_arg(vars, long long))); break;
                    case MOD_j:  result.append(to_u16string(va_arg(vars, intmax_t))); break;
                    case MOD_z:  result.append(to_u16string(va_arg(vars, std::make_signed<size_t>::type))); break;
                    case MOD_t:  result.append(to_u16string(va_arg(vars, ptrdiff_t))); break;
                    default:     result.append(to_u16string(va_arg(vars, int))); break;
                    }
                    break;

                case u'o':
                {
                    std::ostringstream oss;
                    oss << std::oct;
                    switch (modifier) {
                    case MOD_hh: oss << va_arg(vars, unsigned char); break;
                    case MOD_h:  oss << va_arg(vars, unsigned short); break;
                    case MOD_l:  oss << va_arg(vars, unsigned long); break;
                    case MOD_ll: oss << va_arg(vars, unsigned long long); break;
                    case MOD_j:  oss << va_arg(vars, uintmax_t); break;
                    case MOD_z:  oss << va_arg(vars, size_t); break;
                    case MOD_t:  oss << va_arg(vars, std::make_unsigned<ptrdiff_t>::type); break;
                    default:     oss << va_arg(vars, unsigned int); break;
                    }

                    std::u16string u16_str;
                    if (!Unicode::UTF8ToUTF16(oss.str(), &u16_str)) {
                        prev_pos = i + 1;
                        continue;
                    }
                    result.append(u16_str);
                    break;
                }

                case u'x':
                {
                    std::ostringstream oss;
                    oss << std::hex;
                    switch (modifier) {
                    case MOD_hh: oss << va_arg(vars, unsigned char); break;
                    case MOD_h:  oss << va_arg(vars, unsigned short); break;
                    case MOD_l:  oss << va_arg(vars, unsigned long); break;
                    case MOD_ll: oss << va_arg(vars, unsigned long long); break;
                    case MOD_j:  oss << va_arg(vars, uintmax_t); break;
                    case MOD_z:  oss << va_arg(vars, size_t); break;
                    case MOD_t:  oss << va_arg(vars, std::make_unsigned<ptrdiff_t>::type); break;
                    default:     oss << va_arg(vars, unsigned int); break;
                    }

                    std::u16string u16_str;
                    if (!Unicode::UTF8ToUTF16(oss.str(), &u16_str)) {
                        prev_pos = i + 1;
                        continue;
                    }
                    result.append(u16_str);
                    break;
                }

                case u'X':
                {
                    std::ostringstream oss;
                    oss << std::hex << std::uppercase;
                    switch (modifier) {
                    case MOD_hh: oss << va_arg(vars, unsigned char); break;
                    case MOD_h:  oss << va_arg(vars, unsigned short); break;
                    case MOD_l:  oss << va_arg(vars, unsigned long); break;
                    case MOD_ll: oss << va_arg(vars, unsigned long long); break;
                    case MOD_j:  oss << va_arg(vars, uintmax_t); break;
                    case MOD_z:  oss << va_arg(vars, size_t); break;
                    case MOD_t:  oss << va_arg(vars, std::make_unsigned<ptrdiff_t>::type); break;
                    default:     oss << va_arg(vars, unsigned int); break;
                    }

                    std::u16string u16_str;
                    if (!Unicode::UTF8ToUTF16(oss.str(), &u16_str)) {
                        prev_pos = i + 1;
                        continue;
                    }
                    result.append(u16_str);
                    break;
                }

                case u'u':
                {
                    std::ostringstream oss;
                    switch (modifier) {
                    case MOD_hh: oss << va_arg(vars, unsigned char); break;
                    case MOD_h:  oss << va_arg(vars, unsigned short); break;
                    case MOD_l:  oss << va_arg(vars, unsigned long); break;
                    case MOD_ll: oss << va_arg(vars, unsigned long long); break;
                    case MOD_j:  oss << va_arg(vars, uintmax_t); break;
                    case MOD_z:  oss << va_arg(vars, size_t); break;
                    case MOD_t:  oss << va_arg(vars, std::make_unsigned<ptrdiff_t>::type); break;
                    default:     oss << va_arg(vars, unsigned int); break;
                    }

                    std::u16string u16_str;
                    if (!Unicode::UTF8ToUTF16(oss.str(), &u16_str)) {
                        prev_pos = i + 1;
                        continue;
                    }
                    result.append(u16_str);
                    break;
                }

                case u'f':
                {
                    std::ostringstream oss;
                    if (modifier == MOD_L) {
                        oss << va_arg(vars, long double);
                    } else {
                        oss << va_arg(vars, double);
                    }

                    std::u16string u16_str;
                    if (!Unicode::UTF8ToUTF16(oss.str(), &u16_str)) {
                        prev_pos = i + 1;
                        continue;
                    }
                    result.append(u16_str);
                    break;
                }

                case u'F':
                {
                    std::ostringstream oss;
                    oss << std::uppercase;
                    if (modifier == MOD_L) {
                        oss << va_arg(vars, long double);
                    } else {
                        oss << va_arg(vars, double);
                    }

                    std::u16string u16_str;
                    if (!Unicode::UTF8ToUTF16(oss.str(), &u16_str)) {
                        prev_pos = i + 1;
                        continue;
                    }
                    result.append(u16_str);
                    break;
                }

                case u'e':
                {
                    std::ostringstream oss;
                    oss << std::scientific;
                    if (modifier == MOD_L) {
                        oss << va_arg(vars, long double);
                    } else {
                        oss << va_arg(vars, double);
                    }

                    std::u16string u16_str;
                    if (!Unicode::UTF8ToUTF16(oss.str(), &u16_str)) {
                        prev_pos = i + 1;
                        continue;
                    }
                    result.append(u16_str);
                    break;
                }

                case u'E':
                {
                    std::ostringstream oss;
                    oss << std::scientific << std::uppercase;
                    if (modifier == MOD_L) {
                        oss << va_arg(vars, long double);
                    } else {
                        oss << va_arg(vars, double);
                    }

                    std::u16string u16_str;
                    if (!Unicode::UTF8ToUTF16(oss.str(), &u16_str)) {
                        prev_pos = i + 1;
                        continue;
                    }
                    result.append(u16_str);
                    break;
                }

                case u'a':
                {
                    std::ostringstream oss;
                    oss << std::hexfloat;
                    if (modifier == MOD_L) {
                        oss << va_arg(vars, long double);
                    } else {
                        oss << va_arg(vars, double);
                    }

                    std::u16string u16_str;
                    if (!Unicode::UTF8ToUTF16(oss.str(), &u16_str)) {
                        prev_pos = i + 1;
                        continue;
                    }
                    result.append(u16_str);
                    break;
                }

                case u'A':
                {
                    std::ostringstream oss;
                    oss << std::hexfloat << std::uppercase;
                    if (modifier == MOD_L) {
                        oss << va_arg(vars, long double);
                    } else {
                        oss << va_arg(vars, double);
                    }

                    std::u16string u16_str;
                    if (!Unicode::UTF8ToUTF16(oss.str(), &u16_str)) {
                        prev_pos = i + 1;
                        continue;
                    }
                    result.append(u16_str);
                    break;
                }

                case u'g': break;
                case u'G': break;
                case u'n': return {};
                case u'p': break;
                default:break;
                }

                prev_pos = i + 1;
            }
        }

        return result;
    }

}
