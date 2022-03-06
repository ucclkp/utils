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

#include "utils/numbers.hpp"
#include "utils/type_utils.hpp"


namespace utl {
namespace math {

    template <typename Ty, size_t Row, size_t Col>
    class MatrixT;

namespace internal {
    template <typename Ty>
    Ty detTrans(const MatrixT<Ty, 1, 1>& m);

    template <typename Ty>
    Ty detTrans(const MatrixT<Ty, 2, 2>& m);

    template <typename Ty>
    Ty detTrans(const MatrixT<Ty, 3, 3>& m);

    template <typename Ty, size_t Row, size_t Col>
    Ty detTrans(const MatrixT<Ty, Row, Col>& m);

    template <typename Ty>
    void cofactor(
        const MatrixT<Ty, 2, 2>& m,
        size_t row_index, size_t col_index,
        MatrixT<Ty, 1, 1>* out);

    template <typename Ty>
    void cofactor(
        const MatrixT<Ty, 3, 3>& m,
        size_t row_index, size_t col_index,
        MatrixT<Ty, 2, 2>* out);

    template <typename Ty, size_t Row, size_t Col>
    void cofactor(
        const MatrixT<Ty, Row, Col>& m,
        size_t row_index, size_t col_index,
        MatrixT<Ty, Row - 1, Col - 1>* out);
}

    template <typename Ty, size_t Row, size_t Col>
    class MatrixT {
    public:
        static_assert(Row != 0 && Col != 0, "Row and Col must be greater than 0!");

        static MatrixT Z() {
            MatrixT m;
            m.zero();
            return m;
        }

        bool operator==(const MatrixT& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const MatrixT& rhs) const {
            return !equal(rhs);
        }

        MatrixT operator+(const MatrixT& rhs) const {
            MatrixT m(*this);
            m.add(rhs);
            return m;
        }
        MatrixT operator-(const MatrixT& rhs) const {
            MatrixT m(*this);
            m.sub(rhs);
            return m;
        }
        MatrixT operator*(Ty val) const {
            MatrixT m(*this);
            m.mul(val);
            return m;
        }

        template <size_t RCol>
        MatrixT<Ty, Row, RCol> operator*(const MatrixT<Ty, Col, RCol>& rhs) const {
            return mul(rhs);
        }

        MatrixT operator/(Ty val) const {
            MatrixT m(*this);
            m.div(val);
            return m;
        }

        MatrixT& operator+=(const MatrixT& rhs) {
            return add(rhs);
        }
        MatrixT& operator-=(const MatrixT& rhs) {
            return sub(rhs);
        }
        MatrixT& operator*=(Ty val) {
            return mul(val);
        }
        MatrixT& operator/=(Ty val) {
            return div(val);
        }

        MatrixT operator-() const {
            MatrixT m(*this);
            m.minus();
            return m;
        }

        template<typename Cy>
        explicit operator MatrixT<Cy, Row, Col>() const {
            MatrixT<Cy, Row, Col> out;
            for (size_t i = 0; i < Row * Col; ++i) {
                out.data[i] = static_cast<Cy>(data[i]);
            }
            return out;
        }

        Ty operator()(size_t row_index, size_t col_index) const {
            assert(row_index < Row && col_index < Col);
            return data[row_index * Col + col_index];
        }
        Ty& operator()(size_t row_index, size_t col_index) {
            assert(row_index < Row && col_index < Col);
            return data[row_index * Col + col_index];
        }

        MatrixT<Ty, 1, Col> row(size_t index) const {
            assert(index < Row);
            MatrixT<Ty, 1, Col> out;
            for (size_t i = 0; i < Col; ++i) {
                out.data[i] = data[index * Col + i];
            }
            return out;
        }
        MatrixT<Ty, Row, 1> col(size_t index) const {
            assert(index < Col);
            MatrixT<Ty, Row, 1> out;
            for (size_t i = 0; i < Row; ++i) {
                out.data[i] = data[i * Col + index];
            }
            return out;
        }

        template <size_t Idx>
        MatrixT<Ty, 1, Col> row() const {
            if constexpr (Idx >= Row) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            MatrixT<Ty, 1, Col> out;
            for (size_t i = 0; i < Col; ++i) {
                out.data[i] = data[Idx * Col + i];
            }
            return out;
        }
        template <size_t Idx>
        MatrixT<Ty, Row, 1> col() const {
            if constexpr (Idx >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            MatrixT<Ty, Row, 1> out;
            for (size_t i = 0; i < Row; ++i) {
                out.data[i] = data[i * Col + Idx];
            }
            return out;
        }

        template <size_t ReRow, size_t ReCol>
        MatrixT<Ty, ReRow, ReCol> reduce() const {
            if constexpr (ReRow > Row || ReCol > Col) {
                static_assert(
                    sat_stub<Ty>::value,
                    "ReRow must be less than Row, and ReCol must be less than Col!");
            }

            MatrixT<Ty, ReRow, ReCol> out;
            for (size_t r = 0; r < ReRow; ++r) {
                for (size_t c = 0; c < ReCol; ++c) {
                    out.data[r * ReCol + c] = data[r * Col + c];
                }
            }
            return out;
        }

        MatrixT<Ty, Col, Row> T() const {
            return transpose();
        }

        MatrixT& add(const MatrixT& rhs) {
            for (size_t i = 0; i < Row*Col; ++i) {
                data[i] += rhs.data[i];
            }
            return *this;
        }

        MatrixT& sub(const MatrixT& rhs) {
            for (size_t i = 0; i < Row*Col; ++i) {
                data[i] -= rhs.data[i];
            }
            return *this;
        }

        MatrixT& minus() {
            for (size_t i = 0; i < Row*Col; ++i) {
                data[i] = -data[i];
            }
            return *this;
        }

        MatrixT& mul(Ty val) {
            for (size_t i = 0; i < Row*Col; ++i) {
                data[i] *= val;
            }
            return *this;
        }

        MatrixT& div(Ty val) {
            for (size_t i = 0; i < Row*Col; ++i) {
                data[i] /= val;
            }
            return *this;
        }

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

        MatrixT& add_row(size_t index, Ty val) {
            assert(index < Row);
            for (size_t i = 0; i < Col; ++i) {
                data[index * Col + i] += val;
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& add_row(Ty val) {
            if constexpr (Idx >= Row) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Col; ++i) {
                data[Idx * Col + i] += val;
            }
            return *this;
        }

        MatrixT& add_row(size_t index, const MatrixT<Ty, 1, Col>& m) {
            assert(index < Row);
            for (size_t i = 0; i < Col; ++i) {
                data[index * Col + i] += m(i);
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& add_row(const MatrixT<Ty, 1, Col>& m) {
            if constexpr (Idx >= Row) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Col; ++i) {
                data[Idx * Col + i] += m(i);
            }
            return *this;
        }

        MatrixT& sub_row(size_t index, Ty val) {
            assert(index < Row);
            for (size_t i = 0; i < Col; ++i) {
                data[index * Col + i] -= val;
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& sub_row(Ty val) {
            if constexpr (Idx >= Row) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Col; ++i) {
                data[Idx * Col + i] -= val;
            }
            return *this;
        }

        MatrixT& sub_row(size_t index, const MatrixT<Ty, 1, Col>& m) {
            assert(index < Row);
            for (size_t i = 0; i < Col; ++i) {
                data[index * Col + i] -= m(i);
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& sub_row(const MatrixT<Ty, 1, Col>& m) {
            if constexpr (Idx >= Row) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Col; ++i) {
                data[Idx * Col + i] -= m(i);
            }
            return *this;
        }

        MatrixT& mul_row(size_t index, Ty val) {
            assert(index < Row);
            for (size_t i = 0; i < Col; ++i) {
                data[index * Col + i] *= val;
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& mul_row(Ty val) {
            if constexpr (Idx >= Row) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Col; ++i) {
                data[Idx * Col + i] *= val;
            }
            return *this;
        }

        MatrixT& mul_row(size_t index, const MatrixT<Ty, 1, Col>& m) {
            assert(index < Row);
            for (size_t i = 0; i < Col; ++i) {
                data[index * Col + i] *= m(i);
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& mul_row(const MatrixT<Ty, 1, Col>& m) {
            if constexpr (Idx >= Row) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Col; ++i) {
                data[Idx * Col + i] *= m(i);
            }
            return *this;
        }

        MatrixT& div_row(size_t index, Ty val) {
            assert(index < Row);
            for (size_t i = 0; i < Col; ++i) {
                data[index * Col + i] /= val;
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& div_row(Ty val) {
            if constexpr (Idx >= Row) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Col; ++i) {
                data[Idx * Col + i] /= val;
            }
            return *this;
        }

        MatrixT& div_row(size_t index, const MatrixT<Ty, 1, Col>& m) {
            assert(index < Row);
            for (size_t i = 0; i < Col; ++i) {
                data[index * Col + i] /= m(i);
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& div_row(const MatrixT<Ty, 1, Col>& m) {
            if constexpr (Idx >= Row) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Col; ++i) {
                data[Idx * Col + i] /= m(i);
            }
            return *this;
        }

        MatrixT& add_col(size_t index, Ty val) {
            assert(index < Col);
            for (size_t i = 0; i < Row; ++i) {
                data[i * Col + index] += val;
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& add_col(Ty val) {
            if constexpr (Idx >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Row; ++i) {
                data[i * Col + Idx] += val;
            }
            return *this;
        }

        MatrixT& add_col(size_t index, const MatrixT<Ty, Row, 1>& m) {
            assert(index < Col);
            for (size_t i = 0; i < Row; ++i) {
                data[i * Col + index] += m(i);
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& add_col(const MatrixT<Ty, Row, 1>& m) {
            if constexpr (Idx >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Row; ++i) {
                data[i * Col + Idx] += m(i);
            }
            return *this;
        }

        MatrixT& sub_col(size_t index, Ty val) {
            assert(index < Col);
            for (size_t i = 0; i < Row; ++i) {
                data[i * Col + index] -= val;
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& sub_col(Ty val) {
            if constexpr (Idx >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Row; ++i) {
                data[i * Col + Idx] -= val;
            }
            return *this;
        }

        MatrixT& sub_col(size_t index, const MatrixT<Ty, Row, 1>& m) {
            assert(index < Col);
            for (size_t i = 0; i < Row; ++i) {
                data[i * Col + index] -= m(i);
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& sub_col(const MatrixT<Ty, Row, 1>& m) {
            if constexpr (Idx >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Row; ++i) {
                data[i * Col + Idx] -= m(i);
            }
            return *this;
        }

        MatrixT& mul_col(size_t index, Ty val) {
            assert(index < Col);
            for (size_t i = 0; i < Row; ++i) {
                data[i * Col + index] *= val;
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& mul_col(Ty val) {
            if constexpr (Idx >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Row; ++i) {
                data[i * Col + Idx] *= val;
            }
            return *this;
        }

        MatrixT& mul_col(size_t index, const MatrixT<Ty, Row, 1>& m) {
            assert(index < Col);
            for (size_t i = 0; i < Row; ++i) {
                data[i * Col + index] *= m(i);
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& mul_col(const MatrixT<Ty, Row, 1>& m) {
            if constexpr (Idx >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Row; ++i) {
                data[i * Col + Idx] *= m(i);
            }
            return *this;
        }

        MatrixT& div_col(size_t index, Ty val) {
            assert(index < Col);
            for (size_t i = 0; i < Row; ++i) {
                data[i * Col + index] /= val;
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& div_col(Ty val) {
            if constexpr (Idx >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Row; ++i) {
                data[i * Col + Idx] /= val;
            }
            return *this;
        }

        MatrixT& div_col(size_t index, const MatrixT<Ty, Row, 1>& m) {
            assert(index < Col);
            for (size_t i = 0; i < Row; ++i) {
                data[i * Col + index] /= m(i);
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& div_col(const MatrixT<Ty, Row, 1>& m) {
            if constexpr (Idx >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Row; ++i) {
                data[i * Col + Idx] /= m(i);
            }
            return *this;
        }

        MatrixT& set(size_t row_index, size_t col_index, Ty val) {
            assert(row_index < Row && col_index < Col);
            data[row_index * Col + col_index] = val;
            return *this;
        }

        template <size_t IRow, size_t ICol>
        MatrixT& set(Ty val) {
            if constexpr (IRow >= Row || ICol >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            data[IRow * Col + ICol] = val;
            return *this;
        }

        Ty get(size_t row_index, size_t col_index) const {
            assert(row_index < Row && col_index < Col);
            return data[row_index * Col + col_index];
        }

        template <size_t IRow, size_t ICol>
        Ty get() const {
            if constexpr (IRow >= Row || ICol >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            return data[IRow * Col + ICol];
        }

        Ty& get(size_t row_index, size_t col_index) {
            assert(row_index < Row&& col_index < Col);
            return data[row_index * Col + col_index];
        }

        template <size_t IRow, size_t ICol>
        Ty& get() {
            if constexpr (IRow >= Row || ICol >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            return data[IRow * Col + ICol];
        }

        MatrixT& zero() {
            for (size_t i = 0; i < Row * Col; ++i) {
                data[i] = Ty(0);
            }
            return *this;
        }

        MatrixT<Ty, Col, Row> transpose() const {
            MatrixT<Ty, Col, Row> out;
            for (size_t r = 0; r < Row; ++r) {
                for (size_t c = 0; c < Col; ++c) {
                    out.data[c * Row + r] = data[r * Col + c];
                }
            }
            return out;
        }

        bool equal(const MatrixT& rhs) const {
            for (size_t i = 0; i < Row * Col; ++i) {
                if (!utl::is_num_equal(data[i], rhs.data[i])) {
                    return false;
                }
            }
            return true;
        }

        MatrixT& swapCol(size_t c1_index, size_t c2_index) {
            assert(c1_index < Col && c2_index < Col);
            if (c1_index == c2_index) {
                return *this;
            }

            for (size_t i = 0; i < Row; ++i) {
                Ty tmp = data[i * Col + c1_index];
                data[i * Col + c1_index] = data[i * Col + c2_index];
                data[i * Col + c2_index] = tmp;
            }
            return *this;
        }
        MatrixT& swapRow(size_t r1_index, size_t r2_index) {
            assert(r1_index < Row && r2_index < Row);
            if (r1_index == r2_index) {
                return *this;
            }

            Ty tmp[Col];
            std::memcpy(tmp, &data[r1_index * Col], sizeof(Ty) * Col);
            std::memcpy(&data[r1_index * Col], &data[r2_index * Col], sizeof(Ty) * Col);
            std::memcpy(&data[r2_index * Col], tmp, sizeof(Ty) * Col);
            return *this;
        }

        template <size_t ICol1, size_t ICol2>
        MatrixT& swapCol() {
            if constexpr (ICol1 >= Col || ICol2 >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            if constexpr (ICol1 == ICol2) {
                return *this;
            }

            for (size_t i = 0; i < Row; ++i) {
                Ty tmp = data[i * Col + ICol1];
                data[i * Col + ICol1] = data[i * Col + ICol2];
                data[i * Col + ICol2] = tmp;
            }
            return *this;
        }

        template <size_t IRow1, size_t IRow2>
        MatrixT& swapRow() {
            if constexpr (IRow1 >= Row || IRow2 >= Row) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            if constexpr (IRow1 == IRow2) {
                return *this;
            }

            Ty tmp[Col];
            std::memcpy(tmp, &data[IRow1 * Col], sizeof(Ty) * Col);
            std::memcpy(&data[IRow1 * Col], &data[IRow2 * Col], sizeof(Ty) * Col);
            std::memcpy(&data[IRow2 * Col], tmp, sizeof(Ty) * Col);
            return *this;
        }

        MatrixT ref() const {
            size_t r = 0;
            MatrixT m = *this;

            for (size_t i = 0; i < Col; ++i) {
                size_t j;
                for (j = r; j < Row; ++j) {
                    if (!utl::is_num_zero(m.data[j * Col + i])) {
                        break;
                    }
                }

                if (j >= Row) {
                    continue;
                }

                if (j != r) {
                    m.swapRow(r, j);
                }

                auto val = m.data[r * Col + i];
                for (j = r + 1; j < Row; ++j) {
                    auto& p = m.data[j * Col + i];
                    if (utl::is_num_zero(p)) {
                        continue;
                    }

                    auto f = -p / val;
                    p = 0;
                    for (size_t k = i + 1; k < Col; ++k) {
                        m.data[j * Col + k] += m.data[r * Col + k] * f;
                    }
                }
                ++r;
            }
            return m;
        }

        MatrixT rref() const {
            size_t r = 0;
            MatrixT m = *this;
            size_t primary[Row];
            std::memset(primary, 0xFF, sizeof(size_t) * Row);

            for (size_t i = 0; i < Col; ++i) {
                size_t j;
                for (j = r; j < Row; ++j) {
                    if (!utl::is_num_zero(m.data[j * Col + i])) {
                        break;
                    }
                }

                if (j >= Row) {
                    continue;
                }

                if (j != r) {
                    m.swapRow(r, j);
                }

                primary[r] = i;

                auto& val = m.data[r * Col + i];
                for (j = r + 1; j < Row; ++j) {
                    auto& p = m.data[j * Col + i];
                    if (utl::is_num_zero(p)) {
                        continue;
                    }

                    auto f = -p / val;
                    p = 0;
                    for (size_t k = i + 1; k < Col; ++k) {
                        m.data[j * Col + k] += m.data[r * Col + k] * f;
                    }
                }

                for (size_t k = i + 1; k < Col; ++k) {
                    m.data[i * Col + k] *= 1 / val;
                }
                val = 1;
                ++r;
            }

            for (size_t i = Row; i-- > 1;) {
                size_t j = primary[i];
                if (j == size_t(-1)) {
                    continue;
                }

                auto val = m.data[i * Col + j];
                for (size_t k = i; k-- > 0;) {
                    auto& p = m.data[k * Col + j];
                    if (utl::is_num_zero(p)) {
                        continue;
                    }

                    auto f = -p / val;
                    p = 0;
                    for (size_t n = j + 1; n < Col; ++n) {
                        m.data[k * Col + n] += m.data[i * Col + n] * f;
                    }
                }
            }

            return m;
        }

        size_t row_size() const { return Row; }
        size_t col_size() const { return Col; }

        Ty data[Row*Col];

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
    class MatrixT<Ty, Num, Num> {
    public:
        static_assert(Num != 0, "Num must be greater than 0!");

        static MatrixT Z() {
            MatrixT m;
            m.zero();
            return m;
        }
        static MatrixT I() {
            MatrixT m;
            m.identity();
            return m;
        }

        bool operator==(const MatrixT& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const MatrixT& rhs) const {
            return !equal(rhs);
        }

        MatrixT operator+(const MatrixT& rhs) const {
            MatrixT m(*this);
            m.add(rhs);
            return m;
        }
        MatrixT operator-(const MatrixT& rhs) const {
            MatrixT m(*this);
            m.sub(rhs);
            return m;
        }
        MatrixT operator*(Ty val) const {
            MatrixT m(*this);
            m.mul(val);
            return m;
        }
        MatrixT operator/(Ty val) const {
            MatrixT m(*this);
            m.div(val);
            return m;
        }

        template <size_t RCol>
        MatrixT<Ty, Num, RCol> operator*(const MatrixT<Ty, Num, RCol>& rhs) const {
            return mul(rhs);
        }

        MatrixT& operator+=(const MatrixT& rhs) {
            return add(rhs);
        }
        MatrixT& operator-=(const MatrixT& rhs) {
            return sub(rhs);
        }
        MatrixT& operator*=(Ty val) {
            return mul(val);
        }
        MatrixT& operator/=(Ty val) {
            return div(val);
        }

        MatrixT operator-() const {
            MatrixT m(*this);
            m.minus();
            return m;
        }

        template<typename Cy>
        explicit operator MatrixT<Cy, Num, Num>() const {
            MatrixT<Cy, Num, Num> out;
            for (size_t i = 0; i < Num * Num; ++i) {
                out.data[i] = static_cast<Cy>(data[i]);
            }
            return out;
        }

        Ty operator()(size_t row_index, size_t col_index) const {
            assert(row_index < Num && col_index < Num);
            return data[row_index * Num + col_index];
        }
        Ty& operator()(size_t row_index, size_t col_index) {
            assert(row_index < Num && col_index < Num);
            return data[row_index * Num + col_index];
        }

        MatrixT<Ty, 1, Num> row(size_t index) const {
            assert(index < Num);
            MatrixT<Ty, 1, Num> out;
            for (size_t i = 0; i < Num; ++i) {
                out.data[i] = data[index * Num + i];
            }
            return out;
        }
        MatrixT<Ty, Num, 1> col(size_t index) const {
            assert(index < Num);
            MatrixT<Ty, Num, 1> out;
            for (size_t i = 0; i < Num; ++i) {
                out.data[i] = data[i * Num + index];
            }
            return out;
        }

        template <size_t Idx>
        MatrixT<Ty, 1, Num> row() const {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            MatrixT<Ty, 1, Num> out;
            for (size_t i = 0; i < Num; ++i) {
                out.data[i] = data[Idx * Num + i];
            }
            return out;
        }
        template <size_t Idx>
        MatrixT<Ty, Num, 1> col() const {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            MatrixT<Ty, Num, 1> out;
            for (size_t i = 0; i < Num; ++i) {
                out.data[i] = data[i * Num + Idx];
            }
            return out;
        }

        template <size_t ReRow, size_t ReCol>
        MatrixT<Ty, ReRow, ReCol> reduce() const {
            if constexpr (ReRow > Num || ReCol > Num) {
                static_assert(
                    sat_stub<Ty>::value,
                    "ReRow and ReCol must be less than Num!");
            }

            MatrixT<Ty, ReRow, ReCol> out;
            for (size_t r = 0; r < ReRow; ++r) {
                for (size_t c = 0; c < ReCol; ++c) {
                    out.data[r * ReCol + c] = data[r * Num + c];
                }
            }
            return out;
        }

        MatrixT<Ty, Num, Num> T() const {
            return transpose();
        }

        MatrixT& add(const MatrixT& rhs) {
            for (size_t i = 0; i < Num * Num; ++i) {
                data[i] += rhs.data[i];
            }
            return *this;
        }

        MatrixT& sub(const MatrixT& rhs) {
            for (size_t i = 0; i < Num * Num; ++i) {
                data[i] -= rhs.data[i];
            }
            return *this;
        }

        MatrixT& minus() {
            for (size_t i = 0; i < Num * Num; ++i) {
                data[i] = -data[i];
            }
            return *this;
        }

        MatrixT& mul(Ty val) {
            for (size_t i = 0; i < Num * Num; ++i) {
                data[i] *= val;
            }
            return *this;
        }

        MatrixT& div(Ty val) {
            for (size_t i = 0; i < Num * Num; ++i) {
                data[i] /= val;
            }
            return *this;
        }

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

        MatrixT& add_row(size_t index, Ty val) {
            assert(index < Num);
            for (size_t i = 0; i < Num; ++i) {
                data[index * Num + i] += val;
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& add_row(Ty val) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Num; ++i) {
                data[Idx * Num + i] += val;
            }
            return *this;
        }

        MatrixT& add_row(size_t index, const MatrixT<Ty, 1, Num>& m) {
            assert(index < Num);
            for (size_t i = 0; i < Num; ++i) {
                data[index * Num + i] += m(i);
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& add_row(const MatrixT<Ty, 1, Num>& m) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Num; ++i) {
                data[Idx * Num + i] += m(i);
            }
            return *this;
        }

        MatrixT& sub_row(size_t index, Ty val) {
            assert(index < Num);
            for (size_t i = 0; i < Num; ++i) {
                data[index * Num + i] -= val;
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& sub_row(Ty val) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Num; ++i) {
                data[Idx * Num + i] -= val;
            }
            return *this;
        }

        MatrixT& sub_row(size_t index, const MatrixT<Ty, 1, Num>& m) {
            assert(index < Num);
            for (size_t i = 0; i < Num; ++i) {
                data[index * Num + i] -= m(i);
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& sub_row(const MatrixT<Ty, 1, Num>& m) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Num; ++i) {
                data[Idx * Num + i] -= m(i);
            }
            return *this;
        }

        MatrixT& mul_row(size_t index, Ty val) {
            assert(index < Num);
            for (size_t i = 0; i < Num; ++i) {
                data[index * Num + i] *= val;
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& mul_row(Ty val) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Num; ++i) {
                data[Idx * Num + i] *= val;
            }
            return *this;
        }

        MatrixT& mul_row(size_t index, const MatrixT<Ty, 1, Num>& m) {
            assert(index < Num);
            for (size_t i = 0; i < Num; ++i) {
                data[index * Num + i] *= m(i);
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& mul_row(const MatrixT<Ty, 1, Num>& m) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Num; ++i) {
                data[Idx * Num + i] *= m(i);
            }
            return *this;
        }

        MatrixT& div_row(size_t index, Ty val) {
            assert(index < Num);
            for (size_t i = 0; i < Num; ++i) {
                data[index * Num + i] /= val;
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& div_row(Ty val) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Num; ++i) {
                data[Idx * Num + i] /= val;
            }
            return *this;
        }

        MatrixT& div_row(size_t index, const MatrixT<Ty, 1, Num>& m) {
            assert(index < Num);
            for (size_t i = 0; i < Num; ++i) {
                data[index * Num + i] /= m(i);
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& div_row(const MatrixT<Ty, 1, Num>& m) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Num; ++i) {
                data[Idx * Num + i] /= m(i);
            }
            return *this;
        }

        MatrixT& add_col(size_t index, Ty val) {
            assert(index < Num);
            for (size_t i = 0; i < Num; ++i) {
                data[i * Num + index] += val;
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& add_col(Ty val) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Num; ++i) {
                data[i * Num + Idx] += val;
            }
            return *this;
        }

        MatrixT& add_col(size_t index, const MatrixT<Ty, Num, 1>& m) {
            assert(index < Num);
            for (size_t i = 0; i < Num; ++i) {
                data[i * Num + index] += m(i);
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& add_col(const MatrixT<Ty, Num, 1>& m) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Num; ++i) {
                data[i * Num + Idx] += m(i);
            }
            return *this;
        }

        MatrixT& sub_col(size_t index, Ty val) {
            assert(index < Num);
            for (size_t i = 0; i < Num; ++i) {
                data[i * Num + index] -= val;
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& sub_col(Ty val) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Num; ++i) {
                data[i * Num + Idx] -= val;
            }
            return *this;
        }

        MatrixT& sub_col(size_t index, const MatrixT<Ty, Num, 1>& m) {
            assert(index < Num);
            for (size_t i = 0; i < Num; ++i) {
                data[i * Num + index] -= m(i);
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& sub_col(const MatrixT<Ty, Num, 1>& m) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Num; ++i) {
                data[i * Num + Idx] -= m(i);
            }
            return *this;
        }

        MatrixT& mul_col(size_t index, Ty val) {
            assert(index < Num);
            for (size_t i = 0; i < Num; ++i) {
                data[i * Num + index] *= val;
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& mul_col(Ty val) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Num; ++i) {
                data[i * Num + Idx] *= val;
            }
            return *this;
        }

        MatrixT& mul_col(size_t index, const MatrixT<Ty, Num, 1>& m) {
            assert(index < Num);
            for (size_t i = 0; i < Num; ++i) {
                data[i * Num + index] *= m(i);
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& mul_col(const MatrixT<Ty, Num, 1>& m) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Num; ++i) {
                data[i * Num + Idx] *= m(i);
            }
            return *this;
        }

        MatrixT& div_col(size_t index, Ty val) {
            assert(index < Num);
            for (size_t i = 0; i < Num; ++i) {
                data[i * Num + index] /= val;
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& div_col(Ty val) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Num; ++i) {
                data[i * Num + Idx] /= val;
            }
            return *this;
        }

        MatrixT& div_col(size_t index, const MatrixT<Ty, Num, 1>& m) {
            assert(index < Num);
            for (size_t i = 0; i < Num; ++i) {
                data[i * Num + index] /= m(i);
            }
            return *this;
        }

        template <size_t Idx>
        MatrixT& div_col(const MatrixT<Ty, Num, 1>& m) {
            if constexpr (Idx >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            for (size_t i = 0; i < Num; ++i) {
                data[i * Num + Idx] /= m(i);
            }
            return *this;
        }

        MatrixT& set(size_t row_index, size_t col_index, Ty val) {
            assert(row_index < Num && col_index < Num);
            data[row_index * Num + col_index] = val;
            return *this;
        }

        template <size_t IRow, size_t ICol>
        MatrixT& set(Ty val) {
            if constexpr (IRow >= Num || ICol >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            data[IRow * Num + ICol] = val;
            return *this;
        }

        Ty get(size_t row_index, size_t col_index) const {
            assert(row_index < Num && col_index < Num);
            return data[row_index * Num + col_index];
        }

        template <size_t IRow, size_t ICol>
        Ty get() const {
            if constexpr (IRow >= Num || ICol >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            return data[IRow * Num + ICol];
        }

        Ty& get(size_t row_index, size_t col_index) {
            assert(row_index < Num&& col_index < Num);
            return data[row_index * Num + col_index];
        }

        template <size_t IRow, size_t ICol>
        Ty& get() {
            if constexpr (IRow >= Num || ICol >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            return data[IRow * Num + ICol];
        }

        MatrixT& zero() {
            for (size_t i = 0; i < Num * Num; ++i) {
                data[i] = Ty(0);
            }
            return *this;
        }

        MatrixT& identity() {
            for (size_t i = 0; i < Num; ++i) {
                for (size_t j = 0; j < Num; ++j) {
                    data[i * Num + j] = (i == j) ? Ty(1) : Ty(0);
                }
            }
            return *this;
        }

        MatrixT transpose() const {
            MatrixT out;
            for (size_t r = 0; r < Num; ++r) {
                for (size_t c = 0; c < Num; ++c) {
                    out.data[c * Num + r] = data[r * Num + c];
                }
            }
            return out;
        }

        bool equal(const MatrixT& rhs) const {
            for (size_t i = 0; i < Num * Num; ++i) {
                if (!utl::is_num_equal(data[i], rhs.data[i])) {
                    return false;
                }
            }
            return true;
        }

        Ty det() const {
            return internal::detTrans(*this);
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
                    adj.data[i * Num + j] = internal::detTrans(mt) * (int((i + j) % 2) * -2 + 1);
                }
            }
            return adj.transpose();
        }

        MatrixT& swapCol(size_t c1_index, size_t c2_index) {
            assert(c1_index < Num && c2_index < Num);
            if (c1_index == c2_index) {
                return *this;
            }

            for (size_t i = 0; i < Num; ++i) {
                Ty tmp = data[i * Num + c1_index];
                data[i * Num + c1_index] = data[i * Num + c2_index];
                data[i * Num + c2_index] = tmp;
            }
            return *this;
        }
        MatrixT& swapRow(size_t r1_index, size_t r2_index) {
            assert(r1_index < Num && r2_index < Num);
            if (r1_index == r2_index) {
                return *this;
            }

            Ty tmp[Num];
            std::memcpy(tmp, &data[r1_index * Num], sizeof(Ty) * Num);
            std::memcpy(&data[r1_index * Num], &data[r2_index * Num], sizeof(Ty) * Num);
            std::memcpy(&data[r2_index * Num], tmp, sizeof(Ty) * Num);
            return *this;
        }

        template <size_t ICol1, size_t ICol2>
        MatrixT& swapCol() {
            if constexpr (ICol1 >= Num || ICol2 >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            if constexpr (ICol1 == ICol2) {
                return *this;
            }

            for (size_t i = 0; i < Num; ++i) {
                Ty tmp = data[i * Num + ICol1];
                data[i * Num + ICol1] = data[i * Num + ICol2];
                data[i * Num + ICol2] = tmp;
            }
            return *this;
        }

        template <size_t IRow1, size_t IRow2>
        MatrixT& swapRow() {
            if constexpr (IRow1 >= Num || IRow2 >= Num) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            if constexpr (IRow1 == IRow2) {
                return *this;
            }

            Ty tmp[Num];
            std::memcpy(tmp, &data[IRow1 * Num], sizeof(Ty) * Num);
            std::memcpy(&data[IRow1 * Num], &data[IRow2 * Num], sizeof(Ty) * Num);
            std::memcpy(&data[IRow2 * Num], tmp, sizeof(Ty) * Num);
            return *this;
        }

        MatrixT ref() const {
            size_t r = 0;
            MatrixT m = *this;

            for (size_t i = 0; i < Num; ++i) {
                size_t j;
                for (j = r; j < Num; ++j) {
                    if (!utl::is_num_zero(m.data[j * Num + i])) {
                        break;
                    }
                }

                if (j >= Num) {
                    continue;
                }

                if (j != r) {
                    m.swapRow(r, j);
                }

                auto val = m.data[r * Num + i];
                for (j = r + 1; j < Num; ++j) {
                    auto& p = m.data[j * Num + i];
                    if (utl::is_num_zero(p)) {
                        continue;
                    }

                    auto f = -p / val;
                    p = 0;
                    for (size_t k = i + 1; k < Num; ++k) {
                        m.data[j * Num + k] += m.data[r * Num + k] * f;
                    }
                }
                ++r;
            }
            return m;
        }

        MatrixT rref() const {
            size_t r = 0;
            MatrixT m = *this;
            size_t primary[Num];
            std::memset(primary, 0xFF, sizeof(size_t) * Num);

            for (size_t i = 0; i < Num; ++i) {
                size_t j;
                for (j = r; j < Num; ++j) {
                    if (!utl::is_num_zero(m.data[j * Num + i])) {
                        break;
                    }
                }

                if (j >= Num) {
                    continue;
                }

                if (j != r) {
                    m.swapRow(r, j);
                }

                primary[r] = i;

                auto& val = m.data[r * Num + i];
                for (j = r + 1; j < Num; ++j) {
                    auto& p = m.data[j * Num + i];
                    if (utl::is_num_zero(p)) {
                        continue;
                    }

                    auto f = -p / val;
                    p = 0;
                    for (size_t k = i + 1; k < Num; ++k) {
                        m.data[j * Num + k] += m.data[r * Num + k] * f;
                    }
                }

                for (size_t k = i + 1; k < Num; ++k) {
                    m.data[i * Num + k] *= 1 / val;
                }
                val = 1;
                ++r;
            }

            for (size_t i = Num; i-- > 1;) {
                size_t j = primary[i];
                if (j == size_t(-1)) {
                    continue;
                }

                auto val = m.data[i * Num + j];
                for (size_t k = i; k-- > 0;) {
                    auto& p = m.data[k * Num + j];
                    if (utl::is_num_zero(p)) {
                        continue;
                    }

                    auto f = -p / val;
                    p = 0;
                    for (size_t n = j + 1; n < Num; ++n) {
                        m.data[k * Num + n] += m.data[i * Num + n] * f;
                    }
                }
            }

            return m;
        }

        MatrixT inverse(bool* exists = nullptr) const {
            MatrixT<Ty, Num, Num * 2> m;

            // [M] -> [M, I]
            for (size_t i = 0; i < Num; ++i) {
                std::memcpy(
                    m.data + i * Num * 2,
                    data + i * Num, sizeof(Ty) * Num);
                std::memset(m.data + i * Num * 2 + Num, 0, sizeof(Ty) * Num);
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

        size_t row_size() const { return Num; }
        size_t col_size() const { return Num; }

        Ty data[Num * Num];

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
    class MatrixT<Ty, Row, 1> {
    public:
        static_assert(Row != 0, "Row must be greater than 0!");

        static MatrixT Z() {
            MatrixT m;
            m.zero();
            return m;
        }

        bool operator==(const MatrixT& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const MatrixT& rhs) const {
            return !equal(rhs);
        }

        MatrixT operator+(const MatrixT& rhs) const {
            MatrixT m(*this);
            m.add(rhs);
            return m;
        }
        MatrixT operator-(const MatrixT& rhs) const {
            MatrixT m(*this);
            m.sub(rhs);
            return m;
        }
        MatrixT operator*(Ty val) const {
            MatrixT m(*this);
            m.mul(val);
            return m;
        }
        MatrixT operator/(Ty val) const {
            MatrixT m(*this);
            m.div(val);
            return m;
        }
        Ty operator*(const MatrixT& rhs) const {
            return dot(rhs);
        }
        MatrixT operator^(const MatrixT& rhs) const {
            return cross(rhs);
        }

        template <size_t RCol>
        MatrixT<Ty, Row, RCol> operator*(const MatrixT<Ty, 1, RCol>& rhs) const {
            return mul(rhs);
        }

        MatrixT& operator+=(const MatrixT& rhs) {
            return add(rhs);
        }
        MatrixT& operator-=(const MatrixT& rhs) {
            return sub(rhs);
        }
        MatrixT& operator*=(Ty val) {
            return mul(val);
        }
        MatrixT& operator/=(Ty val) {
            return div(val);
        }
        MatrixT& operator^=(const MatrixT& rhs) {
            *this = cross(rhs);
            return *this;
        }

        MatrixT operator-() const {
            MatrixT m(*this);
            m.minus();
            return m;
        }

        template<typename Cy>
        explicit operator MatrixT<Cy, Row, 1>() const {
            MatrixT<Cy, Row, 1> out;
            for (size_t i = 0; i < Row; ++i) {
                out.data[i] = static_cast<Cy>(data[i]);
            }
            return out;
        }

        Ty operator()(size_t index) const {
            assert(index < Row);
            return data[index];
        }
        Ty& operator()(size_t index) {
            assert(index < Row);
            return data[index];
        }

        Ty x() const { return get<0>(); }
        Ty y() const { return get<1>(); }
        Ty z() const { return get<2>(); }
        Ty w() const { return get<3>(); }
        Ty& x() { return get<0>(); }
        Ty& y() { return get<1>(); }
        Ty& z() { return get<2>(); }
        Ty& w() { return get<3>(); }

        template <size_t Re>
        MatrixT<Ty, Re, 1> reduce() const {
            if constexpr (Re > Row) {
                static_assert(sat_stub<Ty>::value, "Re must be less than Row!");
            }

            MatrixT<Ty, Re, 1> out;
            for (size_t i = 0; i < Re; ++i) {
                out.data[i] = data[i];
            }
            return out;
        }

        MatrixT<Ty, 1, Row> T() const {
            return transpose();
        }

        MatrixT N() const {
            MatrixT m(*this);
            m.nor();
            return m;
        }

        MatrixT& add(const MatrixT& rhs) {
            for (size_t i = 0; i < Row; ++i) {
                data[i] += rhs.data[i];
            }
            return *this;
        }

        MatrixT& sub(const MatrixT& rhs) {
            for (size_t i = 0; i < Row; ++i) {
                data[i] -= rhs.data[i];
            }
            return *this;
        }

        MatrixT& minus() {
            for (size_t i = 0; i < Row; ++i) {
                data[i] = -data[i];
            }
            return *this;
        }

        MatrixT& mul(Ty val) {
            for (size_t i = 0; i < Row; ++i) {
                data[i] *= val;
            }
            return *this;
        }

        MatrixT& div(Ty val) {
            for (size_t i = 0; i < Row; ++i) {
                data[i] /= val;
            }
            return *this;
        }

        Ty dot(const MatrixT& rhs) const {
            Ty result = 0;
            for (size_t i = 0; i < Row; ++i) {
                result += data[i] * rhs.data[i];
            }
            return result;
        }

        MatrixT cross(const MatrixT& rhs) const {
            if constexpr (Row != 3) {
                static_assert(sat_stub<Ty>::value, "undefined cross operation!");
            }

            MatrixT out;
            out.data[0] = data[1] * rhs.data[2] - data[2] * rhs.data[1];
            out.data[1] = data[2] * rhs.data[0] - data[0] * rhs.data[2];
            out.data[2] = data[0] * rhs.data[1] - data[1] * rhs.data[0];
            return out;
        }

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
                data[i] += val;
            }
            return *this;
        }

        MatrixT& sub_col(Ty val) {
            for (size_t i = 0; i < Row; ++i) {
                data[i] -= val;
            }
            return *this;
        }

        MatrixT& mul_col(Ty val) {
            for (size_t i = 0; i < Row; ++i) {
                data[i] *= val;
            }
            return *this;
        }

        MatrixT& mul_col(const MatrixT<Ty, Row, 1>& m) {
            for (size_t i = 0; i < Row; ++i) {
                data[i] *= m(i);
            }
            return *this;
        }

        MatrixT& div_col(Ty val) {
            for (size_t i = 0; i < Row; ++i) {
                data[i] /= val;
            }
            return *this;
        }

        MatrixT& div_col(const MatrixT<Ty, Row, 1>& m) {
            for (size_t i = 0; i < Row; ++i) {
                data[i] /= m(i);
            }
            return *this;
        }

        MatrixT& set(size_t index, Ty val) {
            assert(index < Row);
            data[index] = val;
            return *this;
        }

        template <size_t Idx>
        MatrixT& set(Ty val) {
            if constexpr (Idx >= Row) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            data[Idx] = val;
            return *this;
        }

        Ty get(size_t index) const {
            assert(index < Row);
            return data[index];
        }

        template <size_t Idx>
        Ty get() const {
            if constexpr (Idx >= Row) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            return data[Idx];
        }

        Ty& get(size_t index) {
            assert(index < Row);
            return data[index];
        }

        template <size_t Idx>
        Ty& get() {
            if constexpr (Idx >= Row) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            return data[Idx];
        }

        MatrixT& zero() {
            for (size_t i = 0; i < Row; ++i) {
                data[i] = Ty(0);
            }
            return *this;
        }

        MatrixT<Ty, 1, Row> transpose() const {
            MatrixT<Ty, 1, Row> out;
            for (size_t r = 0; r < Row; ++r) {
                out.data[r] = data[r];
            }
            return out;
        }

        bool equal(const MatrixT& rhs) const {
            for (size_t i = 0; i < Row; ++i) {
                if (!utl::is_num_equal(data[i], rhs.data[i])) {
                    return false;
                }
            }
            return true;
        }

        MatrixT& swapRow(size_t r1_index, size_t r2_index) {
            assert(r1_index < Row&& r2_index < Row);
            if (r1_index == r2_index) {
                return *this;
            }

            Ty tmp[1];
            std::memcpy(tmp, &data[r1_index], sizeof(Ty));
            std::memcpy(&data[r1_index], &data[r2_index], sizeof(Ty));
            std::memcpy(&data[r2_index], tmp, sizeof(Ty));
            return *this;
        }

        template <size_t IRow1, size_t IRow2>
        MatrixT& swapRow() {
            if constexpr (IRow1 >= Row || IRow2 >= Row) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            if constexpr (IRow1 == IRow2) {
                return *this;
            }

            Ty tmp[1];
            std::memcpy(tmp, &data[IRow1], sizeof(Ty));
            std::memcpy(&data[IRow1], &data[IRow2], sizeof(Ty));
            std::memcpy(&data[IRow2], tmp, sizeof(Ty));
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

        MatrixT& nor() {
            div(length());
            return *this;
        }

        Ty length() const {
            return std::sqrt(lengsq());
        }

        Ty lengsq() const {
            Ty result = 0;
            for (size_t i = 0; i < Row; ++i) {
                result += data[i] * data[i];
            }
            return result;
        }

        size_t row_size() const { return Row; }
        size_t col_size() const { return 1u; }

        Ty data[Row];

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
    class MatrixT<Ty, 1, Col> {
    public:
        static_assert(Col != 0, "Col must be greater than 0!");

        static MatrixT Z() {
            MatrixT m;
            m.zero();
            return m;
        }

        bool operator==(const MatrixT& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const MatrixT& rhs) const {
            return !equal(rhs);
        }

        MatrixT operator+(const MatrixT& rhs) const {
            MatrixT m(*this);
            m.add(rhs);
            return m;
        }
        MatrixT operator-(const MatrixT& rhs) const {
            MatrixT m(*this);
            m.sub(rhs);
            return m;
        }
        MatrixT operator*(Ty val) const {
            MatrixT m(*this);
            m.mul(val);
            return m;
        }
        MatrixT operator/(Ty val) const {
            MatrixT m(*this);
            m.div(val);
            return m;
        }
        Ty operator*(const MatrixT& rhs) const {
            return dot(rhs);
        }
        MatrixT operator^(const MatrixT& rhs) const {
            return cross(rhs);
        }

        template <size_t RCol>
        MatrixT<Ty, 1, RCol> operator*(const MatrixT<Ty, Col, RCol>& rhs) const {
            return mul(rhs);
        }

        MatrixT& operator+=(const MatrixT& rhs) {
            return add(rhs);
        }
        MatrixT& operator-=(const MatrixT& rhs) {
            return sub(rhs);
        }
        MatrixT& operator*=(Ty val) {
            return mul(val);
        }
        MatrixT& operator/=(Ty val) {
            return div(val);
        }
        MatrixT& operator^=(const MatrixT& rhs) {
            *this = cross(rhs);
            return *this;
        }

        MatrixT operator-() const {
            MatrixT m(*this);
            m.minus();
            return m;
        }

        template<typename Cy>
        explicit operator MatrixT<Cy, 1, Col>() const {
            MatrixT<Cy, 1, Col> out;
            for (size_t i = 0; i < Col; ++i) {
                out.data[i] = static_cast<Cy>(data[i]);
            }
            return out;
        }

        Ty operator()(size_t index) const {
            assert(index < Col);
            return data[index];
        }
        Ty& operator()(size_t index) {
            assert(index < Col);
            return data[index];
        }

        Ty x() const { return get<0>(); }
        Ty y() const { return get<1>(); }
        Ty z() const { return get<2>(); }
        Ty w() const { return get<3>(); }
        Ty& x() { return get<0>(); }
        Ty& y() { return get<1>(); }
        Ty& z() { return get<2>(); }
        Ty& w() { return get<3>(); }

        template <size_t Re>
        MatrixT<Ty, 1, Re> reduce() const {
            if constexpr (Re > Col) {
                static_assert(sat_stub<Ty>::value, "Re must be less than Col!");
            }

            MatrixT<Ty, 1, Re> out;
            for (size_t i = 0; i < Re; ++i) {
                out.data[i] = data[i];
            }
            return out;
        }

        MatrixT<Ty, Col, 1> T() const {
            return transpose();
        }

        MatrixT N() const {
            MatrixT m(*this);
            m.nor();
            return m;
        }

        MatrixT& add(const MatrixT& rhs) {
            for (size_t i = 0; i < Col; ++i) {
                data[i] += rhs.data[i];
            }
            return *this;
        }

        MatrixT& sub(const MatrixT& rhs) {
            for (size_t i = 0; i < Col; ++i) {
                data[i] -= rhs.data[i];
            }
            return *this;
        }

        MatrixT& minus() {
            for (size_t i = 0; i < Col; ++i) {
                data[i] = -data[i];
            }
            return *this;
        }

        MatrixT& mul(Ty val) {
            for (size_t i = 0; i < Col; ++i) {
                data[i] *= val;
            }
            return *this;
        }

        MatrixT& div(Ty val) {
            for (size_t i = 0; i < Col; ++i) {
                data[i] /= val;
            }
            return *this;
        }

        Ty dot(const MatrixT& rhs) const {
            Ty result = 0;
            for (size_t i = 0; i < Col; ++i) {
                result += data[i] * rhs.data[i];
            }
            return result;
        }

        MatrixT cross(const MatrixT& rhs) const {
            if constexpr (Col != 3) {
                static_assert(sat_stub<Ty>::value, "undefined cross operation!");
            }

            MatrixT out;
            out.data[0] = data[1] * rhs.data[2] - data[2] * rhs.data[1];
            out.data[1] = data[2] * rhs.data[0] - data[0] * rhs.data[2];
            out.data[2] = data[0] * rhs.data[1] - data[1] * rhs.data[0];
            return out;
        }

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
                data[i] += val;
            }
            return *this;
        }

        MatrixT& sub_row(Ty val) {
            for (size_t i = 0; i < Col; ++i) {
                data[i] -= val;
            }
            return *this;
        }

        MatrixT& mul_row(Ty val) {
            for (size_t i = 0; i < Col; ++i) {
                data[i] *= val;
            }
            return *this;
        }

        MatrixT& mul_row(const MatrixT<Ty, 1, Col>& m) {
            for (size_t i = 0; i < Col; ++i) {
                data[i] *= m(i);
            }
            return *this;
        }

        MatrixT& div_row(Ty val) {
            for (size_t i = 0; i < Col; ++i) {
                data[i] /= val;
            }
            return *this;
        }

        MatrixT& div_row(const MatrixT<Ty, 1, Col>& m) {
            for (size_t i = 0; i < Col; ++i) {
                data[i] /= m(i);
            }
            return *this;
        }

        MatrixT& set(size_t index, Ty val) {
            assert(index < Col);
            data[index] = val;
            return *this;
        }

        template <size_t Idx>
        MatrixT& set(Ty val) {
            if constexpr (Idx >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            data[Idx] = val;
            return *this;
        }

        Ty get(size_t index) const {
            assert(index < Col);
            return data[index];
        }

        template <size_t Idx>
        Ty get() const {
            if constexpr (Idx >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            return data[Idx];
        }

        Ty& get(size_t index) {
            assert(index < Col);
            return data[index];
        }

        template <size_t Idx>
        Ty& get() {
            if constexpr (Idx >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            return data[Idx];
        }

        MatrixT& zero() {
            for (size_t i = 0; i < Col; ++i) {
                data[i] = Ty(0);
            }
            return *this;
        }

        MatrixT<Ty, Col, 1> transpose() const {
            MatrixT<Ty, Col, 1> out;
            for (size_t c = 0; c < Col; ++c) {
                out.data[c] = data[c];
            }
            return out;
        }

        bool equal(const MatrixT& rhs) const {
            for (size_t i = 0; i < Col; ++i) {
                if (!utl::is_num_equal(data[i], rhs.data[i])) {
                    return false;
                }
            }
            return true;
        }

        MatrixT& swapCol(size_t c1_index, size_t c2_index) {
            assert(c1_index < Col && c2_index < Col);
            if (c1_index == c2_index) {
                return *this;
            }

            Ty tmp = data[c1_index];
            data[c1_index] = data[c2_index];
            data[c2_index] = tmp;
            return *this;
        }

        template <size_t ICol1, size_t ICol2>
        MatrixT& swapCol() {
            if constexpr (ICol1 >= Col || ICol2 >= Col) {
                static_assert(sat_stub<Ty>::value, "Invalid index!");
            }
            if constexpr (ICol1 == ICol2) {
                return *this;
            }

            Ty tmp = data[ICol1];
            data[ICol1] = data[ICol2];
            data[ICol2] = tmp;
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

        MatrixT& nor() {
            div(length());
            return *this;
        }

        Ty length() const {
            return std::sqrt(lengsq());
        }

        Ty lengsq() const {
            Ty result = 0;
            for (size_t i = 0; i < Col; ++i) {
                result += data[i] * data[i];
            }
            return result;
        }

        size_t row_size() const { return 1u; }
        size_t col_size() const { return Col; }

        Ty data[Col];

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
    class MatrixT<Ty, 1, 1> {
    public:
        static MatrixT Z() {
            MatrixT m;
            m.zero();
            return m;
        }
        static MatrixT I() {
            MatrixT m;
            m.identity();
            return m;
        }

        bool operator==(const MatrixT& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const MatrixT& rhs) const {
            return !equal(rhs);
        }

        explicit operator Ty() const {
            return data[0];
        }

        MatrixT operator+(const MatrixT& rhs) const {
            MatrixT m(*this);
            m.add(rhs);
            return m;
        }
        MatrixT operator-(const MatrixT& rhs) const {
            MatrixT m(*this);
            m.sub(rhs);
            return m;
        }
        MatrixT operator*(Ty val) const {
            MatrixT m(*this);
            m.mul(val);
            return m;
        }
        MatrixT operator/(Ty val) const {
            MatrixT m(*this);
            m.div(val);
            return m;
        }

        template <size_t RCol>
        MatrixT<Ty, 1, RCol> operator*(const MatrixT<Ty, 1, RCol>& rhs) const {
            return mul(rhs);
        }

        MatrixT& operator+=(const MatrixT& rhs) {
            return add(rhs);
        }
        MatrixT& operator-=(const MatrixT& rhs) {
            return sub(rhs);
        }
        MatrixT& operator*=(Ty val) {
            return mul(val);
        }
        MatrixT& operator/=(Ty val) {
            return div(val);
        }

        MatrixT operator-() const {
            MatrixT m(*this);
            m.minus();
            return m;
        }

        template<typename Cy>
        explicit operator MatrixT<Cy, 1, 1>() const {
            MatrixT<Cy, 1, 1> out;
            out.data[0] = static_cast<Cy>(data[0]);
            return out;
        }

        Ty operator()() const {
            return data[0];
        }
        Ty& operator()() {
            return data[0];
        }

        Ty x() const { return get(); }
        Ty& x() { return get(); }

        MatrixT T() const {
            return transpose();
        }

        MatrixT& add(const MatrixT& rhs) {
            data[0] += rhs.data[0];
            return *this;
        }

        MatrixT& sub(const MatrixT& rhs) {
            data[0] -= rhs.data[0];
            return *this;
        }

        MatrixT& minus() {
            data[0] = -data[0];
            return *this;
        }

        MatrixT& mul(Ty val) {
            data[0] *= val;
            return *this;
        }

        MatrixT& div(Ty val) {
            data[0] /= val;
            return *this;
        }

        template <size_t RCol>
        MatrixT<Ty, 1, RCol> mul(const MatrixT<Ty, 1, RCol>& rhs) const {
            MatrixT<Ty, 1, RCol> out;
            for (size_t c = 0; c < RCol; ++c) {
                out.data[c] = rowMulCol(*this, 0, rhs, c);
            }
            return out;
        }

        MatrixT& set(Ty val) {
            data[0] = val;
            return *this;
        }

        Ty get() const {
            return data[0];
        }

        Ty& get() {
            return data[0];
        }

        MatrixT& zero() {
            data[0] = 0;
            return *this;
        }

        MatrixT& identity() {
            data[0] = 1;
            return *this;
        }

        MatrixT transpose() const {
            return *this;
        }

        bool equal(const MatrixT& rhs) const {
            return utl::is_num_equal(data[0], rhs.data[0]);
        }

        Ty det() const {
            return data[0];
        }

        MatrixT cofactor() const {
            return MatrixT{ 1 };
        }

        MatrixT adjoint() const {
            return MatrixT{ 1 };
        }

        MatrixT ref() const {
            MatrixT out;
            if (utl::is_num_zero(data[0])) {
                out.data[0] = 0;
            } else {
                out.data[0] = data[0];
            }
            return out;
        }

        MatrixT rref() const {
            MatrixT out;
            if (utl::is_num_zero(data[0])) {
                out.data[0] = 0;
            } else {
                out.data[0] = 1;
            }
            return out;
        }

        MatrixT inverse(bool* exists = nullptr) const {
            if (exists) {
                *exists = !utl::is_num_zero(data[0]);
                if (!*exists) {
                    return {};
                }
            }

            MatrixT out;
            out.data[0] = 1 / data[0];
            return out;
        }

        size_t row_size() const { return 1u; }
        size_t col_size() const { return 1u; }

        Ty data[1];

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


namespace internal {

    template <typename Ty>
    Ty detTrans(const MatrixT<Ty, 1, 1>& m) {
        return m.data[0];
    }

    template <typename Ty>
    Ty detTrans(const MatrixT<Ty, 2, 2>& m) {
        auto data = m.data;
        return data[0] * data[3] - data[1] * data[2];
    }

    template <typename Ty>
    Ty detTrans(const MatrixT<Ty, 3, 3>& m) {
        auto data = m.data;
        return data[0] * data[4] * data[8]
            + data[1] * data[5] * data[6]
            + data[2] * data[3] * data[7]
            - data[2] * data[4] * data[6]
            - data[1] * data[3] * data[8]
            - data[0] * data[5] * data[7];
    }

    template <typename Ty, size_t Row, size_t Col>
    Ty detTrans(const MatrixT<Ty, Row, Col>& m) {
        auto data = m.data;
        Ty result = 0;
        for (size_t i = 0; i < Col; ++i) {
            auto val = data[i];
            MatrixT<Ty, Row - 1, Col - 1> mt;
            cofactor(m, 0, i, &mt);
            result += val * detTrans(mt) * (int((i + 0) % 2) * -2 + 1);
        }

        return result;
    }

    template <typename Ty>
    void cofactor(
        const MatrixT<Ty, 2, 2>& m,
        size_t row_index, size_t col_index,
        MatrixT<Ty, 1, 1>* out)
    {
        assert(row_index < 2 && col_index < 2);

        auto r = (row_index + 1) % 2;
        auto c = (col_index + 1) % 2;
        out->data[0] = m.data[r * 2 + c];
    }

    template <typename Ty>
    void cofactor(
        const MatrixT<Ty, 3u, 3u>& m,
        size_t row_index, size_t col_index,
        MatrixT<Ty, 2, 2>* out)
    {
        assert(row_index < 3 && col_index < 3);

        if (row_index == 0) {
            if (col_index == 0) {
                *out = { m.get(1, 1), m.get(1, 2),
                         m.get(2, 1), m.get(2, 2) }; return;
            }
            if (col_index == 1) {
                *out = { m.get(1, 0), m.get(1, 2),
                         m.get(2, 0), m.get(2, 2) }; return;
            }
            *out = { m.get(1, 0), m.get(1, 1),
                     m.get(2, 0), m.get(2, 1) }; return;
        }

        if (row_index == 1) {
            if (col_index == 0) {
                *out = { m.get(0, 1), m.get(0, 2),
                         m.get(2, 1), m.get(2, 2) }; return;
            }
            if (col_index == 1) {
                *out = { m.get(0, 0), m.get(0, 2),
                         m.get(2, 0), m.get(2, 2) }; return;
            }
            *out = { m.get(0, 0), m.get(0, 1),
                     m.get(2, 0), m.get(2, 1) }; return;
        }

        if (col_index == 0) {
            *out = { m.get(0, 1), m.get(0, 2),
                     m.get(1, 1), m.get(1, 2) }; return;
        }
        if (col_index == 1) {
            *out = { m.get(0, 0), m.get(0, 2),
                     m.get(1, 0), m.get(1, 2) }; return;
        }
        *out = { m.get(0, 0), m.get(0, 1),
                 m.get(1, 0), m.get(1, 1) };
    }

    template <typename Ty, size_t Row, size_t Col>
    void cofactor(
        const MatrixT<Ty, Row, Col>& m,
        size_t row_index, size_t col_index,
        MatrixT<Ty, Row - 1, Col - 1>* out)
    {
        assert(row_index < Row && col_index < Col);

        size_t i = 0;
        auto data = m.data;
        if (col_index > 0 && col_index < Col - 1) {
            for (size_t j = 0; j < Row; ++j) {
                if (j == row_index) {
                    continue;
                }
                std::memcpy(
                    out->data + (Col - 1) * i,
                    data + Col * j,
                    col_index * sizeof(Ty));
                std::memcpy(
                    out->data + (Col - 1) * i + col_index,
                    data + Col * j + (col_index + 1),
                    (Col - 1 - col_index) * sizeof(Ty));
                ++i;
            }
        } else if (col_index > 0 && col_index >= Col - 1) {
            for (size_t j = 0; j < Row; ++j) {
                if (j == row_index) {
                    continue;
                }
                std::memcpy(
                    out->data + (Col - 1) * i,
                    data + Col * j,
                    col_index * sizeof(Ty));
                ++i;
            }
        } else if (col_index <= 0 && col_index < Col - 1) {
            for (size_t j = 0; j < Row; ++j) {
                if (j == row_index) {
                    continue;
                }
                std::memcpy(
                    out->data + (Col - 1) * i + col_index,
                    data + Col * j + (col_index + 1),
                    (Col - 1 - col_index) * sizeof(Ty));
                ++i;
            }
        }
    }

}

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
