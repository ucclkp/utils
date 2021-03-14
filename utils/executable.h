// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_EXECUTABLE_H_
#define UTILS_EXECUTABLE_H_


namespace utl {

    class Executable {
    public:
        virtual ~Executable() = default;

        virtual void run() = 0;
    };

}

#endif  // UTILS_EXECUTABLE_H_