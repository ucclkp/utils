// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "test_case.h"

#include "utils/log.h"
#include "utils/unit_test/test_collector.h"

#define UTLOG  UTLog().stream()

namespace {

    class UTLog {
    public:
        UTLog() {}
        ~UTLog() {
            utl::Log::logMessage(
                Log::Severity::INFO, oss_.str());
        }

        std::ostringstream& stream() {
            return oss_;
        }

    private:
        std::ostringstream oss_;
    };

}

namespace utl {
namespace test {

    TestCase::TestCase(const std::string& name)
        : name_(name)
    {}

    TestCase::TestCase(
        const std::string& name, TestCollector* collector)
        : name_(name)
    {
        collector->addCase(this);
    }

    TestCase::~TestCase() {}

    void TestCase::run() {
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
                << "[测] " << Log::vt_format(Log::VT_DEFAULT) << desc;

            if (t.func()) {
                UTLOG << "\r" << Log::vt_format(Log::VT_FG_GREEN)
                    << "[可] " << Log::vt_format(Log::VT_DEFAULT) << desc << "\n";
            } else {
                UTLOG << "\r" << Log::vt_format(Log::VT_FG_RED)
                    << "[否] " << Log::vt_format(Log::VT_DEFAULT) << desc << "\n";
            }
        }

        UTLOG << "\n";
    }

    bool TestCase::isSuite() const {
        return false;
    }

    size_t TestCase::getFuncCount() const {
        return test_funcs_.size();
    }

    const std::string& TestCase::getName() const {
        return name_;
    }


}
}
