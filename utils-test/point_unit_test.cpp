// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/math/algebra/point.hpp"
#include "utils/unit_test/test_collector.h"


using namespace utl::math;

TEST_CASE(PointUnitTest) {

    TEST_DEF("Point 1 tests.") {
        PointT<double, 1> pt{ 0 };
        PointT<double, 1> pt2{ 7 };
        VectorT<double, 1> vec{ 6 };

        TEST_E(pt.size, 1);
        TEST_E(pt.get(), 0);
        pt.set(10);

        auto p_f = PointT<float, 1>(pt);
        TEST_E(p_f.get(), 10);

        pt.x(1);
        TEST_E(pt(), 1);
        TEST_E(pt.x(), 1);
        pt.at() = 4;
        TEST_E(pt.at(), 4);

        pt.x() = 2;
        TEST_E(pt.get(), 2);
        pt() = 3;
        TEST_E(pt(), 3);

        pt.zeros();
        TEST_E(pt(), 0);

        pt() = 1;
        pt = PointT<double, 1>::Z();
        TEST_E(pt(), 0);

        pt.add(vec);
        TEST_E(pt(), 6);
        TEST_E((pt + vec).x(), 12);

        pt.sub(vec);
        TEST_E(pt(), 0);
        TEST_E(pt.sub(pt2).x(), -7);
        TEST_E(pt.sub_h(pt2).x(), -7);
        TEST_E((pt - pt2).x(), -7);
        TEST_E((pt - vec).x(), -6);

        pt() = 10;
        TEST_E((-pt)(), -10);
        pt.minus();
        TEST_E(pt(), -10);

        TEST_E(pt.vec().x(), -10);
        TEST_E(pt.vec_h().x(), -10);

        pt() = 0;
        TEST_E(pt, (PointT<double, 1>::Z()));
        TEST_TRUE(pt.equal(PointT<double, 1>::Z()));

        return true;
    };

    TEST_DEF("Point 2 tests.") {
        PointT<double, 2> pt{ 2, 1 };
        TEST_E(pt.x(), 2);
        TEST_E(pt.y(), 1);
        pt.x(1);
        pt.y(10);
        TEST_E(pt.x(), 1);
        TEST_E(pt.y(), 10);
        pt.x() = 3;
        pt.y() = 4;
        TEST_E(pt.x(), 3);
        TEST_E(pt.y(), 4);

        return true;
    };

    TEST_DEF("Point 3 tests.") {
        PointT<double, 3> pt{ 0, 1, 2 };
        PointT<double, 3> pt2{ 7, 6, 5 };
        VectorT<double, 3> vec{ 6, 3, 1 };

        TEST_E(pt.size, 3);
        TEST_E(pt.get(0), 0);
        TEST_E(pt.get(1), 1);
        TEST_E(pt.get(2), 2);
        TEST_E(pt.get<0>(), 0);
        TEST_E(pt.get<1>(), 1);
        TEST_E(pt.get<2>(), 2);

        auto p_f = PointT<float, 3>(pt);
        TEST_E(p_f(0), 0);
        TEST_E(p_f(1), 1);
        TEST_E(p_f(2), 2);

        pt.x(1);
        pt.y(10);
        pt.z(100);
        TEST_E(pt(0), 1);
        TEST_E(pt(1), 10);
        TEST_E(pt(2), 100);
        TEST_E(pt.x(), 1);
        TEST_E(pt.y(), 10);
        TEST_E(pt.z(), 100);
        pt.at(0) = 4;
        pt.at(1) = 5;
        pt.at(2) = 6;
        TEST_E(pt.at(0), 4);
        TEST_E(pt.at(1), 5);
        TEST_E(pt.at(2), 6);
        pt.at<0>() = 7;
        pt.at<1>() = 8;
        pt.at<2>() = 9;
        TEST_E(pt.at<0>(), 7);
        TEST_E(pt.at<1>(), 8);
        TEST_E(pt.at<2>(), 9);

        pt.x() = 2;
        pt.y() = 20;
        pt.z() = 200;
        TEST_E(pt(0), 2);
        TEST_E(pt(1), 20);
        TEST_E(pt(2), 200);

        pt.set(0, 2);
        pt.set(1, 8);
        pt.set(2, 16);
        TEST_E(pt.get(0), 2);
        TEST_E(pt.get(1), 8);
        TEST_E(pt.get(2), 16);
        pt(0) = 3;
        pt(1) = 7;
        pt(2) = 9;
        TEST_E(pt(0), 3);
        TEST_E(pt(1), 7);
        TEST_E(pt(2), 9);

        pt.zeros();
        TEST_E(pt(0), 0);
        TEST_E(pt(1), 0);
        TEST_E(pt(2), 0);

        pt.set<0>(1);
        pt.set<1>(2);
        pt.set<2>(3);
        TEST_E(pt(0), 1);
        TEST_E(pt(1), 2);
        TEST_E(pt(2), 3);

        pt = PointT<double, 3>::Z();
        TEST_E(pt(0), 0);
        TEST_E(pt(1), 0);
        TEST_E(pt(2), 0);

        TEST_E(pt, (PointT<double, 3>::Z()));
        TEST_TRUE(pt.equal(PointT<double, 3>::Z()));

        pt.add(vec);
        TEST_E(pt(0), 6);
        TEST_E(pt(1), 3);
        TEST_E(pt(2), 1);
        TEST_E(pt + vec, (PointT<double, 3>{ 12, 6, 2 }));

        pt.sub(vec);
        TEST_E(pt(0), 0);
        TEST_E(pt(1), 0);
        TEST_E(pt(2), 0);
        TEST_E(pt.sub(pt2), (VectorT<double, 3>{ -7, -6, -5 }));
        TEST_E(pt.sub_h(pt2), (HVectorT<double, 3>{ -7, -6, -5 }));
        TEST_E((pt - pt2), (VectorT<double, 3>{ -7, -6, -5 }));
        TEST_E((pt - vec), (PointT<double, 3>{ -6, -3, -1 }));

        pt = { 10, 11, 12 };
        TEST_E(-pt, (PointT<double, 3>{ -10, -11, -12 }));
        pt.minus();
        TEST_E(pt, (PointT<double, 3>{ -10, -11, -12 }));

        TEST_E(pt.vec(), (VectorT<double, 3>{ -10, -11, -12 }));
        TEST_E(pt.vec_h(), (HVectorT<double, 3>{ -10, -11, -12 }));

        return true;
    };

    TEST_DEF("Point 4 tests.") {
        PointT<double, 4> pt{ 1, 2, 3, 4 };
        TEST_E(pt.x(), 1);
        TEST_E(pt.y(), 2);
        TEST_E(pt.z(), 3);
        TEST_E(pt.w(), 4);
        pt.x(5);
        pt.y(6);
        pt.z(7);
        pt.w(8);
        TEST_E(pt.x(), 5);
        TEST_E(pt.y(), 6);
        TEST_E(pt.z(), 7);
        TEST_E(pt.w(), 8);
        pt.x() = 9;
        pt.y() = 10;
        pt.z() = 11;
        pt.w() = 12;
        TEST_E(pt.x(), 9);
        TEST_E(pt.y(), 10);
        TEST_E(pt.z(), 11);
        TEST_E(pt.w(), 12);

        return true;
    };

}