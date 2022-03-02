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

    template <typename Ty, size_t Num>
    class PointT {
    public:
        static_assert(Num != 0, "Num must be greater than 0!");

        static PointT Z() {
            PointT m;
            m.zero();
            return m;
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
                out.data[i] = static_cast<Cy>(data[i]);
            }
            return out;
        }

        Ty operator()(size_t index) const {
            assert(index < Num);
            return data[index];
        }
        Ty& operator()(size_t index) {
            assert(index < Num);
            return data[index];
        }

        template <size_t Idx>
        Ty operator()() const {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            return data[Idx];
        }
        template <size_t Idx>
        Ty& operator()() {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            return data[Idx];
        }

        Ty x() const { return operator()<0>(); }
        Ty y() const { return operator()<1>(); }
        Ty z() const { return operator()<2>(); }
        Ty w() const { return operator()<3>(); }
        Ty& x() { return operator()<0>(); }
        Ty& y() { return operator()<1>(); }
        Ty& z() { return operator()<2>(); }
        Ty& w() { return operator()<3>(); }

        VectorT<Ty, Num> vec() const {
            VectorT<Ty, Num> out;
            for (size_t i = 0; i < Num; ++i) {
                out.data[i] = data[i];
            }
            return out;
        }
        HVectorT<Ty, Num> vec_h() const {
            HVectorT<Ty, Num> out;
            for (size_t i = 0; i < Num; ++i) {
                out.data[i] = data[i];
            }
            return out;
        }

        template <size_t Re>
        PointT<Ty, Re> reduce() const {
            if constexpr (Re >= Num) {
                static_assert(sat_stub<Ty>::value, "Re must be less than Num!");
            }

            PointT<Ty, Re> out;
            for (size_t i = 0; i < Re; ++i) {
                out.data[i] = data[i];
            }
            return out;
        }

        void minus() {
            for (size_t i = 0; i < Num; ++i) {
                data[i] = -data[i];
            }
        }

        void set(size_t index, Ty val) {
            assert(index < Num);
            data[index] = val;
        }

        template <size_t Idx>
        void set(Ty val) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            data[Idx] = val;
        }

        Ty get(size_t index) const {
            assert(index < Num);
            return data[index];
        }

        template <size_t Idx>
        Ty get() const {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            return data[Idx];
        }

        void zero() {
            std::memset(data, 0, sizeof(Ty) * Num);
        }

        PointT& add(const VectorT<Ty, Num>& rhs) {
            for (size_t i = 0; i < Num; ++i) {
                data[i] += rhs.data[i];
            }
            return *this;
        }

        PointT& add(const HVectorT<Ty, Num>& rhs) {
            for (size_t i = 0; i < Num; ++i) {
                data[i] += rhs.data[i];
            }
            return *this;
        }

        PointT& sub(const VectorT<Ty, Num>& rhs) {
            for (size_t i = 0; i < Num; ++i) {
                data[i] -= rhs.data[i];
            }
            return *this;
        }

        PointT& sub(const HVectorT<Ty, Num>& rhs) {
            for (size_t i = 0; i < Num; ++i) {
                data[i] -= rhs.data[i];
            }
            return *this;
        }

        VectorT<Ty, Num> sub(const PointT& rhs) {
            VectorT<Ty, Num> out;
            for (size_t i = 0; i < Num; ++i) {
                out.data[i] = data[i] - rhs.data[i];
            }
            return out;
        }

        HVectorT<Ty, Num> sub_h(const PointT& rhs) {
            HVectorT<Ty, Num> out;
            for (size_t i = 0; i < Num; ++i) {
                out.data[i] = data[i] - rhs.data[i];
            }
            return out;
        }

        bool equal(const PointT& rhs) const {
            for (size_t i = 0; i < Num; ++i) {
                if (!utl::is_num_equal(data[i], rhs.data[i])) {
                    return false;
                }
            }
            return true;
        }

        size_t size() const { return Num; }

        Ty data[Num];
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