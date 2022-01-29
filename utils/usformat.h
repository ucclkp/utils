// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_USFORMAT_HPP_
#define UTILS_USFORMAT_HPP_

#include <string>

#include "utils/usformat_any_internal.hpp"


namespace utl {

namespace internal {

    int usformat_0(
        const char* format, size_t len,
        char* buf, size_t* buf_len,
        const usformat_any* vars, size_t var_count);

    int usformat_0(
        const char16_t* format, size_t len,
        char16_t* buf, size_t* buf_len,
        const usformat_any* vars, size_t var_count);

    int usformat_0(
        const char32_t* format, size_t len,
        char32_t* buf, size_t* buf_len,
        const usformat_any* vars, size_t var_count);

    bool usformat_0(
        const char* format, size_t len,
        std::string* out,
        const usformat_any* vars, size_t var_count);

    bool usformat_0(
        const char16_t* format, size_t len,
        std::u16string* out,
        const usformat_any* vars, size_t var_count);

    bool usformat_0(
        const char32_t* format, size_t len,
        std::u32string* out,
        const usformat_any* vars, size_t var_count);

}

    template <typename... Args>
    int usformatb(char* buf, size_t* len, const std::string_view& fmt, Args&&... args) {
        int ret;
        std::string out;
        constexpr auto count = sizeof...(args);
        if constexpr (count == 0) {
            ret = internal::usformat_0(fmt.data(), fmt.size(), buf, len, nullptr, count);
        } else {
            internal::usformat_any vars[count]{ args... };
            ret = internal::usformat_0(fmt.data(), fmt.size(), buf, len, vars, count);
        }
        return ret;
    }

    template <typename... Args>
    int usformatb(char16_t* buf, size_t* len, const std::u16string_view& fmt, Args&&... args) {
        int ret;
        std::u16string out;
        constexpr auto count = sizeof...(args);
        if constexpr (count == 0) {
            ret = internal::usformat_0(fmt.data(), fmt.size(), buf, len, nullptr, count);
        } else {
            internal::usformat_any vars[count]{ args... };
            ret = internal::usformat_0(fmt.data(), fmt.size(), buf, len, vars, count);
        }
        return ret;
    }

    template <typename... Args>
    int usformatb(char32_t* buf, size_t* len, const std::u32string_view& fmt, Args&&... args) {
        int ret;
        std::u32string out;
        constexpr auto count = sizeof...(args);
        if constexpr (count == 0) {
            ret = internal::usformat_0(fmt.data(), fmt.size(), buf, len, nullptr, count);
        } else {
            internal::usformat_any vars[count]{ args... };
            ret = internal::usformat_0(fmt.data(), fmt.size(), buf, len, vars, count);
        }
        return ret;
    }

    template <typename... Args>
    std::string usformat(const std::string_view& fmt, Args&&... args) {
        bool ret;
        std::string out;
        constexpr auto count = sizeof...(args);
        if constexpr (count == 0) {
            ret = internal::usformat_0(fmt.data(), fmt.size(), &out, nullptr, count);
        } else {
            internal::usformat_any vars[count]{ args... };
            ret = internal::usformat_0(fmt.data(), fmt.size(), &out, vars, count);
        }
        if (!ret) {
            out.clear();
        }
        return out;
    }

    template <typename... Args>
    std::u16string usformat(const std::u16string_view& fmt, Args&&... args) {
        bool ret;
        std::u16string out;
        constexpr auto count = sizeof...(args);
        if constexpr (count == 0) {
            ret = internal::usformat_0(fmt.data(), fmt.size(), &out, nullptr, count);
        } else {
            internal::usformat_any vars[count]{ args... };
            ret = internal::usformat_0(fmt.data(), fmt.size(), &out, vars, count);
        }
        if (!ret) {
            out.clear();
        }
        return out;
    }

    template <typename... Args>
    std::u32string usformat(const std::u32string_view& fmt, Args&&... args) {
        bool ret;
        std::u32string out;
        constexpr auto count = sizeof...(args);
        if constexpr (count == 0) {
            ret = internal::usformat_0(fmt.data(), fmt.size(), &out, nullptr, count);
        } else {
            internal::usformat_any vars[count]{ args... };
            ret = internal::usformat_0(fmt.data(), fmt.size(), &out, vars, count);
        }
        if (!ret) {
            out.clear();
        }
        return out;
    }

}

#endif  // UTILS_USFORMAT_HPP_