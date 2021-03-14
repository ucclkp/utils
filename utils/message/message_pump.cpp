// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/message/message_pump.h"

#include "utils/log.h"
#include "utils/executable.h"
#include "utils/message/cycler.h"
#include "utils/message/message.h"
#include "utils/message/message_queue.h"
#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "utils/message/win/message_pump_win.h"
#elif defined OS_MAC
#include "utils/message/mac/message_looper_mac.h"
#endif


namespace utl {

    std::mutex MessagePump::sync_;
    MessagePump* MessagePump::main_pump_ = nullptr;
    thread_local std::shared_ptr<MessagePump> MessagePump::cur_pump_;


    MessagePump::MessagePump()
        : quit_imm_(false),
          quit_when_idle_(false)
    {
        msg_queue_ = new MessageQueue();
    }

    MessagePump::~MessagePump() {
        delete msg_queue_;
    }

    MessageQueue* MessagePump::getQueue() {
        return msg_queue_;
    }

    // static
    void MessagePump::create() {
        std::lock_guard<std::mutex> lk(sync_);

        if (cur_pump_) {
            return;
        }

#ifdef OS_WINDOWS
        cur_pump_.reset(new MessagePumpWin());
#elif defined OS_MAC
        cur_pump_.reset(new MessageLooperMac());
#endif
    }

    // static
    void MessagePump::createMain() {
        create();
        std::lock_guard<std::mutex> lk(sync_);

        if (main_pump_) {
            return;
        }
        main_pump_ = getCurrent();
    }

    // static
    void MessagePump::run() {
        MessagePump* pump = getCurrent();
        if (!pump) {
            CHECK(false) << "MessagePump::create() wasn't called on this thread!";
            return;
        }
        pump->loop();
    }

    // static
    void MessagePump::quit() {
        MessagePump* pump = getCurrent();
        if (!pump) {
            CHECK(false) << "MessagePump::create() wasn't called on this thread!";
            return;
        }

        pump->quit_when_idle_ = true;
        pump->wakeup();
    }

    // static
    void MessagePump::quitNow() {
        MessagePump* pump = getCurrent();
        if (!pump) {
            CHECK(false) << "MessagePump::create() wasn't called on this thread!";
            return;
        }

        pump->quit_imm_ = true;
        pump->wakeup();
    }

    // static
    void MessagePump::destroy() {
        if (cur_pump_) {
            cur_pump_.reset();
        }
    }

    // static
    MessagePump* MessagePump::getMain() {
        std::lock_guard<std::mutex> lk(sync_);
        return main_pump_;
    }

    // static
    MessagePump* MessagePump::getCurrent() {
        DCHECK(cur_pump_);
        return cur_pump_.get();
    }

    // static
    MessageQueue* MessagePump::getCurrentQueue() {
        return getCurrent()->getQueue();
    }

    bool MessagePump::cosume() {
        msg_queue_->addBarrier();
        for (;;) {
            Message msg;
            if (!msg_queue_->dequeue(&msg)) {
                break;
            }

            if (msg.callback) {
                msg.callback->run();
            } else if (msg.func) {
                msg.func();
            } else if (msg.target) {
                msg.target->dispatchMessage(msg);
            }
        }
        msg_queue_->removeBarrier();

        return msg_queue_->hasMessage();
    }

    bool MessagePump::cosumeDelayed(int64_t* delay) {
        for (;;) {
            Message msg;
            if (!msg_queue_->dequeueDelayed(delay, &msg)) {
                break;
            }

            if (msg.callback) {
                msg.callback->run();
            } else if (msg.func) {
                msg.func();
            } else if (msg.target) {
                msg.target->dispatchMessage(msg);
            }
        }
        return false;
    }

}
