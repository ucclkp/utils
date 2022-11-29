// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/net/uri.hpp"
#include "utils/unit_test/test_collector.h"


TEST_CASE(UriUnitTest) {

    TEST_DEF("URI tests.") {
        using namespace utl;

        uri uri;
        TEST_TRUE(!uri.parse(""));
        TEST_TRUE(!uri.parse("h"));
        TEST_TRUE(!uri.parse("http"));
        TEST_TRUE(!uri.parse("http://["));
        TEST_TRUE(!uri.parse("http://[:"));
        TEST_TRUE(!uri.parse("http://[::"));
        TEST_TRUE(!uri.parse("http://[:::]"));
        TEST_TRUE(!uri.parse("http://[::::]"));
        TEST_TRUE(!uri.parse("http://[00000::]"));
        TEST_TRUE(!uri.parse("http://[0]"));
        TEST_TRUE(!uri.parse("http://[0:]"));
        TEST_TRUE(!uri.parse("http://[0:0]"));
        TEST_TRUE(!uri.parse("http://[0:0:]"));
        TEST_TRUE(!uri.parse("http://[0:0:0]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:0]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:0:]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:0:0]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:0:0:]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:0:0:0:]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:0:0:0:0]"));
        TEST_TRUE(!uri.parse("http://[::0:0:0:0:0:0:0:]"));
        TEST_TRUE(!uri.parse("http://[::0:0:0:0:0:0:0:0]"));
        TEST_TRUE(!uri.parse("http://[0::0:0:0:0:0:0:0]"));
        TEST_TRUE(!uri.parse("http://[0:0::0:0:0:0:0:0]"));
        TEST_TRUE(!uri.parse("http://[0:0:0::0:0:0:0:0]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0::0:0:0:0]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0::0:0:0]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:0::0:0]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:0:0::0]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:0:0:0::]"));

        TEST_TRUE(!uri.parse("http://[1.1.1.1]"));
        TEST_TRUE(!uri.parse("http://[:1.1.1.1]"));
        TEST_TRUE(!uri.parse("http://[0:1.1.1.1]"));
        TEST_TRUE(!uri.parse("http://[0:0:1.1.1.1]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:1.1.1.1]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:1.1.1.1]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:1.1.1.1]"));

        TEST_TRUE(!uri.parse("http://[::0:0:0:0:0:0:1.1.1.1]"));
        TEST_TRUE(!uri.parse("http://[0::0:0:0:0:0:1.1.1.1]"));
        TEST_TRUE(!uri.parse("http://[0:0::0:0:0:0:1.1.1.1]"));
        TEST_TRUE(!uri.parse("http://[0:0:0::0:0:0:1.1.1.1]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0::0:0:1.1.1.1]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0::0:1.1.1.1]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:0::1.1.1.1]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:0:1.1.1.1:]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:0:1.1.1.1::]"));

        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:0:1.0000.1.1]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:11.0000.1.1]"));
        TEST_TRUE(!uri.parse("http://[0:0:0:0:0:1111.0000.1.1]"));

        TEST_TRUE(!uri.parse("http://[::0::]"));

        TEST_TRUE(uri.parse("http:"));
        TEST_TRUE(uri.parse("http:/"));
        TEST_TRUE(uri.parse("http:/a"));
        TEST_TRUE(uri.parse("http:/a/"));
        TEST_TRUE(uri.parse("http://"));
        TEST_TRUE(uri.parse("http://:"));
        TEST_TRUE(uri.parse("http://@"));
        TEST_TRUE(uri.parse("http://@/"));
        TEST_TRUE(uri.parse("http://a"));
        TEST_TRUE(uri.parse("file:///"));

        TEST_TRUE(uri.parse("http://abc.com"));
        TEST_TRUE(uri.parse("http://www.abc.com"));
        TEST_TRUE(uri.parse("http://www.abc.com/"));
        TEST_TRUE(uri.parse("http://www.abc.com:"));
        TEST_TRUE(uri.parse("http://www.abc.com:80"));
        TEST_TRUE(uri.parse("http://www.abc.com:80000000"));
        TEST_TRUE(uri.parse("http://www.abc.com:/"));
        TEST_TRUE(uri.parse("http://www.abc.com:80/"));
        TEST_TRUE(uri.parse("http://www.abc.com:80000000/"));
        TEST_TRUE(uri.parse("http://0.0.0.0"));
        TEST_TRUE(uri.parse("http://1.1.1.1"));
        TEST_TRUE(uri.parse("http://100.10.1.100"));
        TEST_TRUE(uri.parse("http://255."));
        TEST_TRUE(uri.parse("http://255.255.255."));
        TEST_TRUE(uri.parse("http://255.255.255.255"));
        TEST_TRUE(uri.parse("http://256.255.255.255"));
        TEST_TRUE(uri.parse("http://[::]"));
        TEST_TRUE(uri.parse("http://[12::36]"));
        TEST_TRUE(uri.parse("http://[0:0:0:0:0:0:0:0]"));
        TEST_TRUE(uri.parse("http://[::0:0:0:0:0:0:0]"));
        TEST_TRUE(uri.parse("http://[0::0:0:0:0:0:0]"));
        TEST_TRUE(uri.parse("http://[0:0::0:0:0:0:0]"));
        TEST_TRUE(uri.parse("http://[0:0:0::0:0:0:0]"));
        TEST_TRUE(uri.parse("http://[0:0:0:0::0:0:0]"));
        TEST_TRUE(uri.parse("http://[0:0:0:0:0::0:0]"));
        TEST_TRUE(uri.parse("http://[0:0:0:0:0:0::0]"));
        TEST_TRUE(uri.parse("http://[0:0:0:0:0:0:0::]"));

        TEST_TRUE(uri.parse("http://[::0.0.0.0]"));
        TEST_TRUE(uri.parse("http://[0:0:0:0:0:0:1.1.1.1]"));
        TEST_TRUE(uri.parse("http://[::0:0:0:0:0:1.1.1.1]"));
        TEST_TRUE(uri.parse("http://[0::0:0:0:0:1.1.1.1]"));
        TEST_TRUE(uri.parse("http://[0:0::0:0:0:1.1.1.1]"));
        TEST_TRUE(uri.parse("http://[0:0:0::0:0:1.1.1.1]"));
        TEST_TRUE(uri.parse("http://[0:0:0:0::0:1.1.1.1]"));
        TEST_TRUE(uri.parse("http://[0:0:0:0:0::1.1.1.1]"));

        TEST_TRUE(uri.parse("http://@[::]:"));
        TEST_TRUE(uri.parse("http://:@[::]:"));
        TEST_TRUE(uri.parse("http://:a@[::]:"));
        TEST_TRUE(uri.parse("http://a@[::]:"));
        TEST_TRUE(uri.parse("http://ank:ser@[12::36]:443/asdf?qwer#tyu"));

        std::string out;
        uri::removeDotSegs("/a/b/c/./../../g", &out);
        TEST_TRUE(out == "/a/g"); out.clear();
        uri::removeDotSegs("/a/b/c//./../../g", &out);
        TEST_TRUE(out == "/a/b/g"); out.clear();
        uri::removeDotSegs("mid/content=5/../6", &out);
        TEST_TRUE(out == "mid/6"); out.clear();
        uri::removeDotSegs("../../../../g", &out);
        TEST_TRUE(out == "/g"); out.clear();

        return true;
    };

}