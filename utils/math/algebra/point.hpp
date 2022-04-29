// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MATH_ALGEBRA_POINT_HPP_
#define UTILS_MATH_ALGEBRA_POINT_HPP_

#include <algorithm>

#include "utils/numbers.hpp"
#include "utils/math/algebra/matrix.hpp"


namespace utl {
namespace math {

namespace internal {

    template <typename Ty, typename Pt, size_t Num>
    class PointT_num_methods {
    private:
        using PointT = Pt;

    public:
        Ty x() const { return data[0]; }
        Ty y() const { return data[1]; }
        Ty z() const { return data[2]; }
        Ty w() const { return data[3]; }
        Ty& x() { return data[0]; }
        Ty& y() { return data[1]; }
        Ty& z() { return data[2]; }
        Ty& w() { return data[3]; }

        PointT& x(Ty x) {
            data[0] = x;
            return static_cast<PointT&>(*this);
        }
        PointT& y(Ty y) {
            data[1] = y;
            return static_cast<PointT&>(*this);
        }
        PointT& z(Ty z) {
            data[2] = z;
            return static_cast<PointT&>(*this);
        }
        PointT& w(Ty w) {
            data[3] = w;
            return static_cast<PointT&>(*this);
        }

        Ty data[Num];
    };

    template <typename Ty, typename Pt>
    class PointT_num_methods<Ty, Pt, 1> {};

    template <typename Ty, typename Pt>
    class PointT_num_methods<Ty, Pt, 2> {
    private:
        using PointT = Pt;

    public:
        Ty x() const { return data[0]; }
        Ty y() const { return data[1]; }
        Ty& x() { return data[0]; }
        Ty& y() { return data[1]; }

        PointT& x(Ty x) {
            data[0] = x;
            return static_cast<PointT&>(*this);
        }
        PointT& y(Ty y) {
            data[1] = y;
            return static_cast<PointT&>(*this);
        }

        Ty data[2];
    };

    template <typename Ty, typename Pt>
    class PointT_num_methods<Ty, Pt, 3> {
    private:
        using PointT = Pt;

    public:
        Ty x() const { return data[0]; }
        Ty y() const { return data[1]; }
        Ty z() const { return data[2]; }
        Ty& x() { return data[0]; }
        Ty& y() { return data[1]; }
        Ty& z() { return data[2]; }

        PointT& x(Ty x) {
            data[0] = x;
            return static_cast<PointT&>(*this);
        }
        PointT& y(Ty y) {
            data[1] = y;
            return static_cast<PointT&>(*this);
        }
        PointT& z(Ty z) {
            data[2] = z;
            return static_cast<PointT&>(*this);
        }

        Ty data[3];
    };

}

    template <typename Ty, size_t Num>
    class PointT :
        public internal::PointT_num_methods<Ty, PointT<Ty, Num>, Num>
    {
    public:
        static_assert(Num != 0, "Num must be greater than 0!");

        using type = Ty;
        static constexpr size_t size = Num;

        static PointT Z() {
            PointT p;
            p.zeros();
            return p;
        }

        PointT operator+(const VectorT<Ty, Num>& rhs) const {
            return PointT(*this).add(rhs);
        }
        PointT operator+(const HVectorT<Ty, Num>& rhs) const {
            return PointT(*this).add(rhs);
        }
        VectorT<Ty, Num> operator-(const PointT& rhs) const {
            return PointT(*this).sub(rhs);
        }
        PointT operator-(const VectorT<Ty, Num>& rhs) const {
            return PointT(*this).sub(rhs);
        }
        PointT operator-(const HVectorT<Ty, Num>& rhs) const {
            return PointT(*this).sub(rhs);
        }

        bool operator==(const PointT& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const PointT& rhs) const {
            return !equal(rhs);
        }

        PointT operator-() const {
            PointT p(*this);
            p.minus();
            return p;
        }

        template<typename Cy>
        explicit operator PointT<Cy, Num>() const {
            PointT<Cy, Num> out;
            for (size_t i = 0; i < Num; ++i) {
                out.data[i] = static_cast<Cy>(this->data[i]);
            }
            return out;
        }

        Ty operator()(size_t index) const {
            assert(index < Num);
            return this->data[index];
        }
        Ty& operator()(size_t index) {
            assert(index < Num);
            return this->data[index];
        }

        VectorT<Ty, Num> vec() const {
            VectorT<Ty, Num> out;
            for (size_t i = 0; i < Num; ++i) {
                out.data[i] = this->data[i];
            }
            return out;
        }
        HVectorT<Ty, Num> vec_h() const {
            HVectorT<Ty, Num> out;
            for (size_t i = 0; i < Num; ++i) {
                out.data[i] = this->data[i];
            }
            return out;
        }

        template <size_t Re>
        typename std::enable_if<(Re < Num && Re > 0), PointT<Ty, Re>>::
        type reduce() const {
            PointT<Ty, Re> out;
            for (size_t i = 0; i < Re; ++i) {
                out.data[i] = this->data[i];
            }
            return out;
        }

        PointT& minus() {
            for (size_t i = 0; i < Num; ++i) {
                this->data[i] = -this->data[i];
            }
            return *this;
        }

        PointT& set(size_t index, Ty val) {
            assert(index < Num);
            this->data[index] = val;
            return *this;
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Num)>::
        type set(Ty val) {
            this->data[Idx] = val;
        }

        Ty get(size_t index) const {
            assert(index < Num);
            return this->data[index];
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Num), Ty>::
        type get() const {
            return this->data[Idx];
        }

        Ty at(size_t index) const {
            assert(index < Num);
            return this->data[index];
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Num), Ty>::
        type at() const {
            return this->data[Idx];
        }

        Ty& at(size_t index) {
            assert(index < Num);
            return this->data[index];
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Num), Ty&>::
        type at() {
            return this->data[Idx];
        }

        PointT& zeros() {
            std::fill(std::begin(this->data), std::end(this->data), Ty(0));
            return *this;
        }

        PointT& add(const VectorT<Ty, Num>& rhs) {
            for (size_t i = 0; i < Num; ++i) {
                this->data[i] += rhs.data[i];
            }
            return *this;
        }

        PointT& add(const HVectorT<Ty, Num>& rhs) {
            for (size_t i = 0; i < Num; ++i) {
                this->data[i] += rhs.data[i];
            }
            return *this;
        }

        PointT& sub(const VectorT<Ty, Num>& rhs) {
            for (size_t i = 0; i < Num; ++i) {
                this->data[i] -= rhs.data[i];
            }
            return *this;
        }

        PointT& sub(const HVectorT<Ty, Num>& rhs) {
            for (size_t i = 0; i < Num; ++i) {
                this->data[i] -= rhs.data[i];
            }
            return *this;
        }

        VectorT<Ty, Num> sub(const PointT& rhs) {
            VectorT<Ty, Num> out;
            for (size_t i = 0; i < Num; ++i) {
                out.data[i] = this->data[i] - rhs.data[i];
            }
            return out;
        }

        HVectorT<Ty, Num> sub_h(const PointT& rhs) {
            HVectorT<Ty, Num> out;
            for (size_t i = 0; i < Num; ++i) {
                out.data[i] = this->data[i] - rhs.data[i];
            }
            return out;
        }

        bool equal(const PointT& rhs) const {
            for (size_t i = 0; i < Num; ++i) {
                if (!utl::is_num_equal(this->data[i], rhs.data[i])) {
                    return false;
                }
            }
            return true;
        }

    };

    template <typename Ty>
    class PointT<Ty, 1> {
    public:
        using type = Ty;
        static constexpr size_t size = 1u;

        static PointT Z() {
            PointT p;
            p.zeros();
            return p;
        }

        PointT operator+(const VectorT<Ty, 1>& rhs) const {
            return PointT(*this).add(rhs);
        }
        VectorT<Ty, 1> operator-(const PointT& rhs) const {
            return PointT(*this).sub(rhs);
        }
        PointT operator-(const VectorT<Ty, 1>& rhs) const {
            return PointT(*this).sub(rhs);
        }

        bool operator==(const PointT& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const PointT& rhs) const {
            return !equal(rhs);
        }

        PointT operator-() const {
            PointT p(*this);
            p.minus();
            return p;
        }

        template<typename Cy>
        explicit operator PointT<Cy, 1>() const {
            PointT<Cy, 1> out;
            out.data[0] = static_cast<Cy>(this->data[0]);
            return out;
        }

        Ty operator()() const {
            return this->data[0];
        }
        Ty& operator()() {
            return this->data[0];
        }

        VectorT<Ty, 1> vec() const {
            VectorT<Ty, 1> out;
            out.data[0] = this->data[0];
            return out;
        }
        HVectorT<Ty, 1> vec_h() const {
            HVectorT<Ty, 1> out;
            out.data[0] = this->data[0];
            return out;
        }

        PointT& minus() {
            this->data[0] = -this->data[0];
            return *this;
        }

        PointT& set(Ty val) {
            this->data[0] = val;
            return *this;
        }

        Ty get() const {
            return this->data[0];
        }

        Ty at() const {
            return this->data[0];
        }

        Ty& at() {
            return this->data[0];
        }

        Ty x() const { return this->data[0]; }
        Ty& x() { return this->data[0]; }

        PointT& x(Ty x) {
            this->data[0] = x;
            return *this;
        }

        PointT& zeros() {
            this->data[0] = 0;
            return *this;
        }

        PointT& add(const VectorT<Ty, 1>& rhs) {
            this->data[0] += rhs.data[0];
            return *this;
        }

        PointT& sub(const VectorT<Ty, 1>& rhs) {
            this->data[0] -= rhs.data[0];
            return *this;
        }

        VectorT<Ty, 1> sub(const PointT& rhs) {
            VectorT<Ty, 1> out;
            out.data[0] = this->data[0] - rhs.data[0];
            return out;
        }

        HVectorT<Ty, 1> sub_h(const PointT& rhs) {
            HVectorT<Ty, 1> out;
            out.data[0] = this->data[0] - rhs.data[0];
            return out;
        }

        bool equal(const PointT& rhs) const {
            return utl::is_num_equal(this->data[0], rhs.data[0]);
        }

        Ty data[1];
    };

}

    using pt2i = math::PointT<int, 2>;
    using pt2f = math::PointT<float, 2>;
    using pt2d = math::PointT<double, 2>;
    using pt3i = math::PointT<int, 3>;
    using pt3f = math::PointT<float, 3>;
    using pt3d = math::PointT<double, 3>;
    using pt4i = math::PointT<int, 4>;
    using pt4f = math::PointT<float, 4>;
    using pt4d = math::PointT<double, 4>;

}

#endif  // UTILS_MATH_ALGEBRA_POINT_HPP_