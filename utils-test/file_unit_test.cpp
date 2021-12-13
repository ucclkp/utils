// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/files/file_utils.h"
#include "utils/unit_test/test_collector.h"


TEST_CASE(FileUnitTest) {

    TEST_DEF("Get exe path.") {
        TEST_TRUE(!utl::getExecFileName(true).empty());
        TEST_TRUE(!utl::getExecFileName(false).empty());
        return true;
    };

}