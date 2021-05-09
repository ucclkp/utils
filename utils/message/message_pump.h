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

        bool isNested() const;
        MessageQueue* getQueue() const;

        static void create();
        static void createForUI();
        static void run();
        static void quit();
        static void quitNow();
        static void destroy();

        static MessagePump* getMain();
        static MessagePump* getCurrent();
        static MessageQueue* getCurrentQueue();

    protected:
        struct MPContext {
            bool quit_imm_ = false;
            bool quit_when_idle_ = false;
        };

        MessagePump();

        bool cosume();
        bool cosumeDelayed(int64_t* delay);

        int nested_count_ = -1;
        MessageQueue* msg_queue_;
        std::stack<MPContext> context_;

    private:
        static std::mutex sync_;
        static MessagePump* main_pump_;
        static thread_local std::shared_ptr<MessagePump> cur_pump_;
    };

}

#endif  // UTILS_MESSAGE_MESSAGE_PUMP_H_