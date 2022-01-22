// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/json/json_parser.h"

#include <cmath>

#include "utils/int_conv.hpp"
#include "utils/float_conv.h"
#include "utils/stream_utils.h"
#include "utils/unicode_conv.h"


namespace utl {

    JSONParser::JSONParser() {
    }

    bool JSONParser::parse(std::istream& s, ValuePtr* v) {
        char ch;
        PEEK_STREAM(ch);
        if (uint8_t(ch) == 0xEF) {
            SKIP_BYTES(1);
            READ_STREAM(ch, 1);
            if (uint8_t(ch) != 0xBB) RET_FALSE;
            READ_STREAM(ch, 1);
            if (uint8_t(ch) != 0xBF) RET_FALSE;
        }

        READ_STREAM(ch, 1);
        if (ch == '{') {
            // object
            json::ObjectValue* obj_val;
            if (!parseObject(s, &obj_val)) {
                return false;
            }
            v->reset(obj_val);
        } else if (ch == '[') {
            // array
            json::ArrayValue* arr_val;
            if (!parseArray(s, &arr_val)) {
                return false;
            }
            v->reset(arr_val);
        } else {
            return false;
        }

        return true;
    }

    bool JSONParser::parseObject(std::istream& s, json::ObjectValue** v) {
        if (!eatWhitespace(s)) return false;

        char ch;
        READ_STREAM(ch, 1);
        auto obj_val = std::make_unique<json::ObjectValue>();
        if (ch != '}') {
            for (;;) {
                if (ch != '"') return false;

                std::string str_val;
                if (!parseString(s, &str_val)) {
                    return false;
                }

                if (!eatWhitespace(s)) return false;
                READ_STREAM(ch, 1);

                if (ch != ':') {
                    return false;
                }

                json::Value* val;
                if (!parseValue(s, &val)) {
                    return false;
                }
                obj_val->put(str_val, val);

                READ_STREAM(ch, 1);
                if (ch == ',') {
                    if (!eatWhitespace(s)) return false;
                    READ_STREAM(ch, 1);
                } else {
                    break;
                }
            }

            if (ch != '}') {
                return false;
            }
        }

        *v = obj_val.release();
        return true;
    }

    bool JSONParser::parseArray(std::istream& s, json::ArrayValue** v) {
        if (!eatWhitespace(s)) return false;

        char ch;
        PEEK_STREAM(ch);
        auto array_val = std::make_unique<json::ArrayValue>();
        if (ch != ']') {
            for (;;) {
                json::Value* val;
                if (!parseValue(s, &val)) {
                    return false;
                }
                array_val->put(val);

                READ_STREAM(ch, 1);
                if (ch == ',') {
                    if (!eatWhitespace(s)) return false;
                } else {
                    break;
                }
            }

            if (ch != ']') {
                return false;
            }
        } else {
            SKIP_BYTES(1);
        }

        *v = array_val.release();
        return true;
    }

    bool JSONParser::parseValue(std::istream& s, json::Value** v) {
        if (!eatWhitespace(s)) return false;

        int tmp;
        PEEK_STREAM(char ch);
        if (ch == '"') {
            SKIP_BYTES(1);
            // string
            std::string str_val;
            if (!parseString(s, &str_val)) {
                return false;
            }
            *v = new json::StringValue(str_val);
        } else if (ch == '{') {
            SKIP_BYTES(1);
            // object
            json::ObjectValue* obj_val;
            if (!parseObject(s, &obj_val)) {
                return false;
            }
            *v = obj_val;
        } else if (ch == '[') {
            SKIP_BYTES(1);
            // array
            json::ArrayValue* array_obj;
            if (!parseArray(s, &array_obj)) {
                return false;
            }
            *v = array_obj;
        } else if ((tmp = startWith<4>(s, "true")) != 0) {
            if (tmp == -1) return false;
            // true;
            *v = new json::BoolValue(true);
        } else if ((tmp = startWith<5>(s, "false")) != 0) {
            if (tmp == -1) return false;
            // false;
            *v = new json::BoolValue(false);
        } else if ((tmp = startWith<4>(s, "null")) != 0) {
            if (tmp == -1) return false;
            // null;
            *v = new json::NullValue();
        } else {
            // may be number
            json::Value* num_val;
            if (!parseNumber(s, &num_val)) {
                return false;
            }
            *v = num_val;
        }

        if (!eatWhitespace(s)) return false;
        return true;
    }

    bool JSONParser::parseNumber(std::istream& s, json::Value** v) {
        std::string str;
        bool is_frac = false;
        char ch;
        READ_STREAM(ch, 1);
        if (ch == '-') {
            str.push_back('-');
            READ_STREAM(ch, 1);
        }

        // integer
        if (ch != '0') {
            if (!utl::isdigit(ch, 10)) return false;
            str.push_back(ch);
            for (;;) {
                PEEK_STREAM(ch);
                if (!utl::isdigit(ch, 10)) break;
                str.push_back(ch);
                SKIP_BYTES(1);
            }
        } else {
            str.push_back(ch);
            PEEK_STREAM(ch);
        }

        // fraction
        if (ch == '.') {
            is_frac = true;
            str.push_back(ch);
            SKIP_BYTES(1);
            PEEK_STREAM(ch);
            if (!utl::isdigit(ch, 10)) return false;
            str.push_back(ch);
            SKIP_BYTES(1);
            for (;;) {
                PEEK_STREAM(ch);
                if (!utl::isdigit(ch, 10)) break;
                str.push_back(ch);
                SKIP_BYTES(1);
            }
        }

        // exponent
        std::string exp_str;
        if (ch == 'e' || ch == 'E') {
            is_frac = true;
            //str.push_back(ch);
            SKIP_BYTES(1);
            PEEK_STREAM(ch);

            if (ch == '+' || ch == '-') {
                exp_str.push_back(ch);
                SKIP_BYTES(1);
                PEEK_STREAM(ch);
            }

            if (!utl::isdigit(ch, 10)) return false;
            exp_str.push_back(ch);
            SKIP_BYTES(1);
            for (;;) {
                PEEK_STREAM(ch);
                if (!utl::isdigit(ch, 10)) break;
                exp_str.push_back(ch);
                SKIP_BYTES(1);
            }
        }

        if (is_frac) {
            double result;
            int ret = utl::stof(str, &result, UFF_SCI);
            if (ret != UCR_OK) {
                return false;
            }
            *v = new json::DoubleValue(result);
        } else {
            int64_t result;
            if (!utl::stoi(str, &result)) {
                return false;
            }
            *v = new json::IntegerValue(result);
        }

        return true;
    }

    bool JSONParser::parseString(std::istream& s, std::string* val) {
        char ch;
        std::string str;

        READ_STREAM(ch, 1);
        std::u16string u16_tmp;
        for (;;) {
            if (ch == '\\') {
                READ_STREAM(ch, 1);
                if (ch != 'u' && !u16_tmp.empty()) {
                    std::string tmp;
                    utf16_to_utf8(u16_tmp, &tmp);
                    str.append(tmp); u16_tmp.clear();
                }

                switch (ch) {
                case '"': str.push_back('\"'); break;
                case '\\': str.push_back('\\'); break;
                case '/': str.push_back('/'); break;
                case 'b': str.push_back('\b'); break;
                case 'f': str.push_back('\f'); break;
                case 'n': str.push_back('\n'); break;
                case 'r': str.push_back('\r'); break;
                case 't': str.push_back('\t'); break;
                case 'u':
                {
                    uint16_t code = 0;

                    // To UTF-16 LE
                    READ_STREAM(ch, 1);
                    if (!utl::isdigit(ch, 16)) return false;
                    code |= uint16_t(utl::ctoi(ch)) << 12;

                    READ_STREAM(ch, 1);
                    if (!utl::isdigit(ch, 16)) return false;
                    code |= uint16_t(utl::ctoi(ch)) << 8;

                    READ_STREAM(ch, 1);
                    if (!utl::isdigit(ch, 16)) return false;
                    code |= uint16_t(utl::ctoi(ch)) << 4;

                    READ_STREAM(ch, 1);
                    if (!utl::isdigit(ch, 16)) return false;
                    code |= uint16_t(utl::ctoi(ch)) << 0;

                    u16_tmp.append({ code });
                    break;
                }
                default: return false;
                }
                READ_STREAM(ch, 1);
            } else if (ch == '"') {
                if (!u16_tmp.empty()) {
                    std::string tmp;
                    utf16_to_utf8(u16_tmp, &tmp);
                    str.append(tmp); u16_tmp.clear();
                }
                break;
            } else {
                if (!u16_tmp.empty()) {
                    std::string tmp;
                    utf16_to_utf8(u16_tmp, &tmp);
                    str.append(tmp); u16_tmp.clear();
                }

                str.push_back(ch);
                READ_STREAM(ch, 1);
            }
        }

        *val = std::move(str);
        return true;
    }

    bool JSONParser::eatWhitespace(std::istream& s) {
        for (;;) {
            PEEK_STREAM(char ch);
            if (ch != ' ' && ch != '\r' && ch != '\n' && ch != '\t') {
                break;
            }
            SKIP_BYTES(1);
        }
        return true;
    }

}
