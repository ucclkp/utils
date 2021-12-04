// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/command_line.h"
#include "utils/convert.h"
#include "utils/log.h"

#include "utils-test/json_unit_test.h"
#include "utils-test/endian_unit_test.h"
#include "utils-test/math_unit_test.hpp"


int main() {
    utl::Log::Params log_params;
    log_params.file_name = u"utils-debug.log";
    log_params.short_file_name = true;
    log_params.target = utl::Log::OutputTarget::DEBUGGER | utl::Log::OutputTarget::FILE;
    utl::InitLogging(log_params);

    utl::CommandLine::initialize();

    LOG(Log::INFO) << "utils-test start.";

    //utl::test::TEST_ENDIAN();
    //utl::test::TEST_JSON();

    utl::test::TEST_MATRIX_1x1();
    utl::test::TEST_MATRIX_2x2();
    utl::test::TEST_MATRIX_3x3();
    utl::test::TEST_MATRIX_5x5();
    utl::test::TEST_MATRIX_3x5();
    utl::test::TEST_MATRIX_RREF();
    utl::test::TEST_MATRIX_INVERSE();

    LOG(Log::INFO) << "utils-test exit.\n";

    utl::UninitLogging();

    return 0;
}
