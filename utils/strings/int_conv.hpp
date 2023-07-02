// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STRINGS_INT_CONV_HPP_
#define UTILS_STRINGS_INT_CONV_HPP_

#include <cassert>
#include <string>


namespace utl {

namespace internal {

    inline const char kDigitCharUpper[]{
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
        'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
        'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
        'W', 'X', 'Y', 'Z',
    };

    inline const char kDigitCharLower[]{
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z',
    };

    template <typename Ty>
    int decd_slow(Ty v, int base) {
        int r = 0;
        while (v) {
            v /= base;
            ++r;
        }
        return r;
    }

}

    enum ICFormat {
        ICF_UPP = 1 << 0,
        ICF_PUP = 1 << 1,
        ICF_PLO = 1 << 2,

        ICF_PFX_OCT = 1 << 6,
        ICF_PFX_HEX = 1 << 7,
        ICF_PFX_ALL = ICF_PFX_OCT | ICF_PFX_HEX,
    };

    template <typename Cy>
    Cy itocu(uint_fast8_t val) {
        assert(val < 36);
        return Cy(internal::kDigitCharUpper[val]);
    }

    template <typename Cy>
    Cy itocl(uint_fast8_t val) {
        assert(val < 36);
        return Cy(internal::kDigitCharLower[val]);
    }

    template <typename Cy>
    uint_fast8_t ctoi(Cy c) {
        static_assert(
            std::is_integral<Cy>::value, "Cy must be intergral types!");

        switch (c) {
        case Cy('0'): return 0; case Cy('1'): return 1; case Cy('2'): return 2; case Cy('3'): return 3;
        case Cy('4'): return 4; case Cy('5'): return 5; case Cy('6'): return 6; case Cy('7'): return 7;
        case Cy('8'): return 8; case Cy('9'): return 9;

        case Cy('A'): case Cy('a'): return 10; case Cy('B'): case Cy('b'): return 11;
        case Cy('C'): case Cy('c'): return 12; case Cy('D'): case Cy('d'): return 13;
        case Cy('E'): case Cy('e'): return 14; case Cy('F'): case Cy('f'): return 15;
        case Cy('G'): case Cy('g'): return 16; case Cy('H'): case Cy('h'): return 17;
        case Cy('I'): case Cy('i'): return 18; case Cy('J'): case Cy('j'): return 19;
        case Cy('K'): case Cy('k'): return 20; case Cy('L'): case Cy('l'): return 21;
        case Cy('M'): case Cy('m'): return 22; case Cy('N'): case Cy('n'): return 23;
        case Cy('O'): case Cy('o'): return 24; case Cy('P'): case Cy('p'): return 25;
        case Cy('Q'): case Cy('q'): return 26; case Cy('R'): case Cy('r'): return 27;
        case Cy('S'): case Cy('s'): return 28; case Cy('T'): case Cy('t'): return 29;
        case Cy('U'): case Cy('u'): return 30; case Cy('V'): case Cy('v'): return 31;
        case Cy('W'): case Cy('w'): return 32; case Cy('X'): case Cy('x'): return 33;
        case Cy('Y'): case Cy('y'): return 34; case Cy('Z'): case Cy('z'): return 35;

        default: return 255u;
        }
    }

    template <typename Cy>
    bool isdigit(Cy c, int radix) {
        return ctoi(c) < radix;
    }


    template <typename Ty, typename Cy>
    bool itos(Ty val, Cy* buf, size_t* len, int radix = 10, int fmt = 0) {
        static_assert(
            std::is_integral<Ty>::value, "Ty must be a integral type!");

        typedef typename std::make_unsigned<Ty>::type UTy;
        const size_t bc = sizeof(Ty) * CHAR_BIT;
        assert(radix >= 2 && radix <= 36);

        Cy t_buf[bc];
        auto _t_buf = t_buf + bc;

        auto s = buf;
        auto se = s + *len;

        UTy value;
        size_t pre_len = 0;
        if (std::is_signed<Ty>::value) {
            if (val < 0) {
                // 防止溢出
                if (val == (std::numeric_limits<Ty>::min)()) {
                    value = UTy((std::numeric_limits<Ty>::max)()) + 1u;
                } else {
                    value = (val * -1);
                }
                if (s && s < se) *s++ = Cy('-');
                pre_len += 1;
            } else {
                value = val;
            }
        } else {
            value = val;
        }

        if (fmt & ICF_PFX_ALL) {
            // 根据基数加前缀
            if ((fmt & ICF_PFX_OCT) && radix == 8) {
                if (s && s < se) *s++ = Cy('0');
                ++pre_len;
            } else if ((fmt & ICF_PFX_HEX) && radix == 16) {
                if (s) {
                    int pup = (fmt & ICF_PUP) ?
                        1 : ((fmt & ICF_PLO) ?
                            0 : ((fmt & ICF_UPP) ?
                                1 : 0));

                    if (s < se) *s++ = Cy('0');
                    if (s < se) *s++ = Cy(pup ? 'X' : 'x');
                }
                pre_len += 2;
            }
        }

        const char* lut = (fmt & ICF_UPP) ?
            internal::kDigitCharUpper : internal::kDigitCharLower;

        do {
            --_t_buf;
            *_t_buf = Cy(lut[value % radix]);
            value /= radix;
        } while (value);

        auto act_size = size_t(t_buf + bc - _t_buf);
        if (act_size + pre_len > *len) {
            *len = act_size + pre_len;
            return false;
        }
        if (s) {
            std::memcpy(s, _t_buf, act_size * sizeof(Cy));
        }
        *len = act_size + pre_len;
        return true;
    }

    template <typename Ty, typename Cy>
    void itos(Ty val, std::basic_string<Cy>* out, int radix = 10, int fmt = 0) {
        static_assert(
            std::is_integral<Ty>::value, "Ty must be a integral type!");

        typedef typename std::make_unsigned<Ty>::type UTy;
        const size_t bc = sizeof(Ty) * CHAR_BIT;
        assert(radix >= 2 && radix <= 36);

        Cy t_buf[bc];
        auto _t_buf = t_buf + bc;

        std::basic_string<Cy> result;
        result.reserve(bc + 3);

        UTy value;
        if constexpr (std::is_signed<Ty>::value) {
            if (val < 0) {
                // 防止溢出
                if (val == (std::numeric_limits<Ty>::min)()) {
                    value = UTy((std::numeric_limits<Ty>::max)()) + 1;
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

        if (fmt & ICF_PFX_ALL) {
            // 根据基数加前缀
            if ((fmt & ICF_PFX_OCT) && radix == 8) {
                result.push_back(Cy('0'));
            } else if ((fmt & ICF_PFX_HEX) && radix == 16) {
                int pup = (fmt & ICF_PUP) ?
                    1 : ((fmt & ICF_PLO) ?
                        0 : ((fmt & ICF_UPP) ?
                            1 : 0));

                result.push_back(Cy('0'));
                result.push_back(Cy(pup ? 'X' : 'x'));
            }
        }

        const char* lut = (fmt & ICF_UPP) ?
            internal::kDigitCharUpper : internal::kDigitCharLower;

        do {
            --_t_buf;
            *_t_buf = Cy(lut[value % radix]);
            value /= radix;
        } while (value);

        auto size = t_buf + bc - _t_buf;
        result.append(_t_buf, size);
        *out = std::move(result);
    }

    template <typename Cy, typename Ty>
    std::basic_string<Cy> itos(Ty val, int radix = 10, int fmt = 0) {
        std::basic_string<Cy> result;
        itos(val, &result, radix, fmt);
        return result;
    }

    template <typename Ty>
    std::string itos8(Ty val, int radix = 10, int fmt = 0) {
        return itos<char>(val, radix, fmt);
    }

    template <typename Ty>
    std::u16string itos16(Ty val, int radix = 10, int fmt = 0) {
        return itos<char16_t>(val, radix, fmt);
    }

    template <typename Ty>
    std::u32string itos32(Ty val, int radix = 10, int fmt = 0) {
        return itos<char32_t>(val, radix, fmt);
    }

    template <typename Ty>
    std::wstring itosw(Ty val, int radix = 10, int fmt = 0) {
        return itos<wchar_t>(val, radix, fmt);
    }


    template <typename Ty, typename Cy>
    bool stoi_base(
        const Cy* buf, size_t len,
        Ty* out, int radix = 10, int fmt = 0, const Cy** p = nullptr)
    {
        static_assert(
            std::is_integral<Ty>::value, "Ty must be a integral type!");
        typedef typename std::make_unsigned<Ty>::type UTy;
        assert((fmt & ICF_PFX_ALL) || (radix >= 2 && radix <= 36));

        if (!len || !buf) {
            if (p) *p = buf;
            return false;
        }

        bool sign = true;
        auto s = buf;
        auto se = buf + len;
        if (*s == Cy('-')) {
            if (std::is_unsigned<Ty>::value) {
                if (p) *p = buf;
                return false;
            }
            sign = false;
            ++s;
        }

        if (fmt & ICF_PFX_ALL) {
            if (s < se && *s == Cy('0')) {
                ++s;
                if (s < se) {
                    if ((fmt & ICF_PFX_HEX) && (*s == Cy('x') || *s == Cy('X'))) {
                        radix = 16;
                        ++s;
                    } else if ((fmt & ICF_PFX_OCT) && isdigit(*s, 8)) {
                        radix = 8;
                    } else {
                        radix = 10;
                    }
                } else {
                    radix = 10;
                }
            } else {
                radix = 10;
            }
        }

        UTy M;
        if (std::is_unsigned<Ty>::value) {
            M = (std::numeric_limits<Ty>::max)();
        } else {
            M = UTy((std::numeric_limits<Ty>::max)()) + (sign ? 0 : 1);
        }

        auto M_sub = M / radix;
        auto M_rem = M % radix;

        UTy result = 0;
        for (; s < se; ++s) {
            auto c = ctoi(*s);
            if (c >= radix) {
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
        }

        if (s == buf || (!sign && buf + 1 == s)) {
            if (p) *p = buf;
            return false;
        }

        if (!p && s != buf + len) {
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
                    *out = (std::numeric_limits<Ty>::min)();
                } else {
                    *out = Ty(result) * (-1);
                }
            }
        }

        if (p) *p = s;
        return true;
    }

    template <typename Ty, typename Cy>
    bool stoi(
        const Cy* buf, size_t len,
        Ty* out, int radix = 10, const Cy** p = nullptr)
    {
        return stoi_base(buf, len, out, radix, 0, p);
    }

    template <typename Ty>
    bool stoi(
        const std::string_view& str,
        Ty* out, int radix = 10) {
        return stoi_base(str.data(), str.size(), out, radix, 0);
    }

    template <typename Ty>
    bool stoi(
        const std::u16string_view& str,
        Ty* out, int radix = 10) {
        return stoi_base(str.data(), str.size(), out, radix, 0);
    }

    template <typename Ty>
    bool stoi(
        const std::u32string_view& str,
        Ty* out, int radix = 10) {
        return stoi_base(str.data(), str.size(), out, radix, 0);
    }

    template <typename Ty>
    bool stoi(
        const std::wstring_view& str,
        Ty* out, int radix = 10) {
        return stoi_base(str.data(), str.size(), out, radix, 0);
    }

    template <typename Ty, typename Cy>
    bool stoi_ar(
        const Cy* buf, size_t len,
        Ty* out, const Cy** p = nullptr)
    {
        return stoi_base(buf, len, out, 0, ICF_PFX_ALL, p);
    }

    template <typename Ty>
    bool stoi_ar(
        const std::string_view& str, Ty* out) {
        return stoi_base(str.data(), str.size(), out, 0, ICF_PFX_ALL);
    }

    template <typename Ty>
    bool stoi_ar(
        const std::u16string_view& str, Ty* out) {
        return stoi_base(str.data(), str.size(), out, 0, ICF_PFX_ALL);
    }

    template <typename Ty>
    bool stoi_ar(
        const std::u32string_view& str, Ty* out) {
        return stoi_base(str.data(), str.size(), out, 0, ICF_PFX_ALL);
    }

    template <typename Ty>
    bool stoi_ar(
        const std::wstring_view& str, Ty* out) {
        return stoi_base(str.data(), str.size(), out, 0, ICF_PFX_ALL);
    }

}

#endif  // UTILS_STRINGS_INT_CONV_HPP_
