// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "test_collector.h"

#include <memory>


namespace utl {
namespace test {

    std::shared_ptr<TestCollector> g_collector;

    // static
    TestCollector* TestCollector::getInstance() {
        if (!g_collector) {
            g_collector = std::make_shared<TestCollector>();
        }
        return g_collector.get();
    }

    TestCollector::TestCollector() {
    }

    TestCollector::~TestCollector() {
    }

    void TestCollector::run() {
        for (auto child : children_) {
            child->run();
        }
    }

    void TestCollector::addCase(TestCase* c) {
        if (!c) {
            return;
        }
        children_.push_back(c);
    }

    void TestCollector::removeCase(TestCase* c, bool del) {
        for (auto it = children_.begin(); it != children_.end(); ++it) {
            if (*it == c) {
                if (del) {
                    delete c;
                }
                children_.erase(it);
                break;
            }
        }
    }

    void TestCollector::clearCase(bool del) {
        if (del) {
            for (auto child : children_) {
                delete child;
            }
        }
        children_.clear();
    }

    size_t TestCollector::getCaseCount() const {
        return children_.size();
    }

}
}
