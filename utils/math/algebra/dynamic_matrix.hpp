// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef MATH_ALGEBRA_DYNAMIC_MATRIX_HPP_
#define MATH_ALGEBRA_DYNAMIC_MATRIX_HPP_

#include <cassert>
#include <stdexcept>
#include <string>

#include "utils/number.hpp"


namespace math {

    template <typename Ty>
    class DMatrixT;

namespace internal {
    template <typename Ty>
    Ty detTrans(const DMatrixT<Ty>& m);

    template <typename Ty>
    void cofactor(
        const DMatrixT<Ty>& m,
        size_t row_index, size_t col_index,
        DMatrixT<Ty>* out);
}

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
              data(new Ty[row * col])
        {
            if (row == 0 || col == 0) {
                throw std::runtime_error("Illegal params!");
            }
        }

        DMatrixT(
            size_t row_count, size_t col_count,
            const std::initializer_list<Ty>& vals)
            : row(row_count), col(col_count),
              data(new Ty[row * col])
        {
            if (row == 0 || col == 0) {
                throw std::runtime_error("Illegal params!");
            }
            std::copy(vals.begin(), vals.end(), data);
        }

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

        bool operator==(const DMatrixT& rhs) const {
            return equal(rhs);
        }

        bool operator!=(const DMatrixT& rhs) const {
            return !equal(rhs);
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
                throw std::runtime_error("Row must be equal to Col!");
            }

            DMatrixT result(row, rhs.col);
            for (size_t r = 0; r < row; ++r) {
                for (size_t c = 0; c < rhs.col; ++c) {
                    result.data[r * rhs.col + c] = rowMulCol(*this, r, rhs, c);
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

        void zero() {
            std::memset(data, 0, sizeof(Ty) * row * col);
        }

        void identity() {
            if (row != col) {
                throw std::runtime_error("Row must be equal to Col!");
            }
            for (size_t i = 0; i < row; ++i) {
                std::memset(data + i * col, 0, sizeof(Ty) * col);
                data[i * col + i] = 1;
            }
        }

        void transpose() {
            transpose(this);
        }

        void transpose(DMatrixT* out) const {
            DMatrixT result(col, row);
            for (size_t r = 0; r < row; ++r) {
                for (size_t c = 0; c < col; ++c) {
                    result.data[c * row + r] = data[r * col + c];
                }
            }
            *out = std::move(result);
        }

        bool equal(const DMatrixT& rhs) const {
            for (size_t i = 0; i < row * col; ++i) {
                if (!utl::is_num_equal(data[i], rhs.data[i])) {
                    return false;
                }
            }
            return true;
        }

        Ty det() const {
            if (col != row) {
                throw std::runtime_error("Row must be equal to Col!");
            }
            return internal::detTrans(*this);
        }

        void cofactor(
            size_t row_index, size_t col_index, DMatrixT<Ty>* out) const
        {
            if (col != row) {
                throw std::runtime_error("Row must be equal to Col!");
            }
            internal::cofactor(*this, row_index, col_index, out);
        }

        void adjoint(DMatrixT* out) const {
            DMatrixT adj(row, col);
            for (size_t i = 0; i < row; ++i) {
                for (size_t j = 0; j < col; ++j) {
                    DMatrixT<Ty> mt(row - 1, col - 1);
                    internal::cofactor(*this, i, j, &mt);
                    adj.data[i * col + j] = internal::detTrans(mt) * (int((i + j) % 2) * -2 + 1);
                }
            }
            adj.transpose(out);
        }

        bool inverse(DMatrixT* out) const {
            if (col != row) {
                throw std::runtime_error("Row must be equal to Col!");
            }

            if (col == 1) {
                if (utl::is_num_zero(data[0])) {
                    return false;
                }
                out->data[0] = 1 / data[0];
                return true;
            }

            Ty detr = 0;
            DMatrixT adjoint(row, col);
            auto& od = adjoint.data;
            for (size_t i = 0; i < col; ++i) {
                auto val = data[i];
                DMatrixT<Ty> mt(row - 1, col - 1);
                internal::cofactor(*this, 0, i, &mt);
                od[i] = internal::detTrans(mt) * (int((i + 0) % 2) * -2 + 1);
                detr += val * od[i];
            }

            if (utl::is_num_zero(detr)) {
                return false;
            }

            for (size_t i = 1; i < row; ++i) {
                for (size_t j = 0; j < col; ++j) {
                    DMatrixT<Ty> mt(row - 1, col - 1);
                    internal::cofactor(*this, i, j, &mt);
                    od[i * col + j] = internal::detTrans(mt) * (int((i + j) % 2) * -2 + 1);
                }
            }

            adjoint.transpose(out);
            out->mul(1 / detr);
            return true;
        }

        void swapCol(size_t c1_index, size_t c2_index) {
            assert(c1_index < col && c2_index < col);
            if (c1_index == c2_index) {
                return;
            }

            for (size_t i = 0; i < row; ++i) {
                Ty tmp = data[i * col + c1_index];
                data[i * col + c1_index] = data[i * col + c2_index];
                data[i * col + c2_index] = tmp;
            }
        }

        void swapRow(size_t r1_index, size_t r2_index) {
            assert(r1_index < row && r2_index < row);
            if (r1_index == r2_index) {
                return;
            }

            auto tmp = new Ty[col];
            std::memcpy(tmp, &data[r1_index * col], sizeof(Ty) * col);
            std::memcpy(&data[r1_index * col], &data[r2_index * col], sizeof(Ty) * col);
            std::memcpy(&data[r2_index * col], tmp, sizeof(Ty) * col);
            delete[] tmp;
        }

        void ref(DMatrixT* out) const {
            size_t r = 0;
            DMatrixT m = *this;

            for (size_t i = 0; i < col; ++i) {
                size_t j;
                for (j = r; j < row; ++j) {
                    if (!utl::is_num_zero(m.data[j * col + i])) {
                        break;
                    }
                }

                if (j >= row) {
                    continue;
                }

                if (j != r) {
                    m.swapRow(r, j);
                }

                auto val = m.data[r * col + i];
                for (j = r + 1; j < row; ++j) {
                    auto& p = m.data[j * col + i];
                    if (utl::is_num_zero(p)) {
                        continue;
                    }

                    auto f = -p / val;
                    p = 0;
                    for (size_t k = i + 1; k < col; ++k) {
                        m.data[j * col + k] += m.data[r * col + k] * f;
                    }
                }
                ++r;
            }
            *out = m;
        }

        void rref(DMatrixT* out) const {
            size_t r = 0;
            DMatrixT m = *this;
            auto primary = new size_t[row];
            std::memset(primary, 0xFF, sizeof(size_t) * row);

            for (size_t i = 0; i < col; ++i) {
                size_t j;
                for (j = r; j < row; ++j) {
                    if (!utl::is_num_zero(m.data[j * col + i])) {
                        break;
                    }
                }

                if (j >= row) {
                    continue;
                }

                if (j != r) {
                    m.swapRow(r, j);
                }

                primary[r] = i;

                auto& val = m.data[r * col + i];
                for (j = r + 1; j < row; ++j) {
                    auto& p = m.data[j * col + i];
                    if (utl::is_num_zero(p)) {
                        continue;
                    }

                    auto f = -p / val;
                    p = 0;
                    for (size_t k = i + 1; k < col; ++k) {
                        m.data[j * col + k] += m.data[r * col + k] * f;
                    }
                }

                for (size_t k = i + 1; k < col; ++k) {
                    m.data[i * col + k] *= 1 / val;
                }
                val = 1;
                ++r;
            }

            for (size_t i = row; i-- > 1;) {
                size_t j = primary[i];
                if (j == size_t(-1)) {
                    continue;
                }

                auto val = m.data[i * col + j];
                for (size_t k = i; k-- > 0;) {
                    auto& p = m.data[k * col + j];
                    if (utl::is_num_zero(p)) {
                        continue;
                    }

                    auto f = -p / val;
                    p = 0;
                    for (size_t n = j + 1; n < col; ++n) {
                        m.data[k * col + n] += m.data[i * col + n] * f;
                    }
                }
            }

            delete[] primary;
            *out = m;
        }

        bool inverse2(DMatrixT* out) const {
            if (col != row) {
                throw std::runtime_error("Row must be equal to Col!");
            }

            DMatrixT<Ty> m(row, col * 2);

            // [M] -> [M, I]
            for (size_t i = 0; i < row; ++i) {
                std::memcpy(
                    m.data + i * col * 2,
                    data + i * col, sizeof(Ty) * col);
                std::memset(m.data + i * col * 2 + col, 0, sizeof(Ty) * col);
                m.data[i * col * 2 + col + i] = 1;
            }

            // [M, I] -> [I', M^]
            m.rref(&m);

            // Check [I'] == [I]
            for (size_t i = 0; i < row * col; ++i) {
                auto r = i / col;
                auto c = i % col;
                if (!utl::is_num_equal(
                    m.data[r * col * 2 + c],
                    (r == c) ? Ty(1) : Ty(0)))
                {
                    return false;
                }
            }

            // Copy [M^]
            for (size_t i = 0; i < row; ++i) {
                std::memcpy(
                    out->data + i * col,
                    m.data + i * col * 2 + col, sizeof(Ty) * col);
            }
            return true;
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


namespace internal {

    template <typename Ty>
    Ty detTrans(const DMatrixT<Ty>& m) {
        if (m.col == 1) {
            return m.data[0];
        }
        if (m.col == 2) {
            auto data = m.data;
            return data[0] * data[3] - data[1] * data[2];
        }
        if (m.col == 3) {
            auto data = m.data;
            return data[0] * data[4] * data[8]
                + data[1] * data[5] * data[6]
                + data[2] * data[3] * data[7]
                - data[2] * data[4] * data[6]
                - data[1] * data[3] * data[8]
                - data[0] * data[5] * data[7];
        }

        auto data = m.data;
        Ty result = 0;
        for (size_t i = 0; i < m.col; ++i) {
            auto val = data[i];
            DMatrixT<Ty> mt(m.row - 1, m.col - 1);
            cofactor(m, 0, i, &mt);
            result += val * detTrans(mt) * (int((i + 0) % 2) * -2 + 1);
        }

        return result;
    }

    template <typename Ty>
    void cofactor(
        const DMatrixT<Ty>& m,
        size_t row_index, size_t col_index,
        DMatrixT<Ty>* out)
    {
        if (m.col == 1) {
            assert(row_index < 1 && col_index < 1);
            out->data[0] = 1;
            return;
        }
        if (m.col == 2) {
            assert(row_index < 2 && col_index < 2);

            auto r = (row_index + 1) % 2;
            auto c = (col_index + 1) % 2;
            out->data[0] = m.data[r * 2 + c];
            return;
        }
        if (m.col == 3) {
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

        assert(row_index < m.row && col_index < m.col);

        size_t i = 0;
        auto data = m.data;
        if (col_index > 0 && col_index < m.col - 1) {
            for (size_t j = 0; j < m.row; ++j) {
                if (j == row_index) {
                    continue;
                }
                std::memcpy(
                    out->data + (m.col - 1) * i,
                    data + m.col * j,
                    col_index * sizeof(Ty));
                std::memcpy(
                    out->data + (m.col - 1) * i + col_index,
                    data + m.col * j + (col_index + 1),
                    (m.col - 1 - col_index) * sizeof(Ty));
                ++i;
            }
        } else if (col_index > 0 && col_index >= m.col - 1) {
            for (size_t j = 0; j < m.row; ++j) {
                if (j == row_index) {
                    continue;
                }
                std::memcpy(
                    out->data + (m.col - 1) * i,
                    data + m.col * j,
                    col_index * sizeof(Ty));
                ++i;
            }
        } else if (col_index <= 0 && col_index < m.col - 1) {
            for (size_t j = 0; j < m.row; ++j) {
                if (j == row_index) {
                    continue;
                }
                std::memcpy(
                    out->data + (m.col - 1) * i + col_index,
                    data + m.col * j + (col_index + 1),
                    (m.col - 1 - col_index) * sizeof(Ty));
                ++i;
            }
        }
    }

}
}

#endif  // MATH_ALGEBRA_DYNAMIC_MATRIX_HPP_