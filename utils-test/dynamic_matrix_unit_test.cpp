// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/math/algebra/matrix.hpp"
#include "utils/math/algebra/dynamic_matrix.hpp"
#include "utils/number.hpp"
#include "utils/unit_test/test_collector.h"


using namespace math;

TEST_CASE(DMatrixUnitTest) {

    TEST_DEF("DMatrix 1x1 determinant, cofactor and inverse.") {
        DMatrixT<double> m(1, 1, { 5 });
        TEST_NUM_E(m.det(), 5.0);

        DMatrixT<double> cof(1, 1);
        m.cofactor(0, 0, &cof);
        DMatrixT<double> cof_result(1, 1, {1});
        TEST_E(cof, cof_result);

        DMatrixT<double> inv_m(1, 1);
        m.inverse(&inv_m);
        DMatrixT<double> inv_result(1, 1, { 1 / 5.0 });
        TEST_E(inv_m, inv_result);

        DMatrixT<double> ref_m(1, 1);
        m.ref(&ref_m);

        return true;
    };

    TEST_DEF("DMatrix 2x2 determinant, cofactor and inverse.") {
        DMatrixT<double> m(2, 2, {
            5, 1,
            9, 7,
        });
        TEST_NUM_E(m.det(), 26.0);

        DMatrixT<double> cof1(1, 1);
        DMatrixT<double> cof2(1, 1);
        m.cofactor(0, 0, &cof1);
        m.cofactor(1, 1, &cof2);
        DMatrixT<double> cof1_result(1, 1, {7});
        DMatrixT<double> cof2_result(1, 1, {5});
        TEST_E(cof1, cof1_result);
        TEST_E(cof2, cof2_result);

        DMatrixT<double> inv_m(2, 2);
        m.inverse(&inv_m);
        DMatrixT<double> inv_result(2, 2, {
            5, 1,
            9, 7,
        });
        TEST_E(inv_m, inv_result);

        return true;
    };

    TEST_DEF("DMatrix 3x3 determinant, cofactor and inverse.") {
        DMatrixT<double> m(3, 3, {
            5, 1, 4,
            9, 7, 8,
            2, 3, 4
        });
        TEST_NUM_E(m.det(), 26.0);

        DMatrixT<double> cof1(2, 2);
        DMatrixT<double> cof2(2, 2);
        m.cofactor(0, 0, &cof1);
        m.cofactor(1, 1, &cof2);
        DMatrixT<double> cof1_result(2, 2, {
            7, 8,
            3, 4
        });
        DMatrixT<double> cof2_result(2, 2, {
            5, 1,
            9, 7,
        });
        TEST_E(cof1, cof1_result);
        TEST_E(cof2, cof2_result);

        DMatrixT<double> inv_m(3, 3);
        m.inverse(&inv_m);

        DMatrixT<double> e_m(3, 3);
        m.rref(&e_m);

        return true;
    };

    TEST_DEF("DMatrix 5x5 determinant, cofactor and inverse.") {
        DMatrixT<double> m(5, 5, {
            2, 1, 3, 4, 5,
            1, 1, 1, 10, 7,
            4, 8, 9, 5, 1,
            9, 7, 1, 6, 4,
            1, 5, 3, 7, 4,
        });

        DMatrixT<double> cof1(4, 4);
        DMatrixT<double> cof2(4, 4);
        DMatrixT<double> cof3(4, 4);
        m.cofactor(0, 0, &cof1);
        m.cofactor(1, 1, &cof2);
        m.cofactor(2, 2, &cof3);
        auto defr = m.det();

        DMatrixT<double> inv_m(5, 5);
        m.inverse(&inv_m);

        auto ide = m * inv_m;

        auto b1 = utl::is_num_equal(ide(0, 0), 1.0);
        auto b2 = utl::is_num_equal(ide(1, 1), 1.0);
        auto b3 = utl::is_num_equal(ide(2, 2), 1.0);
        auto b4 = utl::is_num_equal(ide(3, 3), 1.0);
        auto b5 = utl::is_num_equal(ide(4, 4), 1.0);

        DMatrixT<double> e_m(5, 5);
        m.rref(&e_m);

        DMatrixT<double> inv2_m(5, 5);
        m.inverse2(&inv2_m);

        auto ide2 = m * inv2_m;
        return true;
    };

    TEST_DEF("DMatrix 3x5 determinant, cofactor and inverse.") {
        DMatrixT<double> m(3, 5, {
            2, 1, 3, 4, 5,
            1, 1, 1, 10, 7,
            4, 8, 9, 5, 1,
        });

        DMatrixT<double> e_m(3, 5);
        m.rref(&e_m);
        return true;
    };

    TEST_DEF("DMatrix rref.") {
        {
            DMatrixT<double> m(1, 2, {
                2, 1
            });
            DMatrixT<double> e_m(1, 2);
            m.rref(&e_m);

            TEST_E(e_m, math::DMatrixT<double>(1, 2, {1, 0.5}));
        }

        {
            DMatrixT<double> m(2, 2, {
                2, 1,
                0, 1,
            });
            DMatrixT<double> e_m(2, 2);
            m.rref(&e_m);

            TEST_E(e_m, math::DMatrixT<double>(2, 2, {
                1, 0,
                0, 1
            }));
        }

        {
            DMatrixT<double> m(2, 2, {
                2, 1,
                1, 1,
            });
            DMatrixT<double> e_m(2, 2);
            m.rref(&e_m);

            TEST_E(e_m, math::DMatrixT<double>(2, 2, {
                1, 0,
                0, 1
            }));
        }

        {
            DMatrixT<double> m(2, 3, {
                2, 1, 6,
                0, 1, 7,
            });
            DMatrixT<double> e_m(2, 3);
            m.rref(&e_m);

            TEST_E(e_m, math::DMatrixT<double>(2, 3, {
                1, 0, -0.5,
                0, 1, 7,
            }));
        }

        {
            DMatrixT<double> m(3, 3, {
                2, 1, 6,
                0, 3, 4,
                6, 1, 7,
            });
            DMatrixT<double> e_m(3, 3);
            m.rref(&e_m);

            TEST_E(e_m, math::DMatrixT<double>(3, 3, {
                1, 0, -0.5,
                0, 1, 7,
            }));
        }
        return true;
    };

    TEST_DEF("DMatrix 10x10 determinant, cofactor and inverse.") {
        DMatrixT<double> m(10, 10, {
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
        });

        DMatrixT<double> e_m(10, 10);
        m.rref(&e_m);

        DMatrixT<double> inv_m(10, 10);
        m.inverse(&inv_m);

        DMatrixT<double> inv2_m(10, 10);
        m.inverse2(&inv2_m);

        auto t1 = m * inv_m;
        auto t2 = m * inv2_m;
        return true;
    };

}