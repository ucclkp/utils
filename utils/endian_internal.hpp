// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

/**
 * 字节序转换。
 * 这里做了负数的编码方式为补码的假设。
 * 'iNb' 指明 Ty 类型必须是有符号整型，并具有 N 个字节大小。这里“字节”定义为 char。
 * 'uiNb' 指明 Ty 类型必须是无符号整型，并具有 N 个字节大小。这里“字节”定义为 char。
 * 'l2b' 和 'b2l' 指明是从小端转大端或从大端转小端。
 * 'mlNu' 指明了从 Ty v 变量的内存地址开头的 N 个字节包含了待转换的无符号数。
 * 'mlNs' 指明了从 Ty v 变量的内存地址开头的 N 个字节包含了待转换的有符号数。
 */

#ifndef UTILS_ENDIAN_INTERNAL_HPP_
#define UTILS_ENDIAN_INTERNAL_HPP_

#include <climits>
#include <cstddef>

#define ff_at(bpos) (Ty(static_cast<unsigned char>(~0u)) << CHAR_BIT * (bpos))


namespace utl {
namespace internal {

    template <typename Ty>
    Ty swap_ui2b(Ty v) {
        return (v << CHAR_BIT) | (v >> CHAR_BIT);
    }

    template <typename Ty>
    Ty swap_ui4b(Ty v) {
        return (v << (CHAR_BIT * 3)) |
            ((v << CHAR_BIT) & ff_at(2)) |
            (v >> (CHAR_BIT * 3)) |
            ((v >> CHAR_BIT) & ff_at(1));
    }

    template <typename Ty>
    Ty swap_ui8b(Ty v) {
        return (v << (CHAR_BIT * 7)) |
            ((v << (CHAR_BIT * 5)) & ff_at(6)) |
            ((v << (CHAR_BIT * 3)) & ff_at(5)) |
            ((v << CHAR_BIT) & ff_at(4)) |
            (v >> (CHAR_BIT * 7)) |
            ((v >> (CHAR_BIT * 5)) & ff_at(1)) |
            ((v >> (CHAR_BIT * 3)) & ff_at(2)) |
            ((v >> CHAR_BIT) & ff_at(3));
    }

    template <typename Ty>
    Ty swap_ui4b_b2l_ml2u(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 1u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_ui4b_b2l_ml3u(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 2u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_ui8b_b2l_ml4u(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 3u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_ui8b_b2l_ml5u(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 4u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_ui8b_b2l_ml6u(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 5u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_ui8b_b2l_ml7u(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 6u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_i4b_b2l_ml2s(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 1u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        *dst++ = *src--;
        *dst   = *src;
        if ((*dst >> (CHAR_BIT - 1u)) & 1u) {
            *++dst = static_cast<unsigned char>(~0u);
            *++dst = static_cast<unsigned char>(~0u);
        }
        return out;
    }

    template <typename Ty>
    Ty swap_i4b_b2l_ml3s(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 2u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        if ((*dst >> (CHAR_BIT - 1u)) & 1u) {
            *++dst = static_cast<unsigned char>(~0u);
        }
        return out;
    }

    template <typename Ty>
    Ty swap_i8b_b2l_ml4s(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 3u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        if ((*dst >> (CHAR_BIT - 1u)) & 1u) {
            *++dst = static_cast<unsigned char>(~0u);
            *++dst = static_cast<unsigned char>(~0u);
            *++dst = static_cast<unsigned char>(~0u);
            *++dst = static_cast<unsigned char>(~0u);
        }
        return out;
    }

    template <typename Ty>
    Ty swap_i8b_b2l_ml5s(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 4u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        if ((*dst >> (CHAR_BIT - 1u)) & 1u) {
            *++dst = static_cast<unsigned char>(~0u);
            *++dst = static_cast<unsigned char>(~0u);
            *++dst = static_cast<unsigned char>(~0u);
        }
        return out;
    }

    template <typename Ty>
    Ty swap_i8b_b2l_ml6s(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 5u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        if ((*dst >> (CHAR_BIT - 1u)) & 1u) {
            *++dst = static_cast<unsigned char>(~0u);
            *++dst = static_cast<unsigned char>(~0u);
        }
        return out;
    }

    template <typename Ty>
    Ty swap_i8b_b2l_ml7s(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 6u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        if ((*dst >> (CHAR_BIT - 1u)) & 1u) {
            *++dst = static_cast<unsigned char>(~0u);
        }
        return out;
    }

    template <typename Ty>
    Ty swap_ui4b_l2b_ml2u(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 1u;
        auto dst = reinterpret_cast<unsigned char*>(&out) + 2u;
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_ui4b_l2b_ml3u(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 2u;
        auto dst = reinterpret_cast<unsigned char*>(&out) + 1u;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_ui8b_l2b_ml4u(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 3u;
        auto dst = reinterpret_cast<unsigned char*>(&out) + 4u;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_ui8b_l2b_ml5u(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 4u;
        auto dst = reinterpret_cast<unsigned char*>(&out) + 3u;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_ui8b_l2b_ml6u(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 5u;
        auto dst = reinterpret_cast<unsigned char*>(&out) + 2u;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_ui8b_l2b_ml7u(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 6u;
        auto dst = reinterpret_cast<unsigned char*>(&out) + 1u;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_i4b_l2b_ml2s(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 1u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        if ((*src >> (CHAR_BIT - 1u)) & 1u) {
            *dst++ = static_cast<unsigned char>(~0u);
            *dst++ = static_cast<unsigned char>(~0u);
        } else {
            dst += 2u;
        }
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_i4b_l2b_ml3s(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 2u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        if ((*src >> (CHAR_BIT - 1u)) & 1u) {
            *dst++ = static_cast<unsigned char>(~0u);
        } else {
            dst += 1u;
        }
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_i8b_l2b_ml4s(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 3u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        if ((*src >> (CHAR_BIT - 1u)) & 1u) {
            *dst++ = static_cast<unsigned char>(~0u);
            *dst++ = static_cast<unsigned char>(~0u);
            *dst++ = static_cast<unsigned char>(~0u);
            *dst++ = static_cast<unsigned char>(~0u);
        } else {
            dst += 4u;
        }
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_i8b_l2b_ml5s(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 4u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        if ((*src >> (CHAR_BIT - 1u)) & 1u) {
            *dst++ = static_cast<unsigned char>(~0u);
            *dst++ = static_cast<unsigned char>(~0u);
            *dst++ = static_cast<unsigned char>(~0u);
        } else {
            dst += 3u;
        }
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_i8b_l2b_ml6s(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 5u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        if ((*src >> (CHAR_BIT - 1u)) & 1u) {
            *dst++ = static_cast<unsigned char>(~0u);
            *dst++ = static_cast<unsigned char>(~0u);
        } else {
            dst += 2u;
        }
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_i8b_l2b_ml7s(Ty v) {
        Ty out = 0;
        auto src = reinterpret_cast<unsigned char*>(&v) + 6u;
        auto dst = reinterpret_cast<unsigned char*>(&out);
        if ((*src >> (CHAR_BIT - 1u)) & 1u) {
            *dst++ = static_cast<unsigned char>(~0u);
        } else {
            dst += 1u;
        }
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst++ = *src--;
        *dst   = *src;
        return out;
    }

    template <typename Ty>
    Ty swap_f(Ty v) {
        Ty out;
        auto src = reinterpret_cast<unsigned char*>(&v) + (sizeof(Ty) - 1);
        auto dst = reinterpret_cast<unsigned char*>(&out);
        for (size_t i = 0; i < sizeof(Ty); ++i) {
            *dst++ = *src--;
        }
        return out;
    }

}
}

#endif  // UTILS_ENDIAN_INTERNAL_HPP_
