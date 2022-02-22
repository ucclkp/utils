// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/memory/aligments.hpp"
#include "utils/unit_test/test_collector.h"


TEST_CASE(MemoryUnitTest) {

    TEST_DEF("Alignment tests.") {
        TEST_E(utl::align2(0), 0);
        TEST_E(utl::align2(1), 2);
        TEST_E(utl::align2(2), 2);
        TEST_E(utl::align2(3), 4);
        TEST_E(utl::align2(15), 16);
        TEST_E(utl::align2(5489), 5490);

        TEST_E(utl::align4(0), 0);
        TEST_E(utl::align4(1), 4);
        TEST_E(utl::align4(2), 4);
        TEST_E(utl::align4(3), 4);
        TEST_E(utl::align4(4), 4);
        TEST_E(utl::align4(15), 16);
        TEST_E(utl::align4(5489), 5492);

        TEST_E(utl::align8(0), 0);
        TEST_E(utl::align8(1), 8);
        TEST_E(utl::align8(2), 8);
        TEST_E(utl::align8(3), 8);
        TEST_E(utl::align8(8), 8);
        TEST_E(utl::align8(15), 16);
        TEST_E(utl::align8(5489), 5496);

        TEST_E(utl::align16(0), 0);
        TEST_E(utl::align16(1), 16);
        TEST_E(utl::align16(2), 16);
        TEST_E(utl::align16(3), 16);
        TEST_E(utl::align16(8), 16);
        TEST_E(utl::align16(15), 16);
        TEST_E(utl::align16(16), 16);
        TEST_E(utl::align16(5489), 5504);

        TEST_E(utl::align32(0), 0);
        TEST_E(utl::align32(1), 32);
        TEST_E(utl::align32(2), 32);
        TEST_E(utl::align32(3), 32);
        TEST_E(utl::align32(8), 32);
        TEST_E(utl::align32(15), 32);
        TEST_E(utl::align32(16), 32);
        TEST_E(utl::align32(32), 32);
        TEST_E(utl::align32(5489), 5504);

        TEST_E(utl::align64(0), 0);
        TEST_E(utl::align64(1), 64);
        TEST_E(utl::align64(2), 64);
        TEST_E(utl::align64(3), 64);
        TEST_E(utl::align64(8), 64);
        TEST_E(utl::align64(15), 64);
        TEST_E(utl::align64(16), 64);
        TEST_E(utl::align64(32), 64);
        TEST_E(utl::align64(64), 64);
        TEST_E(utl::align64(5489), 5504);

        return true;
    };

}