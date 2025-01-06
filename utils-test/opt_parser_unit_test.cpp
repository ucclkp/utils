// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/opt_parser.hpp"
#include "utils/unit_test/test_collector.h"

#define FEED_CMD0(ret, ...) { \
    const char* argv[] = {__VA_ARGS__};  \
    TEST_E(parser.parse(sizeof(argv) / sizeof(argv[0]), argv), ret); }

#define FEED_CMD(ret, ...) \
    FEED_CMD0(ret, "", __VA_ARGS__)


TEST_CASE(OptParserUnitTest) {

    TEST_DEF("Command line normal tests.") {
        using namespace utl;

        // === utility [operand...] ===

        opt_parser parser("opt-parser", "opt-parser unit tests.", 0);
        // utility --test
        
        TEST_FALSE(parser.is_opt_reg("test"));
        FEED_CMD(opt_parser::rc_unk_opt, "--test");

        // === utility [--test|-t] [operand...] ===

        TEST_TRUE(parser.reg_opt("test", 't'));
        TEST_TRUE(parser.reg_opt("test", 't'));
        // utility --test abc
        FEED_CMD(opt_parser::rc_ok, "--test", "abc");
        TEST_TRUE(parser.unreg_opt("test"));
        TEST_FALSE(parser.unreg_opt("test"));
        FEED_CMD(opt_parser::rc_unk_opt, "--test", "abc");

        auto& r = parser.result();
        TEST_E(r.operands().size(), 1);
        TEST_E(r.operand(0).as_str(), "abc");

        TEST_TRUE(r.has_opt("test"));
        TEST_TRUE(r.has_opt('t'));
        TEST_E(r.opt('t').args.size(), 0);

        parser.clear_all();

        // === utility --input|-i [--output|-o] [--intensity|-s arg] [operand...] ===

        TEST_TRUE(parser.reg_opt("input", 'i', "Input file path.", true));
        TEST_TRUE(parser.reg_opt("output", 'o'));
        TEST_TRUE(parser.reg_opt(
            "intensity", 's', "Intensity of color.", false,
            opt_parser::oat_float, true, false));
        parser.set_operands(1, 2);
        TEST_TRUE(parser.reg_opt("help", 'h', "Show help message."));
        FEED_CMD(opt_parser::rc_req_opt, "-h");
        {
            auto& r = parser.result();
            TEST_TRUE(r.has_opt('h'));
        }
        FEED_CMD(opt_parser::rc_req_opt, "--output", "def", "-s", "0.57");
        FEED_CMD(opt_parser::rc_req_arg, "-i", "abc", "--output", "def", "-s");
        FEED_CMD(opt_parser::rc_par_arg, "-i", "abc", "--output", "def", "-s", "aaa");
        FEED_CMD(opt_parser::rc_min_opr, "-i", "--output", "-s", "0.57");
        FEED_CMD(opt_parser::rc_max_opr, "-i", "abc", "--output", "def", "-s", "0.57", "test");
        FEED_CMD(opt_parser::rc_ok, "-i", "abc", "--output", "def", "-s", "0.57");

        parser.print_help_msg();

        return true;
    };

    TEST_DEF("Command line conflict tests.") {
        using namespace utl;
        {
            opt_parser parser("opt-parser", "opt-parser unit tests.", opt_parser::crv_fail);
            FEED_CMD(opt_parser::rc_unk_opt, "--test");

            TEST_TRUE(parser.reg_opt("test", 't'));
            TEST_FALSE(parser.reg_opt("test", 't'));
            TEST_FALSE(parser.reg_opt("test1", 't'));
            TEST_FALSE(parser.is_opt_reg("test1"));
            TEST_FALSE(parser.reg_opt("test", 'n'));
            TEST_FALSE(parser.is_opt_reg('n'));
        }
        {
            opt_parser parser(
                "opt-parser",
                "opt-parser unit tests.",
                opt_parser::crv_fail | opt_parser::crv_ow);

            TEST_TRUE(parser.reg_opt("test", 't'));
            TEST_FALSE(parser.reg_opt("test", 't'));
            TEST_FALSE(parser.reg_opt("test1", 't'));
            TEST_TRUE(parser.is_opt_reg("test1"));
            TEST_FALSE(parser.reg_opt("test", 'n'));
            TEST_TRUE(parser.is_opt_reg('n'));
        }
        return true;
    };

}