// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MATH_ALGEBRA_GEOCAL_HPP_
#define UTILS_MATH_ALGEBRA_GEOCAL_HPP_

#include "utils/math/algebra/point.hpp"


namespace utl {
namespace math {

    /**
     * 计算点 p 到直线 p1-p2 的距离。
     */
    template <typename Ty>
    Ty distanceLinePoint3(
        const PointT<Ty, 3>& p1,
        const PointT<Ty, 3>& p2,
        const PointT<Ty, 3>& p)
    {
        auto pv = p - p1;
        auto lv = p2 - p1;
        auto length_sq = lv.lengsq();

        auto scale = pv * lv / length_sq;
        auto distance = pv - lv * scale;

        return distance.length();
    }

}
}

#endif  // UTILS_MATH_ALGEBRA_GEOCAL_HPP_