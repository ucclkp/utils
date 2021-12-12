// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "test_suite.h"


namespace utl {
namespace test {

    TestSuite::TestSuite(const std::string& name)
        : TestCase(name) {}

    TestSuite::TestSuite(
        const std::string& name,
        TestCollector* collector)
        : TestCase(name, collector)
    {}

    TestSuite::~TestSuite() {}

    void TestSuite::run() {
        if (test_funcs_.empty() && children_.empty()) {
            return;
        }

        onStart();

        if (!test_funcs_.empty()) {
            runTests();
        }

        for (auto child : children_) {
            child.second->run();
        }

        onFinished();
    }

    bool TestSuite::isSuite() const {
        return true;
    }

    void TestSuite::addCase(TestCase* c) {
        if (!c) {
            return;
        }

        auto it = children_.find(c->getName());
        if (it != children_.end()) {
            if (it->second == c) {
                return;
            }
            delete it->second;
        }
        children_[c->getName()] = c;
    }

    void TestSuite::removeCase(TestCase* c, bool del) {
        for (auto it = children_.begin(); it != children_.end(); ++it) {
            if (it->second == c) {
                if (del) {
                    delete c;
                }
                children_.erase(it);
                break;
            }
        }
    }

    TestCase* TestSuite::removeCase(const std::string& name, bool del) {
        auto it = children_.find(name);
        if (it == children_.end()) {
            return nullptr;
        }

        if (del) {
            delete it->second;
            children_.erase(it);
            return nullptr;
        }

        auto c = it->second;
        children_.erase(it);
        return c;
    }

    void TestSuite::clearCase(bool del) {
        if (del) {
            for (auto child : children_) {
                delete child.second;
            }
        }
        children_.clear();
    }

    size_t TestSuite::getCaseCount() const {
        return children_.size();
    }

}
}
