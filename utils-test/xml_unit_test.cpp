// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include <sstream>

#include "utils/unit_test/test_collector.h"
#include "utils/xml/xml_parser.h"


const char* test_xml = R"(<?xml version="1.0" encoding="utf-8"?>
<RestraintLayout
    id="@+id/rl_gallery_window_root"
    width="fill"
    height="fill">

    <Button
        id="@+id/bt_gallery_window_back"
        width="auto"
        height="auto"
        text="后退"
        margin="8, 8"
        handle_start="@id/rl_gallery_window_root"
        handle_top="@id/rl_gallery_window_root" />

    <TextView
        id="@+id/tv_gallery_window_name"
        width="fill"
        height="auto"
        text=""
        auto_wrap="false"
        margin="12, 0, 12, 0"
        handle_start="@id/bt_gallery_window_back, end"
        handle_top="@id/bt_gallery_window_back"
        handle_end="@id/rl_gallery_window_root"
        handle_bottom="@id/bt_gallery_window_back" />

    <ListView
        id="@+id/lv_gallery_window_list"
        width="fill"
        height="fill"
        margin="0, 8"
        handle_start="@id/rl_gallery_window_root"
        handle_top="@id/bt_gallery_window_back, bottom"
        handle_end="@id/rl_gallery_window_root"
        handle_bottom="@id/rl_gallery_window_root" />

</RestraintLayout>)";


TEST_CASE(XMLUnitTest) {

    TEST_DEF("XML parser test 1.") {
        std::istringstream iss(test_xml, std::ios::binary);

        utl::XMLParser parser;
        std::shared_ptr<utl::xml::Element> root;
        TEST_TRUE(parser.parse(iss, &root));

        return true;
    };

}