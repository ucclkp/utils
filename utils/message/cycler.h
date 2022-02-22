// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MESSAGE_CYCLER_H_
#define UTILS_MESSAGE_CYCLER_H_

#include <functional>

#include "utils/time_utils.h"


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
        using ns = TimeUtils::ns;
        using nsp = TimeUtils::nsp;

        Cycler();
        explicit Cycler(const std::weak_ptr<MessagePump>& pump);
        virtual ~Cycler();

        void setListener(CyclerListener* l);

        void post(Executable* exec, int id = -1);
        void postDelayed(Executable* exec, nsp delay, int id = -1);
        void postAtTime(Executable* exec, nsp at_time, int id = -1);

        void post(const std::function<void()>& func, int id = -1);
        void postDelayed(const std::function<void()>& func, nsp delay, int id = -1);
        void postAtTime(const std::function<void()>& func, nsp at_time, int id = -1);

        void post(Message* msg);
        void postDelayed(Message* msg, nsp delay);
        void postAtTime(Message* msg, nsp at_time);

        void post(int id);
        void postDelayed(int id, nsp delay);
        void postAtTime(int id, nsp at_time);

        void enqueueMessage(Message* msg);

        bool hasMessages(int id);
        void removeMessages(int id);

        void dispatchMessage(const Message& msg);

        static ns now();

    private:
        std::weak_ptr<MessagePump> pump_;
        CyclerListener* listener_;
    };

}

#endif  // UTILS_MESSAGE_CYCLER_H_