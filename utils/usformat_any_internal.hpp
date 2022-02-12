// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_USFORMAT_ANY_INTERNAL_HPP_
#define UTILS_USFORMAT_ANY_INTERNAL_HPP_

#include <variant>
#include <filesystem>


namespace utl {
namespace internal {

    struct _std_fspath_wrapper {
        _std_fspath_wrapper()
            : ptr(nullptr) {}
        _std_fspath_wrapper(const std::filesystem::path& p)
            : ptr(&p) {}
        _std_fspath_wrapper& operator=(const std::filesystem::path& p) {
            ptr = &p;
            return *this;
        }
        const std::filesystem::path* ptr;
    };

    using usformat_any = std::variant<
        int,
        unsigned int,
        long,
        unsigned long,
        long long,
        unsigned long long,
        float,
        double,
        long double,
        char16_t,
        char32_t,
        const char*,
        const char16_t*,
        const char32_t*,
        const wchar_t*,
        void*,
        std::string_view,
        std::u16string_view,
        std::u32string_view,
        std::wstring_view,
        _std_fspath_wrapper>;

    enum USFAnyTypes {
        USFA_INT = 0,
        USFA_UINT,
        USFA_LONG,
        USFA_ULONG,
        USFA_LONGLONG,
        USFA_ULONGLONG,
        USFA_FLOAT,
        USFA_DOUBLE,
        USFA_LDOUBLE,
        USFA_CHAR16,
        USFA_CHAR32,
        USFA_CHARP,
        USFA_CHAR16P,
        USFA_CHAR32P,
        USFA_WCHARP,
        USFA_VOIDP,
        USFA_SV,
        USFA_SV16,
        USFA_SV32,
        USFA_WSV,
        USFA_FSPATH,
    };

}
}

#endif  // UTILS_USFORMAT_ANY_INTERNAL_HPP_