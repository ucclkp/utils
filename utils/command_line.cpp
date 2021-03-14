// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/command_line.h"

#include <vector>

#include "utils/convert.h"


namespace {
    const char16_t kNamePrefix[] = u"--";
    const char16_t kShortNamePrefix[] = u"-";
}

namespace utl {

    CommandLine::Map CommandLine::cmds_;

    void CommandLine::initialize(int argc, char16_t* argv[]) {
        std::string name;
        bool prev_name = false;
        size_t np_length = std::char_traits<char16_t>::length(kNamePrefix);
        size_t snp_length = std::char_traits<char16_t>::length(kShortNamePrefix);

        for (int i = 1; i < argc; ++i) {
            std::u16string part(argv[i]);
            if (part.find(kNamePrefix) == 0) {
                if (prev_name) {
                    cmds_[name] = u"";
                }
                name = UTF16ToUTF8(part.substr(np_length));
                prev_name = true;
            } else if (part.find(kShortNamePrefix) == 0 &&
                part.length() == snp_length + 1)
            {
                if (prev_name) {
                    cmds_[name] = u"";
                }
                name = UTF16ToUTF8(part.substr(snp_length));
                prev_name = true;
            } else {
                // 如果遇到一个键有多个值的情况，就忽略除第一个以外的所有其他值
                if (prev_name) {
                    cmds_[name] = argv[i];
                    prev_name = false;
                }
            }
        }

        if (prev_name) {
            cmds_[name] = u"";
        }
    }

    void CommandLine::clear() {
        cmds_.clear();
    }

    bool CommandLine::hasName(const std::string& name) {
        auto it = cmds_.find(name);
        if (it == cmds_.end()) {
            return false;
        }
        return true;
    }

    const CommandLine::Map& CommandLine::getMap() {
        return cmds_;
    }

    const std::u16string& CommandLine::getValue(const std::string& name) {
        // 因为返回值为引用，所以需要在查找不到元素时
        // 返回一个有效的空值
        static std::u16string empty;

        auto it = cmds_.find(name);
        if (it == cmds_.end()) {
            return empty;
        }
        return it->second;
    }

}
