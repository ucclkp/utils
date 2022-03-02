// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MATH_ALGEBRA_SPECIAL_MATRIX_HPP_
#define UTILS_MATH_ALGEBRA_SPECIAL_MATRIX_HPP_

#include "utils/math/algebra/matrix.hpp"
#include "utils/math/algebra/point.hpp"


namespace utl {
namespace math {

    /**
     * 构造 3x3 缩放矩阵。
     */
    template <typename Ty>
    MatrixT<Ty, 3, 3> scale3x3(Ty sx, Ty sy) {
        return {
            sx, 0,  0,
            0,  sy, 0,
            0,  0,  1
        };
    }

    /**
     * 构造以 X 轴为中心的 3x3 旋转矩阵。
     * 注意：构造的矩阵为右手列优先矩阵。
     */
    template <typename Ty>
    MatrixT<Ty, 3, 3> rotateX3x3(Ty angle) {
        return {
            1, 0,               0,
            0, std::cos(angle), -std::sin(angle),
            0, std::sin(angle),  std::cos(angle),
        };
    }

    /**
     * 构造以 Y 轴为中心的 3x3 旋转矩阵。
     * 注意：构造的矩阵为右手列优先矩阵。
     */
    template <typename Ty>
    MatrixT<Ty, 3, 3> rotateY3x3(Ty angle) {
        return {
            std::cos(angle),  0, std::sin(angle),
            0,                1, 0,
            -std::sin(angle), 0, std::cos(angle),
        };
    }

    /**
     * 构造以 Z 轴为中心的 3x3 旋转矩阵。
     * 注意：构造的矩阵为列优先矩阵。
     */
    template <typename Ty>
    MatrixT<Ty, 3, 3> rotateZ3x3(Ty angle) {
        return {
            std::cos(angle), -std::sin(angle),  0,
            std::sin(angle),  std::cos(angle),  0,
            0,               0,                 1
        };
    }

    /**
     * 构造 3x3 平移矩阵。
     * 注意：构造的矩阵为列优先矩阵。
     */
    template <typename Ty>
    MatrixT<Ty, 3, 3> translate3x3(Ty tx, Ty ty) {
        return {
            1, 0, tx,
            0, 1, ty,
            0, 0, 1
        };
    }

    /**
     * 构造 4x4 缩放矩阵。
     */
    template <typename Ty>
    MatrixT<Ty, 4, 4> scale4x4(Ty sx, Ty sy, Ty sz) {
        return {
            sx, 0,  0,  0,
            0,  sy, 0,  0,
            0,  0,  sz, 0,
            0,  0,  0,  1
        };
    }

    /**
     * 构造以 X 轴为中心的 4x4 旋转矩阵。
     * 注意：构造的矩阵为右手列优先矩阵。
     */
    template <typename Ty>
    MatrixT<Ty, 4, 4> rotateX4x4(Ty angle) {
        return {
            1, 0,               0,                0,
            0, std::cos(angle), -std::sin(angle), 0,
            0, std::sin(angle),  std::cos(angle), 0,
            0, 0,               0,                1
        };
    }

    /**
     * 构造以 Y 轴为中心的 4x4 旋转矩阵。
     * 注意：构造的矩阵为右手列优先矩阵。
     */
    template <typename Ty>
    MatrixT<Ty, 4, 4> rotateY4x4(Ty angle) {
        return {
            std::cos(angle),  0, std::sin(angle), 0,
            0,                1, 0,               0,
            -std::sin(angle), 0, std::cos(angle), 0,
            0,                0, 0,               1
        };
    }

    /**
     * 构造以 Z 轴为中心的 4x4 旋转矩阵。
     * 注意：构造的矩阵为右手列优先矩阵。
     */
    template <typename Ty>
    MatrixT<Ty, 4, 4> rotateZ4x4(Ty angle) {
        return {
            std::cos(angle), -std::sin(angle), 0, 0,
            std::sin(angle),  std::cos(angle), 0, 0,
            0,               0,                1, 0,
            0,               0,                0, 1
        };
    }

    /**
     * 构造以指定轴为中心的 4x4 旋转矩阵。
     * 指定的轴为 axis，该轴过原点。
     * 注意：构造的矩阵为右手列优先矩阵。
     */
    template <typename Ty>
    MatrixT<Ty, 4, 4> rotateAxis4x4(const VectorT<Ty, 3>& axis, Ty angle) {
        auto sinc = std::sin(angle);
        auto cosc = std::cos(angle);
        auto x = axis(0);
        auto y = axis(1);
        auto z = axis(2);
        auto x2 = x * x;
        auto y2 = y * y;
        auto xy = x * y;
        auto xz = x * z;
        auto yz = y * z;

        return {
            cosc - x2 * (cosc - 1),      -z * sinc - xy * (cosc - 1), y * sinc - xz * (cosc - 1),  0,
            z * sinc - xy * (cosc - 1),  y2 - cosc * (y2 - 1),        -x * sinc - yz * (cosc - 1), 0,
            -y * sinc - xz * (cosc - 1), x * sinc - yz * (cosc - 1),  (cosc - 1) * (x2 + y2) + 1,  0,
            0, 0, 0, 1 };
    }

    /**
     * 构造 4x4 平移矩阵。
     * 注意：构造的矩阵为列优先矩阵。
     */
    template <typename Ty>
    MatrixT<Ty, 4, 4> translate4x4(Ty tx, Ty ty, Ty tz) {
        return {
            1, 0, 0, tx,
            0, 1, 0, ty,
            0, 0, 1, tz,
            0, 0, 0, 1
        };
    }

    /**
     * 构造 4x4 视图矩阵。
     * 注意：构造的矩阵为右手列优先矩阵。
     */
    template <typename Ty>
    MatrixT<Ty, 4, 4> viewport4x4(int pixel_width, int pixel_height) {
        return {
            pixel_width / 2.0, 0,                  0, (pixel_width - 1) / 2.0,
            0,                 pixel_height / 2.0, 0, (pixel_height - 1) / 2.0,
            0,                 0,                  1, 0,
            0,                 0,                  0, 1
        };
    }

    /**
     * 构造 4x4 正规化矩阵。
     * 该矩阵将指定的位置 [l, t, r, b, n, f] 正规化为 [-1, -1, 1, 1, -1, 1]，
     * 并将正规空间的中心平移至原点。
     * 注意：构造的矩阵为右手列优先矩阵。
     */
    template <typename Ty>
    MatrixT<Ty, 4, 4> orthoProj4x4(Ty l, Ty r, Ty b, Ty t, Ty n, Ty f) {
        return {
            2 / (r - l), 0,           0,           -(r + l) / (r - l),
            0,           2 / (t - b), 0,           -(t + b) / (t - b),
            0,           0,           2 / (n - f), -(n + f) / (n - f),
            0,           0,           0,           1
        };
    }

    /**
     * 构造 4x4 投影矩阵。
     * 该矩阵仅进行缩放操作，请配合正规矩阵使用。
     * 另外，最终的变换结果请除以相应点的 W 分量。
     * 注意：构造的矩阵为右手列优先矩阵。
     */
    template <typename Ty>
    MatrixT<Ty, 4, 4> persp4x4(Ty n, Ty f) {
        return {
            n, 0, 0,     0,
            0, n, 0,     0,
            0, 0, n + f, -f * n,
            0, 0, 1,     0
        };
    }

    /**
     * 构造 4x4 摄像机矩阵。
     * 该矩阵将摄像机平移至原点，朝向 Z 轴负方向。
     * 注意：构造的矩阵为右手列优先矩阵。
     */
    template <typename Ty>
    MatrixT<Ty, 4, 4> camera4x4(
        const PointT<Ty, 3>& eye,
        const VectorT<Ty, 3>& look,
        const VectorT<Ty, 3>& up)
    {
        auto w = -look.N();
        auto u = (up ^ w).N();
        auto v = w ^ u;
        return {
            u(0), u(1), u(2), -u(0) * eye(0) - u(1) * eye(1) - u(2) * eye(2),
            v(0), v(1), v(2), -v(0) * eye(0) - v(1) * eye(1) - v(2) * eye(2),
            w(0), w(1), w(2), -w(0) * eye(0) - w(1) * eye(1) - w(2) * eye(2),
            0,     0,   0,    1
        };
    }

    /**
     * 构造 4x4 摄像机逆矩阵。
     * 注意：构造的矩阵为右手列优先矩阵。
     */
    template <typename Ty>
    MatrixT<Ty, 4, 4> cameraInverse4x4(
        const PointT<Ty, 3>& eye,
        const VectorT<Ty, 3>& look,
        const VectorT<Ty, 3>& up)
    {
        auto w = -look.N();
        auto u = (up ^ w).N();
        auto v = w ^ u;
        return {
            u(0), v(0), w(0), eye(0),
            u(1), v(1), w(1), eye(1),
            u(2), v(2), w(2), eye(2),
            0,    0,    0,    1
        };
    }

}
}

#endif  // UTILS_MATH_ALGEBRA_SPECIAL_MATRIX_HPP_
