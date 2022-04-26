// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MATH_ALGEBRA_DYNAMIC_MATRIX_HPP_
#define UTILS_MATH_ALGEBRA_DYNAMIC_MATRIX_HPP_

#include <cassert>
#include <stdexcept>
#include <string>

#include "utils/numbers.hpp"


namespace utl {
namespace math {

    template <typename Ty>
    class DMatrixT;

namespace internal {
    template <typename Ty>
    Ty det_slow(const DMatrixT<Ty>& m);

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
            : row_sz(row_count), col_sz(col_count),
              data(new Ty[row_sz * col_sz])
        {
            if (row_sz == 0 || col_sz == 0) {
                throw std::runtime_error("Illegal params!");
            }
        }

        DMatrixT(
            size_t row_count, size_t col_count,
            const std::initializer_list<Ty>& vals)
            : row_sz(row_count), col_sz(col_count),
              data(new Ty[row_sz * col_sz])
        {
            if (row_sz == 0 || col_sz == 0) {
                throw std::runtime_error("Illegal params!");
            }
            if (vals.size() > row_sz * col_sz) {
                throw std::runtime_error("Too many params!");
            }
            std::copy(vals.begin(), vals.end(), data);
        }

        DMatrixT(const DMatrixT& rhs)
            : row_sz(rhs.row_sz), col_sz(rhs.col_sz),
              data(new Ty[row_sz * col_sz])
        {
            std::memcpy(data, rhs.data, sizeof(Ty) * row_sz * col_sz);
        }

        DMatrixT(DMatrixT&& rhs) noexcept
            : row_sz(rhs.row_sz), col_sz(rhs.col_sz),
              data(rhs.data)
        {
            rhs.row_sz = 0;
            rhs.col_sz = 0;
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
            assert(row_sz == 1 && col_sz == 1);
            return data[0];
        }

        DMatrixT& operator=(const DMatrixT& rhs) {
            if (&rhs != this) {
                delete[] data;
                row_sz = rhs.row_sz;
                col_sz = rhs.col_sz;
                data = new Ty[row_sz * col_sz];
                std::memcpy(data, rhs.data, sizeof(Ty) * row_sz * col_sz);
            }
            return *this;
        }
        DMatrixT& operator=(DMatrixT&& rhs) noexcept {
            if (&rhs != this) {
                delete[] data;

                row_sz = rhs.row_sz;
                col_sz = rhs.col_sz;
                data = rhs.data;

                rhs.row_sz = 0;
                rhs.col_sz = 0;
                rhs.data = nullptr;
            }
            return *this;
        }
        DMatrixT& operator=(const std::initializer_list<Ty>& vals) {
            if (vals.size() > row_sz * col_sz) {
                throw std::runtime_error("Too many params!");
            }
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
        DMatrixT operator*(const DMatrixT& rhs) const {
            return mul(rhs);
        }
        DMatrixT operator/(Ty val) const {
            DMatrixT m(*this);
            m.div(val);
            return m;
        }
        DMatrixT operator-() const {
            DMatrixT m(*this);
            m.minus();
            return m;
        }
        DMatrixT operator^(const DMatrixT& rhs) const {
            return cross(rhs);
        }

        DMatrixT& operator+=(const DMatrixT& rhs) {
            return add(rhs);
        }
        DMatrixT& operator-=(const DMatrixT& rhs) {
            return sub(rhs);
        }
        DMatrixT& operator*=(Ty val) {
            return mul(val);
        }
        DMatrixT& operator/=(Ty val) {
            return div(val);
        }
        DMatrixT& operator^=(const DMatrixT& rhs) {
            *this = cross(rhs);
            return *this;
        }

        template<typename Cy>
        explicit operator DMatrixT<Cy>() const {
            DMatrixT<Cy> out(row_sz, col_sz);
            for (size_t i = 0; i < row_sz * col_sz; ++i) {
                out.data[i] = static_cast<Cy>(data[i]);
            }
            return out;
        }

        Ty operator()(size_t index) const {
            assert((index < row_sz && col_sz == 1) || (index < col_sz && row_sz == 1));
            return data[index];
        }
        Ty& operator()(size_t index) {
            assert((index < row_sz && col_sz == 1) || (index < col_sz && row_sz == 1));
            return data[index];
        }
        Ty operator()(size_t row_index, size_t col_index) const {
            assert(row_index < row_sz && col_index < col_sz);
            return data[row_index * col_sz + col_index];
        }
        Ty& operator()(size_t row_index, size_t col_index) {
            assert(row_index < row_sz && col_index < col_sz);
            return data[row_index * col_sz + col_index];
        }

        DMatrixT row(size_t index) const {
            assert(index < row_sz);
            DMatrixT out(1, col_sz);
            for (size_t i = 0; i < col_sz; ++i) {
                out.data[i] = data[index * col_sz + i];
            }
            return out;
        }
        DMatrixT col(size_t index) const {
            assert(index < col_sz);
            DMatrixT out(row_sz, 1);
            for (size_t i = 0; i < row_sz; ++i) {
                out.data[i] = data[i * col_sz + index];
            }
            return out;
        }

        DMatrixT reduce(size_t re_row, size_t re_col) const {
            if (re_row > row_sz || re_col > col_sz) {
                throw std::runtime_error(
                    "ReRow must be less than Row, and ReCol must be less than Col!");
            }

            DMatrixT out(re_row, re_col);
            for (size_t r = 0; r < re_row; ++r) {
                for (size_t c = 0; c < re_col; ++c) {
                    out.data[r * re_col + c] = data[r * col_sz + c];
                }
            }
            return out;
        }

        DMatrixT T() const {
            return transpose();
        }

        DMatrixT& add(const DMatrixT& rhs) {
            if (rhs.row_sz != row_sz || rhs.col_sz != col_sz) {
                throw std::runtime_error("Illegal rhs!");
            }

            for (size_t i = 0; i < row_sz*col_sz; ++i) {
                data[i] += rhs.data[i];
            }
            return *this;
        }

        DMatrixT& sub(const DMatrixT& rhs) {
            if (rhs.row_sz != row_sz || rhs.col_sz != col_sz) {
                throw std::runtime_error("Illegal rhs!");
            }

            for (size_t i = 0; i < row_sz*col_sz; ++i) {
                data[i] -= rhs.data[i];
            }
            return *this;
        }

        DMatrixT& minus() {
            for (size_t i = 0; i < row_sz*col_sz; ++i) {
                data[i] = -data[i];
            }
            return *this;
        }

        DMatrixT& mul(Ty val) {
            for (size_t i = 0; i < row_sz*col_sz; ++i) {
                data[i] *= val;
            }
            return *this;
        }

        DMatrixT& div(Ty val) {
            for (size_t i = 0; i < row_sz*col_sz; ++i) {
                data[i] /= val;
            }
            return *this;
        }

        Ty dot(const DMatrixT& rhs) const {
            if (col_sz != 1 && row_sz != 1) {
                throw std::runtime_error("dot() must be invoked on vector!");
            }
            if (col_sz != rhs.col_sz || row_sz != rhs.row_sz) {
                throw std::runtime_error("Dimensions of *this and rhs of dot() must be equal!");
            }

            Ty result = 0;
            for (size_t i = 0; i < row_sz * col_sz; ++i) {
                result += data[i] * rhs.data[i];
            }
            return result;
        }

        DMatrixT cross(const DMatrixT& rhs) const {
            if (col_sz != 1 && row_sz != 1) {
                throw std::runtime_error("cross() must be invoked on vector!");
            }
            if (col_sz != rhs.col_sz || row_sz != rhs.row_sz) {
                throw std::runtime_error("Dimensions of *this and rhs of cross() must be equal!");
            }
            if (row_sz != 3 && col_sz != 3) {
                throw std::runtime_error("undefined cross operation!");
            }

            DMatrixT out(row_sz, col_sz);
            out.data[0] = data[1] * rhs.data[2] - data[2] * rhs.data[1];
            out.data[1] = data[2] * rhs.data[0] - data[0] * rhs.data[2];
            out.data[2] = data[0] * rhs.data[1] - data[1] * rhs.data[0];
            return out;
        }

        DMatrixT mul(const DMatrixT& rhs) const {
            if (rhs.row_sz != col_sz) {
                throw std::runtime_error("Row must be equal to Col!");
            }

            DMatrixT result(row_sz, rhs.col_sz);
            for (size_t r = 0; r < row_sz; ++r) {
                for (size_t c = 0; c < rhs.col_sz; ++c) {
                    result.data[r * rhs.col_sz + c] = rowMulCol(*this, r, rhs, c);
                }
            }
            return result;
        }

        DMatrixT& add_row(size_t index, Ty val) {
            assert(index < row_sz);
            for (size_t i = 0; i < col_sz; ++i) {
                data[index * col_sz + i] += val;
            }
            return *this;
        }

        DMatrixT& add_row(size_t index, const DMatrixT& m) {
            assert(index < row_sz && m.col_sz == col_sz && m.row_sz == 1);
            for (size_t i = 0; i < col_sz; ++i) {
                data[index * col_sz + i] += m(i);
            }
            return *this;
        }

        DMatrixT& sub_row(size_t index, Ty val) {
            assert(index < row_sz);
            for (size_t i = 0; i < col_sz; ++i) {
                data[index * col_sz + i] -= val;
            }
            return *this;
        }

        DMatrixT& sub_row(size_t index, const DMatrixT& m) {
            assert(index < row_sz && m.col_sz == col_sz && m.row_sz == 1);
            for (size_t i = 0; i < col_sz; ++i) {
                data[index * col_sz + i] -= m(i);
            }
            return *this;
        }

        DMatrixT& mul_row(size_t index, Ty val) {
            assert(index < row_sz);
            for (size_t i = 0; i < col_sz; ++i) {
                data[index * col_sz + i] *= val;
            }
            return *this;
        }

        DMatrixT& mul_row(size_t index, const DMatrixT& m) {
            assert(index < row_sz && m.col_sz == col_sz && m.row_sz == 1);
            for (size_t i = 0; i < col_sz; ++i) {
                data[index * col_sz + i] *= m(i);
            }
            return *this;
        }

        DMatrixT& div_row(size_t index, Ty val) {
            assert(index < row_sz);
            for (size_t i = 0; i < col_sz; ++i) {
                data[index * col_sz + i] /= val;
            }
            return *this;
        }

        DMatrixT& div_row(size_t index, const DMatrixT& m) {
            assert(index < row_sz && m.col_sz == col_sz && m.row_sz == 1);
            for (size_t i = 0; i < col_sz; ++i) {
                data[index * col_sz + i] /= m(i);
            }
            return *this;
        }

        DMatrixT& add_col(size_t index, Ty val) {
            assert(index < col_sz);
            for (size_t i = 0; i < row_sz; ++i) {
                data[i * col_sz + index] += val;
            }
            return *this;
        }

        DMatrixT& add_col(size_t index, const DMatrixT& m) {
            assert(index < col_sz && m.row_sz == row_sz && m.col_sz == 1);
            for (size_t i = 0; i < row_sz; ++i) {
                data[i * col_sz + index] += m(i);
            }
            return *this;
        }

        DMatrixT& sub_col(size_t index, Ty val) {
            assert(index < col_sz);
            for (size_t i = 0; i < row_sz; ++i) {
                data[i * col_sz + index] -= val;
            }
            return *this;
        }

        DMatrixT& sub_col(size_t index, const DMatrixT& m) {
            assert(index < col_sz && m.row_sz == row_sz && m.col_sz == 1);
            for (size_t i = 0; i < row_sz; ++i) {
                data[i * col_sz + index] -= m(i);
            }
            return *this;
        }

        DMatrixT& mul_col(size_t index, Ty val) {
            assert(index < col_sz);
            for (size_t i = 0; i < row_sz; ++i) {
                data[i * col_sz + index] *= val;
            }
            return *this;
        }

        DMatrixT& mul_col(size_t index, const DMatrixT& m) {
            assert(index < col_sz && m.row_sz == row_sz && m.col_sz == 1);
            for (size_t i = 0; i < row_sz; ++i) {
                data[i * col_sz + index] *= m(i);
            }
            return *this;
        }

        DMatrixT& div_col(size_t index, Ty val) {
            assert(index < col_sz);
            for (size_t i = 0; i < row_sz; ++i) {
                data[i * col_sz + index] /= val;
            }
            return *this;
        }

        DMatrixT& div_col(size_t index, const DMatrixT& m) {
            assert(index < col_sz && m.row_sz == row_sz && m.col_sz == 1);
            for (size_t i = 0; i < row_sz; ++i) {
                data[i * col_sz + index] /= m(i);
            }
            return *this;
        }

        DMatrixT& set(size_t index, Ty val) {
            assert((index < row_sz && col_sz == 1) || (index < col_sz && row_sz == 1));
            data[index] = val;
            return *this;
        }

        DMatrixT& set(size_t row_index, size_t col_index, Ty val) {
            assert(row_index < row_sz && col_index < col_sz);
            data[row_index * col_sz + col_index] = val;
            return *this;
        }

        Ty get(size_t index) const {
            assert((index < row_sz && col_sz == 1) || (index < col_sz && row_sz == 1));
            return data[index];
        }

        Ty get(size_t row_index, size_t col_index) const {
            assert(row_index < row_sz && col_index < col_sz);
            return data[row_index * col_sz + col_index];
        }

        Ty& get(size_t index) {
            assert((index < row_sz&& col_sz == 1) || (index < col_sz&& row_sz == 1));
            return data[index];
        }

        Ty& get(size_t row_index, size_t col_index) {
            assert(row_index < row_sz&& col_index < col_sz);
            return data[row_index * col_sz + col_index];
        }

        DMatrixT& zero() {
            for (size_t i = 0; i < row_sz * col_sz; ++i) {
                data[i] = 0;
            }
            return *this;
        }

        DMatrixT& identity() {
            if (row_sz != col_sz) {
                throw std::runtime_error("Row must be equal to Col!");
            }
            for (size_t i = 0; i < row_sz; ++i) {
                for (size_t j = 0; j < col_sz; ++j) {
                    data[i * col_sz + j] = (i == j) ? Ty(1) : Ty(0);
                }
            }
            return *this;
        }

        DMatrixT transpose() const {
            DMatrixT result(col_sz, row_sz);
            for (size_t r = 0; r < row_sz; ++r) {
                for (size_t c = 0; c < col_sz; ++c) {
                    result.data[c * row_sz + r] = data[r * col_sz + c];
                }
            }
            return result;
        }

        bool equal(const DMatrixT& rhs) const {
            if (col_sz != rhs.col_sz || row_sz != rhs.row_sz) {
                return false;
            }
            for (size_t i = 0; i < row_sz * col_sz; ++i) {
                if (!utl::is_num_equal(data[i], rhs.data[i])) {
                    return false;
                }
            }
            return true;
        }

        Ty det() const {
            if (col_sz != row_sz) {
                throw std::runtime_error("Row must be equal to Col!");
            }
            return internal::det_slow(*this);
        }

        DMatrixT cofactor(
            size_t row_index, size_t col_index) const
        {
            if (col_sz != row_sz) {
                throw std::runtime_error("Row must be equal to Col!");
            }
            DMatrixT out(row_sz > 1 ? (row_sz - 1) : 1, col_sz > 1 ? (col_sz - 1) : 1);
            internal::cofactor(*this, row_index, col_index, &out);
            return out;
        }

        DMatrixT adjoint() const {
            if (col_sz != row_sz) {
                throw std::runtime_error("Row must be equal to Col!");
            }
            if (row_sz == 1) {
                return DMatrixT(1, 1, { 1 });
            }

            DMatrixT adj(row_sz, col_sz);
            for (size_t i = 0; i < row_sz; ++i) {
                for (size_t j = 0; j < col_sz; ++j) {
                    DMatrixT<Ty> mt(row_sz - 1, col_sz - 1);
                    internal::cofactor(*this, i, j, &mt);
                    adj.data[i * col_sz + j] = internal::det_slow(mt) * (int((i + j) % 2) * -2 + 1);
                }
            }
            return adj.transpose();
        }

        DMatrixT& swapCol(size_t c1_index, size_t c2_index) {
            assert(c1_index < col_sz && c2_index < col_sz);
            if (c1_index == c2_index) {
                return *this;
            }

            for (size_t i = 0; i < row_sz; ++i) {
                Ty tmp = data[i * col_sz + c1_index];
                data[i * col_sz + c1_index] = data[i * col_sz + c2_index];
                data[i * col_sz + c2_index] = tmp;
            }
            return *this;
        }

        DMatrixT& swapRow(size_t r1_index, size_t r2_index) {
            assert(r1_index < row_sz && r2_index < row_sz);
            if (r1_index == r2_index) {
                return *this;
            }

            auto tmp = new Ty[col_sz];
            std::memcpy(tmp, &data[r1_index * col_sz], sizeof(Ty) * col_sz);
            std::memcpy(&data[r1_index * col_sz], &data[r2_index * col_sz], sizeof(Ty) * col_sz);
            std::memcpy(&data[r2_index * col_sz], tmp, sizeof(Ty) * col_sz);
            delete[] tmp;
            return *this;
        }

        DMatrixT ref() const {
            size_t r = 0;
            DMatrixT m = *this;

            for (size_t i = 0; i < col_sz; ++i) {
                size_t j;
                for (j = r; j < row_sz; ++j) {
                    if (!utl::is_num_zero(m.data[j * col_sz + i])) {
                        break;
                    }
                }

                if (j >= row_sz) {
                    continue;
                }

                if (j != r) {
                    m.swapRow(r, j);
                }

                auto val = m.data[r * col_sz + i];
                for (j = r + 1; j < row_sz; ++j) {
                    auto& p = m.data[j * col_sz + i];
                    if (utl::is_num_zero(p)) {
                        continue;
                    }

                    auto f = -p / val;
                    p = 0;
                    for (size_t k = i + 1; k < col_sz; ++k) {
                        m.data[j * col_sz + k] += m.data[r * col_sz + k] * f;
                    }
                }
                ++r;
            }
            return m;
        }

        DMatrixT rref() const {
            size_t r = 0;
            DMatrixT m = *this;
            auto primary = new size_t[row_sz];
            std::memset(primary, 0xFF, sizeof(size_t) * row_sz);

            for (size_t i = 0; i < col_sz; ++i) {
                size_t j;
                for (j = r; j < row_sz; ++j) {
                    if (!utl::is_num_zero(m.data[j * col_sz + i])) {
                        break;
                    }
                }

                if (j >= row_sz) {
                    continue;
                }

                if (j != r) {
                    m.swapRow(r, j);
                }

                primary[r] = i;

                auto& val = m.data[r * col_sz + i];
                for (j = r + 1; j < row_sz; ++j) {
                    auto& p = m.data[j * col_sz + i];
                    if (utl::is_num_zero(p)) {
                        continue;
                    }

                    auto f = -p / val;
                    p = 0;
                    for (size_t k = i + 1; k < col_sz; ++k) {
                        m.data[j * col_sz + k] += m.data[r * col_sz + k] * f;
                    }
                }

                for (size_t k = i + 1; k < col_sz; ++k) {
                    m.data[i * col_sz + k] *= 1 / val;
                }
                val = 1;
                ++r;
            }

            for (size_t i = row_sz; i-- > 1;) {
                size_t j = primary[i];
                if (j == size_t(-1)) {
                    continue;
                }

                auto val = m.data[i * col_sz + j];
                for (size_t k = i; k-- > 0;) {
                    auto& p = m.data[k * col_sz + j];
                    if (utl::is_num_zero(p)) {
                        continue;
                    }

                    auto f = -p / val;
                    p = 0;
                    for (size_t n = j + 1; n < col_sz; ++n) {
                        m.data[k * col_sz + n] += m.data[i * col_sz + n] * f;
                    }
                }
            }

            delete[] primary;
            return m;
        }

        DMatrixT& nor() {
            if (col_sz != 1 && row_sz != 1) {
                throw std::runtime_error("nor() must be invoked on vector!");
            }
            div(length());
            return *this;
        }

        Ty length() const {
            if (col_sz != 1 && row_sz != 1) {
                throw std::runtime_error("nor() must be invoked on vector!");
            }
            return std::sqrt(lengsq());
        }

        Ty lengsq() const {
            if (col_sz != 1 && row_sz != 1) {
                throw std::runtime_error("nor() must be invoked on vector!");
            }

            Ty result = 0;
            for (size_t i = 0; i < col_sz * row_sz; ++i) {
                result += data[i] * data[i];
            }
            return result;
        }

        DMatrixT inverse(bool* exists = nullptr) const {
            if (col_sz != row_sz) {
                throw std::runtime_error("Row must be equal to Col!");
            }

            DMatrixT<Ty> m(row_sz, col_sz * 2);

            // [M] -> [M, I]
            for (size_t i = 0; i < row_sz; ++i) {
                std::memcpy(
                    m.data + i * col_sz * 2,
                    data + i * col_sz, sizeof(Ty) * col_sz);
                std::memset(m.data + i * col_sz * 2 + col_sz, 0, sizeof(Ty) * col_sz);
                m.data[i * col_sz * 2 + col_sz + i] = 1;
            }

            // [M, I] -> [I', M^]
            m = m.rref();

            // Check [I'] == [I]
            if (exists) {
                for (size_t i = 0; i < row_sz * col_sz; ++i) {
                    auto r = i / col_sz;
                    auto c = i % col_sz;
                    if (!utl::is_num_equal(
                        m.data[r * col_sz * 2 + c],
                        (r == c) ? Ty(1) : Ty(0)))
                    {
                        *exists = false;
                        return DMatrixT(row_sz, col_sz);
                    }
                }
                *exists = true;
            }

            // Copy [M^]
            DMatrixT out(row_sz, col_sz);
            for (size_t i = 0; i < row_sz; ++i) {
                std::memcpy(
                    out.data + i * col_sz,
                    m.data + i * col_sz * 2 + col_sz, sizeof(Ty) * col_sz);
            }
            return out;
        }

        size_t row_size() const { return row_sz; }
        size_t col_size() const { return col_sz; }

        size_t row_sz;
        size_t col_sz;
        Ty* data;

    private:
        static Ty rowMulCol(
            const DMatrixT& ml, size_t row_index,
            const DMatrixT& mr, size_t col_index)
        {
            Ty result = 0;
            for (size_t c = 0; c < ml.col_sz; ++c) {
                result += ml.data[row_index*ml.col_sz + c]
                    * mr.data[c*mr.col_sz + col_index];
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
    Ty det_slow(const DMatrixT<Ty>& m) {
        if (m.col_sz == 1) {
            return m.data[0];
        }
        if (m.col_sz == 2) {
            auto data = m.data;
            return data[0] * data[3] - data[1] * data[2];
        }
        if (m.col_sz == 3) {
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
        for (size_t i = 0; i < m.col_sz; ++i) {
            auto val = data[i];
            DMatrixT<Ty> mt(m.row_sz - 1, m.col_sz - 1);
            cofactor(m, 0, i, &mt);
            result += val * det_slow(mt) * (int((i + 0) % 2) * -2 + 1);
        }

        return result;
    }

    template <typename Ty>
    void cofactor(
        const DMatrixT<Ty>& m,
        size_t row_index, size_t col_index,
        DMatrixT<Ty>* out)
    {
        if (m.col_sz == 1) {
            assert(row_index < 1 && col_index < 1);
            out->data[0] = 1;
            return;
        }
        if (m.col_sz == 2) {
            assert(row_index < 2 && col_index < 2);

            auto r = (row_index + 1) % 2;
            auto c = (col_index + 1) % 2;
            out->data[0] = m.data[r * 2 + c];
            return;
        }
        if (m.col_sz == 3) {
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

        assert(row_index < m.row_sz && col_index < m.col_sz);

        size_t i = 0;
        auto data = m.data;
        if (col_index > 0 && col_index < m.col_sz - 1) {
            for (size_t j = 0; j < m.row_sz; ++j) {
                if (j == row_index) {
                    continue;
                }
                std::memcpy(
                    out->data + (m.col_sz - 1) * i,
                    data + m.col_sz * j,
                    col_index * sizeof(Ty));
                std::memcpy(
                    out->data + (m.col_sz - 1) * i + col_index,
                    data + m.col_sz * j + (col_index + 1),
                    (m.col_sz - 1 - col_index) * sizeof(Ty));
                ++i;
            }
        } else if (col_index > 0 && col_index >= m.col_sz - 1) {
            for (size_t j = 0; j < m.row_sz; ++j) {
                if (j == row_index) {
                    continue;
                }
                std::memcpy(
                    out->data + (m.col_sz - 1) * i,
                    data + m.col_sz * j,
                    col_index * sizeof(Ty));
                ++i;
            }
        } else if (col_index <= 0 && col_index < m.col_sz - 1) {
            for (size_t j = 0; j < m.row_sz; ++j) {
                if (j == row_index) {
                    continue;
                }
                std::memcpy(
                    out->data + (m.col_sz - 1) * i + col_index,
                    data + m.col_sz * j + (col_index + 1),
                    (m.col_sz - 1 - col_index) * sizeof(Ty));
                ++i;
            }
        }
    }

}
}
}

#endif  // UTILS_MATH_ALGEBRA_DYNAMIC_MATRIX_HPP_
