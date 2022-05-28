// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include <sstream>

#include "utils/stream_utils.h"
#include "utils/unit_test/test_collector.h"


TEST_CASE(StreamUnitTest) {

    TEST_DEF("Stream unit tests.") {
        std::istringstream iss("1", std::ios::binary);
        TEST_E(utl::startWith(iss, "true"), 0);
        TEST_E(utl::startWith(iss, "false"), 0);
        TEST_E(utl::startWith(iss, "123"), 0);
        TEST_E(utl::startWith(iss, "12"), 0);
        TEST_E(utl::startWith(iss, "1"), 1);

        return true;
    };

}