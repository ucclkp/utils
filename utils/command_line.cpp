// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/command_line.h"

#include <vector>

#include "utils/strings/string_utils.hpp"


namespace {

    const char kNamePrefix8[] = "--";
    const char kShortNamePrefix8[] = "-";

    const wchar_t kNamePrefixW[] = L"--";
    const wchar_t kShortNamePrefixW[] = L"-";

    const char16_t kNamePrefix16[] = u"--";
    const char16_t kShortNamePrefix16[] = u"-";

}

namespace utl {

    CommandLine::Map CommandLine::cmds_;

    // static
    void CommandLine::initialize(int argc, char* argv[]) {
        std::string name;
        bool prev_name = false;
        size_t np_length = std::char_traits<char>::length(kNamePrefix8);
        size_t snp_length = std::char_traits<char>::length(kShortNamePrefix8);

        for (int i = 1; i < argc; ++i) {
            std::string_view part(argv[i]);
            if (part.find(kNamePrefix8) == 0) {
                if (prev_name) {
                    cmds_[name] = u"";
                }
                name = part.substr(np_length);
                prev_name = true;
            } else if (part.find(kShortNamePrefix8) == 0 &&
                part.length() == snp_length + 1)
            {
                if (prev_name) {
                    cmds_[name] = u"";
                }
                name = part.substr(snp_length);
                prev_name = true;
            } else {
                // 如果遇到一个键有多个值的情况，就忽略除第一个以外的所有其他值
                if (prev_name) {
                    cmds_[name] = UTF8ToUTF16(part);
                    prev_name = false;
                }
            }
        }

        if (prev_name) {
            cmds_[name] = u"";
        }
    }

    // static
    void CommandLine::initialize(int argc, wchar_t* argv[]) {
        std::string name;
        bool prev_name = false;
        size_t np_length = std::char_traits<wchar_t>::length(kNamePrefixW);
        size_t snp_length = std::char_traits<wchar_t>::length(kShortNamePrefixW);

        for (int i = 1; i < argc; ++i) {
            std::wstring_view part(argv[i]);
            if (part.find(kNamePrefixW) == 0) {
                if (prev_name) {
                    cmds_[name] = u"";
                }
                name = WideToUTF8(part.substr(np_length));
                prev_name = true;
            } else if (part.find(kShortNamePrefixW) == 0 &&
                part.length() == snp_length + 1)
            {
                if (prev_name) {
                    cmds_[name] = u"";
                }
                name = WideToUTF8(part.substr(snp_length));
                prev_name = true;
            } else {
                // 如果遇到一个键有多个值的情况，就忽略除第一个以外的所有其他值
                if (prev_name) {
                    cmds_[name] = WideToUTF16(part);
                    prev_name = false;
                }
            }
        }

        if (prev_name) {
            cmds_[name] = u"";
        }
    }

    // static
    void CommandLine::initialize(int argc, char16_t* argv[]) {
        std::string name;
        bool prev_name = false;
        size_t np_length = std::char_traits<char16_t>::length(kNamePrefix16);
        size_t snp_length = std::char_traits<char16_t>::length(kShortNamePrefix16);

        for (int i = 1; i < argc; ++i) {
            std::u16string_view part(argv[i]);
            if (part.find(kNamePrefix16) == 0) {
                if (prev_name) {
                    cmds_[name] = u"";
                }
                name = UTF16ToUTF8(part.substr(np_length));
                prev_name = true;
            } else if (part.find(kShortNamePrefix16) == 0 &&
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
                    cmds_[name] = part;
                    prev_name = false;
                }
            }
        }

        if (prev_name) {
            cmds_[name] = u"";
        }
    }

    // static
    void CommandLine::clear() {
        cmds_.clear();
    }

    // static
    bool CommandLine::hasName(const std::string_view& name) {
        auto it = cmds_.find(name);
        if (it == cmds_.end()) {
            return false;
        }
        return true;
    }

    // static
    const CommandLine::Map& CommandLine::getMap() {
        return cmds_;
    }

    // static
    const std::u16string& CommandLine::getValue(const std::string_view& name) {
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
