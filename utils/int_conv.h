// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_INT_CONV_H_
#define UTILS_INT_CONV_H_

#include <cassert>
#include <string>


namespace utl {

namespace internal {

    inline const char kDigitChar[]{
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
        'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
        'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
        'W', 'X', 'Y', 'Z',
    };

    inline char itoc8(uint_fast8_t val) {
        return kDigitChar[val];
    }

    inline char16_t itoc16(uint_fast8_t val) {
        return char16_t(kDigitChar[val]);
    }

    inline char32_t itoc32(uint_fast8_t val) {
        return char32_t(kDigitChar[val]);
    }

    inline uint_fast8_t ctoi(char c) {
        switch (c) {
        case '0': return 0; case '1': return 1; case '2': return 2; case '3': return 3;
        case '4': return 4; case '5': return 5; case '6': return 6; case '7': return 7;
        case '8': return 8; case '9': return 9;

        case 'A': return 10; case 'B': return 11; case 'C': return 12; case 'D': return 13;
        case 'E': return 14; case 'F': return 15; case 'G': return 16; case 'H': return 17;
        case 'I': return 18; case 'J': return 19; case 'K': return 20; case 'L': return 21;
        case 'M': return 22; case 'N': return 23; case 'O': return 24; case 'P': return 25;
        case 'Q': return 26; case 'R': return 27; case 'S': return 28; case 'T': return 29;
        case 'U': return 30; case 'V': return 31; case 'W': return 32; case 'X': return 33;
        case 'Y': return 34; case 'Z': return 35;

        case 'a': return 10; case 'b': return 11; case 'c': return 12; case 'd': return 13;
        case 'e': return 14; case 'f': return 15; case 'g': return 16; case 'h': return 17;
        case 'i': return 18; case 'j': return 19; case 'k': return 20; case 'l': return 21;
        case 'm': return 22; case 'n': return 23; case 'o': return 24; case 'p': return 25;
        case 'q': return 26; case 'r': return 27; case 's': return 28; case 't': return 29;
        case 'u': return 30; case 'v': return 31; case 'w': return 32; case 'x': return 33;
        case 'y': return 34; case 'z': return 35;

        default: return std::uint_fast8_t(-1);
        }
    }

    inline uint_fast8_t ctoi(char16_t c) {
        return ctoi(char(c));
    }

    inline uint_fast8_t ctoi(char32_t c) {
        return ctoi(char(c));
    }

}

    template <typename Ty, typename Cy>
    size_t itos(Ty val, int radix, Cy* buf, size_t len) {
        typedef std::make_unsigned<Ty> UTy;
        const auto bc = sizeof(Ty) * CHAR_BIT;
        assert(radix >= 2 && radix <= 36);

        Cy t_buf[bc];
        auto _t_buf = t_buf + bc;

        UTy value;
        auto _buf = buf;
        if constexpr (std::is_signed<Ty>::value) {
            if (val < 0) {
                // 防止溢出
                if (val == std::numeric_limits<Ty>::min()) {
                    value = UTy(std::numeric_limits<Ty>::max()) + 1u;
                } else {
                    value = -val;
                }
                *_buf = Cy('-');
                ++_buf;
            } else {
                value = val;
            }
        } else {
            value = val;
        }

        do {
            --_t_buf;
            *_t_buf = Cy(internal::kDigitChar[value % radix]);
            value /= radix;
        } while (value);

        auto act_size = t_buf + bc - _t_buf;
        std::memcpy(_buf, _t_buf, std::min(act_size, len));
        return act_size;
    }

    template <typename Ty, typename Cy>
    size_t itos(Ty val, Cy* buf, size_t len) {
        return itos(val, 10, buf, len);
    }

    template <typename Ty, typename Cy>
    void itos(Ty val, int radix, std::basic_string<Cy>* out) {
        typedef std::make_unsigned<Ty>::type UTy;
        const auto bc = sizeof(Ty) * CHAR_BIT;
        assert(radix >= 2 && radix <= 36);

        Cy t_buf[bc];
        auto _t_buf = t_buf + bc;

        std::basic_string<Cy> result;
        result.reserve(bc + 1);

        UTy value;
        if constexpr (std::is_signed<Ty>::value) {
            if (val < 0) {
                // 防止溢出
                if (val == std::numeric_limits<Ty>::min()) {
                    value = UTy(std::numeric_limits<Ty>::max()) + 1;
                } else {
                    value = -val;
                }
                result.push_back(Cy('-'));
            } else {
                value = val;
            }
        } else {
            value = val;
        }

        do {
            --_t_buf;
            *_t_buf = Cy(internal::kDigitChar[value % radix]);
            value /= radix;
        } while (value);

        auto size = t_buf + bc - _t_buf;
        result.append(_t_buf, size);
        *out = std::move(result);
    }

    template <typename Ty, typename Cy>
    void itos(Ty val, std::basic_string<Cy>* out) {
        itos(val, 10, out);
    }


    template <typename Ty, typename Cy>
    bool stoi(const Cy* buf, size_t len, int radix, Ty* out, const Cy** p = nullptr) {
        typedef std::make_unsigned<Ty>::type UTy;
        assert(radix >= 2 && radix <= 36);

        if (len == 0) {
            if (p) *p = buf;
            return false;
        }

        size_t i = 0;
        bool sign = true;
        auto _buf = buf;
        if (*_buf == Cy('-')) {
            if constexpr (std::is_unsigned<Ty>::value) {
                if (p) *p = buf;
                return false;
            }
            sign = false;
            ++_buf;
            ++i;
        }

        UTy M;
        if constexpr (std::is_unsigned<Ty>::value) {
            M = std::numeric_limits<Ty>::max();
        } else {
            M = UTy(std::numeric_limits<Ty>::max()) + (sign ? 0 : 1);
        }

        auto M_sub = M / radix;
        auto M_rem = M % radix;

        UTy result = 0;
        for (; i < len; ++i) {
            auto c = internal::ctoi(*_buf);
            if (c == uint8_t(-1) || c >= radix) {
                break;
            }

            /**
             * M =  M_sub * radix + M_rem
             * S = result * radix + c
             * S > M
             *
             * (result - M_sub) * radix > M_rem - c
             * 当 result > M_sub 时，一定溢出；
             * 当 result == M_sub 时，有：c > M_rem；
             * 当 result < M_sub 时，因 M_rem 最小为 0，c 最大不会超过 radix，因此不会溢出。
             */
            if (result > M_sub || (result == M_sub && c > M_rem)) {
                break;
            }

            result = result * radix + c;
            ++_buf;
        }

        if (_buf == buf || (!sign && buf + 1 == _buf)) {
            if (p) *p = buf;
            return false;
        }

        if (!p && _buf != buf + len) {
            return false;
        }

        if constexpr (std::is_unsigned<Ty>::value) {
            *out = result;
        } else {
            if (sign) {
                *out = Ty(result);
            } else {
                /**
                 * 在 C++ 20 之前，如果目标变量为有符号型，且可容纳源变量的值，
                 * 则转换后值不变，否则结果是 implementation-defined。为了规避
                 * 该行为，检查 result 的值是否为最大值，如果是，说明结果应是有符号最小值。
                 * https://en.cppreference.com/w/cpp/language/implicit_conversion
                 */
                if (result == M) {
                    *out = std::numeric_limits<Ty>::min();
                } else {
                    *out = Ty(result) * (-1);
                }
            }
        }

        if (p) *p = _buf;
        return true;
    }

    template <typename Ty, typename Cy>
    bool stoi(const Cy* buf, size_t len, Ty* out, const Cy** p = nullptr) {
        return stoi(buf, len, 10, out, p);
    }

    template <typename Ty, typename Cy>
    bool stoi(const std::basic_string<Cy>& str, int radix, Ty* out) {
        return stoi(str.data(), str.size(), radix, out);
    }

    template <typename Ty, typename Cy>
    bool stoi(const std::basic_string_view<Cy>& str, int radix, Ty* out) {
        return stoi(str.data(), str.size(), radix, out);
    }

    template <typename Ty, typename Cy>
    bool stoi(const std::basic_string<Cy>& str, Ty* out) {
        return stoi(str.data(), str.size(), 10, out);
    }

    template <typename Ty, typename Cy>
    bool stoi(const std::basic_string_view<Cy>& str, Ty* out) {
        return stoi(str.data(), str.size(), 10, out);
    }

}

#endif  // UTILS_INT_CONV_H_