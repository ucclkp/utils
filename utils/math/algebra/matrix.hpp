// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef MATH_ALGEBRA_MATRIX_HPP_
#define MATH_ALGEBRA_MATRIX_HPP_

#include <cassert>
#include <string>
#include <type_traits>

#include "utils/type_utils.hpp"


namespace math {

    template <typename Ty, size_t Row, size_t Col>
    class MatrixT {
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

        MatrixT() {}
        MatrixT(std::initializer_list<Ty> vals) {
            std::copy(vals.begin(), vals.end(), data);
        }
        MatrixT(const MatrixT& rhs) {
            std::memcpy(data, rhs.data, Row * Col * sizeof(Ty));
        }

        explicit operator Ty() const {
            if constexpr (Row != 1 || Col != 1) {
                static_assert(utl::sat_stub<Ty>::value, "Row and Col must be equal to 1!");
            }
            return data[0];
        }

        MatrixT& operator=(const MatrixT& rhs) {
            if (&rhs != this) {
                std::memcpy(data, rhs.data, Row * Col * sizeof(Ty));
            }
            return *this;
        }

        MatrixT& operator=(std::initializer_list<Ty> vals) {
            std::copy(vals.begin(), vals.end(), data);
            return *this;
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
            MatrixT<Ty, Row, RCol> m;
            mul(rhs, &m);
            return m;
        }

        MatrixT operator-() const {
            MatrixT m(*this);
            m.minus();
            return m;
        }

        Ty operator()(size_t index) const {
            if constexpr (Col != 1) {
                static_assert(utl::sat_stub<Ty>::value, "Col must be equal to 1!");
            }
            assert(index < Row);
            return data[index];
        }

        Ty& operator()(size_t index) {
            if constexpr (Col != 1) {
                static_assert(utl::sat_stub<Ty>::value, "Col must be equal to 1!");
            }
            assert(index < Row);
            return data[index];
        }

        Ty operator()(size_t row_index, size_t col_index) const {
            assert(row_index < Row && col_index < Col);
            return data[row_index * Col + col_index];
        }

        Ty& operator()(size_t row_index, size_t col_index) {
            assert(row_index < Row && col_index < Col);
            return data[row_index * Col + col_index];
        }

        MatrixT<Ty, Col, Row> T() const {
            MatrixT<Ty, Col, Row> m;
            transpose(&m);
            return m;
        }

        void add(const MatrixT& rhs) {
            for (size_t i = 0; i < Row*Col; ++i) {
                data[i] += rhs.data[i];
            }
        }

        void sub(const MatrixT& rhs) {
            for (size_t i = 0; i < Row*Col; ++i) {
                data[i] -= rhs.data[i];
            }
        }

        void minus() {
            for (size_t i = 0; i < Row*Col; ++i) {
                data[i] = -data[i];
            }
        }

        void mul(Ty val) {
            for (size_t i = 0; i < Row*Col; ++i) {
                data[i] *= val;
            }
        }

        void div(Ty val) {
            for (size_t i = 0; i < Row*Col; ++i) {
                data[i] /= val;
            }
        }

        template <size_t RCol>
        void mul(const MatrixT<Ty, Col, RCol>& rhs, MatrixT<Ty, Row, RCol>* out) const {
            for (size_t r = 0; r < Row; ++r) {
                for (size_t c = 0; c < RCol; ++c) {
                    out->data[r * RCol + c] = rowMulCol(*this, r, rhs, c);
                }
            }
        }

        void zero() {
            std::memset(data, 0, sizeof(Ty) * Row * Col);
        }

        void identity() {
            if constexpr (Row == Col) {
                for (size_t i = 0; i < Row*Col; ++i) {
                    data[i] = (i % (Row + 1) == 0 ? 1 : 0);
                }
            } else {
                static_assert(utl::sat_stub<Ty>::value, "Row must be equal to Col!");
            }
        }

        void transpose(MatrixT<Ty, Col, Row>* out) const {
            for (size_t r = 0; r < Row; ++r) {
                for (size_t c = 0; c < Col; ++c) {
                    out->data[c * Row + r] = data[r * Col + c];
                }
            }
        }

        void set(size_t index, Ty val) {
            if constexpr (Col == 1) {
                assert(index < Row);
                data[index] = val;
            } else {
                static_assert(utl::sat_stub<Ty>::value, "Col must be equal to 1!");
            }
        }

        void set(size_t row_index, size_t col_index, Ty val) {
            assert(row_index < Row && col_index < Col);
            data[row_index * Col + col_index] = val;
        }

        Ty get(size_t index) const {
            if constexpr (Col != 1) {
                static_assert(utl::sat_stub<Ty>::value, "Col must be equal to 1!");
            }
            assert(index < Row);
            return data[index];
        }

        Ty get(size_t row_index, size_t col_index) const {
            assert(row_index < Row && col_index < Col);
            return data[row_index * Col + col_index];
        }

        size_t getRow() const { return Row; }
        size_t getCol() const { return Col; }

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

    template <typename Ty, size_t Row, size_t Col>
    MatrixT<Ty, Row, Col> operator*(Ty val, const MatrixT<Ty, Row, Col>& rhs) {
        return rhs * val;
    }

    template <typename Ty, size_t Row>
    using VectorT = MatrixT<Ty, Row, 1>;

}

#endif  // MATH_ALGEBRA_MATRIX_HPP_
