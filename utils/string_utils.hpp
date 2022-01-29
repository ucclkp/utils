// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STRING_UTILS_HPP_
#define UTILS_STRING_UTILS_HPP_

#include <string>
#include <vector>

#include "utils/convert.h"

#define u8c(c) reinterpret_cast<char>(u8##c)
#define u8p(s) reinterpret_cast<const char*>(u8##s)
#define u8r(r) reinterpret_cast<const char&>(u8##r)


namespace utl {

    enum TrimFlags {
        TRF_START = 1,
        TRF_MIDDLE = 2,
        TRF_END = 4,
        TRF_ALL = TRF_START | TRF_MIDDLE | TRF_END,
    };

namespace internal {

    // 去除 text 中所有在 tokens 中出现的字符
    template <typename Cy>
    void trim(
        std::basic_string<Cy>* text,
        const std::basic_string_view<Cy>& tokens,
        int flags = TRF_START | TRF_END)
    {
        size_t i;
        if (flags & TRF_START) {
            i = text->find_first_not_of(tokens);
            if (i == std::basic_string<Cy>::npos) {
                text->clear();
                return;
            }
            if (i > 0) {
                text->erase(0, i);
            }
        }

        if (flags & TRF_END) {
            i = text->find_last_not_of(tokens);
            if (i == std::basic_string<Cy>::npos) {
                text->clear();
                return;
            }
            if (i + 1 < text->length()) {
                text->erase(i + 1);
            }
        }

        if (flags & TRF_MIDDLE) {
            for (i = 1;;) {
                i = text->find_first_of(tokens, i);
                if (i == std::basic_string<Cy>::npos) {
                    break;
                }

                auto j = text->find_first_not_of(tokens, i);
                if (j == std::basic_string<Cy>::npos) {
                    text->erase(i);
                    break;
                }

                text->erase(i, j - i);
                ++i;
            }
        }
    }

    template <typename Cy>
    std::vector<std::basic_string<Cy>> split(
        const std::basic_string_view<Cy>& text,
        const std::basic_string_view<Cy>& tokens,
        bool filter_empty = false)
    {
        typename std::basic_string<Cy>::size_type prev_index = 0;
        std::vector<std::basic_string<Cy>> str_vec;
        for (;;) {
            auto cur_index = text.find_first_of(tokens, prev_index);
            if (cur_index != std::basic_string<Cy>::npos) {
                auto indiv = text.substr(prev_index, cur_index - prev_index);
                if (!filter_empty || !indiv.empty()) {
                    str_vec.push_back(std::basic_string<Cy>(indiv));
                }
                prev_index = cur_index + 1;
            } else {
                auto indiv = text.substr(prev_index);
                if (!filter_empty || !indiv.empty()) {
                    str_vec.push_back(std::basic_string<Cy>(indiv));
                }
                break;
            }
        }

        return str_vec;
    }

    template <typename Cy>
    bool startWith(
        const std::basic_string_view<Cy>& base,
        const std::basic_string_view<Cy>& match,
        size_t off = 0, bool case_sensitive = true)
    {
        for (decltype(off) i = 0; i < match.length(); ++i) {
            if (off >= base.length()) return false;
            if (case_sensitive) {
                if (base[off] != match[i]) return false;
            } else {
                if (tolatl(base[off]) != tolatl(match[i])) return false;
            }
            ++off;
        }
        return true;
    }

    template <typename Cy>
    bool endWith(
        const std::basic_string_view<Cy>& base,
        const std::basic_string_view<Cy>& match,
        bool case_sensitive = true)
    {
        auto off = base.length();
        for (decltype(off) i = match.length(); i > 0; --i) {
            if (off == 0) return false;
            if (case_sensitive) {
                if (base[off - 1] != match[i - 1]) return false;
            } else {
                if (tolatl(base[off - 1]) != tolatl(match[i - 1])) return false;
            }
            --off;
        }
        return true;
    }

    /**
     * 在 base 中查找 sub，如找到，将 sub 替换为 new_s,
     * 直到 base 中无法找出 sub 为止。
     */
    template <typename Cy>
    void replaceAll(
        std::basic_string<Cy>* base,
        const std::basic_string_view<Cy>& sub,
        const std::basic_string_view<Cy>& new_s)
    {
        if (base->empty() || sub.empty()) return;

        size_t prev_pos = 0;
        for (;;) {
            auto i = base->find(sub, prev_pos);
            if (i != std::basic_string<Cy>::npos) {
                base->replace(i, sub.length(), new_s);
                prev_pos = i + new_s.length();
            } else {
                break;
            }
        }
    }

    /**
     * 在 base 中查找 token 中的每个字符，如找到，将字符替换为 new_s,
     * 直到 base 中找不到 token 中的任何字符为止。
     */
    template <typename Cy>
    void replaceAllTokens(
        std::basic_string<Cy>* base,
        const std::basic_string_view<Cy>& token,
        const std::basic_string_view<Cy>& new_s)
    {
        if (base->empty() || token.empty()) return;

        size_t i = 0;
        for (;;) {
            i = base->find_first_of(token, i);
            if (i != std::basic_string<Cy>::npos) {
                base->replace(i, 1, new_s);
                i += new_s.length();
                if (i >= base->length()) {
                    break;
                }
            } else {
                break;
            }
        }
    }

}

    inline void trim(
        std::string* text,
        const std::string_view& tokens,
        int flags = TRF_START | TRF_END)
    {
        internal::trim(text, tokens, flags);
    }

    inline void trim(
        std::u16string* text,
        const std::u16string_view& tokens,
        int flags = TRF_START | TRF_END)
    {
        internal::trim(text, tokens, flags);
    }

    inline void trim(
        std::u32string* text,
        const std::u32string_view& tokens,
        int flags = TRF_START | TRF_END)
    {
        internal::trim(text, tokens, flags);
    }

    inline void trim(
        std::string* text, int flags = TRF_START | TRF_END) {
        trim(text, " ", flags);
    }

    inline void trim(
        std::u16string* text, int flags = TRF_START | TRF_END) {
        trim(text, u" ", flags);
    }

    inline void trim(
        std::u32string* text, int flags = TRF_START | TRF_END) {
        trim(text, U" ", flags);
    }

    inline std::vector<std::string> split(
        const std::string_view& text,
        const std::string_view& tokens,
        bool filter_empty = false)
    {
        return internal::split(text, tokens, filter_empty);
    }

    inline std::vector<std::u16string> split(
        const std::u16string_view& text,
        const std::u16string_view& tokens,
        bool filter_empty = false)
    {
        return internal::split(text, tokens, filter_empty);
    }

    inline std::vector<std::u32string> split(
        const std::u32string_view& text,
        const std::u32string_view& tokens,
        bool filter_empty = false)
    {
        return internal::split(text, tokens, filter_empty);
    }

    inline bool startWith(
        const std::string_view& base,
        const std::string_view& match,
        size_t off = 0, bool case_sensitive = true)
    {
        return internal::startWith(base, match, off, case_sensitive);
    }

    inline bool startWith(
        const std::u16string_view& base,
        const std::u16string_view& match,
        size_t off = 0, bool case_sensitive = true)
    {
        return internal::startWith(base, match, off, case_sensitive);
    }

    inline bool startWith(
        const std::u32string_view& base,
        const std::u32string_view& match,
        size_t off = 0, bool case_sensitive = true)
    {
        return internal::startWith(base, match, off, case_sensitive);
    }

    inline bool endWith(
        const std::string_view& base,
        const std::string_view& match,
        bool case_sensitive = true)
    {
        return internal::endWith(base, match, case_sensitive);
    }

    inline bool endWith(
        const std::u16string_view& base,
        const std::u16string_view& match,
        bool case_sensitive = true)
    {
        return internal::endWith(base, match, case_sensitive);
    }

    inline bool endWith(
        const std::u32string_view& base,
        const std::u32string_view& match,
        bool case_sensitive = true)
    {
        return internal::endWith(base, match, case_sensitive);
    }

    inline void replaceAll(
        std::string* base,
        const std::string_view& sub,
        const std::string_view& new_s)
    {
        return internal::replaceAll(base, sub, new_s);
    }

    inline void replaceAll(
        std::u16string* base,
        const std::u16string_view& sub,
        const std::u16string_view& new_s)
    {
        return internal::replaceAll(base, sub, new_s);
    }

    inline void replaceAll(
        std::u32string* base,
        const std::u32string_view& sub,
        const std::u32string_view& new_s)
    {
        return internal::replaceAll(base, sub, new_s);
    }

    inline void replaceAllTokens(
        std::string* base,
        const std::string_view& token,
        const std::string_view& new_s)
    {
        return internal::replaceAllTokens(base, token, new_s);
    }

    inline void replaceAllTokens(
        std::u16string* base,
        const std::u16string_view& token,
        const std::u16string_view& new_s)
    {
        return internal::replaceAllTokens(base, token, new_s);
    }

    inline void replaceAllTokens(
        std::u32string* base,
        const std::u32string_view& token,
        const std::u32string_view& new_s)
    {
        return internal::replaceAllTokens(base, token, new_s);
    }

}


#endif  // UTILS_STRING_UTILS_HPP_