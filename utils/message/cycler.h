// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MESSAGE_CYCLER_H_
#define UTILS_MESSAGE_CYCLER_H_

#include <cstdint>
#include <functional>


namespace utl {

    class Message;
    class MessagePump;
    class Executable;


    class CyclerListener {
    public:
        virtual ~CyclerListener() = default;

        virtual void onHandleMessage(const Message& msg) = 0;
    };


    class Cycler {
    public:
        Cycler();
        explicit Cycler(MessagePump* pump);
        virtual ~Cycler();

        void setListener(CyclerListener* l);

        void post(Executable* exec, int id = -1);
        void postDelayed(Executable* exec, uint64_t delay, int id = -1);
        void postAtTime(Executable* exec, uint64_t at_time, int id = -1);

        void post(const std::function<void()>& func, int id = -1);
        void postDelayed(const std::function<void()>& func, uint64_t delay, int id = -1);
        void postAtTime(const std::function<void()>& func, uint64_t at_time, int id = -1);

        void post(Message* msg);
        void postDelayed(Message* msg, uint64_t delay);
        void postAtTime(Message* msg, uint64_t at_time);

        void post(int id);
        void postDelayed(int id, uint64_t delay);
        void postAtTime(int id, uint64_t at_time);

        void enqueueMessage(Message* msg);

        bool hasMessages(int id);
        void removeMessages(int id);

        void dispatchMessage(const Message& msg);

        static uint64_t now();

    private:
        MessagePump* pump_;
        CyclerListener* listener_;
    };

}

#endif  // UTILS_MESSAGE_CYCLER_H_