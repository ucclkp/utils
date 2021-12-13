// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_UNIT_TEST_TEST_LOG_HPP_
#define UTILS_UNIT_TEST_TEST_LOG_HPP_

#include "utils/log.h"

#define UTLOG  utl::test::UTLog().stream()


namespace utl {
namespace test {

    class UTLog {
    public:
        UTLog() {}
        ~UTLog() {
            Log::logMessage(
                Log::Severity::INFO, oss_.str());
        }

        std::ostringstream& stream() {
            return oss_;
        }

    private:
        std::ostringstream oss_;
    };

}
}

#endif  // UTILS_UNIT_TEST_TEST_LOG_HPP_