// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "test_case.h"

#include "utils/log.h"
#include "utils/strings/string_utils_types.hpp"
#include "utils/unit_test/test_collector.h"
#include "utils/unit_test/test_log.hpp"


namespace utl {
namespace test {

    TestCase::TestCase(const std::string_view& name)
        : name_(name)
    {}

    TestCase::TestCase(
        const std::string_view& name, TestCollector* collector)
        : name_(name)
    {
        collector->add(this);
    }

    TestCase::~TestCase() {}

    void TestCase::run() {
        total_ = 0;
        error_ = 0;

        if (test_funcs_.empty()) {
            return;
        }

        onStart();
        runTests();
        onFinished();
    }

    void TestCase::runTests() {
        UTLOG << Log::vt_format(Log::VT_FG_YELLOW)
            << "Test Case: " << name_
            << Log::vt_format(Log::VT_DEFAULT) << "\n";

        for (auto& t : test_funcs_) {
            std::string desc;
            if (t.desc.empty()) {
                desc = "Testing...";
            } else {
                desc = t.desc;
            }

            UTLOG << Log::vt_format(Log::VT_FG_YELLOW)
                << u8p("[测] ") << Log::vt_format(Log::VT_DEFAULT) << desc;
            ++total_;

            if (t.func()) {
                UTLOG << "\r" << Log::vt_format(Log::VT_FG_GREEN)
                    << u8p("[可] ") << Log::vt_format(Log::VT_DEFAULT) << desc << "\n";
            } else {
                UTLOG << "\r" << Log::vt_format(Log::VT_FG_RED)
                    << u8p("[否] ") << Log::vt_format(Log::VT_DEFAULT) << desc << "\n";
                ++error_;
            }
        }

        UTLOG << "\n";
    }

    bool TestCase::isSuite() const {
        return false;
    }

    size_t TestCase::getTotal() const {
        return total_;
    }

    size_t TestCase::getError() const {
        return error_;
    }

    size_t TestCase::getFuncCount() const {
        return test_funcs_.size();
    }

    const std::string& TestCase::getName() const {
        return name_;
    }


}
}
