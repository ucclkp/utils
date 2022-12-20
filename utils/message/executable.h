// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MESSAGE_EXECUTABLE_H_
#define UTILS_MESSAGE_EXECUTABLE_H_


namespace utl {

    class Message;

    class Executable {
    public:
        virtual ~Executable() = default;

        virtual void onExecTask(const Message& msg) = 0;
    };

}

#endif  // UTILS_MESSAGE_EXECUTABLE_H_