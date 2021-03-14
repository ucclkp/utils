// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/xml/xml_structs.h"


namespace utl {
namespace xml {

    Content::Content()
        : type(Type::Element) {}

    Element::Element()
        : parent(nullptr) {}

}
}