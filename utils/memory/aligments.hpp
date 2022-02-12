// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MEMORY_ALIGNMENTS_HPP_
#define UTILS_MEMORY_ALIGNMENTS_HPP_

#include <type_traits>


namespace utl {

    template<typename Ty, typename Ny>
    Ty alignNoff(Ty val, Ny N) {
        static_assert(std::is_integral<Ny>::value, "Ny must be a integral type!");
        static_assert(std::is_integral<Ty>::value, "Ty must be a integral type!");
        return (N - (val % N)) % N;
    }

    template<typename Ty>
    Ty align2off(Ty val) { return alignNoff(val, 2); }
    template<typename Ty>
    Ty align4off(Ty val) { return alignNoff(val, 4); }
    template<typename Ty>
    Ty align8off(Ty val) { return alignNoff(val, 8); }
    template<typename Ty>
    Ty align16off(Ty val) { return alignNoff(val, 16); }
    template<typename Ty>
    Ty align32off(Ty val) { return alignNoff(val, 32); }
    template<typename Ty>
    Ty align64off(Ty val) { return alignNoff(val, 64); }

    template<typename Ty>
    Ty align2(Ty val) { return val + align2off(val); }
    template<typename Ty>
    Ty align4(Ty val) { return val + align4off(val); }
    template<typename Ty>
    Ty align8(Ty val) { return val + align8off(val); }
    template<typename Ty>
    Ty align16(Ty val) { return val + align16off(val); }
    template<typename Ty>
    Ty align32(Ty val) { return val + align32off(val); }
    template<typename Ty>
    Ty align64(Ty val) { return val + align64off(val); }

}

#endif  // UTILS_MEMORY_ALIGNMENTS_HPP_