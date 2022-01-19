// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/log.h"
#include "utils/unit_test/test_collector.h"


TEST_CASE(LogUnitTest) {

    TEST_DEF("log") {
        DBREAK(true);
        THROW_IF(false);
        LOG(Log::INFO) << "info";
        LOG(Log::WARNING) << "warning";

        jour_i("123: %d", 123);
        dbreak(true);

        //jour_e("ERROR: %d not found!\n", 233);

        return true;
    };

}