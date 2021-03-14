// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/string_utils.h"

#include <algorithm>
#include <cstdarg>

#include "utils/convert.h"


namespace utl {

    std::string stringPrintf(const char* format, ...) {
        va_list vars;
        va_start(vars, format);
        va_list vars2;
        va_copy(vars2, vars);

        int ret = vsnprintf(nullptr, 0, format, vars);
        va_end(vars);
        if (ret <= 0) {
            return {};
        }

        ++ret;
        std::unique_ptr<char[]> buf(new char[ret]);
        ret = vsnprintf(buf.get(), ret, format, vars2);
        va_end(vars2);
        if (ret <= 0) {
            return {};
        }

        return std::string(buf.get(), ret);
    }

    std::u16string stringPrintf(const char16_t* format, ...) {
        va_list vars;
        va_start(vars, format);
        va_list vars2;
        va_copy(vars2, vars);

        auto u8_f(UTF16ToUTF8(format));
        int ret = vsnprintf(nullptr, 0, u8_f.c_str(), vars);
        va_end(vars);
        if (ret <= 0) {
            return {};
        }

        ++ret;
        std::unique_ptr<char[]> buf(new char[ret]);
        ret = vsnprintf(buf.get(), ret, u8_f.c_str(), vars2);
        va_end(vars2);
        if (ret <= 0) {
            return {};
        }

        return UTF8ToUTF16(std::string(buf.get(), ret));
    }

    namespace ascii {

        std::string trim(const std::string& str, bool all) {
            return trim(str, ' ', all);
        }

        std::u16string trim(const std::u16string& str, bool all) {
            return trim(str, u' ', all);
        }

        std::string trim(const std::string& str, char token, bool all) {
            auto result = str;
            if (all) {
                for (auto it = result.begin(); it != result.end();) {
                    if (*it == token) {
                        it = result.erase(it);
                    } else {
                        ++it;
                    }
                }
            } else {
                auto i = result.find_first_not_of(token);
                if (i == std::string::npos) {
                    return {};
                }
                if (i > 0) {
                    result.erase(0, i);
                }

                i = result.find_last_not_of(token);
                if (i == std::string::npos) {
                    return {};
                }
                if (i + 1 < result.length()) {
                    result.erase(i + 1);
                }
            }

            return result;
        }

        std::u16string trim(const std::u16string& str, char16_t token, bool all) {
            auto result = str;
            if (all) {
                for (auto it = result.begin(); it != result.end();) {
                    if (*it == token) {
                        it = result.erase(it);
                    } else {
                        ++it;
                    }
                }
            } else {
                auto i = result.find_first_not_of(token);
                if (i == std::u16string::npos) {
                    return {};
                }
                if (i > 0) {
                    result.erase(0, i);
                }

                i = result.find_last_not_of(token);
                if (i == std::u16string::npos) {
                    return {};
                }
                if (i + 1 < result.length()) {
                    result.erase(i + 1);
                }
            }

            return result;
        }

        std::vector<std::string> split(
            const std::string_view& str, const std::string_view& token, bool filter_empty)
        {
            std::string::size_type prev_index = 0;
            std::vector<std::string> str_vec;
            for (;;) {
                auto cur_index = str.find_first_of(token, prev_index);
                if (cur_index != std::string::npos) {
                    auto indiv = str.substr(prev_index, cur_index - prev_index);
                    if (!filter_empty || !indiv.empty()) {
                        str_vec.push_back(std::string(indiv));
                    }
                    prev_index = cur_index + 1;
                } else {
                    auto indiv = str.substr(prev_index);
                    if (!filter_empty || !indiv.empty()) {
                        str_vec.push_back(std::string(indiv));
                    }
                    break;
                }
            }

            return str_vec;
        }

        std::vector<std::u16string> split(
            const std::u16string_view& str, const std::u16string_view& token, bool filter_empty)
        {
            std::u16string::size_type prev_index = 0;
            std::vector<std::u16string> str_vec;
            for (;;) {
                auto cur_index = str.find_first_of(token, prev_index);
                if (cur_index != std::u16string::npos) {
                    auto indiv = str.substr(prev_index, cur_index - prev_index);
                    if (!filter_empty || !indiv.empty()) {
                        str_vec.push_back(std::u16string(indiv));
                    }
                    prev_index = cur_index + 1;
                } else {
                    auto indiv = str.substr(prev_index);
                    if (!filter_empty || !indiv.empty()) {
                        str_vec.push_back(std::u16string(indiv));
                    }
                    break;
                }
            }

            return str_vec;
        }

        bool startWith(
            const std::string_view& base, const std::string_view& match,
            std::string_view::size_type off, bool case_sensitive)
        {
            for (decltype(off) i = 0; i < match.length(); ++i) {
                if (off >= base.length()) return false;
                if (case_sensitive) {
                    if (base[off] != match[i]) return false;
                } else {
                    if (toASCIILower(base[off]) != toASCIILower(match[i])) return false;
                }
                ++off;
            }
            return true;
        }

        bool startWith(
            const std::u16string_view& base, const std::u16string_view& match,
            std::u16string_view::size_type off, bool case_sensitive)
        {
            for (decltype(off) i = 0; i < match.length(); ++i) {
                if (off >= base.length()) return false;
                if (case_sensitive) {
                    if (base[off] != match[i]) return false;
                } else {
                    if (toASCIILower(base[off]) != toASCIILower(match[i])) return false;
                }
                ++off;
            }
            return true;
        }

        bool endWith(
            const std::string_view& base, const std::string_view& match,
            bool case_sensitive)
        {
            auto off = base.length();
            for (decltype(off) i = match.length(); i > 0; --i) {
                if (off == 0) return false;
                if (case_sensitive) {
                    if (base[off - 1] != match[i - 1]) return false;
                } else {
                    if (toASCIILower(base[off - 1]) != toASCIILower(match[i - 1])) return false;
                }
                --off;
            }
            return true;
        }

        bool endWith(
            const std::u16string_view& base, const std::u16string_view& match,
            bool case_sensitive)
        {
            auto off = base.length();
            for (decltype(off) i = match.length(); i > 0; --i) {
                if (off == 0) return false;
                if (case_sensitive) {
                    if (base[off - 1] != match[i - 1]) return false;
                } else {
                    if (toASCIILower(base[off - 1]) != toASCIILower(match[i - 1])) return false;
                }
                --off;
            }
            return true;
        }

        std::string replaceAll(
            const std::string& base, const std::string_view& sub, const std::string_view& new_s)
        {
            if (base.empty()) return {};
            if (sub.empty()) return base;

            size_t prev_pos = 0;
            std::string result(base);
            for (;;) {
                auto i = result.find(sub, prev_pos);
                if (i != std::string::npos) {
                    result.replace(i, sub.length(), new_s);
                    prev_pos = i + new_s.length();
                } else {
                    break;
                }
            }
            return result;
        }

        std::u16string replaceAll(
            const std::u16string& base, const std::u16string_view& sub, const std::u16string_view& new_s)
        {
            if (base.empty()) return {};
            if (sub.empty()) return base;

            size_t prev_pos = 0;
            std::u16string result(base);
            for (;;) {
                auto i = result.find(sub, prev_pos);
                if (i != std::u16string::npos) {
                    result.replace(i, sub.length(), new_s);
                    prev_pos = i + new_s.length();
                } else {
                    break;
                }
            }
            return result;
        }

        std::string replaceAllTokens(
            const std::string& base, const std::string_view& token, const std::string_view& new_s)
        {
            if (base.empty()) return {};
            if (token.empty()) return base;

            std::string result(base);
            std::string::size_type i = 0;
            for (;;) {
                i = result.find_first_of(token, i);
                if (i != std::string::npos) {
                    result.replace(i, 1, new_s);
                    i += new_s.length();
                    if (i >= result.length()) {
                        break;
                    }
                } else {
                    break;
                }
            }
            return result;
        }

        std::u16string replaceAllTokens(
            const std::u16string& base, const std::u16string_view& token, const std::u16string_view& new_s)
        {
            if (base.empty()) return {};
            if (token.empty()) return base;

            std::u16string result(base);
            std::u16string::size_type i = 0;
            for (;;) {
                i = result.find_first_of(token, i);
                if (i != std::u16string::npos) {
                    result.replace(i, 1, new_s);
                    i += new_s.length();
                    if (i >= result.length()) {
                        break;
                    }
                } else {
                    break;
                }
            }
            return result;
        }

    }

}
