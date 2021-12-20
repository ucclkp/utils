// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_CONVERT_H_
#define UTILS_CONVERT_H_

#include <charconv>
#include <cstdint>
#include <sstream>
#include <string>
#include <string_view>

#include "utils/platform_utils.h"


namespace utl {

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
    std::basic_string<Cy> tolatu(const std::basic_string<Cy>& str) {
        std::basic_string<Cy> result(str.size(), 0);
        auto it = result.begin();
        for (auto c : str) {
            *it = tolatu(c);
            ++it;
        }
        return result;
    }

    template <typename Cy>
    std::basic_string<Cy> tolatu(const std::basic_string_view<Cy>& str) {
        return tolatu(std::basic_string<Cy>(str));
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
    std::basic_string<Cy> tolatl(const std::basic_string<Cy>& str) {
        std::basic_string<Cy> result(str.size(), 0);
        auto it = result.begin();
        for (auto c : str) {
            *it = tolatl(c);
            ++it;
        }
        return result;
    }

    template <typename Cy>
    std::basic_string<Cy> tolatl(const std::basic_string_view<Cy>& str) {
        return tolatl(std::basic_string<Cy>(str));
    }


    std::string UTF16ToUTF8(const std::u16string_view& str);
    std::u16string UTF8ToUTF16(const std::string_view& str);

    std::string WideToUTF8(const std::wstring_view& str);
    std::u16string WideToUTF16(const std::wstring_view& str);


    template <typename T>
    bool stringToNumber(const std::string_view& text, T* out) {
#ifdef OS_MAC
        // libc++ 似乎没有实现 float/double 版本的 from_char
        T result;
        std::string str(text);
        std::istringstream ss(str);
        if (!(ss >> result)) {
            return false;
        }
        *out = result;
        return true;
#else
        auto result = std::from_chars(text.data(), text.data() + text.length(), *out);
        if (result.ec != std::errc()) {
            return false;
        }
        return true;
#endif
    }

    template <typename T>
    bool stringToNumber(const std::u16string_view& text, T* out) {
        return stringToNumber(UTF16ToUTF8(text), out);
    }

    template <typename T>
    bool hexStringToNumber(const std::string_view& text, T* out) {
#ifdef OS_MAC
        // libc++ 似乎没有实现 float/double 版本的 from_char
        T result;
        std::string str(text);
        std::istringstream ss(str);
        if (!(ss >> std::hex >> result)) {
            return false;
        }
        *out = result;
        return true;
#else
        auto result = std::from_chars(text.data(), text.data() + text.length(), *out, 16);
        if (result.ec != std::errc()) {
            return false;
        }
        return true;
#endif
    }

    template <typename T>
    bool hexStringToNumber(const std::u16string_view& text, T* out) {
        return hexStringToNumber(UTF16ToUTF8(text), out);
    }

}

#endif  // UTILS_CONVERT_H_
