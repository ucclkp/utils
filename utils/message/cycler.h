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
    class MessageLooper;
    class Executable;


    class CyclerListener {
    public:
        virtual ~CyclerListener() = default;

        virtual void onHandleMessage(Message* msg) = 0;
    };


    class Cycler {
    public:
        Cycler();
        explicit Cycler(MessageLooper* looper);
        virtual ~Cycler();

        void setListener(CyclerListener* l);

        void post(Executable* exec);
        void postDelayed(Executable* exec, uint64_t delay);
        void postAtTime(Executable* exec, uint64_t at_time);

        void post(const std::function<void()>& func, int what = -1);
        void postDelayed(const std::function<void()>& func, uint64_t delay, int what = -1);
        void postAtTime(const std::function<void()>& func, uint64_t at_time, int what = -1);

        void sendMessage(Message* msg);
        void sendMessageDelayed(Message* msg, uint64_t delay);
        void sendMessageAtTime(Message* msg, uint64_t at_time);
        void sendEmptyMessage(int what);
        void sendEmptyMessageDelayed(int what, uint64_t delay);
        void sendEmptyMessageAtTime(int what, uint64_t at_time);

        void enqueueMessage(Message* msg);

        bool hasMessages(int what);
        void removeMessages(int what);

        void dispatchMessage(Message* msg);

        static uint64_t now();

    private:
        MessageLooper* looper_;
        CyclerListener* listener_;
    };

}

#endif  // UTILS_MESSAGE_CYCLER_H_