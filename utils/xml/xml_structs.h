// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_XML_XML_STRUCTS_H_
#define UTILS_XML_XML_STRUCTS_H_

#include <map>
#include <string>
#include <vector>


namespace utl {
namespace xml {

    struct Prolog {
        std::string version;
        std::string charset;
    };

    struct Content {
        enum class Type {
            CharData,
            Element,
        };

        Type type;
        std::string char_data;
        std::shared_ptr<struct Element> element;

        Content();
    };

    struct Element {
        std::string tag_name;
        std::map<std::string, std::string> attrs;
        std::vector<Content> contents;
        Element* parent;

        Element();
    };

}
}

#endif  // UTILS_XML_XML_STRUCTS_H_