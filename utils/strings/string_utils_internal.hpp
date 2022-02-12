// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STRINGS_STRING_UTILS_INTERNAL_HPP_
#define UTILS_STRINGS_STRING_UTILS_INTERNAL_HPP_

#include <string>
#include <vector>

#include "utils/strings/string_utils_types.hpp"


namespace utl {
namespace internal {

    template <typename Cy>
    Cy tolatu(Cy ch) {
        switch (ch) {
        case Cy('a'): return Cy('A');
        case Cy('b'): return Cy('B');
        case Cy('c'): return Cy('C');
        case Cy('d'): return Cy('D');
        case Cy('e'): return Cy('E');
        case Cy('f'): return Cy('F');
        case Cy('g'): return Cy('G');
        case Cy('h'): return Cy('H');
        case Cy('i'): return Cy('I');
        case Cy('j'): return Cy('J');
        case Cy('k'): return Cy('K');
        case Cy('l'): return Cy('L');
        case Cy('m'): return Cy('M');
        case Cy('n'): return Cy('N');
        case Cy('o'): return Cy('O');
        case Cy('p'): return Cy('P');
        case Cy('q'): return Cy('Q');
        case Cy('r'): return Cy('R');
        case Cy('s'): return Cy('S');
        case Cy('t'): return Cy('T');
        case Cy('u'): return Cy('U');
        case Cy('v'): return Cy('V');
        case Cy('w'): return Cy('W');
        case Cy('x'): return Cy('X');
        case Cy('y'): return Cy('Y');
        case Cy('z'): return Cy('Z');
        default: return ch;
        }
    }

    template <typename Cy>
    Cy tolatl(Cy ch) {
        switch (ch) {
        case Cy('A'): return Cy('a');
        case Cy('B'): return Cy('b');
        case Cy('C'): return Cy('c');
        case Cy('D'): return Cy('d');
        case Cy('E'): return Cy('e');
        case Cy('F'): return Cy('f');
        case Cy('G'): return Cy('g');
        case Cy('H'): return Cy('h');
        case Cy('I'): return Cy('i');
        case Cy('J'): return Cy('j');
        case Cy('K'): return Cy('k');
        case Cy('L'): return Cy('l');
        case Cy('M'): return Cy('m');
        case Cy('N'): return Cy('n');
        case Cy('O'): return Cy('o');
        case Cy('P'): return Cy('p');
        case Cy('Q'): return Cy('q');
        case Cy('R'): return Cy('r');
        case Cy('S'): return Cy('s');
        case Cy('T'): return Cy('t');
        case Cy('U'): return Cy('u');
        case Cy('V'): return Cy('v');
        case Cy('W'): return Cy('w');
        case Cy('X'): return Cy('x');
        case Cy('Y'): return Cy('y');
        case Cy('Z'): return Cy('z');
        default: return ch;
        }
    }

    template <typename Cy>
    void tolatu(std::basic_string<Cy>* text) {
        auto& s = *text;
        for (size_t i = 0; i < text->size(); ++i) {
            s[i] = tolatu(s[i]);
        }
    }

    template <typename Cy>
    void tolatl(std::basic_string<Cy>* text) {
        auto& s = *text;
        for (size_t i = 0; i < text->size(); ++i) {
            s[i] = tolatl(s[i]);
        }
    }

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
        size_t off = 0, bool latin_case_sens = true)
    {
        if (off >= base.size() ||
            off + match.size() > base.size())
        {
            return false;
        }

        for (size_t i = 0; i < match.length(); ++i, ++off) {
            if (latin_case_sens) {
                if (base[off] != match[i]) return false;
            } else {
                if (tolatl(base[off]) != tolatl(match[i])) return false;
            }
        }
        return true;
    }

    template <typename Cy>
    bool endWith(
        const std::basic_string_view<Cy>& base,
        const std::basic_string_view<Cy>& match,
        bool latin_case_sens = true)
    {
        if (match.size() > base.size()) {
            return false;
        }

        auto off = base.length();
        for (size_t i = match.length(); i > 0;) {
            --i;
            --off;
            if (latin_case_sens) {
                if (base[off] != match[i]) return false;
            } else {
                if (tolatl(base[off]) != tolatl(match[i])) return false;
            }
        }
        return true;
    }

    template <typename Cy>
    bool isLitEqual(
        const std::basic_string_view<Cy>& s1,
        const std::basic_string_view<Cy>& s2,
        bool latin_case_sens = false)
    {
        if (s1.size() != s2.size()) {
            return false;
        }
        for (size_t i = 0; i < s1.length(); ++i) {
            if (latin_case_sens) {
                if (s1[i] != s2[i]) return false;
            } else {
                if (tolatl(s1[i]) != tolatl(s2[i])) return false;
            }
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
}


#endif  // UTILS_STRINGS_STRING_UTILS_INTERNAL_HPP_