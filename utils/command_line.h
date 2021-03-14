// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_COMMAND_LINE_H_
#define UTILS_COMMAND_LINE_H_

#include <map>
#include <string>


namespace utl {

    class CommandLine {
    public:
        using Map = std::map<std::string, std::u16string>;

        static void initialize();
        static void initialize(int argc, char16_t* argv[]);
        static void clear();

        static bool hasName(const std::string& name);

        static const Map& getMap();
        static const std::u16string& getValue(const std::string& name);

    private:
        static Map cmds_;
    };

}

#endif  // UTILS_COMMAND_LINE_H_
