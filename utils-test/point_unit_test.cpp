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

        p_f = pt.cast<float>();
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

        pt() = 1;
        pt.mul(2);
        TEST_E(pt(), 2);
        TEST_E((pt * 2)(), 4);
        TEST_E((2.0 * pt)(), 4);
        TEST_E((pt / 2)(), 1);
        pt.div(2);
        TEST_E(pt(), 1);
        pt *= 2;
        TEST_E(pt(), 2);
        pt /= 2;
        TEST_E(pt(), 1);

        pt.mull(MatrixT<double, 1, 1>{ 2 });
        TEST_E(pt(), 2);
        pt.mulr(MatrixT<double, 1, 1>{ 2 });
        TEST_E(pt(), 4);
        pt *= MatrixT<double, 1, 1>{2};
        TEST_E(pt(), 8);
        TEST_E((pt * MatrixT<double, 1, 1>{2})(), 16);
        TEST_E((MatrixT<double, 1, 1>{2} * pt)(), 16);

        pt() = 10;
        TEST_E((-pt)(), -10);
        pt.minus();
        TEST_E(pt(), -10);

        TEST_E(pt.vec().x(), -10);
        TEST_E(pt.vec_h().x(), -10);

        pt() = 0;
        TEST_E(pt, (PointT<double, 1>::Z()));
        TEST_TRUE(pt.equal(PointT<double, 1>::Z()));

        TEST_E(pt.gain({ 1, 2, 3 }), (PointT<double, 4>{0, 1, 2, 3}));

        pt() = 2;
        auto m = MatrixT<double, 2, 2>{
            1, 2,
            4, 5
        };
        TEST_E(pt * m, (PointT<double, 1>{6}));
        TEST_E(m * pt, (PointT<double, 1>{4}));

        pt2 = pt;
        pt2.mulr(m);
        TEST_E(pt2, (PointT<double, 1>{6}));
        pt2 = pt;
        pt2.mull(m);
        TEST_E(pt2, (PointT<double, 1>{4}));

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

        TEST_E(pt.reduce<1>().x(), 3);
        TEST_E(pt.gain({ 5, 6, 7 }), (PointT<double, 5>{3, 4, 5, 6, 7}));

        auto m = MatrixT<double, 3, 3>{
            1, 2, 3,
            4, 5, 6,
            7, 8, 9
        };
        TEST_E(pt * m, (PointT<double, 2>{26, 34}));
        TEST_E(m * pt, (PointT<double, 2>{14, 38}));

        auto pt2 = pt;
        pt2.mulr(m);
        TEST_E(pt2, (PointT<double, 2>{26, 34}));
        pt2 = pt;
        pt2.mull(m);
        TEST_E(pt2, (PointT<double, 2>{14, 38}));

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

        p_f = pt.cast<float>();
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

        pt = { 1, 2, 3 };
        pt.mul(2);
        TEST_E(pt, (PointT<double, 3>{2, 4, 6}));
        TEST_E(pt * 2, (PointT<double, 3>{4, 8, 12}));
        TEST_E(2.0 * pt, (PointT<double, 3>{4, 8, 12}));
        TEST_E(pt / 2, (PointT<double, 3>{1, 2, 3}));
        pt.div(2);
        TEST_E(pt, (PointT<double, 3>{1, 2, 3}));
        pt *= 2;
        TEST_E(pt, (PointT<double, 3>{2, 4, 6}));
        pt /= 2;
        TEST_E(pt, (PointT<double, 3>{1, 2, 3}));

        MatrixT<double, 3, 3> mat{
            1, 2, 3,
            4, 5, 6,
            7, 8, 9
        };
        auto pt3 = pt;
        pt3.mull(mat);
        TEST_E(pt3, (PointT<double, 3>{14, 32, 50}));
        pt3 = pt;
        pt3.mulr(mat);
        TEST_E(pt3, (PointT<double, 3>{30, 36, 42}));
        pt3 = pt;
        pt3 *= mat;
        TEST_E(pt3, (PointT<double, 3>{30, 36, 42}));

        TEST_E(mat * pt, (PointT<double, 3>{14, 32, 50}));
        TEST_E(pt * mat, (PointT<double, 3>{30, 36, 42}));

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

    TEST_DEF("Point Array tests.") {
        TEST_TRUE((std::is_aggregate<PointT<float, 3>>::value));
        TEST_TRUE((std::is_standard_layout<PointT<float, 3>>::value));
        TEST_TRUE((std::is_trivial<PointT<float, 3>>::value));
        TEST_TRUE((std::is_pod<PointT<float, 3>>::value));

        TEST_TRUE((std::is_aggregate<PointT<float, 5>>::value));
        TEST_TRUE((std::is_standard_layout<PointT<float, 5>>::value));
        TEST_TRUE((std::is_trivial<PointT<float, 5>>::value));
        TEST_TRUE((std::is_pod<PointT<float, 5>>::value));

        PointT<float, 3> arr[]{
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9},
            {10, 11, 12},
        };
        TEST_E(arr[0], (PointT<float, 3>{1, 2, 3}));
        TEST_E(arr[1], (PointT<float, 3>{4, 5, 6}));
        TEST_E(arr[2], (PointT<float, 3>{7, 8, 9}));
        TEST_E(arr[3], (PointT<float, 3>{10, 11, 12}));

        /*class PointT2_base {
        public:
            uint32_t data[3];
        };

        class PointT2_extra {};
        class PointT2_extra2 {};

        class PointT2_method :
            public PointT2_base,
            public PointT2_extra,
            public PointT2_extra2 {};

        class PointT2 : public PointT2_method {};

        auto lsdf = std::is_pod<PointT2>::value;

        PointT2 vd[4]{
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9},
            {10, 11, 12},
        };*/

        return true;
    };

}