// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/math/algebra/dynamic_matrix.hpp"
#include "utils/unit_test/test_collector.h"


using namespace utl::math;

TEST_CASE(DMatrixUnitTest) {

    TEST_DEF("DMatrix 1x1 tests.") {
        using DMatrix = DMatrixT<double>;

        DMatrix m(1, 1, { 5 });
        DMatrix m2(1, 1, { 5 });
        DMatrix m1x3(1, 3, { 1, 2, 3 });

        auto m_f = DMatrixT<float>(m);
        TEST_E(m_f.get(0), 5);

        TEST_E(double(m), 5);

        m.get(0) = 8;
        m.set(0, 5);

        m.add(m2);
        TEST_E(m.get(0), 10);

        m.sub(m2);
        TEST_E(m.get(0), 5);

        TEST_E((m + m2).get(0), 10);
        m += m2;
        TEST_E(m.get(0), 10);
        TEST_E((m - m2).get(0), 5);
        m -= m2;
        TEST_E(m.get(0), 5);

        m.mul(2);
        TEST_E(m.get(0), 10);

        TEST_E((m * m2)(0), 50);
        TEST_E((m * m1x3), DMatrix(1, 3, { 10, 20, 30 }));

        m.div(2);
        TEST_E(m.get(0), 5);

        TEST_E((m * 2).get(0), 10);
        m *= 2;
        TEST_E(m.get(0), 10);
        TEST_E((m / 2).get(0), 5);
        m /= 2;
        TEST_E(m.get(0), 5);

        m.minus();
        TEST_E(m.get(0), -5);

        m = -m;
        TEST_E(m.get(0), 5);

        m.zero();
        TEST_E(m.get(0), 0);

        TEST_E(m, DMatrix::Z(1, 1));

        m.identity();
        TEST_E(m.get(0), 1);

        m(0) = 10;
        TEST_NUM_E(m(0), 10.0);

        m.set(0, 5);

        TEST_NUM_E(m.det(), 5.0);

        auto cof = m.cofactor(0, 0);
        DMatrixT<double> cof_result(1, 1, {1});
        TEST_E(cof, cof_result);

        TEST_E(m.adjoint().get(0), 1);

        TEST_E(m.T().get(0), 5);

        DMatrixT<double> ref_result(1, 1, { 5 });
        TEST_E(m.ref(), ref_result);

        DMatrixT<double> rref_result(1, 1, { 1.0 });
        TEST_E(m.rref(), rref_result);

        DMatrixT<double> inv_result(1, 1, { 1 / 5.0 });
        TEST_E(m.inverse(), inv_result);

        TEST_E(m.row_size(), 1);
        TEST_E(m.col_size(), 1);

        return true;
    };

    TEST_DEF("DMatrix 1x3 tests.") {
        using DMatrix = DMatrixT<double>;

        DMatrix m(1, 3, { 1, 2, 3 });
        DMatrix m2(1, 3, { 4, 5, 6 });

        auto m_f = DMatrixT<float>(m);
        TEST_E(m_f(0), 1);
        TEST_E(m_f(1), 2);
        TEST_E(m_f(2), 3);

        TEST_E(m.get(0), 1);
        m.set(0, 7);
        TEST_E(m.get(0), 7);
        m.set(0, 1);

        m(0) = 7;
        TEST_E(m.get(0), 7);
        m(0) = 1;

        m.add(m2);
        TEST_E(m, DMatrix(1, 3, { 5, 7, 9 }));

        m.sub(m2);
        TEST_E(m, DMatrix(1, 3, { 1, 2, 3 }));

        TEST_E((m + m2), DMatrix(1, 3, { 5, 7, 9 }));
        TEST_E((m - m2), DMatrix(1, 3, { -3, -3, -3 }));

        m += m2;
        TEST_E(m, DMatrix(1, 3, { 5, 7, 9 }));
        m -= m2;
        TEST_E(m, DMatrix(1, 3, { 1, 2, 3 }));

        m.mul(2);
        TEST_E(m, DMatrix(1, 3, { 2, 4, 6 }));

        m.div(2);
        TEST_E(m, DMatrix(1, 3, { 1, 2, 3 }));

        TEST_E(m * 2, DMatrix(1, 3, { 2, 4, 6 }));
        TEST_E(m / 2, DMatrix(1, 3, { 0.5, 1, 1.5 }));

        m *= 2;
        TEST_E(m, DMatrix(1, 3, { 2, 4, 6 }));
        m /= 2;
        TEST_E(m, DMatrix(1, 3, { 1, 2, 3 }));

        DMatrixT<double> m3x3(3, 3, {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9,
        });
        TEST_E(m * m3x3, DMatrix(1, 3, { 30, 36, 42 }));

        m.swapCol(0, 1);
        TEST_E(m, DMatrix(1, 3, { 2, 1, 3 }));

        m.minus();
        TEST_E(m, DMatrix(1, 3, { -2, -1, -3 }));
        TEST_E(-m, DMatrix(1, 3, { 2, 1, 3 }));

        m.zero();
        TEST_E(m, DMatrix(1, 3, { 0, 0, 0 }));
        TEST_E(m, DMatrix::Z(1, 3));

        m = { 1, 2, 3 };

        TEST_E(m.T(), DMatrix(3, 1, { 1, 2, 3 }));

        TEST_E(m.ref(), DMatrix(1, 3, { 1, 2, 3 }));
        TEST_E(m.rref(), DMatrix(1, 3, { 1, 2, 3 }));

        m.mul(2);
        TEST_E(m.ref(), DMatrix(1, 3, { 2, 4, 6 }));
        TEST_E(m.rref(), DMatrix(1, 3, { 1, 2, 3 }));
        m.div(2);

        TEST_E(m.dot(m), 14);

        auto mc = DMatrix(1, 3,
            { 2.0 * 6 - 3 * 5, 3.0 * 4 - 1 * 6, 1.0 * 5 - 2 * 4 });
        TEST_E(m ^ m2, mc);

        m ^= m2;
        TEST_E(m, mc);
        m = { 1, 2, 3 };

        TEST_E(m.lengsq(), 14);
        TEST_E(m.length(), std::sqrt(14));

        m.nor();
        TEST_E(m, DMatrix(1, 3, {
            1 / std::sqrt(14),
            2 / std::sqrt(14),
            3 / std::sqrt(14) }));

        auto m4 = m.reduce(1, 2);
        TEST_E(m4.get(0), 1 / std::sqrt(14));
        TEST_E(m4.get(1), 2 / std::sqrt(14));

        TEST_E(m.row_size(), 1);
        TEST_E(m.col_size(), 3);

        return true;
    };

    TEST_DEF("DMatrix 3x1 tests.") {
        using DMatrix = DMatrixT<double>;

        DMatrix m(3, 1, { 1, 2, 3 });
        DMatrix m2(3, 1, { 4, 5, 6 });

        auto m_f = DMatrixT<float>(m);
        TEST_E(m_f(0), 1);
        TEST_E(m_f(1), 2);
        TEST_E(m_f(2), 3);

        TEST_E(m.get(0), 1);
        m.set(0, 7);
        TEST_E(m.get(0), 7);
        m.set(0, 1);

        m(0) = 7;
        TEST_E(m.get(0), 7);
        m(0) = 1;

        m.add(m2);
        TEST_E(m, DMatrix(3, 1, { 5, 7, 9 }));

        m.sub(m2);
        TEST_E(m, DMatrix(3, 1, { 1, 2, 3 }));

        TEST_E((m + m2), DMatrix(3, 1, { 5, 7, 9 }));
        TEST_E((m - m2), DMatrix(3, 1, { -3, -3, -3 }));

        m += m2;
        TEST_E(m, DMatrix(3, 1, { 5, 7, 9 }));
        m -= m2;
        TEST_E(m, DMatrix(3, 1, { 1, 2, 3 }));

        m.mul(2);
        TEST_E(m, DMatrix(3, 1, { 2, 4, 6 }));

        m.div(2);
        TEST_E(m, DMatrix(3, 1, { 1, 2, 3 }));

        TEST_E(m * 2, DMatrix(3, 1, { 2, 4, 6 }));
        TEST_E(m / 2, DMatrix(3, 1, { 0.5, 1, 1.5 }));

        m *= 2;
        TEST_E(m, DMatrix(3, 1, { 2, 4, 6 }));
        m /= 2;
        TEST_E(m, DMatrix(3, 1, { 1, 2, 3 }));

        DMatrixT<double> m3x3(3, 3, {
            1, 2, 3,
            2, 4, 6,
            3, 6, 9,
        });
        TEST_E(m * m.T(), m3x3);

        m.swapRow(0, 1);
        TEST_E(m, DMatrix(3, 1, { 2, 1, 3 }));

        m.minus();
        TEST_E(m, DMatrix(3, 1, { -2, -1, -3 }));
        TEST_E(-m, DMatrix(3, 1, { 2, 1, 3 }));

        m.zero();
        TEST_E(m, DMatrix(3, 1, { 0, 0, 0 }));
        TEST_E(m, DMatrix::Z(3, 1));

        m = { 1, 2, 3 };

        TEST_E(m.T(), DMatrix(1, 3, { 1, 2, 3 }));

        TEST_E(m.ref(), DMatrix(3, 1, { 1, 0, 0 }));
        TEST_E(m.rref(), DMatrix(3, 1, { 1, 0, 0 }));

        m.mul(2);
        TEST_E(m.ref(), DMatrix(3, 1, { 2, 0, 0 }));
        TEST_E(m.rref(), DMatrix(3, 1, { 1, 0, 0 }));
        m.div(2);

        TEST_E(m.dot(m), 14);

        auto mc = DMatrix(3, 1,
            { 2.0 * 6 - 3 * 5, 3.0 * 4 - 1 * 6, 1.0 * 5 - 2 * 4 });
        TEST_E(m ^ m2, mc);

        m ^= m2;
        TEST_E(m, mc);
        m = { 1, 2, 3 };

        TEST_E(m.lengsq(), 14);
        TEST_E(m.length(), std::sqrt(14));

        m.nor();
        TEST_E(m, DMatrix(3, 1, {
            1 / std::sqrt(14),
            2 / std::sqrt(14),
            3 / std::sqrt(14) }));

        auto m4 = m.reduce(2, 1);
        TEST_E(m4.get(0), 1 / std::sqrt(14));
        TEST_E(m4.get(1), 2 / std::sqrt(14));

        TEST_E(m.row_size(), 3);
        TEST_E(m.col_size(), 1);

        return true;
    };

    TEST_DEF("DMatrix 2x2 tests.") {
        using DMatrix = DMatrixT<double>;
        DMatrixT<double> m(2, 2, {
            5, 1,
            9, 7,
        });
        DMatrixT<double> m2(2, 2, {
            5, 1,
            9, 7,
        });

        auto m_f = DMatrixT<float>(m);
        TEST_E(m_f(0, 0), 5);
        TEST_E(m_f(0, 1), 1);
        TEST_E(m_f(1, 0), 9);
        TEST_E(m_f(1, 1), 7);

        TEST_E(m.get(1, 0), 9);
        m.set(1, 0, 0);
        TEST_E(m.get(1, 0), 0);
        m.set(1, 0, 9);

        m(1, 0) = 0;
        TEST_E(m.get(1, 0), 0);
        m(1, 0) = 9;

        m.add(m2);
        TEST_E(m, DMatrix(2, 2, { 10, 2, 18, 14 }));

        m.sub(m2);
        TEST_E(m, DMatrix(2, 2, { 5, 1, 9, 7 }));

        TEST_E(m + m2, DMatrix(2, 2, { 10, 2, 18, 14 }));
        TEST_E(m - m2, DMatrix(2, 2, { 0, 0, 0, 0 }));

        m += m2;
        TEST_E(m, DMatrix(2, 2, { 10, 2, 18, 14 }));
        m -= m2;
        TEST_E(m, DMatrix(2, 2, { 5, 1, 9, 7 }));

        m.mul(2);
        TEST_E(m, DMatrix(2, 2, { 10, 2, 18, 14 }));

        m.div(2);
        TEST_E(m, DMatrix(2, 2, { 5, 1, 9, 7 }));

        TEST_E(m * 2, DMatrix(2, 2, { 10, 2, 18, 14 }));
        TEST_E(m / 2, DMatrix(2, 2, { 2.5, 0.5, 4.5, 3.5 }));

        m *= 2;
        TEST_E(m, DMatrix(2, 2, { 10, 2, 18, 14 }));
        m /= 2;
        TEST_E(m, DMatrix(2, 2, { 5, 1, 9, 7 }));

        DMatrix m2x2(2, 2, {
            34, 12,
            108, 58,
        });
        TEST_E(m * m, m2x2);

        m.swapCol(0, 1);
        TEST_E(m, DMatrix(2, 2, { 1, 5, 7, 9 }));
        m.swapRow(0, 1);
        TEST_E(m, DMatrix(2, 2, { 7, 9, 1, 5 }));
        m = { 1, 5, 7, 9 };

        m.minus();
        TEST_E(m, DMatrix(2, 2, { -1, -5, -7, -9 }));
        TEST_E(-m, DMatrix(2, 2, { 1, 5, 7, 9 }));

        TEST_E(m.row(1), DMatrix(1, 2, {-7, -9}));
        TEST_E(m.col(1), DMatrix(2, 1, {-5, -9}));
        m.minus();

        m.add_row(1, 1);
        TEST_E(m, DMatrix(2, 2, { 1, 5, 8, 10 }));
        m.sub_row(1, 1);
        TEST_E(m, DMatrix(2, 2, { 1, 5, 7, 9 }));
        m.add_row(1, DMatrix(1, 2, { 1, 2 }));
        TEST_E(m, DMatrix(2, 2, { 1, 5, 8, 11 }));
        m.sub_row(1, DMatrix(1, 2, { 1, 2 }));
        TEST_E(m, DMatrix(2, 2, { 1, 5, 7, 9 }));

        m.mul_row(1, 2);
        TEST_E(m, DMatrix(2, 2, { 1, 5, 14, 18 }));
        m.div_row(1, 2);
        TEST_E(m, DMatrix(2, 2, { 1, 5, 7, 9 }));
        m.mul_row(1, DMatrix(1, 2, { 2, 3 }));
        TEST_E(m, DMatrix(2, 2, { 1, 5, 14, 27 }));
        m.div_row(1, DMatrix(1, 2, { 2, 3 }));
        TEST_E(m, DMatrix(2, 2, { 1, 5, 7, 9 }));

        m.add_col(1, 1);
        TEST_E(m, DMatrix(2, 2, { 1, 6, 7, 10 }));
        m.sub_col(1, 1);
        TEST_E(m, DMatrix(2, 2, { 1, 5, 7, 9 }));
        m.add_col(1, DMatrix(2, 1, { 1, 2 }));
        TEST_E(m, DMatrix(2, 2, { 1, 6, 7, 11 }));
        m.sub_col(1, DMatrix(2, 1, { 1, 2 }));
        TEST_E(m, DMatrix(2, 2, { 1, 5, 7, 9 }));

        m.mul_col(1, 2);
        TEST_E(m, DMatrix(2, 2, { 1, 10, 7, 18 }));
        m.div_col(1, 2);
        TEST_E(m, DMatrix(2, 2, { 1, 5, 7, 9 }));
        m.mul_col(1, DMatrix(2, 1, { 2, 3 }));
        TEST_E(m, DMatrix(2, 2, { 1, 10, 7, 27 }));
        m.div_col(1, DMatrix(2, 1, { 2, 3 }));
        TEST_E(m, DMatrix(2, 2, { 1, 5, 7, 9 }));

        m.zero();
        TEST_E(m, DMatrix(2, 2, { 0, 0, 0, 0 }));
        TEST_E(DMatrix::Z(2, 2), DMatrix(2, 2, { 0, 0, 0, 0 }));

        m.identity();
        TEST_E(m, DMatrix(2, 2, { 1, 0, 0, 1 }));
        TEST_E(DMatrix::I(2), DMatrix(2, 2, { 1, 0, 0, 1 }));

        m = { 5, 1, 9, 7 };

        TEST_NUM_E(m.det(), 26.0);

        DMatrixT<double> cof1_result(1, 1, {7});
        DMatrixT<double> cof2_result(1, 1, {5});
        TEST_E(m.cofactor(0, 0), cof1_result);
        TEST_E(m.cofactor(1, 1), cof2_result);

        TEST_E(m.adjoint(), DMatrix(2, 2, { 7, -1, -9, 5 }));

        TEST_E(m.T(), DMatrix(2, 2, { 5, 9, 1, 7 }));

        TEST_E(m.ref(), DMatrix(2, 2, { 5, 1, 0, 7 - 9 / 5.0 }));
        TEST_E(m.rref(), DMatrix(2, 2, { 1, 0, 0, 1 }));

        auto red_m = m.reduce(1, 1);
        TEST_NUM_E(red_m.get(0), 5.0);

        DMatrixT<double> inv_m = m.inverse();
        DMatrixT<double> inv_result(2, 2, {
            0.26923076923076927,  -0.038461538461538464,
            -0.34615384615384615,   0.19230769230769229,
        });
        TEST_E(inv_m, inv_result);

        TEST_E(m.row_size(), 2);
        TEST_E(m.col_size(), 2);

        return true;
    };

    TEST_DEF("DMatrix 3x3 tests.") {
        DMatrixT<double> m(3, 3, {
            5, 1, 4,
            9, 7, 8,
            2, 3, 4
        });
        TEST_NUM_E(m.det(), 52.0);

        DMatrixT<double> cof1 = m.cofactor(0, 0);
        DMatrixT<double> cof2 = m.cofactor(1, 1);

        DMatrixT<double> cof1_result(2, 2, {
            7, 8,
            3, 4
        });
        DMatrixT<double> cof2_result(2, 2, {
            5, 4,
            2, 4,
        });
        TEST_E(cof1, cof1_result);
        TEST_E(cof2, cof2_result);

        DMatrixT<double> e_m = m.rref();
        auto rref_result = DMatrixT<double>::I(3);
        TEST_E(e_m, rref_result);

        DMatrixT<double> inv_m = m.inverse();
        DMatrixT<double> inv_result(3, 3, {
            0.076923076923076927, 0.15384615384615385,  -0.38461538461538464,
            -0.38461538461538458, 0.23076923076923073, -0.076923076923076900,
                            0.25,               -0.25,                   0.5
        });
        TEST_E(inv_m, inv_result);

        return true;
    };

    TEST_DEF("DMatrix 5x5 tests.") {
        DMatrixT<double> m(5, 5, {
            2, 1, 3,  4, 5,
            1, 1, 1, 10, 7,
            4, 8, 9,  5, 1,
            9, 7, 1,  6, 4,
            1, 5, 3,  7, 4,
        });
        TEST_NUM_E(m.det(), -5373.0);

        DMatrixT<double> cof1 = m.cofactor(0, 0);
        DMatrixT<double> cof2 = m.cofactor(1, 1);
        DMatrixT<double> cof3 = m.cofactor(2, 2);

        DMatrixT<double> cof1_result(4, 4, {
            1, 1, 10, 7,
            8, 9,  5, 1,
            7, 1,  6, 4,
            5, 3,  7, 4,
        });
        DMatrixT<double> cof2_result(4, 4, {
            2, 3, 4, 5,
            4, 9, 5, 1,
            9, 1, 6, 4,
            1, 3, 7, 4,
        });
        DMatrixT<double> cof3_result(4, 4, {
            2, 1,  4, 5,
            1, 1, 10, 7,
            9, 7,  6, 4,
            1, 5,  7, 4,
        });
        TEST_E(cof1, cof1_result);
        TEST_E(cof2, cof2_result);
        TEST_E(cof3, cof3_result);

        DMatrixT<double> e_m = m.rref();
        auto rref_result = DMatrixT<double>::I(5);
        TEST_E(e_m, rref_result);

        DMatrixT<double> inv_m = m.inverse();
        DMatrixT<double> inv_result(5, 5, {
            -0.018425460636516622,  0.10552763819095379, 0.077051926298157880,    0.10050251256281409, -0.28140703517587923,
             0.029964638004839728, -0.24232272473478433, -0.11520565791922555,   0.028475711892797309,  0.38693467336683396,
             0.086357714498418225, 0.040759352317141673,  0.13381723431974657,  -0.067001675041876055, -0.14572864321608048,
             -0.26018983807928536,  0.27805695142378561,  0.11836962590731431, -0.0050251256281407036, -0.18592964824120603,
              0.35771449841801606, -0.24064768285873814, -0.18276568025311743, -0.0016750418760469012,  0.27135678391959800
        });
        TEST_E(inv_m, inv_result);

        return true;
    };

    TEST_DEF("DMatrix 3x5 tests.") {
        using DMatrix = DMatrixT<double>;

        DMatrixT<double> m(3, 5, {
            2, 1, 3,  4, 5,
            1, 1, 1, 10, 7,
            4, 8, 9,  5, 1,
        });
        DMatrixT<double> m2(3, 5, {
            2, 1, 3,  4, 5,
            1, 1, 1, 10, 7,
            4, 8, 9,  5, 1,
        });

        auto m_f = DMatrixT<float>(m);
        TEST_E(m_f(0, 0), 2);
        TEST_E(m_f(0, 1), 1);
        TEST_E(m_f(0, 2), 3);
        TEST_E(m_f(0, 3), 4);
        TEST_E(m_f(0, 4), 5);
        TEST_E(m_f(1, 0), 1);
        TEST_E(m_f(1, 1), 1);
        TEST_E(m_f(1, 2), 1);
        TEST_E(m_f(1, 3), 10);
        TEST_E(m_f(1, 4), 7);
        TEST_E(m_f(2, 0), 4);
        TEST_E(m_f(2, 1), 8);
        TEST_E(m_f(2, 2), 9);
        TEST_E(m_f(2, 3), 5);
        TEST_E(m_f(2, 4), 1);

        TEST_E(m.get(1, 1), 1);
        m.set(1, 1, 0);
        TEST_E(m.get(1, 1), 0);
        m.set(1, 1, 1);

        m(1, 0) = 12;
        TEST_E(m.get(1, 0), 12);
        m(1, 0) = 1;

        m.add(m2);
        TEST_E(m, DMatrix(3, 5, {
            4, 2, 6,  8, 10,
            2, 2, 2, 20, 14,
            8, 16, 18, 10, 2,
            }));
        m.sub(m2);
        TEST_E(m, DMatrix(3, 5, {
            2, 1, 3,  4, 5,
            1, 1, 1, 10, 7,
            4, 8, 9,  5, 1,
            }));

        TEST_E(m + m2, DMatrix(3, 5, {
            4, 2, 6,  8, 10,
            2, 2, 2, 20, 14,
            8, 16, 18, 10, 2,
            }));
        m += m2;
        TEST_E(m, DMatrix(3, 5, {
            4, 2, 6,  8, 10,
            2, 2, 2, 20, 14,
            8, 16, 18, 10, 2,
            }));
        TEST_E(m - m2, DMatrix(3, 5, {
            2, 1, 3,  4, 5,
            1, 1, 1, 10, 7,
            4, 8, 9,  5, 1,
            }));
        m -= m2;
        TEST_E(m, DMatrix(3, 5, {
            2, 1, 3,  4, 5,
            1, 1, 1, 10, 7,
            4, 8, 9,  5, 1,
            }));

        m.mul(2);
        TEST_E(m, DMatrix(3, 5, {
            4, 2, 6,  8, 10,
            2, 2, 2, 20, 14,
            8, 16, 18, 10, 2,
            }));
        m.div(2);
        TEST_E(m, DMatrix(3, 5, {
            2, 1, 3,  4, 5,
            1, 1, 1, 10, 7,
            4, 8, 9,  5, 1,
            }));

        TEST_E(m * 2, DMatrix(3, 5, {
            4, 2, 6,  8, 10,
            2, 2, 2, 20, 14,
            8, 16, 18, 10, 2,
            }));
        m *= 2;
        TEST_E(m, DMatrix(3, 5, {
            4, 2, 6,  8, 10,
            2, 2, 2, 20, 14,
            8, 16, 18, 10, 2,
            }));
        TEST_E(m / 2, DMatrix(3, 5, {
            2, 1, 3,  4, 5,
            1, 1, 1, 10, 7,
            4, 8, 9,  5, 1,
            }));
        m /= 2;
        TEST_E(m, DMatrix(3, 5, {
            2, 1, 3,  4, 5,
            1, 1, 1, 10, 7,
            4, 8, 9,  5, 1,
            }));

        DMatrixT<double> m5x1(5, 1, {
            1, 2, 3, 4, 5
        });
        TEST_E(m * m5x1, DMatrix(3, 1, { 54, 81, 72 }));

        m.swapCol(1, 2);
        TEST_E(m, DMatrix(3, 5, {
            2, 3, 1,  4, 5,
            1, 1, 1, 10, 7,
            4, 9, 8,  5, 1,
        }));

        m.swapRow(1, 2);
        TEST_E(m, DMatrix(3, 5, {
            2, 3, 1,  4, 5,
            4, 9, 8,  5, 1,
            1, 1, 1, 10, 7,
        }));

        m = {
            2, 3, 1,  4, 5,
            1, 1, 1, 10, 7,
            4, 9, 8,  5, 1,
        };

        m.minus();
        TEST_E(m, DMatrix(3, 5, {
            -2, -3, -1,  -4, -5,
            -1, -1, -1, -10, -7,
            -4, -9, -8,  -5, -1,
        }));
        TEST_E(-m, DMatrix(3, 5, {
            2, 3, 1,  4, 5,
            1, 1, 1, 10, 7,
            4, 9, 8,  5, 1,
        }));

        TEST_E(m.row(1), DMatrix(1, 5, {-1, -1, -1, -10, -7}));
        TEST_E(m.col(1), DMatrix(3, 1, {-3, -1, -9}));

        m.mul_row(1, 2);
        TEST_E(m, DMatrix(3, 5, {
            -2, -3, -1,  -4, -5,
            -2, -2, -2, -20, -14,
            -4, -9, -8,  -5, -1,
        }));
        m = {
            -2, -3, -1,  -4, -5,
            -1, -1, -1, -10, -7,
            -4, -9, -8,  -5, -1,
        };
        m.mul_col(1, 2);
        TEST_E(m, DMatrix(3, 5, {
            -2, -6, -1,  -4, -5,
            -1, -2, -1, -10, -7,
            -4, -18, -8,  -5, -1,
        }));

        m.zero();
        TEST_E(m, DMatrix(3, 5, {
            0, 0, 0, 0, 0,
            0, 0, 0, 0, 0,
            0, 0, 0, 0, 0,
            }));
        TEST_E(m, DMatrix::Z(3, 5));

        m = {
            2, 1, 3,  4, 5,
            1, 1, 1, 10, 7,
            4, 8, 9,  5, 1,
        };

        DMatrix m5x3(5, 3, {
            2, 1, 4,
            1, 1, 8,
            3, 1, 9,
            4, 10, 5,
            5, 7, 1
        });
        TEST_E(m.T(), m5x3);

        DMatrix ref_result(3, 5, {
            2, 1,   3,    4,   5,
            0, 0.5, -0.5, 8,   4.5,
            0, 0,   9,    -99, -63,
        });
        TEST_E(m.ref(), ref_result);

        DMatrixT<double> e_m = m.rref();
        DMatrixT<double> rref_result(3, 5, {
            1, 0, 0,  16, 12,
            0, 1, 0,   5,  2,
            0, 0, 1, -11, -7,
        });
        TEST_E(e_m, rref_result);

        TEST_E(m.row_size(), 3);
        TEST_E(m.col_size(), 5);

        return true;
    };

    TEST_DEF("DMatrix 10x10 tests.") {
        DMatrixT<double> m(10, 10, {
             2,  1,  6, 8, 6,  0,  3,  3,  8,  1,
             0,  3,  4, 3, 7,  9,  5,  1,  0,  3,
             6,  1,  7, 8, 6,  4,  9,  5,  2,  5,
             0,  0,  0, 7, 9,  5,  7,  4,  0,  8,
             6,  9,  6, 4, 8,  2,  7,  5,  1,  1,
             9,  9,  0, 0, 0,  7,  7,  8,  5,  4,
             9,  8,  7, 6, 5,  4,  3,  2,  1,  0,
             7,  9,  8, 8, 9,  8,  9,  6,  1,  1,
             8,  8,  8, 3, 3,  4,  2,  3,  7,  6,
            97, 43, 87, 1, 0, 91, 87, 46, 78, 13
        });
        TEST_NUM_E(m.det(), 228902704.0);

        DMatrixT<double> e_m = m.rref();
        auto rref_result = DMatrixT<double>::I(10);
        TEST_E(e_m, rref_result);

        DMatrixT<double> inv_m = m.inverse();
        DMatrixT<double> inv_result(10, 10, {
            -0.31147874513531248,  -1.1127240724950069, -0.93120523818713574,  0.99511547928240951, -0.19129177696389144, -0.61011800891613643, -0.0072943393451558869,  0.80103728700382471,   0.21553447442018756,  0.067867153723094403,
             0.52547161260270348,   1.9314784983929181,   1.5868456800754933,  -1.6505907592948290,  0.43350122679197023,   1.1264020804227779,    0.35948815178696925,  -1.6875661503762664,  -0.47209721035012192,  -0.11295150536972227,
            -0.23842530055913913, -0.68344117070805677, -0.48578143489296677,  0.46371855878120138, -0.16412099701539454, -0.46544634964207265,   -0.29200406474883722,  0.75819090367757003,   0.30171058180247612,  0.035796885999214711,
             0.45586675551023448,   1.4571346566530698,   1.3009006263202536,  -1.2690167259885214,  0.18057694504124119,  0.88077866480773259,    0.34092407226434424,  -1.2488323379526303,  -0.40971374457856907, -0.090478219077743921,
            -0.47530899853415254,  -1.8066048228071561,  -1.6588884375957362,   1.6543645198704136, -0.26048370752317251,  -1.0889874241066171,   -0.27653610417813029,   1.5238173726423021,   0.43811050829700843,   0.10912326749971443,
            -0.18865392695404695, -0.60340525291479197, -0.59479988493276970,  0.55519225321165211, -0.25493524095722242, -0.35859862974794682,   -0.12190257044757280,  0.67220321696155938,   0.17566506335372900,  0.038035942118010063,
             0.75838166595008594,   2.7496772340443787,   2.3435844165475626,  -2.2729003673106432,  0.69410120205482184,   1.5656650172205895,    0.57792098427985217,  -2.5939561902248149,  -0.83341421777175495,  -0.14649632972444032,
            -0.94466844742908407,  -3.4212503404940087,  -2.7603154832107162,   2.7137970025902334, -0.80088126001342008,  -1.8728547828775297,   -0.92028426191068324,   3.3087859634895280,   0.99462688741326311,   0.17810568983055777,
             0.27866497374360349,  0.64408302489952152,  0.44466743389802799, -0.48472731016755416,  0.16061990250669900,  0.38134404912927489,    0.16532423312919850, -0.66675176541383108,  -0.19894810853785236, -0.029383724536517459,
            0.062498182633962431,  0.43922347461653216,  0.42923274947420309, -0.34087628776984513, 0.087475947859487910,  0.24502416537639393,   0.043444930209299272, -0.42637457441306370, -0.012241463080313143, -0.028236254474302676
        });
        TEST_E(inv_m, inv_result);

        return true;
    };

    TEST_DEF("DMatrix rref.") {
        {
            DMatrixT<double> m(1, 2, {
                2, 1
            });
            DMatrixT<double> e_m = m.rref();

            TEST_E(e_m, DMatrixT<double>(1, 2, {1, 0.5}));
        }

        {
            DMatrixT<double> m(2, 2, {
                2, 1,
                0, 1,
            });
            DMatrixT<double> e_m = m.rref();

            TEST_E(e_m, DMatrixT<double>(2, 2, {
                1, 0,
                0, 1
            }));
        }

        {
            DMatrixT<double> m(2, 2, {
                2, 1,
                1, 1,
            });
            DMatrixT<double> e_m = m.rref();

            TEST_E(e_m, DMatrixT<double>(2, 2, {
                1, 0,
                0, 1
            }));
        }

        {
            DMatrixT<double> m(2, 3, {
                2, 1, 6,
                0, 1, 7,
            });
            DMatrixT<double> e_m = m.rref();

            TEST_E(e_m, DMatrixT<double>(2, 3, {
                1, 0, -0.5,
                0, 1,    7,
            }));
        }

        {
            DMatrixT<double> m(3, 3, {
                2, 1, 6,
                0, 3, 4,
                6, 1, 7,
            });
            DMatrixT<double> e_m = m.rref();

            TEST_E(e_m, DMatrixT<double>(3, 3, {
                1, 0, 0,
                0, 1, 0,
                0, 0, 1,
            }));
        }
        return true;
    };

}