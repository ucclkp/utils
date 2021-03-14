// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef MATH_ALGEBRA_DYNAMIC_MATRIX_HPP_
#define MATH_ALGEBRA_DYNAMIC_MATRIX_HPP_

#include <cassert>
#include <string>


namespace math {

    template <typename Ty>
    class DMatrixT {
    public:
        static DMatrixT Z(size_t row_count, size_t col_count) {
            DMatrixT m(row_count, col_count);
            m.zero();
            return m;
        }
        static DMatrixT I(size_t size) {
            DMatrixT m(size, size);
            m.identity();
            return m;
        }

        DMatrixT(size_t row_count, size_t col_count)
            : row(row_count), col(col_count),
              data(new Ty[row * col]) {}

        DMatrixT(const DMatrixT& rhs)
            : row(rhs.row), col(rhs.col),
              data(new Ty[row * col])
        {
            std::memcpy(data, rhs.data, sizeof(Ty) * row * col);
        }

        DMatrixT(DMatrixT&& rhs) noexcept
            : row(rhs.row), col(rhs.col),
              data(rhs.data)
        {
            rhs.row = 0;
            rhs.col = 0;
            rhs.data = nullptr;
        }

        ~DMatrixT() {
            delete[] data;
        }

        explicit operator Ty() const {
            assert(row == 1 && col == 1);
            return data[0];
        }

        DMatrixT& operator=(const DMatrixT& rhs) {
            if (&rhs != this) {
                delete[] data;
                row = rhs.row;
                col = rhs.col;
                data = new Ty[row * col];
                std::memcpy(data, rhs.data, sizeof(Ty) * row * col);
            }
            return *this;
        }
        DMatrixT& operator=(DMatrixT&& rhs) noexcept {
            if (&rhs != this) {
                delete[] data;

                row = rhs.row;
                col = rhs.col;
                data = rhs.data;

                rhs.row = 0;
                rhs.col = 0;
                rhs.data = nullptr;
            }
            return *this;
        }
        DMatrixT& operator=(std::initializer_list<Ty> vals) {
            std::copy(vals.begin(), vals.end(), data);
            return *this;
        }

        DMatrixT operator+(const DMatrixT& rhs) const {
            DMatrixT m(*this);
            m.add(rhs);
            return m;
        }
        DMatrixT operator-(const DMatrixT& rhs) const {
            DMatrixT m(*this);
            m.sub(rhs);
            return m;
        }
        DMatrixT operator*(Ty val) const {
            DMatrixT m(*this);
            m.mul(val);
            return m;
        }

        DMatrixT<Ty> operator*(const DMatrixT<Ty>& rhs) const {
            DMatrixT<Ty> m(*this);
            m.mul(rhs);
            return m;
        }

        DMatrixT operator-() const {
            DMatrixT m(*this);
            m.minus();
            return m;
        }

        Ty operator()(size_t index) const {
            assert(index < row && col == 1);
            return data[index];
        }

        Ty& operator()(size_t index) {
            assert(index < row && col == 1);
            return data[index];
        }

        Ty operator()(size_t row_index, size_t col_index) const {
            assert(row_index < row && col_index < col);
            return data[row_index * col + col_index];
        }

        Ty& operator()(size_t row_index, size_t col_index) {
            assert(row_index < row && col_index < col);
            return data[row_index * col + col_index];
        }

        DMatrixT<Ty> T() const {
            DMatrixT<Ty> m(*this);
            m.transpose();
            return m;
        }

        void add(const DMatrixT& rhs) {
            if (rhs.row != row || rhs.col != col) {
                throw std::runtime_error("Illegal rhs!");
            }

            for (size_t i = 0; i < row*col; ++i) {
                data[i] += rhs.data[i];
            }
        }

        void sub(const DMatrixT& rhs) {
            if (rhs.row != row || rhs.col != col) {
                throw std::runtime_error("Illegal rhs!");
            }

            for (size_t i = 0; i < row*col; ++i) {
                data[i] -= rhs.data[i];
            }
        }

        void minus() {
            for (size_t i = 0; i < row*col; ++i) {
                data[i] = -data[i];
            }
        }

        void mul(Ty val) {
            for (size_t i = 0; i < row*col; ++i) {
                data[i] *= val;
            }
        }

        void div(Ty val) {
            for (size_t i = 0; i < row*col; ++i) {
                data[i] /= val;
            }
        }

        void mul(const DMatrixT& rhs) {
            if (rhs.row != col) {
                throw std::runtime_error("Illegal rhs!");
            }

            DMatrixT result(row, rhs.col);
            for (size_t r = 0; r < row; ++r) {
                for (size_t c = 0; c < rhs.col; ++c) {
                    result.data[r * rhs.col + c] = rowMulCol(*this, r, rhs, c);
                }
            }
            *this = std::move(result);
        }

        void zero() {
            std::memset(data, 0, sizeof(Ty) * row * col);
        }

        void identity() {
            if (row != col) {
                throw std::runtime_error("row != col");
            }
            for (size_t i = 0; i < row*col; ++i) {
                data[i] = (i % (row + 1) == 0 ? 1 : 0);
            }
        }

        void transpose() {
            DMatrixT result(col, row);
            for (size_t r = 0; r < row; ++r) {
                for (size_t c = 0; c < col; ++c) {
                    result.data[c * row + r] = data[r * col + c];
                }
            }
            *this = std::move(result);
        }

        void set(size_t index, Ty val) {
            assert(index < row && col == 1);
            data[index] = val;
        }

        void set(size_t row_index, size_t col_index, Ty val) const {
            assert(row_index < row && col_index < col);
            data[row_index * col + col_index] = val;
        }

        Ty get(size_t index) const {
            assert(index < row && col == 1);
            return data[index];
        }

        Ty get(size_t row_index, size_t col_index) const {
            assert(row_index < row && col_index < col);
            return data[row_index * col + col_index];
        }

        size_t getRow() const { return row; }
        size_t getCol() const { return col; }

        size_t row;
        size_t col;
        Ty* data;

    private:
        static Ty rowMulCol(
            const DMatrixT& ml, size_t row_index,
            const DMatrixT& mr, size_t col_index)
        {
            Ty result = 0;
            for (size_t c = 0; c < ml.col; ++c) {
                result += ml.data[row_index*ml.col + c]
                    * mr.data[c*mr.col + col_index];
            }
            return result;
        }
    };

    template <typename Ty>
    DMatrixT<Ty> operator*(Ty val, const DMatrixT<Ty>& rhs) {
        return rhs * val;
    }

}

#endif  // MATH_ALGEBRA_DYNAMIC_MATRIX_HPP_