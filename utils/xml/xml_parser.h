// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_XML_PARSER_H_
#define UTILS_XML_PARSER_H_

#include <memory>
#include <istream>

#include "utils/xml/xml_structs.h"
#include "utils/xml/xml_pedometer.h"


namespace utl {

    /**
     * XML 1.0 解析实现
     * https://www.w3.org/TR/xml/
     */
    class XMLParser {
    public:
        using Prolog = xml::Prolog;
        using Content = xml::Content;
        using Element = xml::Element;

        XMLParser();

        bool parse(std::istream& s, std::shared_ptr<Element>* out);
        const Prolog& getProlog() const;
        const XMLPedometer& getPedometer() const;

    private:
        enum class DocStepper {
            None,
            Prolog,
            Elements,
            Misc,
        };

        enum class ElementStepper {
            TAG_NAME,
            ATTR_NAME,
        };

        enum class QuesTagType {
            Prolog,
            PIs,
        };

        enum class ExclTagType {
            CDATA,
            COND_SECT,
            DOCTYPE,
            ENTITY,
            ELEMENT,
            ATTLIST,
            COMMENT,
        };

        enum class NormTagType {
            EmptyTag,
            StartTag,
            EndTag,
        };

        bool parseQuesTag(std::istream& s, QuesTagType* type);
        bool parseExclTag(std::istream& s, ExclTagType* type);
        bool parseNormTag(std::istream& s, NormTagType* type, Element* cur);

        bool parseEncodingDecl(std::istream& s);
        bool parseSDDecl(std::istream& s);
        bool eatEq(std::istream& s);
        bool parseAttribute(std::istream& s, Element* cur);

        bool isChar(uint32_t val) const;
        bool isDigit(uint32_t val) const;
        bool isAlphaBet(uint32_t val) const;
        bool isSpace(uint32_t val) const;
        bool isNameStartChar(uint32_t val) const;
        bool isNameChar(uint32_t val) const;
        bool isEncNameChar(uint32_t val) const;

        bool checkTagName(const std::string& str) const;

        Prolog prolog_;
        DocStepper doc_stepper_;
        XMLPedometer pedometer_;
    };

}

#endif  // UTILS_XML_PARSER_H_