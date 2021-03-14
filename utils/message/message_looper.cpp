// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/message/message_looper.h"

#include "utils/log.h"
#include "utils/executable.h"
#include "utils/message/cycler.h"
#include "utils/message/message.h"
#include "utils/message/message_queue.h"
#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "utils/message/win/message_looper_win.h"
#elif defined OS_MAC
#include "utils/message/mac/message_looper_mac.h"
#endif


namespace utl {

    std::mutex MessageLooper::looper_sync_;
    MessageLooper* MessageLooper::main_looper_ = nullptr;
    thread_local std::shared_ptr<MessageLooper> MessageLooper::looper_;


    MessageLooper::MessageLooper()
        : quit_imm_(false),
          quit_when_idle_(false)
    {
        msg_queue_ = new MessageQueue();
    }

    MessageLooper::~MessageLooper() {
        delete msg_queue_;
    }

    MessageQueue* MessageLooper::getQueue() {
        return msg_queue_;
    }

    // static
    void MessageLooper::create() {
        std::lock_guard<std::mutex> lk(looper_sync_);

        if (looper_) {
            CHECK(false) << "Only one Looper may be created per thread!";
            return;
        }

#ifdef OS_WINDOWS
        looper_.reset(new MessageLooperWin());
#elif defined OS_MAC
        looper_.reset(new MessageLooperMac());
#endif
    }

    // static
    void MessageLooper::createMainLooper() {
        create();
        std::lock_guard<std::mutex> lk(looper_sync_);

        if (main_looper_) {
            CHECK(false) << "The main Looper has already been created!";
            return;
        }
        main_looper_ = myLooper();
    }

    // static
    void MessageLooper::run() {
        MessageLooper* looper = myLooper();
        if (!looper) {
            CHECK(false) << "MessageLooper::create() wasn't called on this thread!";
            return;
        }
        looper->loop();
    }

    // static
    void MessageLooper::quit() {
        MessageLooper* looper = myLooper();
        if (!looper) {
            CHECK(false) << "MessageLooper::create() wasn't called on this thread!";
            return;
        }

        looper->quit_when_idle_ = true;
        looper->wakeup();
    }

    // static
    void MessageLooper::quitNow() {
        MessageLooper* looper = myLooper();
        if (!looper) {
            CHECK(false) << "MessageLooper::create() wasn't called on this thread!";
            return;
        }

        looper->quit_imm_ = true;
        looper->wakeup();
    }

    // static
    void MessageLooper::destroy() {
        if (looper_) {
            looper_.reset();
        }
    }

    // static
    MessageQueue* MessageLooper::myQueue() {
        return myLooper()->getQueue();
    }

    // static
    MessageLooper* MessageLooper::myLooper() {
        DCHECK(looper_);
        return looper_.get();
    }

    // static
    MessageLooper* MessageLooper::getMainLooper() {
        std::lock_guard<std::mutex> lk(looper_sync_);
        return main_looper_;
    }

    bool MessageLooper::cosume() {
        msg_queue_->addBarrier();
        for (;;) {
            Message* msg = msg_queue_->dequeue();
            if (!msg) {
                break;
            }

            if (msg->callback) {
                msg->callback->run();
            } else if (msg->func) {
                msg->func();
            } else if (msg->target) {
                msg->target->dispatchMessage(msg);
            }

            msg->recycle();
        }
        msg_queue_->removeBarrier();

        return msg_queue_->hasMessage();
    }

    bool MessageLooper::cosumeDelayed(int64_t* delay) {
        for (;;) {
            Message* msg = msg_queue_->dequeueDelayed(delay);
            if (!msg) {
                break;
            }

            if (msg->callback) {
                msg->callback->run();
            } else if (msg->func) {
                msg->func();
            } else if (msg->target) {
                msg->target->dispatchMessage(msg);
            }

            msg->recycle();
        }
        return false;
    }

}
