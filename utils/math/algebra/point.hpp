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
    class PointT;

namespace internal {

    template <typename Ty, typename Pt, size_t Num>
    class PointT_base {
        using PointT = Pt;
    public:
        using type = Ty;
        static constexpr size_t size = Num;

        static PointT Z() {
            PointT p;
            p.zeros();
            return p;
        }

        Ty data[Num];
    };

    template <typename Ty, typename Pt>
    class PointT_x_methods {
        using PointT = Pt;
        PointT* derived() { return static_cast<PointT*>(this); }
        const PointT* derived() const { return static_cast<const PointT*>(this); }
    public:
        Ty x() const { return derived()->data[0]; }
        Ty& x() { return derived()->data[0]; }
        PointT& x(Ty x) {
            derived()->data[0] = x;
            return *derived();
        }
    };

    template <typename Ty, typename Pt>
    class PointT_y_methods {
        using PointT = Pt;
        PointT* derived() { return static_cast<PointT*>(this); }
        const PointT* derived() const { return static_cast<const PointT*>(this); }
    public:
        Ty y() const { return derived()->data[1]; }
        Ty& y() { return derived()->data[1]; }
        PointT& y(Ty y) {
            derived()->data[1] = y;
            return *derived();
        }
    };

    template <typename Ty, typename Pt>
    class PointT_z_methods {
        using PointT = Pt;
        PointT* derived() { return static_cast<PointT*>(this); }
        const PointT* derived() const { return static_cast<const PointT*>(this); }
    public:
        Ty z() const { return derived()->data[2]; }
        Ty& z() { return derived()->data[2]; }
        PointT& z(Ty z) {
            derived()->data[2] = z;
            return *derived();
        }
    };

    template <typename Ty, typename Pt>
    class PointT_w_methods {
        using PointT = Pt;
        PointT* derived() { return static_cast<PointT*>(this); }
        const PointT* derived() const { return static_cast<const PointT*>(this); }
    public:
        Ty w() const { return derived()->data[3]; }
        Ty& w() { return derived()->data[3]; }
        PointT& w(Ty w) {
            derived()->data[3] = w;
            return *derived();
        }
    };

    template <typename Ty, typename Pt, size_t Num>
    class PointT_num_methods :
        public PointT_x_methods<Ty, Pt>,
        public PointT_y_methods<Ty, Pt>,
        public PointT_z_methods<Ty, Pt>,
        public PointT_w_methods<Ty, Pt> {};

    template <typename Ty, typename Pt>
    class PointT_num_methods<Ty, Pt, 1> :
        public PointT_x_methods<Ty, Pt> {};

    template <typename Ty, typename Pt>
    class PointT_num_methods<Ty, Pt, 2> :
        public PointT_x_methods<Ty, Pt>,
        public PointT_y_methods<Ty, Pt> {};

    template <typename Ty, typename Pt>
    class PointT_num_methods<Ty, Pt, 3> :
        public PointT_x_methods<Ty, Pt>,
        public PointT_y_methods<Ty, Pt>,
        public PointT_z_methods<Ty, Pt> {};

    template <typename Ty, typename Pt>
    class PointT_num_methods<Ty, Pt, 4> :
        public PointT_x_methods<Ty, Pt>,
        public PointT_y_methods<Ty, Pt>,
        public PointT_z_methods<Ty, Pt>,
        public PointT_w_methods<Ty, Pt> {};

}

    template <typename Ty, size_t Num>
    class PointT :
        public internal::PointT_base<Ty, PointT<Ty, Num>, Num>,
        public internal::PointT_num_methods<Ty, PointT<Ty, Num>, Num>
    {
    public:
        static_assert(Num != 0, "Num must be greater than 0!");

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

        PointT operator*(Ty val) const {
            PointT out(*this);
            out.mul(val);
            return out;
        }
        PointT operator*(const MatrixT<Ty, Num, Num>& rhs) const {
            PointT<Ty, Num> out(*this);
            out.mulr(rhs);
            return out;
        }
        PointT operator*(const MatrixT<Ty, Num + 1, Num + 1>& rhs) const {
            PointT<Ty, Num> out(*this);
            out.mulr(rhs);
            return out;
        }
        PointT operator/(Ty val) const {
            PointT out(*this);
            out.div(val);
            return out;
        }
        PointT& operator*=(Ty val) {
            mul(val);
            return *this;
        }
        PointT& operator*=(const MatrixT<Ty, Num, Num>& rhs) {
            mulr(rhs);
            return *this;
        }
        PointT& operator*=(const MatrixT<Ty, Num + 1, Num + 1>& rhs) {
            mulr(rhs);
            return *this;
        }
        PointT& operator/=(Ty val) {
            div(val);
            return *this;
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

        template<typename Cy>
        PointT<Cy, Num> cast() const {
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

        template <size_t N>
        typename std::enable_if<(N > 0), PointT<Ty, (Num + N)>>::
        type gain(const Ty(&args)[N]) const {
            PointT<Ty, (Num + N)> out;
            std::copy(this->data, this->data + Num, out.data);
            std::copy(args, args + N, out.data + Num);
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

        PointT& mul(Ty val) {
            for (size_t i = 0; i < Num; ++i) {
                this->data[i] *= val;
            }
            return *this;
        }

        PointT& mull(const MatrixT<Ty, Num, Num>& lhs) {
            // m * p
            PointT out{};
            for (size_t r = 0; r < Num; ++r) {
                for (size_t c = 0; c < Num; ++c) {
                    out.data[r] += lhs.data[r * Num + c] * this->data[c];
                }
            }
            *this = out;
            return *this;
        }

        PointT& mull(const MatrixT<Ty, Num + 1, Num + 1>& lhs) {
            // m * p
            PointT out{};
            for (size_t r = 0; r < Num; ++r) {
                for (size_t c = 0; c < Num; ++c) {
                    out.data[r] += lhs.data[r * (Num + 1) + c] * this->data[c];
                }
                out.data[r] += lhs.data[r * (Num + 1) + Num];
            }
            *this = out;
            return *this;
        }

        PointT& mulr(const MatrixT<Ty, Num, Num>& rhs) {
            // p * m
            PointT out{};
            for (size_t c = 0; c < Num; ++c) {
                for (size_t r = 0; r < Num; ++r) {
                    out.data[c] += rhs.data[r * Num + c] * this->data[r];
                }
            }
            *this = out;
            return *this;
        }

        PointT& mulr(const MatrixT<Ty, Num + 1, Num + 1>& rhs) {
            // p * m
            PointT out{};
            for (size_t c = 0; c < Num; ++c) {
                for (size_t r = 0; r < Num; ++r) {
                    out.data[c] += rhs.data[r * (Num + 1) + c] * this->data[r];
                }
                out.data[c] += rhs.data[Num * (Num + 1) + c];
            }
            *this = out;
            return *this;
        }

        PointT& div(Ty val) {
            for (size_t i = 0; i < Num; ++i) {
                this->data[i] /= val;
            }
            return *this;
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
    class PointT<Ty, 1> :
        public internal::PointT_base<Ty, PointT<Ty, 1>, 1>,
        public internal::PointT_num_methods<Ty, PointT<Ty, 1>, 1>
    {
    public:
        PointT operator+(const VectorT<Ty, 1>& rhs) const {
            return PointT(*this).add(rhs);
        }
        VectorT<Ty, 1> operator-(const PointT& rhs) const {
            return PointT(*this).sub(rhs);
        }
        PointT operator-(const VectorT<Ty, 1>& rhs) const {
            return PointT(*this).sub(rhs);
        }

        PointT operator*(Ty val) const {
            PointT out(*this);
            out.mul(val);
            return out;
        }
        PointT operator*(const MatrixT<Ty, 1, 1>& rhs) const {
            PointT<Ty, 1> out(*this);
            out.mulr(rhs);
            return out;
        }
        PointT operator*(const MatrixT<Ty, 2, 2>& rhs) const {
            PointT<Ty, 1> out(*this);
            out.mulr(rhs);
            return out;
        }
        PointT operator/(Ty val) const {
            PointT out(*this);
            out.div(val);
            return out;
        }
        PointT& operator*=(Ty val) {
            mul(val);
            return *this;
        }
        PointT& operator*=(const MatrixT<Ty, 1, 1>& rhs) {
            mulr(rhs);
            return *this;
        }
        PointT& operator*=(const MatrixT<Ty, 2, 2>& rhs) {
            mulr(rhs);
            return *this;
        }
        PointT& operator/=(Ty val) {
            div(val);
            return *this;
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

        template<typename Cy>
        PointT<Cy, 1> cast() const {
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

        template <size_t N>
        typename std::enable_if<(N > 0), PointT<Ty, (1 + N)>>::
        type gain(const Ty(&args)[N]) const {
            PointT<Ty, (1 + N)> out;
            out.data[0] = this->data[0];
            std::copy(args, args + N, out.data + 1);
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

        PointT& mul(Ty val) {
            this->data[0] *= val;
            return *this;
        }

        PointT& mull(const MatrixT<Ty, 1, 1>& lhs) {
            // m * p
            PointT out;
            out.data[0] = lhs.data[0] * this->data[0];
            *this = out;
            return *this;
        }

        PointT& mull(const MatrixT<Ty, 2, 2>& lhs) {
            // m * p
            PointT out;
            out.data[0] = lhs.data[0] * this->data[0] + lhs.data[1];
            *this = out;
            return *this;
        }

        PointT& mulr(const MatrixT<Ty, 1, 1>& rhs) {
            // p * m
            PointT out;
            out.data[0] = rhs.data[0] * this->data[0];
            *this = out;
            return *this;
        }

        PointT& mulr(const MatrixT<Ty, 2, 2>& rhs) {
            // p * m
            PointT out;
            out.data[0] = rhs.data[0] * this->data[0] + rhs.data[2];
            *this = out;
            return *this;
        }

        PointT& div(Ty val) {
            this->data[0] /= val;
            return *this;
        }

        bool equal(const PointT& rhs) const {
            return utl::is_num_equal(this->data[0], rhs.data[0]);
        }
    };


    template <typename Ty, size_t Num>
    PointT<Ty, Num> operator*(Ty val, const PointT<Ty, Num>& p) {
        PointT<Ty, Num> out(p);
        out.mul(val);
        return out;
    }

    template <typename Ty, size_t Num>
    PointT<Ty, Num> operator*(const MatrixT<Ty, Num, Num>& m, const PointT<Ty, Num>& p) {
        PointT<Ty, Num> out(p);
        out.mull(m);
        return out;
    }

    template <typename Ty, size_t Num>
    PointT<Ty, Num> operator*(const MatrixT<Ty, Num + 1, Num + 1>& m, const PointT<Ty, Num>& p) {
        PointT<Ty, Num> out(p);
        out.mull(m);
        return out;
    }

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