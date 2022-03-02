// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MATH_ALGEBRA_OPTIMIZATION_HPP_
#define UTILS_MATH_ALGEBRA_OPTIMIZATION_HPP_

#include <cmath>

#include "utils/math/algebra/matrix.hpp"


namespace utl {
namespace math {

    class Optimization {
    public:
        template <typename Ty, size_t M, size_t N>
        static void conjugateGradient(
            const VectorT<Ty, N>& x,
            const VectorT<Ty, N>& T,
            VectorT<Ty, M>* out)
        {
            MatrixT<double, N, M> X;
            for (size_t i = 0; i < N; ++i) {
                for (size_t j = 0; j < M; ++j) {
                    X(i, j) = std::pow(x(i), j);
                }
            }

            auto Q = X.T()*X;
            auto W = VectorT<Ty, M>::Z();

            VectorT<Ty, M> r;
            gradient(X, W, T, &r);
            r.minus();

            auto p = r;

            for (size_t i = 0; i < M; ++i) {
                auto a = Ty(r.T()*r) / Ty(p.T()*Q*p);
                auto r_prev = r;
                W = W + a * p;
                r = r - a * Q * p;
                p = r + (Ty(r.T()*r) / Ty(r_prev.T()*r_prev))*p;
            }

            *out = W;
        }

        template <typename Ty, size_t M, size_t N>
        static void conjugateGradient(
            const VectorT<Ty, N>& x,
            const VectorT<Ty, N>& T,
            Ty lambda, VectorT<Ty, M>* out)
        {
            MatrixT<double, N, M> X;
            for (size_t i = 0; i < N; ++i) {
                for (size_t j = 0; j < M; ++j) {
                    X(i, j) = std::pow(x(i), j);
                }
            }

            auto Q = (1 / Ty(M)) * (X.T()*X + lambda * MatrixT<Ty, M, M>::I());
            auto W = VectorT<Ty, M>::Z();

            VectorT<Ty, M> r;
            gradient(X, W, T, lambda, &r);
            r.minus();

            auto p = r;

            for (size_t i = 0; i < M; ++i) {
                auto a = Ty(r.T()*r) / Ty(p.T()*Q*p);
                auto r_prev = r;
                W = W + a * p;
                r = r - a * Q * p;
                p = r + (Ty(r.T()*r) / Ty(r_prev.T()*r_prev))*p;
            }

            *out = W;
        }

    private:
        /**
         * 求 E(W) 的梯度。
         * @tparam Ty 数据类型
         * @tparam M  数据维数
         * @tparam N  采样数
         * @param X 自变量矩阵
         * @param W 参数
         * @param T 学习值
         * @param out 梯度
         */
        template <typename Ty, size_t M, size_t N>
        static void gradient(
            const MatrixT<Ty, N, M>& X,
            const VectorT<Ty, M>& W,
            const VectorT<Ty, N>& T,
            VectorT<Ty, M>* out)
        {
            *out = X.T() * X * W - X.T() * T;
        }

        /**
         * 求 E(W) 的梯度。
         * @tparam Ty 数据类型
         * @tparam M  数据维数
         * @tparam N  采样数
         * @param X 自变量矩阵
         * @param W 参数
         * @param T 学习值
         * @param lambda 正则系数
         * @param out 梯度
         */
        template <typename Ty, size_t M, size_t N>
        static void gradient(
            const MatrixT<Ty, N, M>& X,
            const VectorT<Ty, M>& W,
            const VectorT<Ty, N>& T,
            Ty lambda,
            VectorT<Ty, M>* out)
        {
            *out = (1 / Ty(M)) * (X.T()*X*W + lambda * MatrixT<Ty, M, M>::I()*W - X.T()*T);
        }
    };

}
}

#endif  // UTILS_MATH_ALGEBRA_OPTIMIZATION_HPP_
