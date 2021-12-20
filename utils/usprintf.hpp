// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_USPRINTF_HPP_
#define UTILS_USPRINTF_HPP_

#include "utils/usprintf_internal.hpp"


namespace utl {

    inline int usprintf(char* buf, size_t* len, const char* format, ...) {
        va_list vars;
        va_start(vars, format);
        size_t f_len = std::char_traits<char>::length(format);
        int ret = internal::usprintf_base(format, f_len, buf, len, vars);
        va_end(vars);
        return ret;
    }

    inline std::string usprintf(const char* format, ...) {
        va_list vars;
        va_start(vars, format);
        size_t len = std::char_traits<char>::length(format);

        std::string out;
        internal::usprintf_base(format, len, &out, vars);
        va_end(vars);
        return out;
    }

    inline int usprintf(char16_t* buf, size_t* len, const char16_t* format, ...) {
        va_list vars;
        va_start(vars, format);
        size_t f_len = std::char_traits<char16_t>::length(format);
        int ret = internal::usprintf_base(format, f_len, buf, len, vars);
        va_end(vars);
        return ret;
    }

    inline std::u16string usprintf(const char16_t* format, ...) {
        va_list vars;
        va_start(vars, format);
        size_t len = std::char_traits<char16_t>::length(format);

        std::u16string out;
        auto ret = internal::usprintf_base(format, len, &out, vars);
        va_end(vars);
        return out;
    }

    inline int usprintf(char32_t* buf, size_t* len, const char32_t* format, ...) {
        va_list vars;
        va_start(vars, format);
        size_t f_len = std::char_traits<char32_t>::length(format);
        int ret = internal::usprintf_base(format, f_len, buf, len, vars);
        va_end(vars);
        return ret;
    }

    inline std::u32string usprintf(const char32_t* format, ...) {
        va_list vars;
        va_start(vars, format);
        size_t len = std::char_traits<char32_t>::length(format);

        std::u32string out;
        auto ret = internal::usprintf_base(format, len, &out, vars);
        va_end(vars);
        return out;
    }

}

#endif  // UTILS_USPRINTF_HPP_