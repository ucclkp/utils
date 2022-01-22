// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_INT_EXP_H_
#define UTILS_INT_EXP_H_

#include <cstdint>

#include "utils/platform_utils.h"


namespace utl {
namespace internal {

    struct UIntExp_base {
        static constexpr size_t ubits = 64u;
        static constexpr size_t ubytes = ubits / CHAR_BIT;
        static constexpr uint_fast64_t umax = 0xFFFF'FFFF'FFFF'FFFFu;
    };

    template <typename Ty, size_t N>
    struct UIntExp {
        static constexpr size_t ubits = UIntExp_base::ubits;
        static constexpr size_t ubytes = UIntExp_base::ubytes;

        static UIntExp bitAt(size_t pos) {
            UIntExp out(0);
            out.setBit(pos);
            return out;
        }

        UIntExp() {
            static_assert(sizeof(Ty) <= ubytes * N);
        }

        UIntExp(uint_fast64_t u) {
            for (size_t i = 0; i < N - 1; ++i) {
                d[i] = 0;
            }
            d[N - 1] = u;
        }

        UIntExp& operator=(uint_fast64_t val) {
            for (size_t i = 0; i < N - 1; ++i) {
                d[i] = 0;
            }
            d[N - 1] = val;
            return *this;
        }

        UIntExp operator+(uint_fast64_t val) const {
            UIntExp out(*this);
            out.operator+=(val);
            return out;
        }

        UIntExp operator+(const UIntExp& rhs) const {
            UIntExp out(*this);
            out.operator+=(rhs);
            return out;
        }

        UIntExp& operator+=(uint_fast64_t val) {
            auto r = val;
            for (size_t i = N; i-- > 0;) {
                if (UIntExp_base::umax - d[i] < r) {
                    d[i] += r;
                    r = 1;
                } else {
                    d[i] += r;
                    break;
                }
            }
            return *this;
        }

        UIntExp& operator+=(const UIntExp& rhs) {
            uint_fast64_t r = 0;
            for (size_t i = N; i-- > 0;) {
                if (r) {
                    if (UIntExp_base::umax - d[i] <= rhs.d[i]) {
                        d[i] += rhs.d[i] + r;
                        r = 1;
                    } else {
                        d[i] += rhs.d[i] + r;
                        r = 0;
                    }
                } else {
                    if (UIntExp_base::umax - d[i] < rhs.d[i]) {
                        d[i] += rhs.d[i] + r;
                        r = 1;
                    } else {
                        d[i] += rhs.d[i] + r;
                        r = 0;
                    }
                }
            }
            return *this;
        }

        UIntExp operator-(uint_fast64_t val) const {
            UIntExp out(*this);
            out.operator-=(val);
            return out;
        }

        UIntExp operator-(const UIntExp& rhs) const {
            UIntExp out(*this);
            out.operator-=(rhs);
            return out;
        }

        UIntExp& operator-=(uint_fast64_t val) {
            auto r = val;
            for (size_t i = N; i-- > 0;) {
                if (d[i] < r) {
                    d[i] = UIntExp_base::umax - r + d[i] + 1;
                    r = 1;
                } else {
                    d[i] -= r;
                    break;
                }
            }
            return *this;
        }

        UIntExp& operator-=(const UIntExp& rhs) {
            uint_fast64_t r = 0;
            for (size_t i = N; i-- > 0;) {
                if (r) {
                    if (d[i] <= rhs.d[i]) {
                        d[i] = UIntExp_base::umax - rhs.d[i] + d[i];
                        r = 1;
                    } else {
                        d[i] -= rhs.d[i] + r;
                        r = 0;
                    }
                } else {
                    if (d[i] < rhs.d[i]) {
                        d[i] = UIntExp_base::umax - rhs.d[i] + d[i] + 1;
                        r = 1;
                    } else {
                        d[i] -= rhs.d[i] + r;
                        r = 0;
                    }
                }
            }
            return *this;
        }

        UIntExp operator&(uint_fast64_t val) const {
            UIntExp out(*this);
            out.operator&=(val);
            return out;
        }

        UIntExp operator&(const UIntExp& rhs) const {
            UIntExp out(*this);
            out.operator&=(rhs);
            return out;
        }

        UIntExp& operator&=(uint_fast64_t val) {
            for (size_t i = 0; i < N - 1; ++i) {
                d[i] = 0;
            }
            d[N - 1] &= val;
            return *this;
        }

        UIntExp& operator&=(const UIntExp& rhs) {
            for (size_t i = N; i-- > 0;) {
                d[i] &= rhs.d[i];
            }
            return *this;
        }

        UIntExp operator|(uint_fast64_t val) const {
            UIntExp out(*this);
            out.operator|=(val);
            return out;
        }

        UIntExp operator|(const UIntExp& rhs) const {
            UIntExp out(*this);
            out.operator|=(rhs);
            return out;
        }

        UIntExp& operator|=(uint_fast64_t val) {
            d[N - 1] |= val;
            return *this;
        }

        UIntExp& operator|=(const UIntExp& rhs) {
            for (size_t i = N; i-- > 0;) {
                d[i] |= rhs.d[i];
            }
            return *this;
        }

        UIntExp operator<<(size_t val) const {
            UIntExp out(*this);
            out.operator<<=(val);
            return out;
        }

        UIntExp& operator<<=(size_t val) {
            auto u = val / 64u;
            auto b = val % 64u;
            if (u) {
                size_t i;
                for (i = 0; i + u < N; ++i) {
                    d[i] = d[i + u];
                }
                for (; i < N; ++i) {
                    d[i] = 0;
                }
            }
            if (b) {
                uint_fast64_t r = 0;
                for (size_t i = N; i-- > 0;) {
                    auto r1 = d[i] >> (64u - b);
                    d[i] <<= b;
                    d[i] |= r;
                    r = r1;
                }
            }
            return *this;
        }

        UIntExp operator>>(size_t val) const {
            UIntExp out(*this);
            out.operator>>=(val);
            return out;
        }

        UIntExp& operator>>=(size_t val) {
            auto u = val / ubits;
            auto b = val % ubits;
            if (u) {
                size_t i;
                for (i = N; i-- > u;) {
                    d[i] = d[i - u];
                }
                for (i = size_t(u); i-- > 0;) {
                    d[i] = 0;
                }
            }
            if (b) {
                uint_fast64_t r = 0;
                for (size_t i = 0; i < N; ++i) {
                    auto r1 = d[i] & ((uint_fast64_t(1u) << b) - 1u);
                    d[i] >>= b;
                    d[i] |= r << (ubits - b);
                    r = r1;
                }
            }
            return *this;
        }

        bool operator==(const UIntExp& rhs) const {
            for (size_t i = 0; i < N; ++i) {
                if (d[i] != rhs.d[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator==(uint_fast64_t val) const {
            if (d[N - 1] != val) return false;
            for (size_t i = 0; i < N - 1; ++i) {
                if (d[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const UIntExp& rhs) const {
            return !operator==(rhs);
        }

        bool operator!=(uint_fast64_t val) const {
            return !operator==(val);
        }

        explicit operator bool() const {
            return *this != 0;
        }

        bool isZeroAfter(size_t pos) const {
            auto u = pos / ubits;
            auto b = pos % ubits;
            if (u >= N) {
                return *this == 0;
            }

            if (d[N - u - 1] & ((uint_fast64_t(1u) << b) - 1u)) {
                return false;
            }

            for (; u-- > 0;) {
                if (d[N - u - 1]) {
                    return false;
                }
            }
            return true;
        }

        UIntExp& incAt(size_t pos) {
            auto u = pos / ubits;
            auto b = pos % ubits;
            if (u >= N) {
                return *this;
            }

            auto r = uint_fast64_t(1u) << b;
            for (size_t i = N - u; i-- > 0;) {
                if (UIntExp_base::umax - d[i] < r) {
                    d[i] += r;
                    r = 1;
                } else {
                    d[i] += r;
                    break;
                }
            }
            return *this;
        }

        UIntExp& zero() {
            for (size_t i = 0; i < N; ++i) {
                d[i] = 0;
            }
            return *this;
        }

        UIntExp& setBit(size_t pos) {
            auto u = pos / ubits;
            auto b = pos % ubits;
            if (u < N) {
                d[N - u - 1] |= (uint_fast64_t(1u) << b);
            }
            return *this;
        }

        UIntExp& clearBit(size_t pos) {
            auto u = pos / ubits;
            auto b = pos % ubits;
            if (u < N) {
                d[N - u - 1] &= ~(uint_fast64_t(1u) << b);
            }
            return *this;
        }

        uint_fast8_t getBit(size_t pos) const {
            auto u = pos / ubits;
            auto b = pos % ubits;
            if (u < N) {
                return (d[N - u - 1] >> b) & 1u;
            }
            return 0;
        }

        UIntExp& from(Ty src) {
            zero();
#ifdef ENDIAN_LE
            size_t si = 0;
            for (size_t i = N; i-- > 0 && si < sizeof(Ty);) {
                const size_t len = (sizeof(Ty) - si) >= ubytes ? ubytes : (sizeof(Ty) - si);
                std::memcpy(
                    &d[i],
                    reinterpret_cast<unsigned char*>(&src) + si,
                    len);
                si += len;
            }
#else
            size_t diff = ubytes * N - sizeof(Ty);
            size_t si = 0;
            size_t di = diff % ubytes;
            for (size_t i = diff / ubytes; i < N; ++i) {
                const size_t len = ubytes - di;
                std::memcpy(
                    reinterpret_cast<unsigned char*>(&d[i]) + di,
                    reinterpret_cast<unsigned char*>(&src) + si,
                    len);
                si += len;
                di = 0;
            }
#endif
            return *this;
        }

        const UIntExp& to(Ty* dst) const {
#ifdef ENDIAN_LE
            size_t si = 0;
            for (size_t i = N; i-- > 0 && si < sizeof(Ty);) {
                const size_t len = (sizeof(Ty) - si) >= ubytes ? ubytes : (sizeof(Ty) - si);
                std::memcpy(
                    reinterpret_cast<unsigned char*>(dst) + si,
                    &d[i],
                    len);
                si += len;
            }
#else
            size_t diff = ubytes * N - sizeof(Ty);
            size_t si = 0;
            size_t di = diff % ubytes;
            for (size_t i = diff / ubytes; i < N; ++i) {
                const size_t len = ubytes - di;
                std::memcpy(
                    reinterpret_cast<unsigned char*>(&src) + si,
                    reinterpret_cast<unsigned char*>(&d[i]) + di,
                    len);
                si += len;
                di = 0;
            }
#endif
            return *this;
        }

        size_t size() const { return N; }

        uint_fast8_t uf8() const  { return uint_fast8_t(d[N - 1]); }
        uint_fast16_t uf16() const { return uint_fast16_t(d[N - 1]); }
        uint_fast32_t uf32() const { return uint_fast32_t(d[N - 1]); }
        uint_fast64_t uf64() const { return uint_fast64_t(d[N - 1]); }
        uint_least8_t ul8() const { return uint_least8_t(d[N - 1]); }
        uint_least16_t ul16() const { return uint_least16_t(d[N - 1]); }
        uint_least32_t ul32() const { return uint_least32_t(d[N - 1]); }
        uint_least64_t ul64() const { return uint_least64_t(d[N - 1]); }

        // 0 存储高位部分
        uint_fast64_t d[N];
    };

    template <typename Ty>
    struct UIntExp<Ty, 1> {
        static constexpr size_t ubits = UIntExp_base::ubits;
        static constexpr size_t ubytes = UIntExp_base::ubytes;

        static UIntExp bitAt(size_t pos) {
            UIntExp out(0);
            out.setBit(pos);
            return out;
        }

        UIntExp() {
            static_assert(sizeof(Ty) <= ubytes);
        }

        UIntExp(uint_fast64_t u) {
            d = u;
        }

        UIntExp& operator=(uint_fast64_t val) {
            d = val;
            return *this;
        }

        UIntExp operator+(uint_fast64_t val) const {
            UIntExp out(*this);
            out.operator+=(val);
            return out;
        }

        UIntExp operator+(const UIntExp& rhs) const {
            UIntExp out(*this);
            out.operator+=(rhs);
            return out;
        }

        UIntExp& operator+=(uint_fast64_t val) {
            d += val;
            return *this;
        }

        UIntExp& operator+=(const UIntExp& rhs) {
            d += rhs.d;
            return *this;
        }

        UIntExp operator-(uint_fast64_t val) const {
            UIntExp out(*this);
            out.operator-=(val);
            return out;
        }

        UIntExp operator-(const UIntExp& rhs) const {
            UIntExp out(*this);
            out.operator-=(rhs);
            return out;
        }

        UIntExp& operator-=(uint_fast64_t val) {
            d -= val;
            return *this;
        }

        UIntExp& operator-=(const UIntExp& rhs) {
            d -= rhs.d;
            return *this;
        }

        UIntExp operator&(uint_fast64_t val) const {
            UIntExp out(*this);
            out.operator&=(val);
            return out;
        }

        UIntExp operator&(const UIntExp& rhs) const {
            UIntExp out(*this);
            out.operator&=(rhs);
            return out;
        }

        UIntExp& operator&=(uint_fast64_t val) {
            d &= val;
            return *this;
        }

        UIntExp& operator&=(const UIntExp& rhs) {
            d &= rhs.d;
            return *this;
        }

        UIntExp operator|(uint_fast64_t val) const {
            UIntExp out(*this);
            out.operator|=(val);
            return out;
        }

        UIntExp operator|(const UIntExp& rhs) const {
            UIntExp out(*this);
            out.operator|=(rhs);
            return out;
        }

        UIntExp& operator|=(uint_fast64_t val) {
            d |= val;
            return *this;
        }

        UIntExp& operator|=(const UIntExp& rhs) {
            d |= rhs.d;
            return *this;
        }

        UIntExp operator<<(size_t val) const {
            UIntExp out(*this);
            out.operator<<=(val);
            return out;
        }

        UIntExp& operator<<=(size_t val) {
            d <<= val;
            return *this;
        }

        UIntExp operator>>(size_t val) const {
            UIntExp out(*this);
            out.operator>>=(val);
            return out;
        }

        UIntExp& operator>>=(size_t val) {
            d >>= val;
            return *this;
        }

        bool operator==(const UIntExp& rhs) const {
            return d == rhs.d;
        }

        bool operator==(uint_fast64_t val) const {
            return d == val;
        }

        bool operator!=(const UIntExp& rhs) const {
            return d != rhs.d;
        }

        bool operator!=(uint_fast64_t val) const {
            return d != val;
        }

        explicit operator bool() const {
            return *this != 0;
        }

        bool isZeroAfter(size_t pos) const {
            auto u = pos / ubits;
            auto b = pos % ubits;
            if (u >= 1) {
                return *this == 0;
            }

            if (d & ((uint_fast64_t(1u) << b) - 1u)) {
                return false;
            }
            return true;
        }

        UIntExp& incAt(size_t pos) {
            auto u = pos / ubits;
            auto b = pos % ubits;
            if (u >= 1) {
                return *this;
            }

            d += uint_fast64_t(1u) << b;
            return *this;
        }

        UIntExp& zero() {
            d = 0;
            return *this;
        }

        UIntExp& setBit(size_t pos) {
            auto u = pos / ubits;
            auto b = pos % ubits;
            if (!u) {
                d |= (uint_fast64_t(1u) << b);
            }
            return *this;
        }

        UIntExp& clearBit(size_t pos) {
            auto u = pos / ubits;
            auto b = pos % ubits;
            if (!u) {
                d &= ~(uint_fast64_t(1u) << b);
            }
            return *this;
        }

        uint_fast8_t getBit(size_t pos) const {
            auto u = pos / ubits;
            auto b = pos % ubits;
            if (!u) {
                return (d >> b) & 1u;
            }
            return 0;
        }

        UIntExp& from(Ty src) {
            d = 0;
#ifdef ENDIAN_LE
            std::memcpy(&d, &src, sizeof(Ty));
#else
            size_t diff = ubytes - sizeof(Ty);
            std::memcpy(d + diff, &src, sizeof(Ty));
#endif
            return *this;
        }

        const UIntExp& to(Ty* dst) const {
#ifdef ENDIAN_LE
            std::memcpy(dst, &d, sizeof(Ty));
#else
            size_t diff = ubytes - sizeof(Ty);
            std::memcpy(dst, d + diff, sizeof(Ty));
#endif
            return *this;
        }

        size_t size() const { return 1; }

        uint_fast8_t uf8() const { return uint_fast8_t(d); }
        uint_fast16_t uf16() const { return uint_fast16_t(d); }
        uint_fast32_t uf32() const { return uint_fast32_t(d); }
        uint_fast64_t uf64() const { return uint_fast64_t(d); }
        uint_least8_t ul8() const { return uint_least8_t(d); }
        uint_least16_t ul16() const { return uint_least16_t(d); }
        uint_least32_t ul32() const { return uint_least32_t(d); }
        uint_least64_t ul64() const { return uint_least64_t(d); }

        uint_fast64_t d;
    };

    template <typename Ty>
    using uint_e = UIntExp<
        Ty, (sizeof(Ty) * CHAR_BIT + UIntExp_base::ubits - 1) / UIntExp_base::ubits>;

    template <typename Ty, size_t N>
    bool operator==(uint_fast64_t val, const UIntExp<Ty, N>& rhs) {
        return rhs == val;
    }

    template <typename Ty, size_t N>
    bool operator!=(uint_fast64_t val, const UIntExp<Ty, N>& rhs) {
        return rhs != val;
    }

}
}

#endif  // UTILS_INT_EXP_H_