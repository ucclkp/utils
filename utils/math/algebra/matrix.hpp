// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MATH_ALGEBRA_MATRIX_HPP_
#define UTILS_MATH_ALGEBRA_MATRIX_HPP_

#include <algorithm>
#include <cassert>
#include <cstring>
#include <type_traits>

#include "utils/math/algebra/matrix_internal.hpp"
#include "utils/numbers.hpp"


namespace utl {
namespace math {

    template <typename Ty, size_t Row, size_t Col>
    class MatrixT :
        public internal::MatrixT_base<Ty, MatrixT<Ty, Row, Col>, Row, Col>,
        public internal::MatrixT_row_col_methods<Ty, MatrixT<Ty, Row, Col>, Row, Col>
    {
        static_assert(Row != 0 && Col != 0, "Row and Col must be greater than 0!");
        using super = internal::MatrixT_base<Ty, MatrixT, Row, Col>;

    public:
        using super::operator*;

        template <size_t RCol>
        MatrixT<Ty, Row, RCol> operator*(const MatrixT<Ty, Col, RCol>& rhs) const {
            return this->mul(rhs);
        }

        using super::mul;

        template <size_t RCol>
        MatrixT<Ty, Row, RCol> mul(const MatrixT<Ty, Col, RCol>& rhs) const {
            MatrixT<Ty, Row, RCol> out;
            for (size_t r = 0; r < Row; ++r) {
                for (size_t c = 0; c < RCol; ++c) {
                    out.data[r * RCol + c] = rowMulCol(*this, r, rhs, c);
                }
            }
            return out;
        }

    private:
        template <size_t RCol>
        static Ty rowMulCol(
            const MatrixT& ml, size_t row_index,
            const MatrixT<Ty, Col, RCol>& mr, size_t col_index)
        {
            Ty result = 0;
            for (size_t c = 0; c < Col; ++c) {
                result += ml.data[row_index*Col + c] * mr.data[c*RCol + col_index];
            }
            return result;
        }
    };

    template <typename Ty, size_t Num>
    class MatrixT<Ty, Num, Num> :
        public internal::MatrixT_base<Ty, MatrixT<Ty, Num, Num>, Num, Num>,
        public internal::MatrixT_row_col_methods<Ty, MatrixT<Ty, Num, Num>, Num, Num>
    {
        static_assert(Num != 0, "Num must be greater than 0!");
        using super = internal::MatrixT_base<Ty, MatrixT, Num, Num>;

    public:
        static MatrixT I() {
            MatrixT m;
            m.identity();
            return m;
        }

        using super::operator*;

        template <size_t RCol>
        MatrixT<Ty, Num, RCol> operator*(const MatrixT<Ty, Num, RCol>& rhs) const {
            return this->mul(rhs);
        }

        using super::mul;

        template <size_t RCol>
        MatrixT<Ty, Num, RCol> mul(const MatrixT<Ty, Num, RCol>& rhs) const {
            MatrixT<Ty, Num, RCol> out;
            for (size_t r = 0; r < Num; ++r) {
                for (size_t c = 0; c < RCol; ++c) {
                    out.data[r * RCol + c] = rowMulCol(*this, r, rhs, c);
                }
            }
            return out;
        }

        MatrixT& identity() {
            for (size_t i = 0; i < Num; ++i) {
                for (size_t j = 0; j < Num; ++j) {
                    this->data[i * Num + j] = (i == j) ? Ty(1) : Ty(0);
                }
            }
            return *this;
        }

        Ty det() const {
            return internal::det_fast(*this);
        }

        Ty det_prec() const {
            return internal::det_slow(*this);
        }

        MatrixT<Ty, Num - 1, Num - 1> cofactor(
            size_t row_index, size_t col_index) const
        {
            MatrixT<Ty, Num - 1, Num - 1> out;
            internal::cofactor(*this, row_index, col_index, &out);
            return out;
        }

        MatrixT adjoint() const {
            MatrixT adj;
            for (size_t i = 0; i < Num; ++i) {
                for (size_t j = 0; j < Num; ++j) {
                    MatrixT<Ty, Num - 1, Num - 1> mt;
                    internal::cofactor(*this, i, j, &mt);
                    adj.data[i * Num + j] = internal::det_slow(mt) * (int((i + j) % 2) * -2 + 1);
                }
            }
            return adj.transpose();
        }

        MatrixT inverse(bool* exists = nullptr) const {
            MatrixT<Ty, Num, Num * 2> m;

            // [M] -> [M, I]
            for (size_t i = 0; i < Num; ++i) {
                std::memcpy(
                    m.data + i * Num * 2,
                    this->data + i * Num, sizeof(Ty) * Num);
                std::fill(
                    m.data + i * Num * 2 + Num,
                    m.data + i * Num * 2 + Num * 2, Ty(0));
                m.data[i * Num * 2 + Num + i] = 1;
            }

            // [M, I] -> [I', M^]
            m = m.rref();

            // Check [I'] == [I]
            if (exists) {
                for (size_t i = 0; i < Num * Num; ++i) {
                    auto r = i / Num;
                    auto c = i % Num;
                    if (!utl::is_num_equal(
                        m.data[r * Num * 2 + c],
                        (r == c) ? Ty(1) : Ty(0)))
                    {
                        *exists = false;
                        return {};
                    }
                }
                *exists = true;
            }

            // Copy [M^]
            MatrixT out;
            for (size_t i = 0; i < Num; ++i) {
                std::memcpy(
                    out.data + i * Num,
                    m.data + i * Num * 2 + Num, sizeof(Ty) * Num);
            }
            return out;
        }

    private:
        template <size_t RCol>
        static Ty rowMulCol(
            const MatrixT& ml, size_t row_index,
            const MatrixT<Ty, Num, RCol>& mr, size_t col_index)
        {
            Ty result = 0;
            for (size_t c = 0; c < Num; ++c) {
                result += ml.data[row_index * Num + c] * mr.data[c * RCol + col_index];
            }
            return result;
        }
    };

    template <typename Ty, size_t Row>
    class MatrixT<Ty, Row, 1> :
        public internal::MatrixT_base<Ty, MatrixT<Ty, Row, 1>, Row, 1>,
        public internal::MatrixT_vec_base<Ty, MatrixT<Ty, Row, 1>, Row>
    {
        static_assert(Row != 0, "Row must be greater than 0!");
        using super = internal::MatrixT_base<Ty, MatrixT, Row, 1>;
        using vec_super = internal::MatrixT_vec_base<Ty, MatrixT<Ty, Row, 1>, Row>;

    public:
        using super::operator*;
        using vec_super::operator*;

        template <size_t RCol>
        MatrixT<Ty, Row, RCol> operator*(const MatrixT<Ty, 1, RCol>& rhs) const {
            return this->mul(rhs);
        }

        template <size_t Re>
        typename std::enable_if<(Re < Row), MatrixT<Ty, Re, 1>>::
        type reduce() const {
            MatrixT<Ty, Re, 1> out;
            for (size_t i = 0; i < Re; ++i) {
                out.data[i] = this->data[i];
            }
            return out;
        }

        template <size_t NRow>
        typename std::enable_if<(NRow > Row), MatrixT<Ty, NRow, 1>>::
        type gain() const {
            MatrixT<Ty, NRow, 1> out;
            std::copy(this->data, this->data + Row, out.data);
            std::fill(out.data + Row, out.data + NRow, Ty());
            return out;
        }

        template <size_t NExt>
        typename std::enable_if<(NExt > 0), MatrixT<Ty, (Row + NExt), 1>>::
        type gain(const Ty(&args)[NExt]) const {
            MatrixT<Ty, (Row + NExt), 1> out;
            std::copy(this->data, this->data + Row, out.data);
            std::copy(args, args + NExt, out.data + Row);
            return out;
        }

        MatrixT<Ty, 1, Row> T() const {
            return transpose();
        }

        using super::mul;

        template <size_t RCol>
        MatrixT<Ty, Row, RCol> mul(const MatrixT<Ty, 1, RCol>& rhs) const {
            MatrixT<Ty, Row, RCol> out;
            for (size_t r = 0; r < Row; ++r) {
                for (size_t c = 0; c < RCol; ++c) {
                    out.data[r * RCol + c] = rowMulCol(*this, r, rhs, c);
                }
            }
            return out;
        }

        MatrixT& add_col(Ty val) {
            for (size_t i = 0; i < Row; ++i) {
                this->data[i] += val;
            }
            return *this;
        }

        MatrixT& sub_col(Ty val) {
            for (size_t i = 0; i < Row; ++i) {
                this->data[i] -= val;
            }
            return *this;
        }

        MatrixT& mul_col(Ty val) {
            for (size_t i = 0; i < Row; ++i) {
                this->data[i] *= val;
            }
            return *this;
        }

        MatrixT& mul_col(const MatrixT<Ty, Row, 1>& m) {
            for (size_t i = 0; i < Row; ++i) {
                this->data[i] *= m(i);
            }
            return *this;
        }

        MatrixT& div_col(Ty val) {
            for (size_t i = 0; i < Row; ++i) {
                this->data[i] /= val;
            }
            return *this;
        }

        MatrixT& div_col(const MatrixT<Ty, Row, 1>& m) {
            for (size_t i = 0; i < Row; ++i) {
                this->data[i] /= m(i);
            }
            return *this;
        }

        MatrixT<Ty, 1, Row> transpose() const {
            MatrixT<Ty, 1, Row> out;
            for (size_t r = 0; r < Row; ++r) {
                out.data[r] = this->data[r];
            }
            return out;
        }

        MatrixT& swapRow(size_t r1_index, size_t r2_index) {
            assert(r1_index < Row&& r2_index < Row);
            if (r1_index == r2_index) {
                return *this;
            }

            Ty tmp[1];
            std::memcpy(tmp, &this->data[r1_index], sizeof(Ty));
            std::memcpy(&this->data[r1_index], &this->data[r2_index], sizeof(Ty));
            std::memcpy(&this->data[r2_index], tmp, sizeof(Ty));
            return *this;
        }

        template <size_t IRow1, size_t IRow2>
        typename std::enable_if<(IRow1 < Row && IRow2 < Row), MatrixT&>::
        type swapRow() {
            if constexpr (IRow1 != IRow2) {
                Ty tmp[1];
                std::memcpy(tmp, &this->data[IRow1], sizeof(Ty));
                std::memcpy(&this->data[IRow1], &this->data[IRow2], sizeof(Ty));
                std::memcpy(&this->data[IRow2], tmp, sizeof(Ty));
            }
            return *this;
        }

        MatrixT ref() const {
            MatrixT m = *this;

            do {
                size_t j;
                for (j = 0; j < Row; ++j) {
                    if (!utl::is_num_zero(m.data[j])) {
                        break;
                    }
                }

                if (j >= Row) {
                    break;
                }

                if (j != 0) {
                    m.swapRow(0, j);
                }

                for (j = 1; j < Row; ++j) {
                    m.data[j] = 0;
                }
            } while (false);

            return m;
        }

        MatrixT rref() const {
            MatrixT m = *this;

            do {
                size_t j;
                for (j = 0; j < Row; ++j) {
                    if (!utl::is_num_zero(m.data[j])) {
                        break;
                    }
                }

                if (j >= Row) {
                    break;
                }

                if (j != 0) {
                    m.swapRow(0, j);
                }

                m.data[0] = 1;
                for (j = 1; j < Row; ++j) {
                    m.data[j] = 0;
                }
            } while (false);

            return m;
        }

    private:
        template <size_t RCol>
        static Ty rowMulCol(
            const MatrixT& ml, size_t row_index,
            const MatrixT<Ty, 1, RCol>& mr, size_t col_index)
        {
            return ml.data[row_index] * mr.data[col_index];
        }
    };

    template <typename Ty, size_t Col>
    class MatrixT<Ty, 1, Col> :
        public internal::MatrixT_base<Ty, MatrixT<Ty, 1, Col>, 1, Col>,
        public internal::MatrixT_vec_base<Ty, MatrixT<Ty, 1, Col>, Col>
    {
        static_assert(Col != 0, "Col must be greater than 0!");
        using super = internal::MatrixT_base<Ty, MatrixT, 1, Col>;
        using vec_super = internal::MatrixT_vec_base<Ty, MatrixT<Ty, 1, Col>, Col>;

    public:
        using super::operator*;
        using vec_super::operator*;

        template <size_t RCol>
        MatrixT<Ty, 1, RCol> operator*(const MatrixT<Ty, Col, RCol>& rhs) const {
            return this->mul(rhs);
        }

        template <size_t Re>
        typename std::enable_if<(Re < Col), MatrixT<Ty, 1, Re>>::
        type reduce() const {
            MatrixT<Ty, 1, Re> out;
            for (size_t i = 0; i < Re; ++i) {
                out.data[i] = this->data[i];
            }
            return out;
        }

        template <size_t NCol>
        typename std::enable_if<(NCol > Col), MatrixT<Ty, 1, NCol>>::
        type gain() const {
            MatrixT<Ty, 1, NCol> out;
            std::copy(this->data, this->data + Col, out.data);
            std::fill(out.data + Col, out.data + NCol, Ty());
            return out;
        }

        template <size_t NExt>
        typename std::enable_if<(NExt > 0), MatrixT<Ty, 1, (Col + NExt)>>::
        type gain(const Ty(&args)[NExt]) const {
            MatrixT<Ty, 1, (Col + NExt)> out;
            std::copy(this->data, this->data + Col, out.data);
            std::copy(args, args + NExt, out.data + Col);
            return out;
        }

        MatrixT<Ty, Col, 1> T() const {
            return transpose();
        }

        using super::mul;

        template <size_t RCol>
        MatrixT<Ty, 1, RCol> mul(const MatrixT<Ty, Col, RCol>& rhs) const {
            MatrixT<Ty, 1, RCol> out;
            for (size_t c = 0; c < RCol; ++c) {
                out.data[c] = rowMulCol(*this, 0, rhs, c);
            }
            return out;
        }

        MatrixT& add_row(Ty val) {
            for (size_t i = 0; i < Col; ++i) {
                this->data[i] += val;
            }
            return *this;
        }

        MatrixT& sub_row(Ty val) {
            for (size_t i = 0; i < Col; ++i) {
                this->data[i] -= val;
            }
            return *this;
        }

        MatrixT& mul_row(Ty val) {
            for (size_t i = 0; i < Col; ++i) {
                this->data[i] *= val;
            }
            return *this;
        }

        MatrixT& mul_row(const MatrixT<Ty, 1, Col>& m) {
            for (size_t i = 0; i < Col; ++i) {
                this->data[i] *= m(i);
            }
            return *this;
        }

        MatrixT& div_row(Ty val) {
            for (size_t i = 0; i < Col; ++i) {
                this->data[i] /= val;
            }
            return *this;
        }

        MatrixT& div_row(const MatrixT<Ty, 1, Col>& m) {
            for (size_t i = 0; i < Col; ++i) {
                this->data[i] /= m(i);
            }
            return *this;
        }

        MatrixT<Ty, Col, 1> transpose() const {
            MatrixT<Ty, Col, 1> out;
            for (size_t c = 0; c < Col; ++c) {
                out.data[c] = this->data[c];
            }
            return out;
        }

        MatrixT& swapCol(size_t c1_index, size_t c2_index) {
            assert(c1_index < Col && c2_index < Col);
            if (c1_index == c2_index) {
                return *this;
            }

            Ty tmp = this->data[c1_index];
            this->data[c1_index] = this->data[c2_index];
            this->data[c2_index] = tmp;
            return *this;
        }

        template <size_t ICol1, size_t ICol2>
        typename std::enable_if<(ICol1 < Col && ICol2 < Col), MatrixT&>::
        type swapCol() {
            if constexpr (ICol1 != ICol2) {
                Ty tmp = this->data[ICol1];
                this->data[ICol1] = this->data[ICol2];
                this->data[ICol2] = tmp;
            }
            return *this;
        }

        MatrixT ref() const {
            return *this;
        }

        MatrixT rref() const {
            MatrixT m = *this;

            do {
                if (utl::is_num_zero(m.data[0])) {
                    break;
                }

                auto& val = m.data[0];
                for (size_t k = 1; k < Col; ++k) {
                    m.data[k] *= 1 / val;
                }
                val = 1;
            } while (false);

            return m;
        }

    private:
        template <size_t RCol>
        static Ty rowMulCol(
            const MatrixT& ml, size_t row_index,
            const MatrixT<Ty, Col, RCol>& mr, size_t col_index)
        {
            Ty result = 0;
            for (size_t c = 0; c < Col; ++c) {
                result += ml.data[row_index * Col + c] * mr.data[c * RCol + col_index];
            }
            return result;
        }
    };

    template <typename Ty>
    class MatrixT<Ty, 1, 1> :
        public internal::MatrixT_base<Ty, MatrixT<Ty, 1, 1>, 1, 1>,
        public internal::MatrixT_vec_num_methods<Ty, MatrixT<Ty, 1, 1>, 1>
    {
        using super = internal::MatrixT_base<Ty, MatrixT, 1, 1>;

    public:
        static MatrixT I() {
            MatrixT m;
            m.identity();
            return m;
        }

        explicit operator Ty() const {
            return this->data[0];
        }

        using super::operator*;

        template <size_t RCol>
        MatrixT<Ty, 1, RCol> operator*(const MatrixT<Ty, 1, RCol>& rhs) const {
            return this->mul(rhs);
        }

        Ty operator()() const {
            return this->data[0];
        }
        Ty& operator()() {
            return this->data[0];
        }

        MatrixT T() const {
            return transpose();
        }

        using super::mul;

        template <size_t RCol>
        MatrixT<Ty, 1, RCol> mul(const MatrixT<Ty, 1, RCol>& rhs) const {
            MatrixT<Ty, 1, RCol> out;
            for (size_t c = 0; c < RCol; ++c) {
                out.data[c] = rowMulCol(*this, 0, rhs, c);
            }
            return out;
        }

        MatrixT& set(Ty val) {
            this->data[0] = val;
            return *this;
        }

        Ty get() const {
            return this->data[0];
        }

        Ty& at() {
            return this->data[0];
        }

        MatrixT& identity() {
            this->data[0] = 1;
            return *this;
        }

        MatrixT transpose() const {
            return *this;
        }

        Ty det() const {
            return this->data[0];
        }

        Ty det_prec() const {
            return this->data[0];
        }

        MatrixT cofactor() const {
            return MatrixT{ 1 };
        }

        MatrixT adjoint() const {
            return MatrixT{ 1 };
        }

        MatrixT ref() const {
            MatrixT out;
            if (utl::is_num_zero(this->data[0])) {
                out.data[0] = 0;
            } else {
                out.data[0] = this->data[0];
            }
            return out;
        }

        MatrixT rref() const {
            MatrixT out;
            if (utl::is_num_zero(this->data[0])) {
                out.data[0] = 0;
            } else {
                out.data[0] = 1;
            }
            return out;
        }

        MatrixT inverse(bool* exists = nullptr) const {
            if (exists) {
                *exists = !utl::is_num_zero(this->data[0]);
                if (!*exists) {
                    return {};
                }
            }

            MatrixT out;
            out.data[0] = 1 / this->data[0];
            return out;
        }

    private:
        template <size_t RCol>
        static Ty rowMulCol(
            const MatrixT& ml, size_t row_index,
            const MatrixT<Ty, 1, RCol>& mr, size_t col_index)
        {
            Ty result = 0;
            result += ml.data[row_index] * mr.data[col_index];
            return result;
        }
    };

    template <typename Ty, size_t Row, size_t Col>
    MatrixT<Ty, Row, Col> operator*(Ty val, const MatrixT<Ty, Row, Col>& rhs) {
        return rhs * val;
    }

    template <typename Ty, size_t Row>
    using VectorT = MatrixT<Ty, Row, 1>;

    template <typename Ty, size_t Col>
    using HVectorT = MatrixT<Ty, 1, Col>;

}

    using mat3f = math::MatrixT<float, 3, 3>;
    using mat3d = math::MatrixT<double, 3, 3>;
    using mat4f = math::MatrixT<float, 4, 4>;
    using mat4d = math::MatrixT<double, 4, 4>;

    using vec2f = math::VectorT<float, 2>;
    using vec2d = math::VectorT<double, 2>;
    using vec3f = math::VectorT<float, 3>;
    using vec3d = math::VectorT<double, 3>;
    using vec4f = math::VectorT<float, 4>;
    using vec4d = math::VectorT<double, 4>;

    using hvec2f = math::HVectorT<float, 2>;
    using hvec2d = math::HVectorT<double, 2>;
    using hvec3f = math::HVectorT<float, 3>;
    using hvec3d = math::HVectorT<double, 3>;
    using hvec4f = math::HVectorT<float, 4>;
    using hvec4d = math::HVectorT<double, 4>;

}

#endif  // UTILS_MATH_ALGEBRA_MATRIX_HPP_
