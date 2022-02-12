// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef MATH_ALGEBRA_DYNAMIC_OPTIMIZATION_HPP_
#define MATH_ALGEBRA_DYNAMIC_OPTIMIZATION_HPP_

#include "utils/math/algebra/dynamic_matrix.hpp"


namespace utl {
namespace math {

    class DOptimization {
    public:
        /**
         * 共轭梯度。
         * @param x 自变量（采样数 N）
         * @param T 学习值（采样数 N）
         * @param out 拟合曲线参数（数据维数 M）
         */
        template <typename Ty>
        static void conjugateGradient(
            const DMatrixT<Ty>& x,
            const DMatrixT<Ty>& T,
            DMatrixT<Ty>* out)
        {
            auto N = x.row;
            auto M = out->row;

            DMatrixT<double> X(N, M);
            for (decltype(N) i = 0; i < N; ++i) {
                for (decltype(M) j = 0; j < M; ++j) {
                    X(i, j) = std::pow(x(i), j);
                }
            }

            auto Q = X.T()*X;
            auto W = DMatrixT<Ty>::Z(M, 1);

            DMatrixT<Ty> r(M, 1);
            gradient(X, W, T, &r);
            r.minus();

            auto p = r;

            for (decltype(M) i = 0; i < M; ++i) {
                auto a = Ty(r.T()*r) / Ty(p.T()*Q*p);
                auto r_prev = r;
                W = W + a * p;
                r = r - a * Q * p;
                p = r + (Ty(r.T()*r) / Ty(r_prev.T()*r_prev))*p;
            }

            *out = W;
        }

        /**
         * 共轭梯度。
         * @param x 自变量（采样数 N）
         * @param T 学习值（采样数 N）
         * @param lambda 正则系数
         * @param out 拟合曲线参数（数据维数 M）
         */
        template <typename Ty>
        static void conjugateGradient(
            const DMatrixT<Ty>& x,
            const DMatrixT<Ty>& T,
            Ty lambda, DMatrixT<Ty>* out)
        {
            auto N = x.row;
            auto M = out->row;

            DMatrixT<double> X(N, M);
            for (decltype(N) i = 0; i < N; ++i) {
                for (decltype(M) j = 0; j < M; ++j) {
                    X(i, j) = std::pow(x(i), j);
                }
            }

            auto Q = (1 / Ty(M)) * (X.T()*X + lambda * DMatrixT<Ty>::I(M));
            auto W = DMatrixT<Ty>::Z(M, 1);

            DMatrixT<Ty> r(M, 1);
            gradient(X, W, T, lambda, &r);
            r.minus();

            auto p = r;

            for (decltype(M) i = 0; i < M; ++i) {
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
         * @param X 自变量矩阵（采样数 N x 数据维数 M）
         * @param W 参数（数据维数 M）
         * @param T 学习值（采样数 N）
         * @param out 梯度（数据维数 M）
         */
        template <typename Ty>
        static void gradient(
            const DMatrixT<Ty>& X,
            const DMatrixT<Ty>& W,
            const DMatrixT<Ty>& T,
            DMatrixT<Ty>* out)
        {
            *out = X.T() * X * W - X.T() * T;
        }

        /**
         * 求 E(W) 的梯度。
         * @tparam Ty 数据类型
         * @param X 自变量矩阵（采样数 N x 数据维数 M）
         * @param W 参数（数据维数 M）
         * @param T 学习值（采样数 N）
         * @param lambda 正则系数
         * @param out 梯度（数据维数 M）
         */
        template <typename Ty>
        static void gradient(
            const DMatrixT<Ty>& X,
            const DMatrixT<Ty>& W,
            const DMatrixT<Ty>& T,
            Ty lambda,
            DMatrixT<Ty>* out)
        {
            *out = (1 / Ty(W.row)) * (X.T()*X*W + lambda * DMatrixT<Ty>::I(W.row)*W - X.T()*T);
        }
    };

}
}

#endif  // MATH_ALGEBRA_DYNAMIC_OPTIMIZATION_HPP_