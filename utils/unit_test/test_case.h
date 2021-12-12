// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_UNIT_TEST_TEST_CASE_H_
#define UTILS_UNIT_TEST_TEST_CASE_H_

#include <functional>
#include <string>
#include <vector>

#include "utils/define_utils.hpp"
#include "utils/number.hpp"

#define TEST_CASE_CLASS_START(class_name)  \
    class class_name : public utl::test::TestCase

#define TEST_CASE_CLASS_CONSTUCTOR(class_name)  \
    public:           \
        class_name()  \
            : TestCase(#class_name, utl::test::TestCollector::getInstance())  \
        {defineTests();}

#define TEST_CASE_DEFINE  void defineTests()

#define TEST_CASE(class_name)  \
    TEST_CASE_CLASS_START(class_name) {  \
        TEST_CASE_CLASS_CONSTUCTOR(class_name)  \
        void defineTests();  \
    } ANONYMOUS_VAR(class_name);  \
    void class_name::defineTests()

#define TEST_DEF(description)  \
    *this + TestFunction(description) + [this]() -> bool

#define TEST_TRUE(a)   if (!(a)) return false;
#define TEST_FALSE(a)  if ((a))  return false;
#define TEST_E(a, b)   if ((a) != (b)) return false;
#define TEST_G(a, b)   if ((a) <= (b)) return false;
#define TEST_L(a, b)   if ((a) >= (b)) return false;
#define TEST_GE(a, b)  if ((a) < (b))  return false;
#define TEST_LE(a, b)  if ((a) > (b))  return false;
#define TEST_NE(a, b)  if ((a) == (b)) return false;
#define TEST_NUM_E(a, b)   if (!utl::is_num_equal(a, b)) return false;
#define TEST_NUM_NE(a, b)  if (utl::is_num_equal(a, b))  return false;


namespace utl {
namespace test {

    class TestCollector;

    class TestCase {
    public:
        explicit TestCase(const std::string& name);
        TestCase(
            const std::string& name,
            TestCollector* collector);
        virtual ~TestCase();

        virtual void run();

        virtual bool isSuite() const;

        size_t getFuncCount() const;
        const std::string& getName() const;

    protected:
        struct TestFunction {
            explicit TestFunction(std::string desc) {
                this->desc = std::move(desc);
            }

            template <typename F>
            TestFunction& operator+(F&& f) {
                func = f;
                return *this;
            }

            std::function<bool()> func;
            std::string desc;
        };

        TestFunction& operator+(const TestFunction& f) {
            test_funcs_.push_back(f);
            return test_funcs_.back();
        }

        virtual void onStart() {}
        virtual void onFinished() {}

        void runTests();

        std::vector<TestFunction> test_funcs_;

    private:
        std::string name_;
    };

}
}

#endif  // UTILS_UNIT_TEST_TEST_CASE_H_