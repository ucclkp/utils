// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/unit_test/test_collector.h"
#include "utils/var.hpp"


TEST_CASE(VarUnitTest) {

    TEST_DEF("Normal tests.") {
        utl::var<std::string, int, long long, double, const char*, std::wstring> v(L"asd");
        auto& test1 = v.get<std::wstring>();
        auto& test2 = v.get<5>();
        TEST_E(test1, L"asd");
        TEST_E(test1, test2);
        return true;
    };

}