// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils-test/json_unit_test.h"

#include <fstream>

#include "utils/json/json_parser.h"


namespace utl {
namespace test {

    void TEST_JSON() {
        std::ifstream dau_file("D:\\test.json", std::ios::binary);
        if (dau_file) {
            JSONParser parser;
            JSONParser::ValuePtr value;
            if (parser.parse(dau_file, &value)) {
                auto object = value->asObject();
                json::ArrayValue* list_val;
                if (object->getArray("result", &list_val)) {
                    for (size_t i = 0; i < list_val->getCount(); ++i) {
                        json::ObjectValue* item_val;
                        if (!list_val->getObject(i, &item_val)) {
                            continue;
                        }
                    }
                }
            }
        }
    }

}
}
