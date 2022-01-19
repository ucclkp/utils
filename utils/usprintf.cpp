// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "usprintf.h"

#include "utils/usprintf_internal.hpp"


namespace utl {

    int usprintfb(char* buf, size_t* len, const char* format, ...) {
        internal::vlw args;
        va_start(args.args, format);
        size_t f_len = std::char_traits<char>::length(format);
        int ret = internal::usprintf_base(format, f_len, buf, len, &args);
        va_end(args.args);
        return ret;
    }

    int usprintfb(char16_t* buf, size_t* len, const char16_t* format, ...) {
        internal::vlw args;
        va_start(args.args, format);
        size_t f_len = std::char_traits<char16_t>::length(format);
        int ret = internal::usprintf_base(format, f_len, buf, len, &args);
        va_end(args.args);
        return ret;
    }

    int usprintfb(char32_t* buf, size_t* len, const char32_t* format, ...) {
        internal::vlw args;
        va_start(args.args, format);
        size_t f_len = std::char_traits<char32_t>::length(format);
        int ret = internal::usprintf_base(format, f_len, buf, len, &args);
        va_end(args.args);
        return ret;
    }

    std::string usprintf(const char* format, ...) {
        internal::vlw args;
        va_start(args.args, format);
        size_t len = std::char_traits<char>::length(format);

        std::string out;
        internal::usprintf_base(format, len, &out, &args);
        va_end(args.args);
        return out;
    }

    std::u16string usprintf(const char16_t* format, ...) {
        internal::vlw args;
        va_start(args.args, format);
        size_t len = std::char_traits<char16_t>::length(format);

        std::u16string out;
        auto ret = internal::usprintf_base(format, len, &out, &args);
        va_end(args.args);
        return out;
    }

    std::u32string usprintf(const char32_t* format, ...) {
        internal::vlw args;
        va_start(args.args, format);
        size_t len = std::char_traits<char32_t>::length(format);

        std::u32string out;
        auto ret = internal::usprintf_base(format, len, &out, &args);
        va_end(args.args);
        return out;
    }

}