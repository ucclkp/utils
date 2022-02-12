// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_USFORMAT_INTERNAL_HPP_
#define UTILS_USFORMAT_INTERNAL_HPP_

#include <cmath>
#include <string>

#include "utils/float_conv.h"
#include "utils/number.hpp"
#include "utils/unicode_conv.h"
#include "utils/usformat_any_internal.hpp"
#include "utils/ustring_print_internal.hpp"

#define USFA_ITOSB(type)  \
    {auto val = std::get<type>(arg);  \
    *zero = val == 0; *minus = val < 0;    \
    return itos(val, buf, len, radix, upper) ? UCR_OK : UCR_BUFFER;}

#define USFA_ITOSB_X(type, cty)  \
    {auto val = std::get<type>(arg);  \
    *zero = val == 0; *minus = val < 0;    \
    return itos(static_cast<cty>(val), buf, len, radix, upper) ? UCR_OK : UCR_BUFFER;}

#define USFA_ITOSS(type)  \
    {auto val = std::get<type>(arg);  \
    *zero = val == 0; *minus = val < 0;    \
    itos(val, str, radix, upper); return true;}

#define USFA_ITOSS_X(type, cty)  \
    {auto val = std::get<type>(arg);  \
    *zero = val == 0; *minus = val < 0;    \
    itos(static_cast<cty>(val), str, radix, upper); return true;}

#define USFA_FTOSB(type)  \
    {auto val = std::get<type>(arg); \
    *minus = std::signbit(val);  \
    return ftos(val, buf, len, precision, fmt) ? UCR_OK : UCR_BUFFER;}

#define USFA_FTOSB_INT(type)  \
    {auto val = std::get<type>(arg);  \
    if (utl::can_num_cast<float>(val)) {  \
        auto val1 = static_cast<float>(val);  \
        *minus = std::signbit(val1);  \
        return ftos(val1, buf, len, precision, fmt) ? UCR_OK : UCR_BUFFER;  \
    }  \
    if (utl::can_num_cast<double>(val)) {  \
        auto val1 = static_cast<double>(val);  \
        *minus = std::signbit(val1);  \
        return ftos(val1, buf, len, precision, fmt) ? UCR_OK : UCR_BUFFER;  \
    }  \
    auto val1 = static_cast<long double>(val);  \
    *minus = std::signbit(val1);  \
    return ftos(val1, buf, len, precision, fmt) ? UCR_OK : UCR_BUFFER;}

#define USFA_FTOSS(type)  \
    {auto val = std::get<type>(arg); \
    *minus = std::signbit(val);  \
    ftos(val, str, precision, fmt); return true;}

#define USFA_FTOSS_INT(type)  \
    {auto val = std::get<type>(arg);  \
    if (utl::can_num_cast<float>(val)) {  \
        auto val1 = static_cast<float>(val);  \
        *minus = std::signbit(val1);  \
        ftos(val1, str, precision, fmt); return true;  \
    }  \
    if (utl::can_num_cast<double>(val)) {  \
        auto val1 = static_cast<double>(val);  \
        *minus = std::signbit(val1);  \
        ftos(val1, str, precision, fmt); return true;  \
    }  \
    auto val1 = static_cast<long double>(val);  \
    *minus = std::signbit(val1);  \
    ftos(val1, str, precision, fmt); return true;}


namespace utl {
namespace internal {

    template <typename Ty>
    bool get_usfa_i(const usformat_any& arg, Ty* out) {
        static_assert(std::is_integral<Ty>::value, "Ty must be a integral type!");

        switch (arg.index()) {
        case USFA_INT:       *out = utl::num_cast<Ty>(std::get<USFA_INT>(arg));       return true;
        case USFA_UINT:      *out = utl::num_cast<Ty>(std::get<USFA_UINT>(arg));      return true;
        case USFA_LONG:      *out = utl::num_cast<Ty>(std::get<USFA_LONG>(arg));      return true;
        case USFA_ULONG:     *out = utl::num_cast<Ty>(std::get<USFA_ULONG>(arg));     return true;
        case USFA_LONGLONG:  *out = utl::num_cast<Ty>(std::get<USFA_LONGLONG>(arg));  return true;
        case USFA_ULONGLONG: *out = utl::num_cast<Ty>(std::get<USFA_ULONGLONG>(arg)); return true;
        default: return false;
        }
    }

    template <typename Ty>
    bool get_usfa_ci(const usformat_any& arg, Ty* out) {
        static_assert(std::is_integral<Ty>::value, "Ty must be a integral type!");

        switch (arg.index()) {
        case USFA_INT:       *out = utl::num_cast<Ty>(std::get<USFA_INT>(arg));       return true;
        case USFA_UINT:      *out = utl::num_cast<Ty>(std::get<USFA_UINT>(arg));      return true;
        case USFA_LONG:      *out = utl::num_cast<Ty>(std::get<USFA_LONG>(arg));      return true;
        case USFA_ULONG:     *out = utl::num_cast<Ty>(std::get<USFA_ULONG>(arg));     return true;
        case USFA_LONGLONG:  *out = utl::num_cast<Ty>(std::get<USFA_LONGLONG>(arg));  return true;
        case USFA_ULONGLONG: *out = utl::num_cast<Ty>(std::get<USFA_ULONGLONG>(arg)); return true;
        case USFA_CHAR16:    *out = utl::num_cast<Ty>(std::get<USFA_CHAR16>(arg));    return true;
        case USFA_CHAR32:    *out = utl::num_cast<Ty>(std::get<USFA_CHAR32>(arg));    return true;
        default: return false;
        }
    }

    template <typename Cy, typename Sy>
    bool path_to_ub(const Sy& p, const Cy** dig, size_t* len, Cy* r) {
        assert(false);
        return false;
    }

    template <typename Cy>
    bool path_to_ub(const std::string& p, const Cy** dig, size_t* len, Cy* r) {
        if constexpr (std::is_same<Cy, char>::value) {
            *dig = p.c_str();
            *len = p.size();
        } else if constexpr (std::is_same<Cy, char16_t>::value) {
            int ret = utf8_to_utf16(p, r, len);
            if (ret == UCR_FAILED) {
                return false;
            }
            if (ret == UCR_OK) {
                *dig = r;
            } else {
                *dig = nullptr;
            }
        } else if constexpr (std::is_same<Cy, char32_t>::value) {
            int ret = utf8_to_utf32(p, r, len);
            if (ret == UCR_FAILED) {
                return false;
            }
            if (ret == UCR_OK) {
                *dig = r;
            } else {
                *dig = nullptr;
            }
        } else {
            return false;
        }
        return true;
    }

    template <typename Cy>
    bool path_to_ub(const std::wstring& p, const Cy** dig, size_t* len, Cy* r) {
        if constexpr (std::is_same<Cy, char>::value) {
            int ret = wchar_to_utf8(p, r, len);
            if (ret == UCR_FAILED) {
                return false;
            }
            if (ret == UCR_OK) {
                *dig = r;
            } else {
                *dig = nullptr;
            }
        } else if constexpr (std::is_same<Cy, char16_t>::value) {
            int ret = wchar_to_utf16(p, r, len);
            if (ret == UCR_FAILED) {
                return false;
            }
            if (ret == UCR_OK) {
                *dig = r;
            } else {
                *dig = nullptr;
            }
        } else if constexpr (std::is_same<Cy, char32_t>::value) {
            int ret = wchar_to_utf32(p, r, len);
            if (ret == UCR_FAILED) {
                return false;
            }
            if (ret == UCR_OK) {
                *dig = r;
            } else {
                *dig = nullptr;
            }
        } else {
            return false;
        }
        return true;
    }

    template <typename Cy, typename Sy>
    bool path_to_usv(
        const std::string& p,
        std::basic_string_view<Cy>& sv,
        std::basic_string<Cy>& str)
    {
        assert(false);
        return false;
    }

    template <typename Cy>
    bool path_to_usv(
        const std::string& p,
        std::basic_string_view<Cy>& sv,
        std::basic_string<Cy>& str)
    {
        if constexpr (std::is_same<Cy, char>::value) {
            sv = p;
        } else if constexpr (std::is_same<Cy, char16_t>::value) {
            if (!utf8_to_utf16(p, &str)) {
                return false;
            }
            sv = str;
        } else if constexpr (std::is_same<Cy, char32_t>::value) {
            if (!utf8_to_utf32(p, &str)) {
                return false;
            }
            sv = str;
        } else {
            return false;
        }
        return true;
    }

    template <typename Cy>
    bool path_to_usv(
        const std::wstring& p,
        std::basic_string_view<Cy>& sv,
        std::basic_string<Cy>& str)
    {
        if constexpr (std::is_same<Cy, char>::value) {
            if (!wchar_to_utf8(p, &str)) {
                return false;
            }
            sv = str;
        } else if constexpr (std::is_same<Cy, char16_t>::value) {
            if (!wchar_to_utf16(p, &str)) {
                return false;
            }
            sv = str;
        } else if constexpr (std::is_same<Cy, char32_t>::value) {
            if (!wchar_to_utf32(p, &str)) {
                return false;
            }
            sv = str;
        } else {
            return false;
        }
        return true;
    }

    inline int extract_t_c(
        char* r, size_t* len,
        int flags, int modifier, int width, const usformat_any& arg)
    {
        char buf[4];
        size_t _len = 4;
        if (modifier == MOD_ll) {
            char32_t val;
            if (!get_usfa_ci(arg, &val)) {
                return UCR_FAILED;
            }
            utf32_to_utf8(val, buf, &_len);
        } else if (modifier == MOD_l) {
            char16_t val;
            if (!get_usfa_ci(arg, &val)) {
                return UCR_FAILED;
            }
            if (!utf16_to_utf8(val, buf, &_len)) {
                return UCR_FAILED;
            }
        } else {
            char val;
            if (!get_usfa_ci(arg, &val)) {
                return UCR_FAILED;
            }
            buf[0] = val;
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

    inline int extract_t_c(
        char16_t* r, size_t* len,
        int flags, int modifier, int width, const usformat_any& arg)
    {
        char16_t buf[2];
        size_t _len = 2;
        if (modifier == MOD_ll) {
            char32_t val;
            if (!get_usfa_ci(arg, &val)) {
                return UCR_FAILED;
            }
            utf32_to_utf16(val, buf, &_len);
        } else if (modifier == MOD_l) {
            char16_t val;
            if (!get_usfa_ci(arg, &val)) {
                return UCR_FAILED;
            }
            buf[0] = val;
            _len = 1;
        } else {
            char val;
            if (!get_usfa_ci(arg, &val)) {
                return UCR_FAILED;
            }
            if (!utf8_to_utf16(val, buf)) {
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

    inline int extract_t_c(
        char32_t* r, size_t* len,
        int flags, int modifier, int width, const usformat_any& arg)
    {
        char32_t ch;
        if (modifier == MOD_ll) {
            if (!get_usfa_ci(arg, &ch)) {
                return UCR_FAILED;
            }
        } else if (modifier == MOD_l) {
            char16_t val;
            if (!get_usfa_ci(arg, &val)) {
                return UCR_FAILED;
            }
            if (!utf16_to_utf32(val, &ch)) {
                return UCR_FAILED;
            }
        } else {
            char val;
            if (!get_usfa_ci(arg, &val)) {
                return UCR_FAILED;
            }
            if (!utf8_to_utf32(val, &ch)) {
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

    inline bool extract_t_c(
        std::string& r,
        int flags, int modifier, int width, const usformat_any& arg)
    {
        char buf[4];
        size_t len = 4;
        if (modifier == MOD_ll) {
            char32_t val;
            if (!get_usfa_ci(arg, &val)) {
                return false;
            }
            utf32_to_utf8(val, buf, &len);
        } else if (modifier == MOD_l) {
            char16_t val;
            if (!get_usfa_ci(arg, &val)) {
                return false;
            }
            if (!utf16_to_utf8(val, buf, &len)) {
                return false;
            }
        } else {
            char val;
            if (!get_usfa_ci(arg, &val)) {
                return false;
            }
            buf[0] = val;
            len = 1;
        }

        fill_c(buf, len, r, flags, width);
        return true;
    }

    inline bool extract_t_c(
        std::u16string& r,
        int flags, int modifier, int width, const usformat_any& arg)
    {
        char16_t buf[2];
        size_t len = 2;
        if (modifier == MOD_ll) {
            char32_t val;
            if (!get_usfa_ci(arg, &val)) {
                return false;
            }
            utf32_to_utf16(val, buf, &len);
        } else if (modifier == MOD_l) {
            char16_t val;
            if (!get_usfa_ci(arg, &val)) {
                return false;
            }
            buf[0] = val;
            len = 1;
        } else {
            char val;
            if (!get_usfa_ci(arg, &val)) {
                return false;
            }
            if (!utf8_to_utf16(val, buf)) {
                return false;
            }
            len = 1;
        }

        fill_c(buf, len, r, flags, width);
        return true;
    }

    inline bool extract_t_c(
        std::u32string& r,
        int flags, int modifier, int width, const usformat_any& arg)
    {
        char32_t ch;
        if (modifier == MOD_ll) {
            if (!get_usfa_ci(arg, &ch)) {
                return false;
            }
        } else if (modifier == MOD_l) {
            char16_t val;
            if (!get_usfa_ci(arg, &val)) {
                return false;
            }
            if (!utf16_to_utf32(val, &ch)) {
                return false;
            }
        } else {
            char val;
            if (!get_usfa_ci(arg, &val)) {
                return false;
            }
            if (!utf8_to_utf32(val, &ch)) {
                return false;
            }
        }

        fill_c(&ch, 1, r, flags, width);
        return true;
    }

    inline int extract_t_s(
        char* r, size_t* len,
        int flags, int precision, int width, const usformat_any& arg)
    {
        const char* dig;
        size_t _len = *len;

        switch (arg.index()) {
        case USFA_SV32:
        case USFA_CHAR32P:
        {
            std::u32string_view u32_s(
                arg.index() == USFA_CHAR32P ?
                std::get<USFA_CHAR32P>(arg) : std::get<USFA_SV32>(arg));
            if (utf32_to_utf8(u32_s, r, &_len)) {
                dig = r;
            } else {
                dig = nullptr;
            }
            break;
        }

        case USFA_SV16:
        case USFA_CHAR16P:
        {
            std::u16string_view u16_s(
                arg.index() == USFA_CHAR16P ?
                std::get<USFA_CHAR16P>(arg) : std::get<USFA_SV16>(arg));
            int ret = utf16_to_utf8(u16_s, r, &_len);
            if (ret == UCR_FAILED) {
                return UCR_FAILED;
            }
            if (ret == UCR_OK) {
                dig = r;
            } else {
                dig = nullptr;
            }
            break;
        }

        case USFA_SV:
        case USFA_CHARP:
        {
            dig = arg.index() == USFA_CHARP ?
                std::get<USFA_CHARP>(arg) : std::get<USFA_SV>(arg).data();
            _len = std::char_traits<char>::length(dig);
            if (_len > *len) {
                dig = nullptr;
            }
            break;
        }

        case USFA_WSV:
        case USFA_WCHARP:
        {
            std::wstring_view w_s(
                arg.index() == USFA_WCHARP ?
                std::get<USFA_WCHARP>(arg) : std::get<USFA_WSV>(arg));
            int ret = wchar_to_utf8(w_s, r, &_len);
            if (ret == UCR_FAILED) {
                return UCR_FAILED;
            }
            if (ret == UCR_OK) {
                dig = r;
            } else {
                dig = nullptr;
            }
            break;
        }

        case USFA_FSPATH:
        {
            auto fsp = std::get<USFA_FSPATH>(arg);
            if (!path_to_ub(fsp.ptr->native(), &dig, &_len, r)) {
                return false;
            }
            break;
        }

        default: return UCR_FAILED;
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

    inline int extract_t_s(
        char16_t* r, size_t* len,
        int flags, int precision, int width, const usformat_any& arg)
    {
        const char16_t* dig;
        size_t _len = *len;

        switch (arg.index()) {
        case USFA_SV32:
        case USFA_CHAR32P:
        {
            std::u32string_view u32_s(
                arg.index() == USFA_CHAR32P ?
                std::get<USFA_CHAR32P>(arg) : std::get<USFA_SV32>(arg));
            if (utf32_to_utf16(u32_s, r, &_len)) {
                dig = r;
            } else {
                dig = nullptr;
            }
            break;
        }

        case USFA_SV16:
        case USFA_CHAR16P:
        {
            dig = arg.index() == USFA_CHAR16P ?
                std::get<USFA_CHAR16P>(arg) : std::get<USFA_SV16>(arg).data();
            _len = std::char_traits<char16_t>::length(dig);
            if (_len > *len) {
                dig = nullptr;
            }
            break;
        }

        case USFA_SV:
        case USFA_CHARP:
        {
            std::string_view u8_s(
                arg.index() == USFA_CHARP ?
                std::get<USFA_CHARP>(arg) : std::get<USFA_SV>(arg));
            int ret = utf8_to_utf16(u8_s, r, &_len);
            if (ret == UCR_FAILED) {
                return UCR_FAILED;
            }
            if (ret == UCR_OK) {
                dig = r;
            } else {
                dig = nullptr;
            }
            break;
        }

        case USFA_WSV:
        case USFA_WCHARP:
        {
            std::wstring_view w_s(
                arg.index() == USFA_WCHARP ?
                std::get<USFA_WCHARP>(arg) : std::get<USFA_WSV>(arg));
            int ret = wchar_to_utf16(w_s, r, &_len);
            if (ret == UCR_FAILED) {
                return UCR_FAILED;
            }
            if (ret == UCR_OK) {
                dig = r;
            } else {
                dig = nullptr;
            }
            break;
        }

        case USFA_FSPATH:
        {
            auto fsp = std::get<USFA_FSPATH>(arg);
            if (!path_to_ub(fsp.ptr->native(), &dig, &_len, r)) {
                return false;
            }
            break;
        }

        default: return UCR_FAILED;
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

    inline int extract_t_s(
        char32_t* r, size_t* len,
        int flags, int precision, int width, const usformat_any& arg)
    {
        const char32_t* dig;
        size_t _len = *len;

        switch (arg.index()) {
        case USFA_SV32:
        case USFA_CHAR32P:
        {
            dig = arg.index() == USFA_CHAR32P ?
                std::get<USFA_CHAR32P>(arg) : std::get<USFA_SV32>(arg).data();
            _len = std::char_traits<char32_t>::length(dig);
            if (_len > *len) {
                dig = nullptr;
            }
            break;
        }

        case USFA_SV16:
        case USFA_CHAR16P:
        {
            std::u16string_view u16_s(
                arg.index() == USFA_CHAR16P ?
                std::get<USFA_CHAR16P>(arg) : std::get<USFA_SV16>(arg));
            int ret = utf16_to_utf32(u16_s, r, &_len);
            if (ret == UCR_FAILED) {
                return UCR_FAILED;
            }
            if (ret == UCR_OK) {
                dig = r;
            } else {
                dig = nullptr;
            }
            break;
        }

        case USFA_SV:
        case USFA_CHARP:
        {
            std::string_view u8_s(
                arg.index() == USFA_CHARP ?
                std::get<USFA_CHARP>(arg) : std::get<USFA_SV>(arg));
            int ret = utf8_to_utf32(u8_s, r, &_len);
            if (ret == UCR_FAILED) {
                return UCR_FAILED;
            }
            if (ret == UCR_OK) {
                dig = r;
            } else {
                dig = nullptr;
            }
            break;
        }

        case USFA_WSV:
        case USFA_WCHARP:
        {
            std::wstring_view w_s(
                arg.index() == USFA_WCHARP ?
                std::get<USFA_WCHARP>(arg) : std::get<USFA_WSV>(arg));
            int ret = wchar_to_utf32(w_s, r, &_len);
            if (ret == UCR_FAILED) {
                return UCR_FAILED;
            }
            if (ret == UCR_OK) {
                dig = r;
            } else {
                dig = nullptr;
            }
            break;
        }

        case USFA_FSPATH:
        {
            auto fsp = std::get<USFA_FSPATH>(arg);
            if (!path_to_ub(fsp.ptr->native(), &dig, &_len, r)) {
                return false;
            }
            break;
        }

        default: return UCR_FAILED;
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

    inline bool extract_t_s(
        std::string& r,
        int flags, int precision, int width, const usformat_any& arg)
    {
        std::string u8_str;
        std::string_view sv;

        switch (arg.index()) {
        case USFA_SV32:
        case USFA_CHAR32P:
        {
            std::u32string_view u32_s(
                arg.index() == USFA_CHAR32P ?
                std::get<USFA_CHAR32P>(arg) : std::get<USFA_SV32>(arg));
            utf32_to_utf8(u32_s, &u8_str);
            sv = u8_str;
            break;
        }

        case USFA_SV16:
        case USFA_CHAR16P:
        {
            std::u16string_view u16_s(
                arg.index() == USFA_CHAR16P ?
                std::get<USFA_CHAR16P>(arg) : std::get<USFA_SV16>(arg));
            if (!utf16_to_utf8(u16_s, &u8_str)) {
                return false;
            }
            sv = u8_str;
            break;
        }

        case USFA_SV:
        case USFA_CHARP:
        {
            sv = arg.index() == USFA_CHARP ?
                std::get<USFA_CHARP>(arg) : std::get<USFA_SV>(arg);
            break;
        }

        case USFA_WSV:
        case USFA_WCHARP:
        {
            std::wstring_view w_s(
                arg.index() == USFA_WCHARP ?
                std::get<USFA_WCHARP>(arg) : std::get<USFA_WSV>(arg));
            if (!wchar_to_utf8(w_s, &u8_str)) {
                return false;
            }
            sv = u8_str;
            break;
        }

        case USFA_FSPATH:
        {
            auto fsp = std::get<USFA_FSPATH>(arg);
            if (!path_to_usv(fsp.ptr->native(), sv, u8_str)) {
                return false;
            }
            break;
        }

        default: return false;
        }

        if (precision != -2) {
            sv = sv.substr(0, size_t(precision));
        }

        fill_s(r, sv, flags, width);
        return true;
    }

    inline bool extract_t_s(
        std::u16string& r,
        int flags, int precision, int width, const usformat_any& arg)
    {
        std::u16string u16_str;
        std::u16string_view sv;

        switch (arg.index()) {
        case USFA_SV32:
        case USFA_CHAR32P:
        {
            std::u32string_view u32_s(
                arg.index() == USFA_CHAR32P ?
                std::get<USFA_CHAR32P>(arg) : std::get<USFA_SV32>(arg));
            utf32_to_utf16(u32_s, &u16_str);
            sv = u16_str;
            break;
        }

        case USFA_SV16:
        case USFA_CHAR16P:
        {
            sv = arg.index() == USFA_CHAR16P ?
                std::get<USFA_CHAR16P>(arg) : std::get<USFA_SV16>(arg);
            break;
        }

        case USFA_SV:
        case USFA_CHARP:
        {
            std::string_view u8_s(
                arg.index() == USFA_CHARP ?
                std::get<USFA_CHARP>(arg) : std::get<USFA_SV>(arg));
            if (!utf8_to_utf16(u8_s, &u16_str)) {
                return false;
            }
            sv = u16_str;
            break;
        }

        case USFA_WSV:
        case USFA_WCHARP:
        {
            std::wstring_view w_s(
                arg.index() == USFA_WCHARP ?
                std::get<USFA_WCHARP>(arg) : std::get<USFA_WSV>(arg));
            if (!wchar_to_utf16(w_s, &u16_str)) {
                return false;
            }
            sv = u16_str;
            break;
        }

        case USFA_FSPATH:
        {
            auto fsp = std::get<USFA_FSPATH>(arg);
            if (!path_to_usv(fsp.ptr->native(), sv, u16_str)) {
                return false;
            }
            break;
        }

        default: return false;
        }

        if (precision != -2) {
            sv = sv.substr(0, size_t(precision));
        }

        fill_s(r, sv, flags, width);
        return true;
    }

    inline bool extract_t_s(
        std::u32string& r,
        int flags, int precision, int width, const usformat_any& arg)
    {
        std::u32string u32_str;
        std::u32string_view sv;

        switch (arg.index()) {
        case USFA_SV32:
        case USFA_CHAR32P:
        {
            sv = arg.index() == USFA_CHAR32P ?
                std::get<USFA_CHAR32P>(arg) : std::get<USFA_SV32>(arg);
            break;
        }

        case USFA_SV16:
        case USFA_CHAR16P:
        {
            std::u16string_view u16_s(
                arg.index() == USFA_CHAR16P ?
                std::get<USFA_CHAR16P>(arg) : std::get<USFA_SV16>(arg));
            if (!utf16_to_utf32(u16_s, &u32_str)) {
                return false;
            }
            sv = u32_str;
            break;
        }

        case USFA_SV:
        case USFA_CHARP:
        {
            std::string_view u8_s(
                arg.index() == USFA_CHARP ?
                std::get<USFA_CHARP>(arg) : std::get<USFA_SV>(arg));
            if (!utf8_to_utf32(u8_s, &u32_str)) {
                return false;
            }
            sv = u32_str;
            break;
        }

        case USFA_WSV:
        case USFA_WCHARP:
        {
            std::wstring_view w_s(
                arg.index() == USFA_WCHARP ?
                std::get<USFA_WCHARP>(arg) : std::get<USFA_WSV>(arg));
            if (!wchar_to_utf32(w_s, &u32_str)) {
                return false;
            }
            sv = u32_str;
            break;
        }

        case USFA_FSPATH:
        {
            auto fsp = std::get<USFA_FSPATH>(arg);
            if (!path_to_usv(fsp.ptr->native(), sv, u32_str)) {
                return false;
            }
            break;
        }

        default: return false;
        }

        if (precision != -2) {
            sv = sv.substr(0, size_t(precision));
        }

        fill_s(r, sv, flags, width);
        return true;
    }

    template <typename Cy>
    int usfa_itos(
        int radix, bool upper,
        Cy* buf, size_t* len, bool* minus, bool* zero, const usformat_any& arg)
    {
        switch (arg.index()) {
        case USFA_INT:       USFA_ITOSB(USFA_INT);
        case USFA_UINT:      USFA_ITOSB(USFA_UINT);
        case USFA_LONG:      USFA_ITOSB(USFA_LONG);
        case USFA_ULONG:     USFA_ITOSB(USFA_ULONG);
        case USFA_LONGLONG:  USFA_ITOSB(USFA_LONGLONG);
        case USFA_ULONGLONG: USFA_ITOSB(USFA_ULONGLONG);
        case USFA_FLOAT:     USFA_ITOSB_X(USFA_FLOAT, intmax_t);
        case USFA_DOUBLE:    USFA_ITOSB_X(USFA_DOUBLE, intmax_t);
        case USFA_LDOUBLE:   USFA_ITOSB_X(USFA_LDOUBLE, intmax_t);
        case USFA_CHAR16:    USFA_ITOSB(USFA_CHAR16);
        case USFA_CHAR32:    USFA_ITOSB(USFA_CHAR32);
        default: return UCR_FAILED;
        }
    }

    template <typename Cy>
    bool usfa_itos(
        int radix, bool upper,
        std::basic_string<Cy>* str, bool* minus, bool* zero, const usformat_any& arg)
    {
        switch (arg.index()) {
        case USFA_INT:       USFA_ITOSS(USFA_INT);
        case USFA_UINT:      USFA_ITOSS(USFA_UINT);
        case USFA_LONG:      USFA_ITOSS(USFA_LONG);
        case USFA_ULONG:     USFA_ITOSS(USFA_ULONG);
        case USFA_LONGLONG:  USFA_ITOSS(USFA_LONGLONG);
        case USFA_ULONGLONG: USFA_ITOSS(USFA_ULONGLONG);
        case USFA_FLOAT:     USFA_ITOSS_X(USFA_FLOAT, intmax_t);
        case USFA_DOUBLE:    USFA_ITOSS_X(USFA_DOUBLE, intmax_t);
        case USFA_LDOUBLE:   USFA_ITOSS_X(USFA_LDOUBLE, intmax_t);
        case USFA_CHAR16:    USFA_ITOSS(USFA_CHAR16);
        case USFA_CHAR32:    USFA_ITOSS(USFA_CHAR32);
        default: return false;
        }
    }

    template <typename Cy>
    int usfa_ftos_fFeEgG(
        int flags, int precision, int fmt,
        Cy* buf, size_t* len, bool* minus, const usformat_any& arg)
    {
        if (precision == -2) {
            precision = 6;
        }
        if (flags & FLAG_ALTER) {
            fmt |= UFF_DIG;
        }

        switch (arg.index()) {
        case USFA_FLOAT:     USFA_FTOSB(USFA_FLOAT);
        case USFA_DOUBLE:    USFA_FTOSB(USFA_DOUBLE);
        case USFA_LDOUBLE:   USFA_FTOSB(USFA_LDOUBLE);
        case USFA_INT:       USFA_FTOSB_INT(USFA_INT);
        case USFA_UINT:      USFA_FTOSB_INT(USFA_UINT);
        case USFA_LONG:      USFA_FTOSB_INT(USFA_LONG);
        case USFA_ULONG:     USFA_FTOSB_INT(USFA_ULONG);
        case USFA_LONGLONG:  USFA_FTOSB_INT(USFA_LONGLONG);
        case USFA_ULONGLONG: USFA_FTOSB_INT(USFA_ULONGLONG);
        case USFA_CHAR16:    USFA_FTOSB_INT(USFA_CHAR16);
        case USFA_CHAR32:    USFA_FTOSB_INT(USFA_CHAR32);
        default: return UCR_FAILED;
        }
    }

    template <typename Cy>
    bool usfa_ftos_fFeEgG(
        int flags, int precision, int fmt,
        std::basic_string<Cy>* str, bool* minus, const usformat_any& arg)
    {
        if (precision == -2) {
            precision = 6;
        }
        if (flags & FLAG_ALTER) {
            fmt |= UFF_DIG;
        }

        switch (arg.index()) {
        case USFA_FLOAT:     USFA_FTOSS(USFA_FLOAT);
        case USFA_DOUBLE:    USFA_FTOSS(USFA_DOUBLE);
        case USFA_LDOUBLE:   USFA_FTOSS(USFA_LDOUBLE);
        case USFA_INT:       USFA_FTOSS_INT(USFA_INT);
        case USFA_UINT:      USFA_FTOSS_INT(USFA_UINT);
        case USFA_LONG:      USFA_FTOSS_INT(USFA_LONG);
        case USFA_ULONG:     USFA_FTOSS_INT(USFA_ULONG);
        case USFA_LONGLONG:  USFA_FTOSS_INT(USFA_LONGLONG);
        case USFA_ULONGLONG: USFA_FTOSS_INT(USFA_ULONGLONG);
        case USFA_CHAR16:    USFA_FTOSS_INT(USFA_CHAR16);
        case USFA_CHAR32:    USFA_FTOSS_INT(USFA_CHAR32);
        default: return false;
        }
    }

    template <typename Cy>
    int usfa_ftos_aA(
        int flags, int precision, int fmt,
        Cy* buf, size_t* len, bool* minus, const usformat_any& arg)
    {
        fmt |= (UFF_HEX2 | UFF_ENZ);
        if (flags & FLAG_ALTER) {
            fmt |= UFF_DIG;
        }
        if (precision == -2) {
            precision = 0;
            fmt |= UFF_EXA;
        }

        switch (arg.index()) {
        case USFA_FLOAT:     USFA_FTOSB(USFA_FLOAT);
        case USFA_DOUBLE:    USFA_FTOSB(USFA_DOUBLE);
        case USFA_LDOUBLE:   USFA_FTOSB(USFA_LDOUBLE);
        case USFA_INT:       USFA_FTOSB_INT(USFA_INT);
        case USFA_UINT:      USFA_FTOSB_INT(USFA_UINT);
        case USFA_LONG:      USFA_FTOSB_INT(USFA_LONG);
        case USFA_ULONG:     USFA_FTOSB_INT(USFA_ULONG);
        case USFA_LONGLONG:  USFA_FTOSB_INT(USFA_LONGLONG);
        case USFA_ULONGLONG: USFA_FTOSB_INT(USFA_ULONGLONG);
        case USFA_CHAR16:    USFA_FTOSB_INT(USFA_CHAR16);
        case USFA_CHAR32:    USFA_FTOSB_INT(USFA_CHAR32);
        default: return UCR_FAILED;
        }
    }

    template <typename Cy>
    bool usfa_ftos_aA(
        int flags, int precision, int fmt,
        std::basic_string<Cy>* str, bool* minus, const usformat_any& arg)
    {
        fmt |= (UFF_HEX2 | UFF_ENZ);
        if (flags & FLAG_ALTER) {
            fmt |= UFF_DIG;
        }
        if (precision == -2) {
            precision = 0;
            fmt |= UFF_EXA;
        }

        switch (arg.index()) {
        case USFA_FLOAT:     USFA_FTOSS(USFA_FLOAT);
        case USFA_DOUBLE:    USFA_FTOSS(USFA_DOUBLE);
        case USFA_LDOUBLE:   USFA_FTOSS(USFA_LDOUBLE);
        case USFA_INT:       USFA_FTOSS_INT(USFA_INT);
        case USFA_UINT:      USFA_FTOSS_INT(USFA_UINT);
        case USFA_LONG:      USFA_FTOSS_INT(USFA_LONG);
        case USFA_ULONG:     USFA_FTOSS_INT(USFA_ULONG);
        case USFA_LONGLONG:  USFA_FTOSS_INT(USFA_LONGLONG);
        case USFA_ULONGLONG: USFA_FTOSS_INT(USFA_ULONGLONG);
        case USFA_CHAR16:    USFA_FTOSS_INT(USFA_CHAR16);
        case USFA_CHAR32:    USFA_FTOSS_INT(USFA_CHAR32);
        default: return false;
        }
    }


    template <typename Cy>
    bool usformat_base(
        const Cy* format, size_t len,
        std::basic_string<Cy>* out,
        const usformat_any* vars, size_t var_count)
    {
        int flags;
        int width;
        int precision;
        int modifier;
        std::basic_string<Cy> result;

        auto s = format;
        auto se = s + len;
        auto prev_pos = s;

        auto v = vars;
        auto ve = v ? v + var_count : nullptr;

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
                if (v && v < ve && get_usfa_i(*v, &width)) {
                    ++v;
                    if (width < 0) {
                        flags |= FLAG_LEFT_JUST;
                        // 防止未定义行为
                        if (width == INT_MIN) {
                            width = INT_MAX;
                        } else {
                            width = -width;
                        }
                    }
                } else {
                    width = 0;
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
                if (v && v < ve && get_usfa_i(*v, &precision)) {
                    ++v;
                    if (precision < 0) {
                        precision = -2;
                    }
                } else {
                    precision = 0;
                }
            }

            // length modifier
            parseLengthModifier(&s, se - s, &modifier);
            if (s >= se) {
                prev_pos = s;
                continue;
            }

            switch (*s) {
            case Cy('%'):
                result.push_back(Cy('%'));
                break;

            case Cy('c'):
                if (!v || v >= ve) { return false; }
                if (!extract_t_c(result, flags, modifier, width, *v)) {
                    return false;
                }
                ++v;
                break;

            case Cy('s'):
                if (!v || v >= ve) { return false; }
                if (!extract_t_s(result, flags, precision, width, *v)) {
                    return false;
                }
                ++v;
                break;

            case Cy('d'):
            case Cy('i'):
            {
                if (!v || v >= ve) { return false; }

                bool zero, minus;
                std::basic_string<Cy> _str;
                if (!usfa_itos(10, false, &_str, &minus, &zero, *v)) {
                    return false;
                }
                extract_diu(_str, false, minus, zero, flags, precision, width);
                result.append(_str);
                ++v;
                break;
            }

            case Cy('o'):
            {
                if (!v || v >= ve) { return false; }

                bool zero, minus;
                std::basic_string<Cy> _str;
                if (!usfa_itos(8, false, &_str, &minus, &zero, *v)) {
                    return false;
                }
                extract_o(_str, false, minus, zero, flags, precision, width);
                result.append(_str);
                ++v;
                break;
            }

            case Cy('x'):
            {
                if (!v || v >= ve) { return false; }

                bool zero, minus;
                std::basic_string<Cy> _str;
                if (!usfa_itos(16, false, &_str, &minus, &zero, *v)) {
                    return false;
                }
                extract_xX(_str, false, minus, zero, flags, precision, width, Cy('x'));
                result.append(_str);
                ++v;
                break;
            }

            case Cy('X'):
            {
                if (!v || v >= ve) { return false; }

                bool zero, minus;
                std::basic_string<Cy> _str;
                if (!usfa_itos(16, true, &_str, &minus, &zero, *v)) {
                    return false;
                }
                extract_xX(_str, false, minus, zero, flags, precision, width, Cy('X'));
                result.append(_str);
                ++v;
                break;
            }

            case Cy('u'):
            {
                if (!v || v >= ve) { return false; }

                bool zero, minus;
                std::basic_string<Cy> _str;
                if (!usfa_itos(10, false, &_str, &minus, &zero, *v)) {
                    return false;
                }
                extract_diu(_str, true, minus, zero, flags, precision, width);
                result.append(_str);
                ++v;
                break;
            }

            case Cy('f'):
            {
                if (!v || v >= ve) { return false; }

                bool minus;
                std::basic_string<Cy> _str;
                if (!usfa_ftos_fFeEgG(flags, precision, 0, &_str, &minus, *v)) {
                    return false;
                }
                extract_fFeEgG(&result, flags, width, minus, _str);
                ++v;
                break;
            }

            case Cy('F'):
            {
                if (!v || v >= ve) { return false; }

                bool minus;
                std::basic_string<Cy> _str;
                if (!usfa_ftos_fFeEgG(flags, precision, UFF_UPP, &_str, &minus, *v)) {
                    return false;
                }
                extract_fFeEgG(&result, flags, width, minus, _str);
                ++v;
                break;
            }

            case Cy('e'):
            {
                if (!v || v >= ve) { return false; }

                bool minus;
                std::basic_string<Cy> _str;
                if (!usfa_ftos_fFeEgG(flags, precision, UFF_SCI, &_str, &minus, *v)) {
                    return false;
                }
                extract_fFeEgG(&result, flags, width, minus, _str);
                ++v;
                break;
            }

            case Cy('E'):
            {
                if (!v || v >= ve) { return false; }

                bool minus;
                std::basic_string<Cy> _str;
                if (!usfa_ftos_fFeEgG(flags, precision, UFF_SCI | UFF_UPP, &_str, &minus, *v)) {
                    return false;
                }
                extract_fFeEgG(&result, flags, width, minus, _str);
                ++v;
                break;
            }

            case Cy('a'):
            {
                if (!v || v >= ve) { return false; }

                bool minus;
                std::basic_string<Cy> _str;
                if (!usfa_ftos_aA(flags, precision, 0, &_str, &minus, *v)) {
                    return false;
                }
                extract_aA(&result, flags, width, Cy('x'), minus, _str);
                ++v;
                break;
            }

            case Cy('A'):
            {
                if (!v || v >= ve) { return false; }

                bool minus;
                std::basic_string<Cy> _str;
                if (!usfa_ftos_aA(flags, precision, UFF_UPP, &_str, &minus, *v)) {
                    return false;
                }
                extract_aA(&result, flags, width, Cy('X'), minus, _str);
                ++v;
                break;
            }

            case Cy('g'):
            {
                if (!v || v >= ve) { return false; }

                int fmt = 0;
                if (!(flags & FLAG_ALTER)) { fmt |= UFF_NTZ; }

                bool minus;
                std::basic_string<Cy> _str;
                if (!usfa_ftos_fFeEgG(flags, precision, fmt, &_str, &minus, *v)) {
                    return false;
                }
                extract_fFeEgG(&result, flags, width, minus, _str);
                ++v;
                break;
            }
            case Cy('G'):
            {
                if (!v || v >= ve) { return false; }

                int fmt = UFF_UPP;
                if (!(flags & FLAG_ALTER)) { fmt |= UFF_NTZ; }

                bool minus;
                std::basic_string<Cy> _str;
                if (!usfa_ftos_fFeEgG(flags, precision, fmt, &_str, &minus, *v)) {
                    return false;
                }
                extract_fFeEgG(&result, flags, width, minus, _str);
                ++v;
                break;
            }
            case Cy('n'): return false;
            case Cy('p'):
            {
                if (!v || v >= ve) { return false; }

                std::basic_string<Cy> _str;
                if (v->index() != USFA_VOIDP) {
                    return false;
                }
                auto p = uintptr_t(std::get<USFA_VOIDP>(*v));
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
    int usformat_base(
        const Cy* format, size_t len,
        Cy* buf, size_t* buf_len,
        const usformat_any* vars, size_t var_count)
    {
        int flags;
        int width;
        int precision;
        int modifier;

        auto s = format;
        auto se = s + len;
        auto prev_pos = s;

        auto rs = buf;
        auto rse = rs ? rs + *buf_len : nullptr;
        size_t act_len = 0;

        auto v = vars;
        auto ve = v ? v + var_count : nullptr;

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
                if (v && v < ve && get_usfa_i(*v, &width)) {
                    ++v;
                    if (width < 0) {
                        flags |= FLAG_LEFT_JUST;
                        // 防止未定义行为
                        if (width == INT_MIN) {
                            width = INT_MAX;
                        } else {
                            width = -width;
                        }
                    }
                } else {
                    width = 0;
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
                if (v && v < ve && get_usfa_i(*v, &precision)) {
                    ++v;
                    if (precision < 0) {
                        precision = -2;
                    }
                } else {
                    precision = 0;
                }
            }

            // length modifier
            parseLengthModifier(&s, se - s, &modifier);
            if (s >= se) {
                prev_pos = s;
                continue;
            }

            switch (*s) {
            case Cy('%'):
                if (rs && rs < rse) {
                    *rs++ = Cy('%');
                }
                ++act_len;
                break;

            case Cy('c'):
            {
                if (!v || v >= ve) { return UCR_FAILED; }
                size_t _len = rs ? rse - rs : 0u;
                int ret = extract_t_c(rs, &_len, flags, modifier, width, *v);
                if (ret == UCR_FAILED) {
                    return ret;
                }
                if (ret == UCR_BUFFER) {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                ++v;
                break;
            }

            case Cy('s'):
            {
                if (!v || v >= ve) { return UCR_FAILED; }
                size_t _len = rs ? rse - rs : 0u;
                int ret = extract_t_s(rs, &_len, flags, precision, width, *v);
                if (ret == UCR_FAILED) {
                    return ret;
                }
                if (ret == UCR_BUFFER) {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                ++v;
                break;
            }

            case Cy('d'):
            case Cy('i'):
            {
                if (!v || v >= ve) { return UCR_FAILED; }

                bool zero, minus;
                size_t d_len = rs ? rse - rs : 0u;
                int ret = usfa_itos(10, false, rs, &d_len, &minus, &zero, *v);
                if (ret == UCR_FAILED) {
                    return ret;
                }
                if (ret == UCR_BUFFER) {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_diu(rs, &_len, d_len, false, minus, zero, flags, precision, width)) {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                ++v;
                break;
            }

            case Cy('o'):
            {
                if (!v || v >= ve) { return UCR_FAILED; }

                bool zero, minus;
                size_t d_len = rs ? rse - rs : 0u;
                int ret = usfa_itos(8, false, rs, &d_len, &minus, &zero, *v);
                if (ret == UCR_FAILED) {
                    return ret;
                }
                if (ret == UCR_BUFFER) {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_o(rs, &_len, d_len, false, minus, zero, flags, precision, width)) {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                ++v;
                break;
            }

            case Cy('x'):
            {
                if (!v || v >= ve) { return UCR_FAILED; }

                bool zero, minus;
                size_t d_len = rs ? rse - rs : 0u;
                int ret = usfa_itos(16, false, rs, &d_len, &minus, &zero, *v);
                if (ret == UCR_FAILED) {
                    return ret;
                }
                if (ret == UCR_BUFFER) {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_xX(rs, &_len, d_len, false, minus, zero, flags, precision, width, Cy('x'))) {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                ++v;
                break;
            }

            case Cy('X'):
            {
                if (!v || v >= ve) { return UCR_FAILED; }

                bool zero, minus;
                size_t d_len = rs ? rse - rs : 0u;
                int ret = usfa_itos(16, true, rs, &d_len, &minus, &zero, *v);
                if (ret == UCR_FAILED) {
                    return ret;
                }
                if (ret == UCR_BUFFER) {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_xX(rs, &_len, d_len, false, minus, zero, flags, precision, width, Cy('X'))) {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                ++v;
                break;
            }

            case Cy('u'):
            {
                if (!v || v >= ve) { return UCR_FAILED; }

                bool zero, minus;
                size_t d_len = rs ? rse - rs : 0u;
                int ret = usfa_itos(10, false, rs, &d_len, &minus, &zero, *v);
                if (ret == UCR_FAILED) {
                    return ret;
                }
                if (ret == UCR_BUFFER) {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_diu(rs, &_len, d_len, true, minus, zero, flags, precision, width)) {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                ++v;
                break;
            }

            case Cy('f'):
            {
                if (!v || v >= ve) { return UCR_FAILED; }

                bool minus;
                size_t d_len = rs ? rse - rs : 0u;
                int ret = usfa_ftos_fFeEgG(flags, precision, 0, rs, &d_len, &minus, *v);
                if (ret == UCR_FAILED) {
                    return ret;
                }
                if (ret == UCR_BUFFER) {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_fFeEgG(rs, &_len, flags, width, minus, d_len)) {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                ++v;
                break;
            }

            case Cy('F'):
            {
                if (!v || v >= ve) { return UCR_FAILED; }

                bool minus;
                size_t d_len = rs ? rse - rs : 0u;
                int ret = usfa_ftos_fFeEgG(flags, precision, UFF_UPP, rs, &d_len, &minus, *v);
                if (ret == UCR_FAILED) {
                    return ret;
                }
                if (ret == UCR_BUFFER) {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_fFeEgG(rs, &_len, flags, width, minus, d_len)) {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                ++v;
                break;
            }

            case Cy('e'):
            {
                if (!v || v >= ve) { return UCR_FAILED; }

                bool minus;
                size_t d_len = rs ? rse - rs : 0u;
                int ret = usfa_ftos_fFeEgG(flags, precision, UFF_SCI, rs, &d_len, &minus, *v);
                if (ret == UCR_FAILED) {
                    return ret;
                }
                if (ret == UCR_BUFFER) {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_fFeEgG(rs, &_len, flags, width, minus, d_len)) {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                ++v;
                break;
            }

            case Cy('E'):
            {
                if (!v || v >= ve) { return UCR_FAILED; }

                bool minus;
                size_t d_len = rs ? rse - rs : 0u;
                int ret = usfa_ftos_fFeEgG(flags, precision, UFF_SCI | UFF_UPP, rs, &d_len, &minus, *v);
                if (ret == UCR_FAILED) {
                    return ret;
                }
                if (ret == UCR_BUFFER) {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_fFeEgG(rs, &_len, flags, width, minus, d_len)) {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                ++v;
                break;
            }

            case Cy('a'):
            {
                if (!v || v >= ve) { return UCR_FAILED; }

                bool minus;
                size_t d_len = rs ? rse - rs : 0u;
                int ret = usfa_ftos_aA(flags, precision, 0, rs, &d_len, &minus, *v);
                if (ret == UCR_FAILED) {
                    return ret;
                }
                if (ret == UCR_BUFFER) {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_aA(rs, &_len, flags, width, Cy('x'), minus, d_len)) {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                ++v;
                break;
            }

            case Cy('A'):
            {
                if (!v || v >= ve) { return UCR_FAILED; }

                bool minus;
                size_t d_len = rs ? rse - rs : 0u;
                int ret = usfa_ftos_aA(flags, precision, UFF_UPP, rs, &d_len, &minus, *v);
                if (ret == UCR_FAILED) {
                    return ret;
                }
                if (ret == UCR_BUFFER) {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_aA(rs, &_len, flags, width, Cy('X'), minus, d_len)) {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                ++v;
                break;
            }

            case Cy('g'):
            {
                if (!v || v >= ve) { return UCR_FAILED; }

                int fmt = 0;
                if (!(flags & FLAG_ALTER)) { fmt |= UFF_NTZ; }

                bool minus;
                size_t d_len = rs ? rse - rs : 0u;
                int ret = usfa_ftos_fFeEgG(flags, precision, fmt, rs, &d_len, &minus, *v);
                if (ret == UCR_FAILED) {
                    return ret;
                }
                if (ret == UCR_BUFFER) {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_fFeEgG(rs, &_len, flags, width, minus, d_len)) {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                ++v;
                break;
            }
            case Cy('G'):
            {
                if (!v || v >= ve) { return UCR_FAILED; }

                int fmt = UFF_UPP;
                if (!(flags & FLAG_ALTER)) { fmt |= UFF_NTZ; }

                bool minus;
                size_t d_len = rs ? rse - rs : 0u;
                int ret = usfa_ftos_fFeEgG(flags, precision, fmt, rs, &d_len, &minus, *v);
                if (ret == UCR_FAILED) {
                    return ret;
                }
                if (ret == UCR_BUFFER) {
                    rs = nullptr;
                }

                size_t _len = rs ? rse - rs : 0u;
                if (!extract_fFeEgG(rs, &_len, flags, width, minus, d_len)) {
                    rs = nullptr;
                }
                act_len += _len;
                if (rs) rs += _len;
                ++v;
                break;
            }
            case Cy('n'): return UCR_FAILED;
            case Cy('p'):
            {
                if (!v || v >= ve) { return UCR_FAILED; }

                std::basic_string<Cy> _str;
                if (v->index() != USFA_VOIDP) {
                    return UCR_FAILED;
                }

                size_t _len = rs ? rse - rs : 0u;
                auto p = uintptr_t(std::get<USFA_VOIDP>(*v));
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

#endif  // UTILS_USFORMAT_INTERNAL_HPP_