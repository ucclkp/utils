// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MESSAGE_MESSAGE_LOOPER_H_
#define UTILS_MESSAGE_MESSAGE_LOOPER_H_

#include <memory>
#include <mutex>


namespace utl {

    class Cycler;
    class Message;
    class MessageQueue;

    class MessageLooper {
    public:
        virtual ~MessageLooper();

        virtual void wakeup() = 0;
        virtual void loop() = 0;

        MessageQueue* getQueue();

        static void create();
        static void createMainLooper();
        static void run();
        static void quit();
        static void quitNow();
        static void destroy();

        static MessageQueue* myQueue();
        static MessageLooper* myLooper();
        static MessageLooper* getMainLooper();

    protected:
        MessageLooper();

        bool cosume();
        bool cosumeDelayed(int64_t* delay);

        bool quit_imm_;
        bool quit_when_idle_;
        MessageQueue* msg_queue_;

    private:
        static std::mutex looper_sync_;
        static MessageLooper* main_looper_;
        static thread_local std::shared_ptr<MessageLooper> looper_;
    };

}

#endif  // UTILS_MESSAGE_MESSAGE_LOOPER_H_