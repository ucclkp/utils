// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MESSAGE_MESSAGE_H_
#define UTILS_MESSAGE_MESSAGE_H_

#include <mutex>
#include <functional>


namespace utl {

    class Cycler;
    class Executable;

    class Message {
    public:
        static Message* obtain();
        void recycle();

        static void init(uint64_t pool_capacity);
        static void clean();

    public:
        int what;
        uint64_t when;
        Cycler* target;
        Executable* callback;
        std::function<void()> func;
        void* data;
        std::shared_ptr<void> shared_data;

        Message* next;
        bool is_barrier;

    private:
        Message();
        ~Message();

        static Message* pool;
        static uint64_t pool_size;

        static bool is_initialized;
        static std::mutex pool_sync;
    };

}

#endif  // UTILS_MESSAGE_MESSAGE_H_