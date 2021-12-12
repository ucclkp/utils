// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_UNIT_TEST_TEST_COLLECTOR_H_
#define UTILS_UNIT_TEST_TEST_COLLECTOR_H_

#include "utils/unit_test/test_suite.h"

#define RUN_TESTS  utl::test::TestCollector::getInstance()->run();


namespace utl {
namespace test {

    class TestCollector {
    public:
        static TestCollector* getInstance();

        TestCollector();
        ~TestCollector();

        void run();

        void addCase(TestCase* c);
        void removeCase(TestCase* c, bool del = true);
        void clearCase(bool del = true);

        size_t getCaseCount() const;

    private:
        std::vector<TestCase*> children_;
    };

}
}

#endif  // UTILS_UNIT_TEST_TEST_COLLECTOR_H_