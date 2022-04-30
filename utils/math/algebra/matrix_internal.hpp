// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MATH_ALGEBRA_MATRIX_INTERNAL_HPP_
#define UTILS_MATH_ALGEBRA_MATRIX_INTERNAL_HPP_

#include "utils/numbers.hpp"


namespace utl {
namespace math {

    template <typename Ty, size_t Row, size_t Col>
    class MatrixT;

namespace internal {

    template <typename Ty, typename Mat, size_t Row, size_t Col>
    class MatrixT_base {
    private:
        using MatrixT = Mat;
        static constexpr size_t Count = Row * Col;

    public:
        using type = Ty;
        static constexpr size_t row_size = Row;
        static constexpr size_t col_size = Col;

        static MatrixT Z() {
            MatrixT m;
            m.zeros();
            return m;
        }

        template<typename Cy>
        explicit operator math::MatrixT<Cy, Row, Col>() const {
            math::MatrixT<Cy, Row, Col> out;
            for (size_t i = 0; i < Count; ++i) {
                out.data[i] = static_cast<Cy>(this->data[i]);
            }
            return out;
        }

        template<typename Cy>
        math::MatrixT<Cy, Row, Col> cast() const {
            math::MatrixT<Cy, Row, Col> out;
            for (size_t i = 0; i < Count; ++i) {
                out.data[i] = static_cast<Cy>(this->data[i]);
            }
            return out;
        }

        template<size_t GaRow, size_t GaCol>
        typename std::enable_if<
            (GaRow >= Row && GaCol >= Col &&
                (GaRow != Row || GaCol != Col)),
            math::MatrixT<Ty, GaRow, GaCol>>::
        type gain_rc() const {
            size_t r = 0;
            math::MatrixT<Ty, GaRow, GaCol> out;
            for (; r < Row; ++r) {
                std::copy(
                    this->data + r * Col,
                    this->data + r * Col + Col,
                    out.data + r * GaCol);
                if constexpr (Col < GaCol) {
                    std::fill(
                        out.data + r * GaCol + Col,
                        out.data + r * GaCol + GaCol, Ty());
                }
            }
            if constexpr (Row < GaRow) {
                std::fill(
                    out.data + Row * GaCol,
                    out.data + GaRow * GaCol, Ty());
            }
            return out;
        }

        template<size_t GaRow, size_t GaCol, size_t N>
        typename std::enable_if<
            (GaRow >= Row && GaCol >= Col &&
                (GaRow != Row || GaCol != Col) && N > 0),
            math::MatrixT<Ty, GaRow, GaCol>>::
        type gain_rc(const Ty(&args)[N]) const {
            size_t r = 0;
            size_t src = 0;
            math::MatrixT<Ty, GaRow, GaCol> out;
            for (; r < Row; ++r) {
                std::copy(
                    this->data + r * Col,
                    this->data + r * Col + Col,
                    out.data + r * GaCol);
                if constexpr (Col < GaCol) {
                    size_t rem_count = (std::min)(GaCol - Col, N - src);
                    std::copy(
                        args + src,
                        args + src + rem_count,
                        out.data + r * GaCol + Col);
                    src += rem_count;
                    if (rem_count < GaCol - Col) {
                        std::fill(
                            out.data + r * GaCol + Col + rem_count,
                            out.data + r * GaCol + GaCol, Ty());
                    }
                }
            }

            if constexpr (Row < GaRow) {
                size_t rem_count = (std::min)((GaRow - Row) * GaCol, N - src);
                std::copy(
                    args + src,
                    args + src + rem_count,
                    out.data + Row * GaCol);
                if (rem_count < (GaRow - Row) * GaCol) {
                    std::fill(
                        out.data + Row * GaCol + rem_count,
                        out.data + GaRow * GaCol, Ty());
                }
            }
            return out;
        }

        template <size_t GaRow>
        typename std::enable_if<
            (GaRow > Row), math::MatrixT<Ty, GaRow, Col>>::
        type gain_row() const {
            math::MatrixT<Ty, GaRow, Col> out;
            std::copy(
                this->data,
                this->data + Row * Col,
                out.data);
            std::fill(
                out.data + Row * Col,
                out.data + GaRow * Col, Ty());
            return out;
        }

        template <size_t GaRow, size_t N>
        typename std::enable_if<
            (GaRow > Row && N > 0), math::MatrixT<Ty, GaRow, Col>>::
        type gain_row(const Ty(&args)[N]) const {
            size_t src = 0;
            math::MatrixT<Ty, GaRow, Col> out;
            std::copy(
                this->data,
                this->data + Row * Col,
                out.data);

            size_t rem_count = (std::min)((GaRow - Row) * Col, N - src);
            std::copy(
                args + src,
                args + src + rem_count,
                out.data + Row * Col);
            if (rem_count < (GaRow - Row) * Col) {
                std::fill(
                    out.data + Row * Col + rem_count,
                    out.data + GaRow * Col, Ty());
            }

            return out;
        }

        template <size_t GaCol>
        typename std::enable_if<
            (GaCol > Col), math::MatrixT<Ty, Row, GaCol>>::
        type gain_col() const {
            size_t r = 0;
            math::MatrixT<Ty, Row, GaCol> out;
            for (; r < Row; ++r) {
                std::copy(
                    this->data + r * Col,
                    this->data + r * Col + Col,
                    out.data + r * GaCol);
                std::fill(
                    out.data + r * GaCol + Col,
                    out.data + r * GaCol + GaCol, Ty());
            }
            return out;
        }

        template <size_t GaCol, size_t N>
        typename std::enable_if<
            (GaCol > Col && N > 0), math::MatrixT<Ty, Row, GaCol>>::
        type gain_col(const Ty(&args)[N]) const {
            size_t r = 0;
            size_t src = 0;
            math::MatrixT<Ty, Row, GaCol> out;
            for (; r < Row; ++r) {
                std::copy(
                    this->data + r * Col,
                    this->data + r * Col + Col,
                    out.data + r * GaCol);
                size_t rem_count = (std::min)(GaCol - Col, N - src);
                std::copy(
                    args + src,
                    args + src + rem_count,
                    out.data + r * GaCol + Col);
                src += rem_count;
                if (rem_count < GaCol - Col) {
                    std::fill(
                        out.data + r * GaCol + Col + rem_count,
                        out.data + r * GaCol + GaCol, Ty());
                }
            }
            return out;
        }

        template <size_t IRow, size_t ICol, size_t NRow, size_t NCol>
        typename std::enable_if<
            (IRow < Row && ICol < Col && NRow > 0 && NCol > 0),
            math::MatrixT<Ty, NRow, NCol>>::
        type shape() const {
            size_t r = IRow;
            size_t dr = 0;
            math::MatrixT<Ty, NRow, NCol> out;
            constexpr size_t _R = (IRow + NRow > Row) ? (Row - IRow) : NRow;
            constexpr size_t _C = (ICol + NCol > Col) ? Col : (ICol + NCol);
            for (; dr < _R; ++dr, ++r) {
                std::copy(
                    this->data + r * Col + ICol,
                    this->data + r * Col + _C,
                    out.data + dr * NCol);
                if constexpr (ICol + NCol > Col) {
                    std::fill(
                        out.data + dr * NCol + _C - ICol,
                        out.data + dr * NCol + NCol, Ty());
                }
            }
            if constexpr (_R < NRow) {
                std::fill(
                    out.data + _R * NCol,
                    out.data + NRow * NCol, Ty());
            }
            return out;
        }

        template <size_t IRow, size_t ICol, size_t NRow, size_t NCol, size_t N>
        typename std::enable_if<
            (IRow < Row && ICol < Col && NRow > 0 && NCol > 0),
            math::MatrixT<Ty, NRow, NCol>>::
        type shape(const Ty(&args)[N]) const {
            size_t r = IRow;
            size_t dr = 0;
            size_t src = 0;
            math::MatrixT<Ty, NRow, NCol> out;
            constexpr size_t _R = (IRow + NRow > Row) ? (Row - IRow) : NRow;
            constexpr size_t _C = (ICol + NCol > Col) ? Col : (ICol + NCol);
            for (; dr < _R; ++dr, ++r) {
                std::copy(
                    this->data + r * Col + ICol,
                    this->data + r * Col + _C,
                    out.data + dr * NCol);
                if constexpr (ICol + NCol > Col) {
                    size_t rem_count = (std::min)(ICol + NCol - Col, N - src);
                    std::copy(
                        args + src,
                        args + src + rem_count,
                        out.data + dr * NCol + _C - ICol);
                    src += rem_count;
                    if (rem_count < ICol + NCol - Col) {
                        std::fill(
                            out.data + dr * NCol + _C - ICol + rem_count,
                            out.data + dr * NCol + NCol, Ty());
                    }
                }
            }

            if constexpr (IRow + NRow > Row) {
                size_t rem_count = (std::min)((IRow + NRow - Row) * NCol, N - src);
                std::copy(
                    args + src,
                    args + src + rem_count,
                    out.data + _R * NCol);
                if (rem_count < (IRow + NRow - Row) * NCol) {
                    std::fill(
                        out.data + _R * NCol + rem_count,
                        out.data + NRow * NCol, Ty());
                }
            }
            return out;
        }

        bool operator==(const MatrixT& rhs) const {
            return equal(rhs);
        }
        bool operator!=(const MatrixT& rhs) const {
            return !equal(rhs);
        }

        bool equal(const MatrixT& rhs) const {
            for (size_t i = 0; i < Count; ++i) {
                if (!utl::is_num_equal(this->data[i], rhs.data[i])) {
                    return false;
                }
            }
            return true;
        }

        MatrixT operator+(const MatrixT& rhs) const {
            MatrixT m(static_cast<const MatrixT&>(*this));
            m.add(rhs);
            return m;
        }
        MatrixT operator-(const MatrixT& rhs) const {
            MatrixT m(static_cast<const MatrixT&>(*this));
            m.sub(rhs);
            return m;
        }
        MatrixT operator*(Ty val) const {
            MatrixT m(static_cast<const MatrixT&>(*this));
            m.mul(val);
            return m;
        }
        MatrixT operator/(Ty val) const {
            MatrixT m(static_cast<const MatrixT&>(*this));
            m.div(val);
            return m;
        }

        MatrixT& operator+=(const MatrixT& rhs) {
            return this->add(rhs);
        }
        MatrixT& operator-=(const MatrixT& rhs) {
            return this->sub(rhs);
        }
        MatrixT& operator*=(Ty val) {
            return this->mul(val);
        }
        MatrixT& operator/=(Ty val) {
            return this->div(val);
        }

        MatrixT operator-() const {
            MatrixT m(static_cast<const MatrixT&>(*this));
            m.minus();
            return m;
        }

        MatrixT& zeros() {
            std::fill(std::begin(this->data), std::end(this->data), Ty(0));
            return static_cast<MatrixT&>(*this);
        }

        MatrixT& add(const MatrixT& rhs) {
            for (size_t i = 0; i < Count; ++i) {
                this->data[i] += rhs.data[i];
            }
            return static_cast<MatrixT&>(*this);
        }

        MatrixT& sub(const MatrixT& rhs) {
            for (size_t i = 0; i < Count; ++i) {
                this->data[i] -= rhs.data[i];
            }
            return static_cast<MatrixT&>(*this);
        }

        MatrixT& minus() {
            for (size_t i = 0; i < Count; ++i) {
                this->data[i] = -this->data[i];
            }
            return static_cast<MatrixT&>(*this);
        }

        MatrixT& mul(Ty val) {
            for (size_t i = 0; i < Count; ++i) {
                this->data[i] *= val;
            }
            return static_cast<MatrixT&>(*this);
        }

        MatrixT& div(Ty val) {
            for (size_t i = 0; i < Count; ++i) {
                this->data[i] /= val;
            }
            return static_cast<MatrixT&>(*this);
        }

        Ty data[Count];
    };

    template <typename Ty, typename Mat, size_t Row, size_t Col>
    class MatrixT_row_col_methods {
    private:
        using MatrixT = Mat;

    public:
        math::MatrixT<Ty, 1, Col> row(size_t index) const {
            assert(index < Row);
            math::MatrixT<Ty, 1, Col> out;
            for (size_t i = 0; i < Col; ++i) {
                out.data[i] = derived()->data[index * Col + i];
            }
            return out;
        }
        math::MatrixT<Ty, Row, 1> col(size_t index) const {
            assert(index < Col);
            math::MatrixT<Ty, Row, 1> out;
            for (size_t i = 0; i < Row; ++i) {
                out.data[i] = derived()->data[i * Col + index];
            }
            return out;
        }

        Ty operator()(size_t row_index, size_t col_index) const {
            assert(row_index < Row&& col_index < Col);
            return derived()->data[row_index * Col + col_index];
        }
        Ty& operator()(size_t row_index, size_t col_index) {
            assert(row_index < Row&& col_index < Col);
            return derived()->data[row_index * Col + col_index];
        }

        MatrixT& set(size_t row_index, size_t col_index, Ty val) {
            assert(row_index < Row&& col_index < Col);
            derived()->data[row_index * Col + col_index] = val;
            return *derived();
        }

        template <size_t IRow, size_t ICol>
        typename std::enable_if<(IRow < Row && ICol < Col), MatrixT&>::
        type set(Ty val) {
            derived()->data[IRow * Col + ICol] = val;
            return *derived();
        }

        Ty get(size_t row_index, size_t col_index) const {
            assert(row_index < Row&& col_index < Col);
            return derived()->data[row_index * Col + col_index];
        }

        template <size_t IRow, size_t ICol>
        typename std::enable_if<(IRow < Row && ICol < Col), Ty>::
        type get() const {
            return derived()->data[IRow * Col + ICol];
        }

        Ty at(size_t row_index, size_t col_index) const {
            assert(row_index < Row&& col_index < Col);
            return derived()->data[row_index * Col + col_index];
        }

        template <size_t IRow, size_t ICol>
        typename std::enable_if<(IRow < Row&& ICol < Col), Ty>::
        type at() const {
            return derived()->data[IRow * Col + ICol];
        }

        Ty& at(size_t row_index, size_t col_index) {
            assert(row_index < Row&& col_index < Col);
            return derived()->data[row_index * Col + col_index];
        }

        template <size_t IRow, size_t ICol>
        typename std::enable_if<(IRow < Row && ICol < Col), Ty&>::
        type at() {
            return derived()->data[IRow * Col + ICol];
        }

        template <size_t ReRow, size_t ReCol>
        typename std::enable_if<
            (ReRow <= Row && ReCol <= Col && (ReRow != Row || ReCol != Col)),
            math::MatrixT<Ty, ReRow, ReCol>>::
        type reduce() const {
            math::MatrixT<Ty, ReRow, ReCol> out;
            for (size_t r = 0; r < ReRow; ++r) {
                for (size_t c = 0; c < ReCol; ++c) {
                    out.data[r * ReCol + c] = derived()->data[r * Col + c];
                }
            }
            return out;
        }

        math::MatrixT<Ty, Col, Row> T() const {
            return transpose();
        }

        math::MatrixT<Ty, Col, Row> transpose() const {
            math::MatrixT<Ty, Col, Row> out;
            for (size_t r = 0; r < Row; ++r) {
                for (size_t c = 0; c < Col; ++c) {
                    out.data[c * Row + r] = derived()->data[r * Col + c];
                }
            }
            return out;
        }

        MatrixT ref() const {
            size_t r = 0;
            MatrixT m = *derived();

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
            MatrixT m = *derived();
            size_t primary[Row];
            std::fill(
                std::begin(primary),
                std::end(primary),
                (std::numeric_limits<size_t>::max)());

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
                if (j == (std::numeric_limits<size_t>::max)()) {
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

        template <size_t Idx>
        typename std::enable_if<(Idx < Row), math::MatrixT<Ty, 1, Col>>::
        type row() const {
            math::MatrixT<Ty, 1, Col> out;
            for (size_t i = 0; i < Col; ++i) {
                out.data[i] = derived()->data[Idx * Col + i];
            }
            return out;
        }
        template <size_t Idx>
        typename std::enable_if<(Idx < Col), math::MatrixT<Ty, Row, 1>>::
        type col() const {
            math::MatrixT<Ty, Row, 1> out;
            for (size_t i = 0; i < Row; ++i) {
                out.data[i] = derived()->data[i * Col + Idx];
            }
            return out;
        }

        MatrixT& swapCol(size_t c1_index, size_t c2_index) {
            assert(c1_index < Col&& c2_index < Col);
            if (c1_index != c2_index) {
                for (size_t i = 0; i < Row; ++i) {
                    Ty tmp = derived()->data[i * Col + c1_index];
                    derived()->data[i * Col + c1_index] = derived()->data[i * Col + c2_index];
                    derived()->data[i * Col + c2_index] = tmp;
                }
            }
            return *derived();
        }
        MatrixT& swapRow(size_t r1_index, size_t r2_index) {
            assert(r1_index < Row&& r2_index < Row);
            if (r1_index != r2_index) {
                Ty tmp[Col];
                std::memcpy(tmp, &derived()->data[r1_index * Col], sizeof(Ty) * Col);
                std::memcpy(&derived()->data[r1_index * Col], &derived()->data[r2_index * Col], sizeof(Ty) * Col);
                std::memcpy(&derived()->data[r2_index * Col], tmp, sizeof(Ty) * Col);
            }
            return *derived();
        }

        template <size_t ICol1, size_t ICol2>
        typename std::enable_if<(ICol1 < Col && ICol2 < Col), MatrixT&>::
        type swapCol() {
            if constexpr (ICol1 != ICol2) {
                for (size_t i = 0; i < Row; ++i) {
                    Ty tmp = derived()->data[i * Col + ICol1];
                    derived()->data[i * Col + ICol1] = derived()->data[i * Col + ICol2];
                    derived()->data[i * Col + ICol2] = tmp;
                }
            }
            return *derived();
        }

        template <size_t IRow1, size_t IRow2>
        typename std::enable_if<(IRow1 < Row && IRow2 < Row), MatrixT&>::
        type swapRow() {
            if constexpr (IRow1 != IRow2) {
                Ty tmp[Col];
                std::memcpy(tmp, &derived()->data[IRow1 * Col], sizeof(Ty) * Col);
                std::memcpy(&derived()->data[IRow1 * Col], &derived()->data[IRow2 * Col], sizeof(Ty) * Col);
                std::memcpy(&derived()->data[IRow2 * Col], tmp, sizeof(Ty) * Col);
            }
            return *derived();
        }

        MatrixT& add_row(size_t index, Ty val) {
            assert(index < Row);
            for (size_t i = 0; i < Col; ++i) {
                derived()->data[index * Col + i] += val;
            }
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Row), MatrixT&>::
        type add_row(Ty val) {
            for (size_t i = 0; i < Col; ++i) {
                derived()->data[Idx * Col + i] += val;
            }
            return *derived();
        }

        MatrixT& add_row(size_t index, const math::MatrixT<Ty, 1, Col>& m) {
            assert(index < Row);
            for (size_t i = 0; i < Col; ++i) {
                derived()->data[index * Col + i] += m(i);
            }
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Row), MatrixT&>::
        type add_row(const math::MatrixT<Ty, 1, Col>& m) {
            for (size_t i = 0; i < Col; ++i) {
                derived()->data[Idx * Col + i] += m(i);
            }
            return *derived();
        }

        MatrixT& sub_row(size_t index, Ty val) {
            assert(index < Row);
            for (size_t i = 0; i < Col; ++i) {
                derived()->data[index * Col + i] -= val;
            }
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Row), MatrixT&>::
        type sub_row(Ty val) {
            for (size_t i = 0; i < Col; ++i) {
                derived()->data[Idx * Col + i] -= val;
            }
            return *derived();
        }

        MatrixT& sub_row(size_t index, const math::MatrixT<Ty, 1, Col>& m) {
            assert(index < Row);
            for (size_t i = 0; i < Col; ++i) {
                derived()->data[index * Col + i] -= m(i);
            }
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Row), MatrixT&>::
        type sub_row(const math::MatrixT<Ty, 1, Col>& m) {
            for (size_t i = 0; i < Col; ++i) {
                derived()->data[Idx * Col + i] -= m(i);
            }
            return *derived();
        }

        MatrixT& mul_row(size_t index, Ty val) {
            assert(index < Row);
            for (size_t i = 0; i < Col; ++i) {
                derived()->data[index * Col + i] *= val;
            }
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Row), MatrixT&>::
        type mul_row(Ty val) {
            for (size_t i = 0; i < Col; ++i) {
                derived()->data[Idx * Col + i] *= val;
            }
            return *derived();
        }

        MatrixT& mul_row(size_t index, const math::MatrixT<Ty, 1, Col>& m) {
            assert(index < Row);
            for (size_t i = 0; i < Col; ++i) {
                derived()->data[index * Col + i] *= m(i);
            }
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Row), MatrixT&>::
        type mul_row(const math::MatrixT<Ty, 1, Col>& m) {
            for (size_t i = 0; i < Col; ++i) {
                derived()->data[Idx * Col + i] *= m(i);
            }
            return *derived();
        }

        MatrixT& div_row(size_t index, Ty val) {
            assert(index < Row);
            for (size_t i = 0; i < Col; ++i) {
                derived()->data[index * Col + i] /= val;
            }
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Row), MatrixT&>::
        type div_row(Ty val) {
            for (size_t i = 0; i < Col; ++i) {
                derived()->data[Idx * Col + i] /= val;
            }
            return *derived();
        }

        MatrixT& div_row(size_t index, const math::MatrixT<Ty, 1, Col>& m) {
            assert(index < Row);
            for (size_t i = 0; i < Col; ++i) {
                derived()->data[index * Col + i] /= m(i);
            }
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Row), MatrixT&>::
        type div_row(const math::MatrixT<Ty, 1, Col>& m) {
            for (size_t i = 0; i < Col; ++i) {
                derived()->data[Idx * Col + i] /= m(i);
            }
            return *derived();
        }

        MatrixT& add_col(size_t index, Ty val) {
            assert(index < Col);
            for (size_t i = 0; i < Row; ++i) {
                derived()->data[i * Col + index] += val;
            }
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Col), MatrixT&>::
        type add_col(Ty val) {
            for (size_t i = 0; i < Row; ++i) {
                derived()->data[i * Col + Idx] += val;
            }
            return *derived();
        }

        MatrixT& add_col(size_t index, const math::MatrixT<Ty, Row, 1>& m) {
            assert(index < Col);
            for (size_t i = 0; i < Row; ++i) {
                derived()->data[i * Col + index] += m(i);
            }
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Col), MatrixT&>::
        type add_col(const math::MatrixT<Ty, Row, 1>& m) {
            for (size_t i = 0; i < Row; ++i) {
                derived()->data[i * Col + Idx] += m(i);
            }
            return *derived();
        }

        MatrixT& sub_col(size_t index, Ty val) {
            assert(index < Col);
            for (size_t i = 0; i < Row; ++i) {
                derived()->data[i * Col + index] -= val;
            }
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Col), MatrixT&>::
        type sub_col(Ty val) {
            for (size_t i = 0; i < Row; ++i) {
                derived()->data[i * Col + Idx] -= val;
            }
            return *derived();
        }

        MatrixT& sub_col(size_t index, const math::MatrixT<Ty, Row, 1>& m) {
            assert(index < Col);
            for (size_t i = 0; i < Row; ++i) {
                derived()->data[i * Col + index] -= m(i);
            }
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Col), MatrixT&>::
        type sub_col(const math::MatrixT<Ty, Row, 1>& m) {
            for (size_t i = 0; i < Row; ++i) {
                derived()->data[i * Col + Idx] -= m(i);
            }
            return *derived();
        }

        MatrixT& mul_col(size_t index, Ty val) {
            assert(index < Col);
            for (size_t i = 0; i < Row; ++i) {
                derived()->data[i * Col + index] *= val;
            }
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Col), MatrixT&>::
        type mul_col(Ty val) {
            for (size_t i = 0; i < Row; ++i) {
                derived()->data[i * Col + Idx] *= val;
            }
            return *derived();
        }

        MatrixT& mul_col(size_t index, const math::MatrixT<Ty, Row, 1>& m) {
            assert(index < Col);
            for (size_t i = 0; i < Row; ++i) {
                derived()->data[i * Col + index] *= m(i);
            }
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Col), MatrixT&>::
        type mul_col(const math::MatrixT<Ty, Row, 1>& m) {
            for (size_t i = 0; i < Row; ++i) {
                derived()->data[i * Col + Idx] *= m(i);
            }
            return *derived();
        }

        MatrixT& div_col(size_t index, Ty val) {
            assert(index < Col);
            for (size_t i = 0; i < Row; ++i) {
                derived()->data[i * Col + index] /= val;
            }
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Col), MatrixT&>::
        type div_col(Ty val) {
            for (size_t i = 0; i < Row; ++i) {
                derived()->data[i * Col + Idx] /= val;
            }
            return *derived();
        }

        MatrixT& div_col(size_t index, const math::MatrixT<Ty, Row, 1>& m) {
            assert(index < Col);
            for (size_t i = 0; i < Row; ++i) {
                derived()->data[i * Col + index] /= m(i);
            }
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Col), MatrixT&>::
        type div_col(const math::MatrixT<Ty, Row, 1>& m) {
            for (size_t i = 0; i < Row; ++i) {
                derived()->data[i * Col + Idx] /= m(i);
            }
            return *derived();
        }

    private:
        MatrixT* derived() {
            return static_cast<MatrixT*>(this);
        }
        const MatrixT* derived() const {
            return static_cast<const MatrixT*>(this);
        }
    };

    template <typename Ty, typename Mat>
    class MatrixT_x_methods {
        using MatrixT = Mat;
        MatrixT* derived() { return static_cast<MatrixT*>(this); }
        const MatrixT* derived() const { return static_cast<const MatrixT*>(this); }
    public:
        Ty x() const { return derived()->data[0]; }
        Ty& x() { return derived()->data[0]; }
        MatrixT& x(Ty x) {
            derived()->data[0] = x;
            return *derived();
        }
    };

    template <typename Ty, typename Mat>
    class MatrixT_y_methods {
        using MatrixT = Mat;
        MatrixT* derived() { return static_cast<MatrixT*>(this); }
        const MatrixT* derived() const { return static_cast<const MatrixT*>(this); }
    public:
        Ty y() const { return derived()->data[1]; }
        Ty& y() { return derived()->data[1]; }
        MatrixT& y(Ty y) {
            derived()->data[1] = y;
            return *derived();
        }
    };

    template <typename Ty, typename Mat>
    class MatrixT_z_methods {
        using MatrixT = Mat;
        MatrixT* derived() { return static_cast<MatrixT*>(this); }
        const MatrixT* derived() const { return static_cast<const MatrixT*>(this); }
    public:
        Ty z() const { return derived()->data[2]; }
        Ty& z() { return derived()->data[2]; }
        MatrixT& z(Ty z) {
            derived()->data[2] = z;
            return *derived();
        }
    };

    template <typename Ty, typename Mat>
    class MatrixT_w_methods {
        using MatrixT = Mat;
        MatrixT* derived() { return static_cast<MatrixT*>(this); }
        const MatrixT* derived() const { return static_cast<const MatrixT*>(this); }
    public:
        Ty w() const { return derived()->data[3]; }
        Ty& w() { return derived()->data[3]; }
        MatrixT& w(Ty w) {
            derived()->data[3] = w;
            return *derived();
        }
    };

    template <typename Ty, typename Mat, size_t Num>
    class MatrixT_vec_num_methods :
        public MatrixT_x_methods<Ty, Mat>,
        public MatrixT_y_methods<Ty, Mat>,
        public MatrixT_z_methods<Ty, Mat>,
        public MatrixT_w_methods<Ty, Mat> {};

    template <typename Ty, typename Mat>
    class MatrixT_vec_num_methods<Ty, Mat, 1> :
        public MatrixT_x_methods<Ty, Mat> {};

    template <typename Ty, typename Mat>
    class MatrixT_vec_num_methods<Ty, Mat, 2> :
        public MatrixT_x_methods<Ty, Mat>,
        public MatrixT_y_methods<Ty, Mat> {};

    template <typename Ty, typename Mat>
    class MatrixT_vec_num_methods<Ty, Mat, 3> :
        public MatrixT_x_methods<Ty, Mat>,
        public MatrixT_y_methods<Ty, Mat>,
        public MatrixT_z_methods<Ty, Mat>
    {
        using MatrixT = Mat;
        MatrixT* derived() { return static_cast<MatrixT*>(this); }
        const MatrixT* derived() const { return static_cast<const MatrixT*>(this); }
    public:
        MatrixT operator^(const MatrixT& rhs) const {
            return cross(rhs);
        }

        MatrixT& operator^=(const MatrixT& rhs) {
            *derived() = cross(rhs);
            return *derived();
        }

        MatrixT cross(const MatrixT& rhs) const {
            MatrixT out;
            out.data[0] = derived()->data[1] * rhs.data[2] - derived()->data[2] * rhs.data[1];
            out.data[1] = derived()->data[2] * rhs.data[0] - derived()->data[0] * rhs.data[2];
            out.data[2] = derived()->data[0] * rhs.data[1] - derived()->data[1] * rhs.data[0];
            return out;
        }
    };

    template <typename Ty, typename Mat, size_t Num>
    class MatrixT_vec_base :
        public MatrixT_vec_num_methods<Ty, Mat, Num>
    {
    private:
        using MatrixT = Mat;

    public:
        Ty operator()(size_t index) const {
            assert(index < Num);
            return derived()->data[index];
        }
        Ty& operator()(size_t index) {
            assert(index < Num);
            return derived()->data[index];
        }

        Ty operator*(const MatrixT& rhs) const {
            return dot(rhs);
        }

        MatrixT N() const {
            MatrixT m(*derived());
            m.nor();
            return m;
        }

        MatrixT& set(size_t index, Ty val) {
            assert(index < Num);
            derived()->data[index] = val;
            return *derived();
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Num), MatrixT&>::
            type set(Ty val) {
            derived()->data[Idx] = val;
            return *derived();
        }

        Ty get(size_t index) const {
            assert(index < Num);
            return derived()->data[index];
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Num), Ty>::
            type get() const {
            return derived()->data[Idx];
        }

        Ty at(size_t index) const {
            assert(index < Num);
            return derived()->data[index];
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Num), Ty>::
            type at() const {
            return derived()->data[Idx];
        }

        Ty& at(size_t index) {
            assert(index < Num);
            return derived()->data[index];
        }

        template <size_t Idx>
        typename std::enable_if<(Idx < Num), Ty&>::
            type at() {
            return derived()->data[Idx];
        }

        Ty dot(const MatrixT& rhs) const {
            Ty result = 0;
            for (size_t i = 0; i < Num; ++i) {
                result += derived()->data[i] * rhs.data[i];
            }
            return result;
        }

        MatrixT& nor() {
            derived()->div(length());
            return *derived();
        }

        Ty length() const {
            return std::sqrt(lengsq());
        }

        Ty lengsq() const {
            Ty result = 0;
            for (size_t i = 0; i < Num; ++i) {
                result += derived()->data[i] * derived()->data[i];
            }
            return result;
        }

    private:
        MatrixT* derived() {
            return static_cast<MatrixT*>(this);
        }
        const MatrixT* derived() const {
            return static_cast<const MatrixT*>(this);
        }
    };


    template <typename Ty, size_t Row, size_t Col>
    size_t find_primary(
        const MatrixT<Ty, Row, Col>& m, size_t r, size_t i)
    {
        size_t j;
        for (j = r; j < Row; ++j) {
            if (!utl::is_num_zero(m.data[j * Col + i])) {
                break;
            }
        }
        return j;
    }

    template <typename Ty, size_t Row, size_t Col>
    size_t find_primary_mid(
        const MatrixT<Ty, Row, Col>& m, size_t r, size_t i)
    {
        size_t j = Row;
        Ty cols[Row];
        Ty min_val = (std::numeric_limits<Ty>::max)();
        Ty max_val = 0;
        for (auto k = r; k < Row; ++k) {
            auto val = std::abs(m.data[k * Col + i]);
            if (utl::is_num_zero(val)) {
                val = 0;
            }
            if (val > max_val) {
                max_val = val;
            }
            if (val < min_val) {
                min_val = val;
            }
            cols[k] = val;
        }

        auto mid = (min_val + max_val) / 2;

        Ty tmp_val = (std::numeric_limits<Ty>::max)();
        for (auto k = r; k < Row; ++k) {
            auto val = cols[k];
            if (!utl::is_num_zero(val)) {
                auto diff = mid - val;
                if (diff >= 0 && diff < tmp_val) {
                    tmp_val = diff;
                    j = k;
                }
            }
        }
        return j;
    }

    template <typename Ty, size_t Row, size_t Col>
    size_t find_primary_min(
        const MatrixT<Ty, Row, Col>& m, size_t r, size_t i)
    {
        size_t j = Row;
        Ty min_val = (std::numeric_limits<Ty>::max)();
        for (auto k = r; k < Row; ++k) {
            auto val = std::abs(m.data[k * Col + i]);
            if (val < min_val && !utl::is_num_zero(val)) {
                min_val = val;
                j = k;
            }
        }
        return j;
    }

    template <typename Ty, size_t Row, size_t Col>
    size_t find_primary_max(
        const MatrixT<Ty, Row, Col>& m, size_t r, size_t i)
    {
        size_t j = Row;
        Ty max_val = 0;
        for (auto k = r; k < Row; ++k) {
            auto val = std::abs(m.data[k * Col + i]);
            if (val > max_val && !utl::is_num_zero(val)) {
                max_val = val;
                j = r;
            }
        }
        return j;
    }


    template <typename Ty>
    Ty det_slow(const MatrixT<Ty, 1, 1>& m) {
        return m.data[0];
    }

    template <typename Ty>
    Ty det_slow(const MatrixT<Ty, 2, 2>& m) {
        auto data = m.data;
        return data[0] * data[3] - data[1] * data[2];
    }

    template <typename Ty>
    Ty det_slow(const MatrixT<Ty, 3, 3>& m) {
        auto data = m.data;
        return data[0] * data[4] * data[8]
            + data[1] * data[5] * data[6]
            + data[2] * data[3] * data[7]
            - data[2] * data[4] * data[6]
            - data[1] * data[3] * data[8]
            - data[0] * data[5] * data[7];
    }

    template <typename Ty, size_t Row, size_t Col>
    Ty det_slow(const MatrixT<Ty, Row, Col>& m) {
        auto data = m.data;
        Ty result = 0;
        for (size_t i = 0; i < Col; ++i) {
            auto val = data[i];
            MatrixT<Ty, Row - 1, Col - 1> mt;
            cofactor(m, 0, i, &mt);
            result += val * det_slow(mt) * (int((i + 0) % 2) * -2 + 1);
        }

        return result;
    }

    template <typename Ty>
    Ty det_fast(const MatrixT<Ty, 1, 1>& m) {
        return m.data[0];
    }

    template <typename Ty>
    Ty det_fast(const MatrixT<Ty, 2, 2>& m) {
        auto data = m.data;
        return data[0] * data[3] - data[1] * data[2];
    }

    template <typename Ty>
    Ty det_fast(const MatrixT<Ty, 3, 3>& m) {
        auto data = m.data;
        return data[0] * data[4] * data[8]
            + data[1] * data[5] * data[6]
            + data[2] * data[3] * data[7]
            - data[2] * data[4] * data[6]
            - data[1] * data[3] * data[8]
            - data[0] * data[5] * data[7];
    }

    template <typename Ty, size_t Row, size_t Col>
    Ty det_fast(const MatrixT<Ty, Row, Col>& matrix) {
        size_t r = 0;
        MatrixT m = matrix;

        Ty result = 1;
        for (size_t i = 0; i < Col; ++i) {
            size_t j = find_primary_max(matrix, r, i);
            if (j >= Row) {
                continue;
            }

            if (j != r) {
                m.swapRow(r, j);
                result = -result;
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

        for (size_t i = 0; i < Col; ++i) {
            result *= m.data[i * Col + i];
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
        assert(row_index < Row&& col_index < Col);

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
}

#endif  // UTILS_MATH_ALGEBRA_MATRIX_INTERNAL_HPP_