// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_JSON_JSON_PARSER_H_
#define UTILS_JSON_JSON_PARSER_H_

#include <memory>
#include <string>

#include "utils/json/json_structs.h"


namespace utl {

    /**
     * JSON 解析实现
     * 参考 http://www.json.org/
     * 以及 https://tools.ietf.org/html/rfc8259
     */
    class JSONParser {
    public:
        using ValuePtr = std::shared_ptr<json::Value>;

        JSONParser();

        bool parse(std::istream& s, ValuePtr* v);

    private:
        bool parseObject(std::istream& s, json::ObjectValue** v);
        bool parseArray(std::istream& s, json::ArrayValue** v);
        bool parseValue(std::istream& s, json::Value** v);
        bool parseNumber(std::istream& s, json::Value** v);
        bool parseString(std::istream& s, std::string* val);
        bool eatWhitespace(std::istream& s);

        bool isDigit(char ch) const;
        bool isDigit1_9(char ch) const;
        bool isHexDigit(char ch) const;
        uint8_t getHexVal(char ch) const;
    };

}

#endif  // UTILS_JSON_JSON_PARSER_H_