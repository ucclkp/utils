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
#include "utils/unit_test/test_collector.h"


int main() {
    utl::Log::Params log_params;
    log_params.file_name = u"utils-debug.log";
    log_params.short_file_name = true;
    log_params.enable_vt = true;
    log_params.target = utl::Log::OutputTarget::STANDARD;
    utl::InitLogging(log_params);

    utl::CommandLine::initialize();

    LOG(Log::INFO) << "utils-test start.";

    LOG(Log::WARNING) << "Test LOG_WARNING";
    LOG(Log::ERR) << "Test LOG_ERR";
    //LOG(Log::FATAL) << "Test LOG_FATAL";
    DBREAK(false);

    //utl::test::TEST_ENDIAN();
    //utl::test::TEST_JSON();

    RUN_TESTS;

    LOG(Log::INFO) << "utils-test exit.\n";

    utl::UninitLogging();

    return 0;
}
