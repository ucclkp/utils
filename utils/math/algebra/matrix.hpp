// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef MATH_ALGEBRA_MATRIX_HPP_
#define MATH_ALGEBRA_MATRIX_HPP_

#include <algorithm>
#include <cassert>
#include <cstring>
#include <type_traits>

#include "utils/number.hpp"
#include "utils/type_utils.hpp"


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
        static MatrixT I() {
            MatrixT m;
            m.identity();
            return m;
        }

        MatrixT() {}
        MatrixT(const std::initializer_list<Ty>& vals) {
            std::copy(vals.begin(), vals.end(), data);
        }
        MatrixT(const MatrixT& rhs) {
            std::memcpy(data, rhs.data, Row * Col * sizeof(Ty));
        }

        bool operator==(const MatrixT& rhs) const {
            return equal(rhs);
        }

        bool operator!=(const MatrixT& rhs) const {
            return !equal(rhs);
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

        void set(size_t index, Ty val) {
            if constexpr (Col != 1) {
                static_assert(utl::sat_stub<Ty>::value, "Col must be equal to 1!");
            }
            assert(index < Row);
            data[index] = val;
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

        void zero() {
            std::memset(data, 0, sizeof(Ty) * Row * Col);
        }

        void identity() {
            if constexpr (Row != Col) {
                static_assert(utl::sat_stub<Ty>::value, "Row must be equal to Col!");
            }
            for (size_t i = 0; i < Row; ++i) {
                std::memset(data + i * Col, 0, sizeof(Ty) * Col);
                data[i * Col + i] = 1;
            }
        }

        void transpose(MatrixT<Ty, Col, Row>* out) const {
            for (size_t r = 0; r < Row; ++r) {
                for (size_t c = 0; c < Col; ++c) {
                    out->data[c * Row + r] = data[r * Col + c];
                }
            }
        }

        bool equal(const MatrixT& rhs) const {
            for (size_t i = 0; i < Row * Col; ++i) {
                if (!utl::is_num_equal(data[i], rhs.data[i])) {
                    return false;
                }
            }
            return true;
        }

        Ty det() const {
            if constexpr (Col != Row) {
                static_assert(utl::sat_stub<Ty>::value, "Row must be equal to Col!");
            }
            return internal::detTrans(*this);
        }

        void cofactor(
            size_t row_index, size_t col_index, MatrixT<Ty, Row - 1, Col - 1>* out) const
        {
            if constexpr (Col != Row) {
                static_assert(utl::sat_stub<Ty>::value, "Row must be equal to Col!");
            }
            internal::cofactor(*this, row_index, col_index, out);
        }

        void adjoint(MatrixT* out) const {
            MatrixT adj;
            for (size_t i = 0; i < Row; ++i) {
                for (size_t j = 0; j < Col; ++j) {
                    MatrixT<Ty, Row - 1, Col - 1> mt;
                    internal::cofactor(*this, i, j, &mt);
                    adj.data[i * Col + j] = internal::detTrans(mt) * (int((i + j) % 2) * -2 + 1);
                }
            }
            adj.transpose(out);
        }

        bool inverse(MatrixT* out) const {
            if constexpr (Col != Row) {
                static_assert(utl::sat_stub<Ty>::value, "Row must be equal to Col!");
            }

            Ty detr = 0;
            MatrixT adjoint;
            auto& od = adjoint.data;
            for (size_t i = 0; i < Col; ++i) {
                auto val = data[i];
                MatrixT<Ty, Row - 1, Col - 1> mt;
                internal::cofactor(*this, 0, i, &mt);
                od[i] = internal::detTrans(mt) * (int((i + 0) % 2) * -2 + 1);
                detr += val * od[i];
            }

            if (utl::is_num_zero(detr)) {
                return false;
            }

            for (size_t i = 1; i < Row; ++i) {
                for (size_t j = 0; j < Col; ++j) {
                    MatrixT<Ty, Row - 1, Col - 1> mt;
                    internal::cofactor(*this, i, j, &mt);
                    od[i * Col + j] = internal::detTrans(mt) * (int((i + j) % 2) * -2 + 1);
                }
            }

            adjoint.transpose(out);
            out->mul(1 / detr);
            return true;
        }

        void swapCol(size_t c1_index, size_t c2_index) {
            assert(c1_index < Col && c2_index < Col);
            if (c1_index == c2_index) {
                return;
            }

            for (size_t i = 0; i < Row; ++i) {
                Ty tmp = data[i * Col + c1_index];
                data[i * Col + c1_index] = data[i * Col + c2_index];
                data[i * Col + c2_index] = tmp;
            }
        }

        void swapRow(size_t r1_index, size_t r2_index) {
            assert(r1_index < Row && r2_index < Row);
            if (r1_index == r2_index) {
                return;
            }

            Ty tmp[Col];
            std::memcpy(tmp, &data[r1_index * Col], sizeof(Ty) * Col);
            std::memcpy(&data[r1_index * Col], &data[r2_index * Col], sizeof(Ty) * Col);
            std::memcpy(&data[r2_index * Col], tmp, sizeof(Ty) * Col);
        }

        void ref(MatrixT* out) const {
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
            *out = m;
        }

        void rref(MatrixT* out) const {
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

            *out = m;
        }

        bool inverse2(MatrixT* out) const {
            if constexpr (Col != Row) {
                static_assert(utl::sat_stub<Ty>::value, "Row must be equal to Col!");
            }

            MatrixT<Ty, Row, Col * 2> m;

            // [M] -> [M, I]
            for (size_t i = 0; i < Row; ++i) {
                std::memcpy(
                    m.data + i * Col * 2,
                    data + i * Col, sizeof(Ty) * Col);
                std::memset(m.data + i * Col * 2 + Col, 0, sizeof(Ty) * Col);
                m.data[i * Col * 2 + Col + i] = 1;
            }

            // [M, I] -> [I', M^]
            m.rref(&m);

            // Check [I'] == [I]
            for (size_t i = 0; i < Row * Col; ++i) {
                auto r = i / Col;
                auto c = i % Col;
                if (!utl::is_num_equal(
                    m.data[r * Col * 2 + c],
                    (r == c) ? Ty(1) : Ty(0)))
                {
                    return false;
                }
            }

            // Copy [M^]
            for (size_t i = 0; i < Row; ++i) {
                std::memcpy(
                    out->data + i * Col,
                    m.data + i * Col * 2 + Col, sizeof(Ty) * Col);
            }
            return true;
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

        MatrixT() {}
        explicit MatrixT(Ty val) {
            data[0] = val;
        }
        MatrixT(const MatrixT& rhs) {
            data[0] = rhs.data[0];
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

        MatrixT& operator=(const MatrixT& rhs) {
            if (&rhs != this) { data[0] = rhs.data[0]; }
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
        MatrixT<Ty, 1, RCol> operator*(const MatrixT<Ty, 1, RCol>& rhs) const {
            MatrixT<Ty, 1, RCol> m;
            mul(rhs, &m);
            return m;
        }

        MatrixT operator-() const {
            MatrixT m(*this);
            m.minus();
            return m;
        }

        Ty operator()(size_t index) const {
            assert(index < 1);
            return data[0];
        }

        Ty& operator()(size_t index) {
            assert(index < 1);
            return data[0];
        }

        Ty operator()(size_t row_index, size_t col_index) const {
            assert(row_index < 1 && col_index < 1);
            return data[0];
        }

        Ty& operator()(size_t row_index, size_t col_index) {
            assert(row_index < 1 && col_index < 1);
            return data[0];
        }

        MatrixT T() const {
            MatrixT m;
            transpose(&m);
            return m;
        }

        void add(const MatrixT& rhs) {
            data[0] += rhs.data[0];
        }

        void sub(const MatrixT& rhs) {
            data[0] -= rhs.data[0];
        }

        void minus() {
            data[0] = -data[0];
        }

        void mul(Ty val) {
            data[0] *= val;
        }

        void div(Ty val) {
            data[0] /= val;
        }

        template <size_t RCol>
        void mul(const MatrixT<Ty, 1, RCol>& rhs, MatrixT<Ty, 1, RCol>* out) const {
            for (size_t c = 0; c < RCol; ++c) {
                out->data[c] = rowMulCol(*this, 0, rhs, c);
            }
        }

        void set(size_t index, Ty val) {
            assert(index < 1);
            data[0] = val;
        }

        void set(size_t row_index, size_t col_index, Ty val) {
            assert(row_index < 1 && col_index < 1);
            data[0] = val;
        }

        Ty get(size_t index) const {
            assert(index < 1);
            return data[0];
        }

        Ty get(size_t row_index, size_t col_index) const {
            assert(row_index < 1 && col_index < 1);
            return data[0];
        }

        void zero() {
            data[0] = 0;
        }

        void identity() {
            data[0] = 1;
        }

        void transpose(MatrixT* out) const {
            out->data[0] = data[0];
        }

        bool equal(const MatrixT& rhs) const {
            return utl::is_num_equal(data[0], rhs.data[0]);
        }

        Ty det() const {
            return data[0];
        }

        void cofactor(
            size_t row_index, size_t col_index, MatrixT* out) const
        {
            assert(row_index < 1 && col_index < 1);
            out->data[0] = 1;
        }

        void adjoint(MatrixT* out) const {
            out->data[0] = 1;
        }

        bool inverse(MatrixT* out) const {
            if (utl::is_num_zero(data[0])) {
                return false;
            }
            out->data[0] = 1 / data[0];
            return true;
        }

        void swapCol(size_t c1_index, size_t c2_index) {
            assert(c1_index < 1 && c2_index < 1);
        }

        void swapRow(size_t r1_index, size_t r2_index) {
            assert(r1_index < 1 && r2_index < 1);
        }

        void ref(MatrixT* out) const {
            if (utl::is_num_zero(data[0])) {
                out->data[0] = 0;
            } else {
                out->data[0] = data[0];
            }
        }

        void rref(MatrixT* out) const {
            if (utl::is_num_zero(data[0])) {
                out->data[0] = 0;
            } else {
                out->data[0] = 1;
            }
        }

        size_t getRow() const { return 1; }
        size_t getCol() const { return 1; }

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
        const MatrixT<Ty, 3, 3>& m,
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

#endif  // MATH_ALGEBRA_MATRIX_HPP_
