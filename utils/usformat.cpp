// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "usformat.h"

#include <string>

#include "utils/usformat_internal.hpp"


namespace utl {
namespace internal {

    int usformat_0(
        const char* format, size_t len,
        char* buf, size_t* buf_len,
        const usformat_any* vars, size_t var_count)
    {
        return usformat_base(format, len, buf, buf_len, vars, var_count);
    }

    int usformat_0(
        const char16_t* format, size_t len,
        char16_t* buf, size_t* buf_len,
        const usformat_any* vars, size_t var_count)
    {
        return usformat_base(format, len, buf, buf_len, vars, var_count);
    }

    int usformat_0(
        const char32_t* format, size_t len,
        char32_t* buf, size_t* buf_len,
        const usformat_any* vars, size_t var_count)
    {
        return usformat_base(format, len, buf, buf_len, vars, var_count);
    }

    bool usformat_0(
        const char* format, size_t len,
        std::string* out,
        const usformat_any* vars, size_t var_count)
    {
        return usformat_base(format, len, out, vars, var_count);
    }

    bool usformat_0(
        const char16_t* format, size_t len,
        std::u16string* out,
        const usformat_any* vars, size_t var_count)
    {
        return usformat_base(format, len, out, vars, var_count);
    }

    bool usformat_0(
        const char32_t* format, size_t len,
        std::u32string* out,
        const usformat_any* vars, size_t var_count)
    {
        return usformat_base(format, len, out, vars, var_count);
    }

}
}