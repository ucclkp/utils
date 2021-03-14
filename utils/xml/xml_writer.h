// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_XML_WRITER_H_
#define UTILS_XML_WRITER_H_

#include "utils/xml/xml_parser.h"


namespace utl {

    class XMLWriter {
    public:
        using Prolog = xml::Prolog;
        using Content = xml::Content;
        using Element = xml::Element;

        enum class Indent {
            Tab,
            Space,
        };

        enum class LineBreak {
            CR,
            LF,
            CRLF,
        };

        XMLWriter();

        void setIndent(Indent indent, uint32_t count);
        void setLineBreak(LineBreak lb);
        bool write(const Element& root, std::string* out);

    private:
        void writeProlog(std::string* out);
        bool writeElements(const Element& root, uint32_t layer, std::string* out);
        std::string getIndent(uint32_t layer) const;
        std::string getLineBreak() const;

        Indent indent_;
        uint32_t indent_count_;
        LineBreak line_break_;
    };

}

#endif  // UTILS_XML_WRITER_H_