// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_TYPE_UTILS_HPP_
#define UTILS_TYPE_UTILS_HPP_

#include <type_traits>


namespace utl {

    // 可用于防止编译器直接触发 static_assert
    template <typename Ty>
    class sat_stub : public std::false_type {};

    template <typename Ty>
    struct upromote {
        using type = typename std::conditional<
            sizeof(Ty) <= sizeof(unsigned int),
            unsigned int,
            typename std::conditional<
            sizeof(Ty) <= sizeof(unsigned long),
            unsigned long,
            typename std::conditional<
            sizeof(Ty) <= sizeof(unsigned long long),
            unsigned long long,
            Ty>::type>::type>::type;
    };

    template <typename Ty>
    class raw_pointer {
        using _r_cvref_type = typename std::remove_reference<
            typename std::remove_cv<Ty>::type>::type;
        using _r_ptr_type = typename std::remove_pointer<_r_cvref_type>::type;
        using _raw_type = typename std::remove_reference<
            typename std::remove_cv<_r_ptr_type>::type>::type;
    public:
        using type = typename std::add_pointer<_raw_type>::type;
    };

    template <typename Ty1, typename Ty2>
    struct is_same_pointer {
        static constexpr auto value = std::is_same<
            typename raw_pointer<Ty1>::type, typename raw_pointer<Ty2>::type>::value;
    };


    template <typename Ty, typename Arg, typename... Args>
    class is_all_convertible;

    template <typename Ty, typename Arg>
    class is_all_convertible<Ty, Arg> {
    public:
        using type = Ty;
        static constexpr auto value =
            std::is_convertible<Ty, Arg>::value;
    };

    template <typename Ty, typename Arg, typename... Args>
    class is_all_convertible {
    public:
        using type = Ty;
        static constexpr auto value =
            std::is_convertible<Ty, Arg>::value &&
            is_all_convertible<Ty, Args...>::value;
    };

}

#endif  // UTILS_TYPE_UTILS_HPP_
