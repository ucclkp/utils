// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/command_line.h"
#include "utils/unit_test/test_collector.h"


TEST_CASE(CmdlineUnitTest) {

    TEST_DEF("Command line UTF-8 tests.") {
        using namespace utl;

        CommandLine::clear();
        CommandLine::initialize(0, static_cast<char**>(nullptr));
        TEST_TRUE(CommandLine::getMap().empty());

        char path[] = "test";
        {
            // *.exe tst.//\p
            char buf[] = "tst.//\\p";
            char* pbuf[]{ path, buf };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(CommandLine::getMap().empty());
        }

        {
            // *.exe -s123 tst.//\p
            char sw[] = "-s123";
            char buf[] = "tst.//\\p";
            char* pbuf[]{ path, sw, buf };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(CommandLine::getMap().empty());
        }

        {
            // *.exe --s123 tst.//\p
            char sw[] = "--s123";
            char buf[] = "tst.//\\p";
            char* pbuf[]{ path, sw, buf };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(!CommandLine::getMap().empty());
            TEST_E(CommandLine::getValue("s123"), u"tst.//\\p");
        }

        {
            // *.exe -s tst.//\p
            char sw[] = "-s";
            char buf[] = "tst.//\\p";
            char* pbuf[]{ path, sw, buf };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(!CommandLine::getMap().empty());
            TEST_E(CommandLine::getValue("s"), u"tst.//\\p");
        }

        {
            // *.exe -s test -e
            char sw1[] = "-s";
            char buf[] = "test";
            char sw2[] = "-e";
            char* pbuf[]{ path, sw1, buf, sw2 };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(!CommandLine::getMap().empty());
            TEST_E(CommandLine::getValue("s"), u"test");
            TEST_TRUE(CommandLine::hasName("e"));
            TEST_E(CommandLine::getValue("e"), u"");
        }

        {
            // *.exe -s test test2 -e
            char sw1[] = "-s";
            char buf[] = "test";
            char buf2[] = "test2";
            char sw2[] = "-e";
            char* pbuf[]{ path, sw1, buf, buf2, sw2 };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(!CommandLine::getMap().empty());
            TEST_E(CommandLine::getValue("s"), u"test");
            TEST_TRUE(CommandLine::hasName("e"));
            TEST_E(CommandLine::getValue("e"), u"");
        }

        {
            // *.exe -s test test2 -e -f
            char sw1[] = "-s";
            char buf[] = "test";
            char buf2[] = "test2";
            char sw2[] = "-e";
            char sw3[] = "-f";
            char* pbuf[]{ path, sw1, buf, buf2, sw2, sw3 };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(!CommandLine::getMap().empty());
            TEST_E(CommandLine::getValue("s"), u"test");
            TEST_TRUE(CommandLine::hasName("e"));
            TEST_E(CommandLine::getValue("e"), u"");
            TEST_TRUE(CommandLine::hasName("f"));
            TEST_E(CommandLine::getValue("f"), u"");
        }

        return true;
    };

    TEST_DEF("Command line UTF-16 tests.") {
        using namespace utl;

        CommandLine::clear();
        CommandLine::initialize(0, static_cast<char16_t**>(nullptr));
        TEST_TRUE(CommandLine::getMap().empty());

        char16_t path[] = u"test";
        {
            // *.exe tst.//\p
            char16_t buf[] = u"tst.//\\p";
            char16_t* pbuf[]{ path, buf };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(CommandLine::getMap().empty());
        }

        {
            // *.exe -s123 tst.//\p
            char16_t sw[] = u"-s123";
            char16_t buf[] = u"tst.//\\p";
            char16_t* pbuf[]{ path, sw, buf };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(CommandLine::getMap().empty());
        }

        {
            // *.exe --s123 tst.//\p
            char16_t sw[] = u"--s123";
            char16_t buf[] = u"tst.//\\p";
            char16_t* pbuf[]{ path, sw, buf };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(!CommandLine::getMap().empty());
            TEST_E(CommandLine::getValue("s123"), u"tst.//\\p");
        }

        {
            // *.exe -s tst.//\p
            char16_t sw[] = u"-s";
            char16_t buf[] = u"tst.//\\p";
            char16_t* pbuf[]{ path, sw, buf };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(!CommandLine::getMap().empty());
            TEST_E(CommandLine::getValue("s"), u"tst.//\\p");
        }

        {
            // *.exe -s test -e
            char16_t sw1[] = u"-s";
            char16_t buf[] = u"test";
            char16_t sw2[] = u"-e";
            char16_t* pbuf[]{ path, sw1, buf, sw2 };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(!CommandLine::getMap().empty());
            TEST_E(CommandLine::getValue("s"), u"test");
            TEST_TRUE(CommandLine::hasName("e"));
            TEST_E(CommandLine::getValue("e"), u"");
        }

        {
            // *.exe -s test test2 -e
            char16_t sw1[] = u"-s";
            char16_t buf[] = u"test";
            char16_t buf2[] = u"test2";
            char16_t sw2[] = u"-e";
            char16_t* pbuf[]{ path, sw1, buf, buf2, sw2 };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(!CommandLine::getMap().empty());
            TEST_E(CommandLine::getValue("s"), u"test");
            TEST_TRUE(CommandLine::hasName("e"));
            TEST_E(CommandLine::getValue("e"), u"");
        }

        {
            // *.exe -s test test2 -e -f
            char16_t sw1[] = u"-s";
            char16_t buf[] = u"test";
            char16_t buf2[] = u"test2";
            char16_t sw2[] = u"-e";
            char16_t sw3[] = u"-f";
            char16_t* pbuf[]{ path, sw1, buf, buf2, sw2, sw3 };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(!CommandLine::getMap().empty());
            TEST_E(CommandLine::getValue("s"), u"test");
            TEST_TRUE(CommandLine::hasName("e"));
            TEST_E(CommandLine::getValue("e"), u"");
            TEST_TRUE(CommandLine::hasName("f"));
            TEST_E(CommandLine::getValue("f"), u"");
        }

        return true;
    };

    TEST_DEF("Command line WCHAR tests.") {
        using namespace utl;

        CommandLine::clear();
        CommandLine::initialize(0, static_cast<wchar_t**>(nullptr));
        TEST_TRUE(CommandLine::getMap().empty());

        wchar_t path[] = L"test";
        {
            // *.exe tst.//\p
            wchar_t buf[] = L"tst.//\\p";
            wchar_t* pbuf[]{ path, buf };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(CommandLine::getMap().empty());
        }

        {
            // *.exe -s123 tst.//\p
            wchar_t sw[] = L"-s123";
            wchar_t buf[] = L"tst.//\\p";
            wchar_t* pbuf[]{ path, sw, buf };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(CommandLine::getMap().empty());
        }

        {
            // *.exe --s123 tst.//\p
            wchar_t sw[] = L"--s123";
            wchar_t buf[] = L"tst.//\\p";
            wchar_t* pbuf[]{ path, sw, buf };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(!CommandLine::getMap().empty());
            TEST_E(CommandLine::getValue("s123"), u"tst.//\\p");
        }

        {
            // *.exe -s tst.//\p
            wchar_t sw[] = L"-s";
            wchar_t buf[] = L"tst.//\\p";
            wchar_t* pbuf[]{ path, sw, buf };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(!CommandLine::getMap().empty());
            TEST_E(CommandLine::getValue("s"), u"tst.//\\p");
        }

        {
            // *.exe -s test -e
            wchar_t sw1[] = L"-s";
            wchar_t buf[] = L"test";
            wchar_t sw2[] = L"-e";
            wchar_t* pbuf[]{ path, sw1, buf, sw2 };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(!CommandLine::getMap().empty());
            TEST_E(CommandLine::getValue("s"), u"test");
            TEST_TRUE(CommandLine::hasName("e"));
            TEST_E(CommandLine::getValue("e"), u"");
        }

        {
            // *.exe -s test test2 -e
            wchar_t sw1[] = L"-s";
            wchar_t buf[] = L"test";
            wchar_t buf2[] = L"test2";
            wchar_t sw2[] = L"-e";
            wchar_t* pbuf[]{ path, sw1, buf, buf2, sw2 };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(!CommandLine::getMap().empty());
            TEST_E(CommandLine::getValue("s"), u"test");
            TEST_TRUE(CommandLine::hasName("e"));
            TEST_E(CommandLine::getValue("e"), u"");
        }

        {
            // *.exe -s test test2 -e -f
            wchar_t sw1[] = L"-s";
            wchar_t buf[] = L"test";
            wchar_t buf2[] = L"test2";
            wchar_t sw2[] = L"-e";
            wchar_t sw3[] = L"-f";
            wchar_t* pbuf[]{ path, sw1, buf, buf2, sw2, sw3 };
            CommandLine::initialize(std::size(pbuf), pbuf);
            TEST_TRUE(!CommandLine::getMap().empty());
            TEST_E(CommandLine::getValue("s"), u"test");
            TEST_TRUE(CommandLine::hasName("e"));
            TEST_E(CommandLine::getValue("e"), u"");
            TEST_TRUE(CommandLine::hasName("f"));
            TEST_E(CommandLine::getValue("f"), u"");
        }

        return true;
    };

}