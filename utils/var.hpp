// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_VAR_HPP_
#define UTILS_VAR_HPP_

#include <algorithm>
#include <cassert>


namespace utl {
namespace internal {

    /**
     * 用于生成重载函数列表。
     * 只能用于非求值上下文，因为 dummy() 没有实现。
     */
    template <typename Ty>
    struct _var_func {
        Ty dummy(Ty);
    };

    /**
     * 利用包展开生成重载函数列表。
     */
    template <typename... Tys>
    struct _var_func_overloads : _var_func<Tys>... {
        using _var_func<Tys>::dummy...;
    };

    template <typename Ta, typename... Tys>
    struct _var_find_type {
        using type = decltype(internal::_var_func_overloads<Tys...>{}.dummy(std::declval<Ta>()));
    };

    template <typename Ty, typename Arg, typename... Args>
    class _var_has_same;

    template <typename Ty, typename Arg>
    class _var_has_same<Ty, Arg> {
    public:
        using type = Ty;
        static constexpr bool value =
            std::is_same<Ty, Arg>::value;
        static constexpr size_t index =
            std::is_same<Ty, Arg>::value ?
                0 : (std::numeric_limits<size_t>::max)();
    };

    template <typename Ty, typename Arg, typename... Args>
    class _var_has_same {
    public:
        using type = Ty;
        static constexpr bool value =
            std::is_same<Ty, Arg>::value ||
            _var_has_same<Ty, Args...>::value;
        static constexpr size_t index =
            std::is_same<Ty, Arg>::value ?
                sizeof...(Args) : _var_has_same<Ty, Args...>::index;
    };


    template <size_t S, size_t N, typename Arg, typename... Args>
    struct _var_find_index;

    template <size_t N, typename Arg, typename... Args>
    struct _var_find_index<N, N, Arg, Args...> {
        using type = Arg;
    };

    //template <size_t S, size_t N, typename Arg>
    //struct _var_find_index<S, N, Arg> {};

    template <size_t S, size_t N, typename Arg, typename... Args>
    struct _var_find_index {
        using type = typename _var_find_index<S + 1, N, Args...>::type;
    };

    template <size_t N, typename... Args>
    struct _var_find_index1 {
        using type = typename _var_find_index<0, N, Args...>::type;
    };

    template <typename Ty>
    void _var_destroyer(char* addr) {
        reinterpret_cast<Ty*>(addr)->~Ty();
    }

}

    template <typename... Tys>
    class var {
    public:
        template <
            typename Ty,
            typename std::enable_if<
                sizeof...(Tys) != 0, int>::type = 0>
        var(const Ty& v) {
            using type = typename internal::_var_find_type<Ty, Tys...>::type;
            //auto id1 = typeid(Ty).name();
            //auto id2 = typeid(type).name();
            //auto is_c = internal::_var_has_same<type, Tys...>::value;

            index_ = sizeof...(Tys) - internal::_var_has_same<type, Tys...>::index - 1;
            new(storage_) type(v);

            deleter_ = internal::_var_destroyer<type>;
        }

        ~var() {
            deleter_(storage_);
        }

        template <
            typename Ty,
            typename std::enable_if<
                internal::_var_has_same<Ty, Tys...>::value, int>::type = 0>
        Ty& get() {
            size_t index = sizeof...(Tys) - internal::_var_has_same<Ty, Tys...>::index - 1;
            assert(index == index_);
            return reinterpret_cast<Ty&>(storage_[0]);
        }

        template <
            size_t Idx,
            typename std::enable_if<(Idx < sizeof...(Tys)), int>::type = 0>
        typename internal::_var_find_index1<Idx, Tys...>::type& get() {
            using type = typename internal::_var_find_index1<Idx, Tys...>::type;
            assert(Idx == index_);
            return reinterpret_cast<type&>(storage_[0]);
        }

    private:
        size_t index_;
        void (*deleter_)(char*);
        alignas(Tys...) char storage_[(std::max)({ sizeof(Tys)... })];
    };

}

#endif  // UTILS_VAR_HPP_
