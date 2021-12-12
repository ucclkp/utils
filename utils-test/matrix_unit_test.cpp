// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/math/algebra/matrix.hpp"
#include "utils/number.hpp"
#include "utils/unit_test/test_collector.h"


using namespace math;

TEST_CASE(MatrixUnitTest) {

    TEST_DEF("Matrix 1x1 determinant, cofactor and inverse.") {
        MatrixT<double, 1, 1> m{ 5 };
        TEST_NUM_E(m.det(), 5.0);

        MatrixT<double, 1, 1> cof;
        m.cofactor(0, 0, &cof);
        MatrixT<double, 1, 1> cof_result{1};
        TEST_E(cof, cof_result);

        MatrixT<double, 1, 1> inv_m;
        m.inverse(&inv_m);
        MatrixT<double, 1, 1> inv_result{ 1 / 5.0 };
        TEST_E(inv_m, inv_result);

        return true;
    };

    TEST_DEF("Matrix 2x2 determinant, cofactor and inverse.") {
        MatrixT<double, 2, 2> m{
            5, 1,
            9, 7,
        };
        TEST_NUM_E(m.det(), 26.0);

        MatrixT<double, 1, 1> cof1;
        MatrixT<double, 1, 1> cof2;
        m.cofactor(0, 0, &cof1);
        m.cofactor(1, 1, &cof2);
        MatrixT<double, 1, 1> cof1_result{7};
        MatrixT<double, 1, 1> cof2_result{5};
        TEST_E(cof1, cof1_result);
        TEST_E(cof2, cof2_result);

        MatrixT<double, 2, 2> inv_m;
        m.inverse(&inv_m);
        MatrixT<double, 2, 2> inv_result{
            5, 1,
            9, 7,
        };
        TEST_E(inv_m, inv_result);

        return true;
    };

    TEST_DEF("Matrix 3x3 determinant, cofactor and inverse.") {
        MatrixT<double, 3, 3> m{
            5, 1, 4,
            9, 7, 8,
            2, 3, 4
        };
        TEST_NUM_E(m.det(), 26.0);

        MatrixT<double, 2, 2> cof1;
        MatrixT<double, 2, 2> cof2;
        m.cofactor(0, 0, &cof1);
        m.cofactor(1, 1, &cof2);
        MatrixT<double, 2, 2> cof1_result{
            7, 8,
            3, 4
        };
        MatrixT<double, 2, 2> cof2_result{
            5, 1,
            9, 7,
        };
        TEST_E(cof1, cof1_result);
        TEST_E(cof2, cof2_result);

        MatrixT<double, 3, 3> inv_m;
        m.inverse(&inv_m);

        MatrixT<double, 3, 3> e_m;
        m.rref(&e_m);

        return true;
    };

    TEST_DEF("Matrix 5x5 determinant, cofactor and inverse.") {
        MatrixT<double, 5, 5> m{
            2, 1, 3, 4, 5,
            1, 1, 1, 10, 7,
            4, 8, 9, 5, 1,
            9, 7, 1, 6, 4,
            1, 5, 3, 7, 4,
        };

        MatrixT<double, 4, 4> cof1;
        MatrixT<double, 4, 4> cof2;
        MatrixT<double, 4, 4> cof3;
        m.cofactor(0, 0, &cof1);
        m.cofactor(1, 1, &cof2);
        m.cofactor(2, 2, &cof3);
        auto defr = m.det();

        MatrixT<double, 5, 5> inv_m;
        m.inverse(&inv_m);

        auto ide = m * inv_m;

        auto b1 = utl::is_num_equal(ide(0, 0), 1.0);
        auto b2 = utl::is_num_equal(ide(1, 1), 1.0);
        auto b3 = utl::is_num_equal(ide(2, 2), 1.0);
        auto b4 = utl::is_num_equal(ide(3, 3), 1.0);
        auto b5 = utl::is_num_equal(ide(4, 4), 1.0);

        MatrixT<double, 5, 5> e_m;
        m.rref(&e_m);

        MatrixT<double, 5, 5> inv2_m;
        m.inverse2(&inv2_m);

        auto ide2 = m * inv2_m;
        return true;
    };

    TEST_DEF("Matrix 3x5 determinant, cofactor and inverse.") {
        MatrixT<double, 3, 5> m{
            2, 1, 3, 4, 5,
            1, 1, 1, 10, 7,
            4, 8, 9, 5, 1,
        };

        MatrixT<double, 3, 5> e_m;
        m.rref(&e_m);
        return true;
    };

    TEST_DEF("Matrix rref.") {
        {
            MatrixT<double, 1, 2> m{
                2, 1
            };
            MatrixT<double, 1, 2> e_m;
            m.rref(&e_m);

            TEST_TRUE((e_m == math::MatrixT<double, 1, 2>{1, 0.5}));
        }

        {
            MatrixT<double, 2, 2> m{
                2, 1,
                0, 1,
            };
            MatrixT<double, 2, 2> e_m;
            m.rref(&e_m);

            TEST_TRUE((e_m == math::MatrixT<double, 2, 2>{
                1, 0,
                0, 1
            }));
        }

        {
            MatrixT<double, 2, 2> m{
                2, 1,
                1, 1,
            };
            MatrixT<double, 2, 2> e_m;
            m.rref(&e_m);

            TEST_TRUE((e_m == math::MatrixT<double, 2, 2>{
                1, 0,
                0, 1
            }));
        }

        {
            MatrixT<double, 2, 3> m{
                2, 1, 6,
                0, 1, 7,
            };
            MatrixT<double, 2, 3> e_m;
            m.rref(&e_m);

            TEST_TRUE((e_m == math::MatrixT<double, 2, 3>{
                1, 0, -0.5,
                0, 1, 7,
            }));
        }

        {
            MatrixT<double, 3, 3> m{
                2, 1, 6,
                0, 3, 4,
                6, 1, 7,
            };
            MatrixT<double, 3, 3> e_m;
            m.rref(&e_m);

            TEST_TRUE((e_m == math::MatrixT<double, 3, 3>{
                1, 0, -0.5,
                0, 1, 7,
            }));
        }
        return true;
    };

    TEST_DEF("Matrix 10x10 determinant, cofactor and inverse.") {
        return true;
        MatrixT<double, 10, 10> m{
            2, 1, 6, 8, 6, 0, 3, 3, 8, 1,
            0, 3, 4, 3, 7, 9, 5, 1, 0, 3,
            6, 1, 7, 8, 6, 4, 9, 5, 2, 5,
            0, 0, 0, 7, 9, 5, 7, 4, 0, 8,
            6, 9, 6, 4, 8, 2, 7, 5, 1, 1,
            9, 9, 0, 0, 0, 7, 7, 8, 5, 4,
            9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
            7, 9, 8, 8, 9, 8, 9, 6, 1, 1,
            8, 8, 8, 3, 3, 4, 2, 3, 7, 6,
            97, 43, 87, 1, 0, 91, 87, 46, 78, 13,
        };

        MatrixT<double, 10, 10> e_m;
        m.rref(&e_m);

        MatrixT<double, 10, 10> inv_m;
        m.inverse(&inv_m);

        MatrixT<double, 10, 10> inv2_m;
        m.inverse2(&inv2_m);

        auto t1 = m * inv_m;
        auto t2 = m * inv2_m;
        return true;
    };

}