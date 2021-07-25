// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MESSAGE_MESSAGE_H_
#define UTILS_MESSAGE_MESSAGE_H_

#include <functional>


namespace utl {

    class Cycler;
    class Executable;

    class Message {
    public:
        Message();
        ~Message();

        int id;
        uint64_t time_ns;
        Cycler* target;
        Executable* callback;
        std::function<void()> func;

        uint64_t ui1;
        uint64_t ui2;
        void* data;
        std::shared_ptr<void> shared_data;

        bool is_barrier;
    };

}

#endif  // UTILS_MESSAGE_MESSAGE_H_