// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/math/algebra/matrix.hpp"
#include "utils/math/algebra/dynamic_matrix.hpp"
#include "utils/numbers.hpp"
#include "utils/unit_test/test_collector.h"


using namespace utl::math;

TEST_CASE(DMatrixUnitTest) {

    TEST_DEF("DMatrix 1x1 determinant, cofactor and inverse.") {
        DMatrixT<double> m(1, 1, { 5 });
        TEST_NUM_E(m.det(), 5.0);

        DMatrixT<double> cof(1, 1);
        m.cofactor(0, 0, &cof);
        DMatrixT<double> cof_result(1, 1, {1});
        TEST_E(cof, cof_result);

        DMatrixT<double> ref_m(1, 1);
        m.ref(&ref_m);
        DMatrixT<double> ref_result(1, 1, { 5 });
        TEST_E(ref_m, ref_result);

        DMatrixT<double> inv_m(1, 1);
        m.inverse(&inv_m);
        DMatrixT<double> inv_result(1, 1, { 1 / 5.0 });
        TEST_E(inv_m, inv_result);

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
            0.26923076923076927,  -0.038461538461538464,
            -0.34615384615384615,   0.19230769230769229,
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
        TEST_NUM_E(m.det(), 52.0);

        DMatrixT<double> cof1(2, 2);
        DMatrixT<double> cof2(2, 2);
        m.cofactor(0, 0, &cof1);
        m.cofactor(1, 1, &cof2);
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

        DMatrixT<double> e_m(3, 3);
        m.rref(&e_m);
        auto rref_result = DMatrixT<double>::I(3);
        TEST_E(e_m, rref_result);

        DMatrixT<double> inv_m(3, 3);
        m.inverse(&inv_m);
        DMatrixT<double> inv_result(3, 3, {
            0.076923076923076927, 0.15384615384615385,  -0.38461538461538464,
            -0.38461538461538458, 0.23076923076923073, -0.076923076923076900,
                            0.25,               -0.25,                   0.5
        });
        TEST_E(inv_m, inv_result);

        return true;
    };

    TEST_DEF("DMatrix 5x5 determinant, cofactor and inverse.") {
        DMatrixT<double> m(5, 5, {
            2, 1, 3,  4, 5,
            1, 1, 1, 10, 7,
            4, 8, 9,  5, 1,
            9, 7, 1,  6, 4,
            1, 5, 3,  7, 4,
        });
        TEST_NUM_E(m.det(), -5373.0);

        DMatrixT<double> cof1(4, 4);
        DMatrixT<double> cof2(4, 4);
        DMatrixT<double> cof3(4, 4);
        m.cofactor(0, 0, &cof1);
        m.cofactor(1, 1, &cof2);
        m.cofactor(2, 2, &cof3);
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

        DMatrixT<double> e_m(5, 5);
        m.rref(&e_m);
        auto rref_result = DMatrixT<double>::I(5);
        TEST_E(e_m, rref_result);

        DMatrixT<double> inv_m(5, 5);
        m.inverse(&inv_m);
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

    TEST_DEF("DMatrix 3x5 determinant, cofactor and inverse.") {
        DMatrixT<double> m(3, 5, {
            2, 1, 3,  4, 5,
            1, 1, 1, 10, 7,
            4, 8, 9,  5, 1,
        });

        DMatrixT<double> e_m(3, 5);
        m.rref(&e_m);
        DMatrixT<double> rref_result(3, 5, {
            1, 0, 0,  16, 12,
            0, 1, 0,   5,  2,
            0, 0, 1, -11, -7,
        });
        TEST_E(e_m, rref_result);

        return true;
    };

    TEST_DEF("DMatrix rref.") {
        {
            DMatrixT<double> m(1, 2, {
                2, 1
            });
            DMatrixT<double> e_m(1, 2);
            m.rref(&e_m);

            TEST_E(e_m, DMatrixT<double>(1, 2, {1, 0.5}));
        }

        {
            DMatrixT<double> m(2, 2, {
                2, 1,
                0, 1,
            });
            DMatrixT<double> e_m(2, 2);
            m.rref(&e_m);

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
            DMatrixT<double> e_m(2, 2);
            m.rref(&e_m);

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
            DMatrixT<double> e_m(2, 3);
            m.rref(&e_m);

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
            DMatrixT<double> e_m(3, 3);
            m.rref(&e_m);

            TEST_E(e_m, DMatrixT<double>(3, 3, {
                1, 0, 0,
                0, 1, 0,
                0, 0, 1,
            }));
        }
        return true;
    };

    TEST_DEF("DMatrix 10x10 determinant, cofactor and inverse.") {
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

        DMatrixT<double> e_m(10, 10);
        m.rref(&e_m);
        auto rref_result = DMatrixT<double>::I(10);
        TEST_E(e_m, rref_result);

        DMatrixT<double> inv_m(10, 10);
        m.inverse(&inv_m);
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

}