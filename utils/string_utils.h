// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STRING_UTILS_H_
#define UTILS_STRING_UTILS_H_

#include <string>
#include <vector>


namespace utl {

    std::string stringPrintf(const char* format, ...);
    std::u16string stringPrintf(const char16_t* format, ...);


    namespace ascii {

        std::string trim(const std::string& str, bool all = false);
        std::u16string trim(const std::u16string& str, bool all = false);

        std::string trim(const std::string& str, char token, bool all);
        std::u16string trim(const std::u16string& str, char16_t token, bool all);

        // 按照 token 中的每个字符分割字符串 str
        std::vector<std::string> split(
            const std::string_view& str, const std::string_view& token, bool filter_empty = false);
        // 按照 token 中的每个字符分割字符串 str
        std::vector<std::u16string> split(
            const std::u16string_view& str, const std::u16string_view& token, bool filter_empty = false);

        bool startWith(
            const std::string_view& base, const std::string_view& match,
            std::string_view::size_type off = 0, bool case_sensitive = true);
        bool startWith(
            const std::u16string_view& base, const std::u16string_view& match,
            std::u16string_view::size_type off = 0, bool case_sensitive = true);

        bool endWith(
            const std::string_view& base, const std::string_view& match,
            bool case_sensitive = true);
        bool endWith(
            const std::u16string_view& base, const std::u16string_view& match,
            bool case_sensitive = true);

        /**
         * 在 base 中查找 sub，如找到，将 sub 替换为 new_s,
         * 直到 base 中无法找出 sub 为止。
         */
        std::string replaceAll(
            const std::string& base,
            const std::string_view& sub,
            const std::string_view& new_s);

        /**
         * 在 base 中查找 sub，如找到，将 sub 替换为 new_s,
         * 直到 base 中无法找出 sub 为止。
         */
        std::u16string replaceAll(
            const std::u16string& base,
            const std::u16string_view& sub,
            const std::u16string_view& new_s);

        /**
         * 在 base 中查找 token 中的每个字符，如找到，将字符替换为 new_s,
         * 直到 base 中找不到 token 中的任何字符为止。
         */
        std::string replaceAllTokens(
            const std::string& base,
            const std::string_view& token,
            const std::string_view& new_s);

        /**
         * 在 base 中查找 token 中的每个字符，如找到，将字符替换为 new_s,
         * 直到 base 中找不到 token 中的任何字符为止。
         */
        std::u16string replaceAllTokens(
            const std::u16string& base,
            const std::u16string_view& token,
            const std::u16string_view& new_s);

    }
}


#endif  // UTILS_STRING_UTILS_H_