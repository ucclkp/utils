// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/xml/xml_parser.h"

#include <cassert>

#include "utils/log.h"
#include "utils/stream_utils.h"
#include "utils/strings/string_utils.hpp"

#define ADV_PEDO(adv)  \
    pedometer_.step(adv);

#define LOOP_S() {  \
    for (;;) {  \
        char l_buf = s.peek();     \
        if (!s.good()) RET_FALSE  \
        if (!isSpace(l_buf)) break;  \
        pedometer_.space(l_buf);  \
        SKIP_BYTES(1);  \
    }  \
}

#define ONE_LOOP_S() {  \
    GET_STREAM(char l_buf);  \
    if (!isSpace(l_buf)) RET_FALSE;  \
    pedometer_.space(l_buf);  \
    LOOP_S();  \
}


namespace utl {

    XMLParser::XMLParser()
        : doc_stepper_(DocStepper::None) {}

    bool XMLParser::parse(std::istream& s, std::shared_ptr<Element>* out) {
        assert(out && !*out);

        prolog_.version.clear();
        prolog_.charset.clear();
        doc_stepper_ = DocStepper::None;
        pedometer_.reset();

        // UTF-8 BOM
        char buf;
        PEEK_STREAM(buf);
        if (uint8_t(buf) == 0xEF) {
            SKIP_BYTES(1);
            GET_STREAM(buf);
            if (uint8_t(buf) != 0xBB) RET_FALSE;
            GET_STREAM(buf);
            if (uint8_t(buf) != 0xBF) RET_FALSE;
        }

        std::string char_data;
        auto cur_element = out->get();

        for (;;) {
            // 读到末尾，结束
            auto peek_ch = s.peek();
            if (!s) {
                return false;
            }
            if (peek_ch == EOF) {
                break;
            }

            GET_STREAM(buf);
            if (buf == '<') {
                if (!char_data.empty()) {
                    Content content;
                    content.type = Content::Type::CharData;
                    content.char_data = char_data;
                    cur_element->contents.push_back(content);
                    char_data.clear();
                }

                PEEK_STREAM(buf);
                if (buf == '?') {
                    QuesTagType type;
                    ADV_PEDO(2);
                    SKIP_BYTES(1);
                    if (!parseQuesTag(s, &type)) RET_FALSE;
                    if (type == QuesTagType::Prolog) {
                        doc_stepper_ = DocStepper::Prolog;
                    }
                } else if (buf == '!') {
                    ExclTagType type;
                    ADV_PEDO(2);
                    SKIP_BYTES(1);
                    if (!parseExclTag(s, &type)) RET_FALSE;
                } else {
                    if (doc_stepper_ == DocStepper::Misc) RET_FALSE;

                    NormTagType type;
                    auto tmp = std::make_shared<Element>();
                    ADV_PEDO(1);
                    if (!parseNormTag(s, &type, tmp.get())) return false;
                    doc_stepper_ = DocStepper::Elements;
                    switch (type) {
                    case NormTagType::StartTag:
                        if (cur_element) {
                            tmp->parent = cur_element;
                            Content content;
                            content.type = Content::Type::Element;
                            content.element = tmp;
                            cur_element->contents.push_back(content);
                        } else {
                            *out = tmp;
                        }
                        cur_element = tmp.get();
                        break;
                    case NormTagType::EndTag:
                        if (!cur_element) RET_FALSE;
                        if (cur_element->tag_name != tmp->tag_name) RET_FALSE;
                        cur_element = cur_element->parent;
                        if (!cur_element) {
                            doc_stepper_ = DocStepper::Misc;
                        }
                        break;
                    case NormTagType::EmptyTag:
                        if (cur_element) {
                            tmp->parent = cur_element;
                            Content content;
                            content.type = Content::Type::Element;
                            content.element = tmp;
                            cur_element->contents.push_back(content);
                        } else {
                            *out = tmp;
                            doc_stepper_ = DocStepper::Misc;
                        }
                        break;
                    }
                }
            } else {
                if (doc_stepper_ == DocStepper::None) {
                    RET_FALSE;
                }
                if (doc_stepper_ == DocStepper::Prolog) {
                    if (!isSpace(buf)) RET_FALSE;
                    pedometer_.space(buf);
                } else if (doc_stepper_ == DocStepper::Elements) {
                    char_data.push_back(buf);
                    if (isSpace(buf)) {
                        pedometer_.space(buf);
                    } else {
                        pedometer_.step(1);
                    }
                } else if(doc_stepper_ == DocStepper::Misc){
                    if (!isSpace(buf) && buf != 0) RET_FALSE;
                    if (isSpace(buf)) {
                        pedometer_.space(buf);
                    } else {
                        pedometer_.step(1);
                    }
                }
            }
        }

        if (cur_element) RET_FALSE;

        return true;
    }

    const XMLParser::Prolog& XMLParser::getProlog() const {
        return prolog_;
    }

    const XMLPedometer& XMLParser::getPedometer() const {
        return pedometer_;
    }

    bool XMLParser::parseQuesTag(std::istream& s, QuesTagType* type) {
        char buf;
        int result = startWith(s, "xml");
        if (result < 0) RET_FALSE;
        bool is_prolog = result == 1;

        if (is_prolog) {
            if (doc_stepper_ != DocStepper::None) RET_FALSE;
            *type = QuesTagType::Prolog;
            // Prolog
            ADV_PEDO(3);

            // VersionInfo
            ONE_LOOP_S();
            NEXT_EQUAL("version", 7);
            ADV_PEDO(7);
            if (!eatEq(s)) return false;
            GET_STREAM(char cur_sign);
            if (cur_sign != '\"' && cur_sign != '\'') RET_FALSE;
            ADV_PEDO(1);
            NEXT_EQUAL("1.", 2);
            ADV_PEDO(2);

            for (;;) {
                GET_STREAM(buf);
                ADV_PEDO(1);
                if (isDigit(buf)) prolog_.version.push_back(buf);
                else break;
            }
            if (buf != cur_sign) RET_FALSE;

            pedometer_.save();
            auto prev_pos = s.tellg();
            if (!parseEncodingDecl(s)) {
                pedometer_.restore();
                s.seekg(prev_pos);
            } else {
                pedometer_.discard();
            }
            pedometer_.save();
            prev_pos = s.tellg();
            if (!parseSDDecl(s)) {
                pedometer_.restore();
                s.seekg(prev_pos);
            } else {
                pedometer_.discard();
            }

            LOOP_S();
            NEXT_EQUAL("?>", 2);
            ADV_PEDO(2);
        } else {
            *type = QuesTagType::PIs;
            // PIs
            for (;;) {
                GET_STREAM(buf);
                if (buf == '?') {
                    PEEK_STREAM(char l_buf);
                    if (l_buf == '>') {
                        break;
                    }
                }

                if (isSpace(buf)) {
                    pedometer_.space(buf);
                    // TODO:
                } else {
                    ADV_PEDO(1);
                }
            }
            ADV_PEDO(1);
        }
        return true;
    }

    bool XMLParser::parseExclTag(std::istream& s, ExclTagType* type) {
        char buf;
        int result;
        PEEK_STREAM(buf);
        if (buf == '[') {
            ADV_PEDO(1);
            SKIP_BYTES(1);

            // <![CDATA[...]]>
            result = startWith(s, "CDATA[");
            if (result < 0) RET_FALSE;
            if (result == 1) {
                if (doc_stepper_ == DocStepper::Misc) RET_FALSE;
                *type = ExclTagType::CDATA;
                ADV_PEDO(6);
                for (;;) {
                    GET_STREAM(buf);
                    if (buf == ']') {
                        result = startWith(s, "]>");
                        if (result < 0) RET_FALSE;
                        if (result == 1) break;
                    }

                    // TODO:
                    if (isSpace(buf)) {
                        pedometer_.space(buf);
                    } else {
                        ADV_PEDO(1);
                    }
                }
                return true;
            }

            // <![...]>
            if (doc_stepper_ == DocStepper::Misc) RET_FALSE;
            *type = ExclTagType::COND_SECT;
            for (;;) {
                GET_STREAM(buf);
                if (buf == ']') {
                    result = startWith(s, "]>");
                    if (result < 0) RET_FALSE;
                    if (result == 1) break;
                }

                // TODO:
                if (isSpace(buf)) {
                    pedometer_.space(buf);
                } else {
                    ADV_PEDO(1);
                }
            }
            return true;
        }

        // <!DOCTYPE...>
        result = startWith(s, "DOCTYPE");
        if (result < 0) RET_FALSE;
        if (result == 1) {
            if (doc_stepper_ == DocStepper::Misc) RET_FALSE;
            *type = ExclTagType::DOCTYPE;
            ADV_PEDO(7);
            for (;;) {
                GET_STREAM(buf);
                if (buf == '>') break;

                // TODO:
                if (isSpace(buf)) {
                    pedometer_.space(buf);
                } else {
                    ADV_PEDO(1);
                }
            }
            return true;
        }

        // <!ENTITY...>
        result = startWith(s, "ENTITY");
        if (result < 0) RET_FALSE;
        if (result == 1) {
            if (doc_stepper_ == DocStepper::Misc) RET_FALSE;
            *type = ExclTagType::ENTITY;
            ADV_PEDO(6);
            for (;;) {
                GET_STREAM(buf);
                if (buf == '>') break;

                // TODO:
                if (isSpace(buf)) {
                    pedometer_.space(buf);
                } else {
                    ADV_PEDO(1);
                }
            }
            return true;
        }

        // <!ELEMENT...>
        result = startWith(s, "ELEMENT");
        if (result < 0) RET_FALSE;
        if (result == 1) {
            if (doc_stepper_ == DocStepper::Misc) RET_FALSE;
            *type = ExclTagType::ELEMENT;
            ADV_PEDO(7);
            for (;;) {
                GET_STREAM(buf);
                if (buf == '>') break;

                // TODO:
                if (isSpace(buf)) {
                    pedometer_.space(buf);
                } else {
                    ADV_PEDO(1);
                }
            }
            return true;
        }

        // <!ATTLIST...>
        result = startWith(s, "ATTLIST");
        if (result < 0) RET_FALSE;
        if (result == 1) {
            if (doc_stepper_ == DocStepper::Misc) RET_FALSE;
            *type = ExclTagType::ATTLIST;
            ADV_PEDO(7);
            for (;;) {
                GET_STREAM(buf);
                if (buf == '>') break;

                // TODO:
                if (isSpace(buf)) {
                    pedometer_.space(buf);
                } else {
                    ADV_PEDO(1);
                }
            }
            return true;
        }

        // <!--...-->
        result = startWith(s, "--");
        if (result < 0) RET_FALSE;
        if (result == 1) {
            *type = ExclTagType::COMMENT;
            ADV_PEDO(2);
            bool illegal = false;
            for (;;) {
                GET_STREAM(buf);
                if (buf == '-') {
                    result = startWith(s, "->");
                    if (result < 0) RET_FALSE;
                    if (result == 1) break;

                    // 不能连着两次出现'-'
                    if (illegal) RET_FALSE;
                    illegal = true;
                } else {
                    illegal = false;
                }

                // TODO: 解出注释的内容并保存
                if (isSpace(buf)) {
                    pedometer_.space(buf);
                } else {
                    ADV_PEDO(1);
                }
            }
            // 最后一个字符不能是'-'
            if (illegal) RET_FALSE;
            return true;
        }
        RET_FALSE;
    }

    bool XMLParser::parseNormTag(std::istream& s, NormTagType* type, Element* cur) {
        char buf;
        PEEK_STREAM(buf);

        if (buf == '/') {
            *type = NormTagType::EndTag;
            SKIP_BYTES(1);
            ADV_PEDO(1);
            bool has_space = false;
            for (;;) {
                GET_STREAM(buf);
                if (buf == '>') break;

                if (isSpace(buf)) {
                    has_space = true;
                    pedometer_.space(buf);
                } else {
                    if (has_space) RET_FALSE;
                    ADV_PEDO(1);
                }
                cur->tag_name.push_back(buf);
            }
            if (!checkTagName(cur->tag_name)) RET_FALSE;
        } else {
            bool first_char = true;
            auto stepper = ElementStepper::TAG_NAME;
            for (;;) {
                PEEK_STREAM(buf);
                if (buf == '>') {
                    SKIP_BYTES(1);
                    *type = NormTagType::StartTag;
                    if (stepper == ElementStepper::TAG_NAME) {
                        if (cur->tag_name.empty()) RET_FALSE;
                        if (!checkTagName(cur->tag_name)) RET_FALSE;
                    }
                    break;
                }
                if (buf == '/') {
                    SKIP_BYTES(1);
                    PEEK_STREAM(char l_buf);
                    if (l_buf == '>') {
                        SKIP_BYTES(1);
                        *type = NormTagType::EmptyTag;
                        if (stepper == ElementStepper::TAG_NAME) {
                            if (cur->tag_name.empty()) RET_FALSE;
                            if (!checkTagName(cur->tag_name)) RET_FALSE;
                        }
                        ADV_PEDO(1);
                        break;
                    }
                }

                if (isSpace(buf)) {
                    SKIP_BYTES(1);
                    if (stepper == ElementStepper::TAG_NAME) {
                        if (cur->tag_name.empty()) RET_FALSE;
                        if (!checkTagName(cur->tag_name)) RET_FALSE;
                        stepper = ElementStepper::ATTR_NAME;
                    } else if (stepper == ElementStepper::ATTR_NAME) {
                        // Do nothing
                    }
                    pedometer_.space(buf);
                } else {
                    if (stepper == ElementStepper::TAG_NAME) {
                        SKIP_BYTES(1);
                        if (first_char) {
                            first_char = false;
                            if (!isNameStartChar(buf)) RET_FALSE;
                        } else {
                            if (!isNameChar(buf)) RET_FALSE;
                        }
                        cur->tag_name.push_back(buf);
                        ADV_PEDO(1);
                    } else if (stepper == ElementStepper::ATTR_NAME) {
                        if (!parseAttribute(s, cur)) RET_FALSE;
                    }
                }
            }
        }
        return true;
    }

    bool XMLParser::parseEncodingDecl(std::istream& s) {
        char buf;

        ONE_LOOP_S();
        NEXT_EQUAL("encoding", 8);
        ADV_PEDO(8);
        if (!eatEq(s)) RET_FALSE;
        GET_STREAM(char cur_sign);
        if (cur_sign != '\"' && cur_sign != '\'') RET_FALSE;
        ADV_PEDO(1);

        GET_STREAM(buf);
        if (!isAlphaBet(buf)) RET_FALSE;
        ADV_PEDO(1);
        prolog_.charset.push_back(buf);
        for (;;) {
            GET_STREAM(buf);
            ADV_PEDO(1);
            if (!isEncNameChar(buf)) break;
            prolog_.charset.push_back(buf);
        }
        if (buf != cur_sign) RET_FALSE;
        return true;
    }

    bool XMLParser::parseSDDecl(std::istream& s) {
        ONE_LOOP_S();
        NEXT_EQUAL("standalone", 10);
        ADV_PEDO(10);
        if (!eatEq(s)) RET_FALSE;
        GET_STREAM(char cur_sign);
        if (cur_sign != '\"' && cur_sign != '\'') RET_FALSE;
        ADV_PEDO(1);

        GET_STREAM(char buf);
        if (buf == 'y') {
            NEXT_EQUAL("es", 2);
        } else if (buf == 'n') {
            GET_STREAM(buf);
            if (buf != 'o') RET_FALSE;
        } else {
            RET_FALSE;
        }
        if (buf != cur_sign) RET_FALSE;
        return true;
    }

    bool XMLParser::eatEq(std::istream& s) {
        char buf;
        LOOP_S();
        GET_STREAM(buf);
        if (buf != '=') RET_FALSE;
        ADV_PEDO(1);
        LOOP_S();
        return true;
    }

    bool XMLParser::parseAttribute(std::istream& s, Element* cur) {
        char buf;
        bool name_first = true;
        std::string attr_name;
        for (;;) {
            PEEK_STREAM(buf);
            if (isSpace(buf) || buf == '=') break;
            SKIP_BYTES(1);

            if (name_first) {
                name_first = false;
                if (!isNameStartChar(buf)) RET_FALSE;
            } else {
                if (!isNameChar(buf)) RET_FALSE;
            }
            attr_name.push_back(buf);
            ADV_PEDO(1);
        }
        if (cur->attrs.find(attr_name) != cur->attrs.end()) RET_FALSE;

        if (!eatEq(s)) return false;
        GET_STREAM(char cur_sign);
        if (cur_sign != '\"' && cur_sign != '\'') RET_FALSE;
        ADV_PEDO(1);

        std::string attr_val;
        for (;;) {
            GET_STREAM(buf);
            ADV_PEDO(1);
            if (buf == '\"' || buf == '\'') break;
            attr_val.push_back(buf);
        }
        if (buf != cur_sign) RET_FALSE;

        cur->attrs[attr_name] = attr_val;
        return true;
    }

    bool XMLParser::isChar(uint32_t val) const {
        return (val == 0x9 ||
            val == 0xA ||
            val == 0xD ||
            (val >= 0x20 && val <= 0xD7FF) ||
            (val >= 0xE000 && val <= 0xFFFD) ||
            (val >= 0x10000 && val <= 0x10FFFF));
    }

    bool XMLParser::isDigit(uint32_t val) const {
        return (val >= '0' && val <= '9');
    }

    bool XMLParser::isAlphaBet(uint32_t val) const {
        return ((val >= 'a' && val <= 'z') ||
            (val >= 'A' && val <= 'Z'));
    }

    bool XMLParser::isSpace(uint32_t val) const {
        return (val == 0x20 || val == 0x9 || val == 0xD || val == 0xA);
    }

    bool XMLParser::isNameStartChar(uint32_t val) const {
        return (val == ':' ||
            (val >= 'A' && val <= 'Z') ||
            val == '_' ||
            (val >= 'a' && val <= 'z') ||
            (val >= 0xC0 && val <= 0xD6) ||
            (val >= 0xD8 && val <= 0xF6) ||
            (val >= 0xF8 && val <= 0x2FF) ||
            (val >= 0x370 && val <= 0x37D) ||
            (val >= 0x37F && val <= 0x1FFF) ||
            (val >= 0x200C && val <= 0x200D) ||
            (val >= 0x2070 && val <= 0x218F) ||
            (val >= 0x2C00 && val <= 0x2FEF) ||
            (val >= 0x3001 && val <= 0xD7FF) ||
            (val >= 0xF900 && val <= 0xFDCF) ||
            (val >= 0xFDF0 && val <= 0xFFFD) ||
            (val >= 0x10000 && val <= 0xEFFFF));
    }

    bool XMLParser::isNameChar(uint32_t val) const {
        return (isNameStartChar(val) ||
            val == '-' ||
            val == '.' ||
            (val >= '0' && val <= '9') ||
            val == 0xB7 ||
            (val >= 0x300 && val <= 0x36F) ||
            (val >= 0x203F && val <= 0x2040));
    }

    bool XMLParser::isEncNameChar(uint32_t val) const {
        return (isAlphaBet(val) ||
            isDigit(val) ||
            val == '.' ||
            val == '-' ||
            val == '_');
    }

    bool XMLParser::checkTagName(const std::string& str) const {
        return !isLitEqual(str, "xml");
    }

}