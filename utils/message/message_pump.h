// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MESSAGE_MESSAGE_PUMP_H_
#define UTILS_MESSAGE_MESSAGE_PUMP_H_

#include <memory>
#include <mutex>
#include <stack>


namespace utl {

    class Cycler;
    class Message;
    class MessageQueue;

    class MessagePump {
    public:
        virtual ~MessagePump();

        virtual void wakeup() = 0;
        virtual void loop() = 0;

        MessageQueue* getQueue();

        static void create();
        static void createMain();
        static void run();
        static void quit();
        static void quitNow();
        static void destroy();

        static MessagePump* getMain();
        static MessagePump* getCurrent();
        static MessageQueue* getCurrentQueue();

    protected:
        MessagePump();

        bool cosume();
        bool cosumeDelayed(int64_t* delay);

        bool quit_imm_;
        bool quit_when_idle_;
        MessageQueue* msg_queue_;

    private:
        static std::mutex sync_;
        static MessagePump* main_pump_;
        static thread_local std::stack<std::shared_ptr<MessagePump>> cur_pump_;
    };

}

#endif  // UTILS_MESSAGE_MESSAGE_PUMP_H_