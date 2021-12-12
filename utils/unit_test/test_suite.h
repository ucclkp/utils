// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_UNIT_TEST_TEST_SUITE_H_
#define UTILS_UNIT_TEST_TEST_SUITE_H_

#include "utils/unit_test/test_case.h"

#include <map>


namespace utl {
namespace test {

    class TestSuite : public TestCase {
    public:
        explicit TestSuite(const std::string& name);
        TestSuite(
            const std::string& name,
            TestCollector* collector);
        ~TestSuite();

        void run() override;

        bool isSuite() const override;

        void addCase(TestCase* c);
        void removeCase(TestCase* c, bool del = true);
        TestCase* removeCase(const std::string& name, bool del = true);
        void clearCase(bool del = true);

        size_t getCaseCount() const;

    private:
        std::map<std::string, TestCase*> children_;
    };

}
}

#endif  // UTILS_UNIT_TEST_TEST_SUITE_H_