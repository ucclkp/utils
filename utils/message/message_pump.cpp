// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/message/message_pump.h"

#include <cassert>

#include "utils/log.h"
#include "utils/executable.h"
#include "utils/message/cycler.h"
#include "utils/message/message.h"
#include "utils/message/message_queue.h"
#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#include "utils/message/win/message_pump_ui_win.h"
#include "utils/message/win/message_pump_win.h"
#elif defined OS_MAC
#include "utils/message/mac/message_pump_mac.h"
#include "utils/message/mac/message_pump_ui_mac.h"
#endif


namespace utl {

    std::mutex MessagePump::sync_;
    MessagePump* MessagePump::main_pump_ = nullptr;
    thread_local std::shared_ptr<MessagePump> MessagePump::cur_pump_;


    MessagePump::MessagePump() {
        msg_queue_ = new MessageQueue();
    }

    MessagePump::~MessagePump() {
        delete msg_queue_;
    }

    bool MessagePump::isNested() const {
        return nested_count_ > 0;
    }

    MessageQueue* MessagePump::getQueue() const {
        return msg_queue_;
    }

    // static
    void MessagePump::create() {
        std::lock_guard<std::mutex> lk(sync_);
        if (cur_pump_) {
            LOG(Log::ERR) << "Current thread already have a MessagePump!";
            return;
        }

        std::shared_ptr<MessagePump> pump;

#ifdef OS_WINDOWS
        pump.reset(new MessagePumpWin());
#elif defined OS_MAC
        pump.reset(new MessagePumpMac());
#endif
        cur_pump_ = pump;
    }

    // static
    void MessagePump::createForUI() {
        std::lock_guard<std::mutex> lk(sync_);
        if (cur_pump_) {
            LOG(Log::ERR) << "Current thread already have a MessagePump!";
            return;
        }

        std::shared_ptr<MessagePump> pump;

#ifdef OS_WINDOWS
        pump.reset(new MessagePumpUIWin());
#elif defined OS_MAC
        pump.reset(new MessagePumpUIMac());
#endif
        cur_pump_ = pump;

        if (main_pump_) {
            return;
        }
        main_pump_ = getCurrent();
    }

    // static
    void MessagePump::run() {
        MessagePump* pump = getCurrent();
        if (!pump) {
            LOG(Log::ERR) << "Current thread dose not have a MessagePump!";
            return;
        }

        ++pump->nested_count_;
        pump->context_.push(MPContext());

        pump->loop();

        pump->context_.pop();
        --pump->nested_count_;
    }

    // static
    void MessagePump::quit() {
        MessagePump* pump = getCurrent();
        if (!pump) {
            LOG(Log::ERR) << "Current thread dose not have a MessagePump!";
            return;
        }

        pump->context_.top().quit_when_idle_ = true;
        pump->wakeup();
    }

    // static
    void MessagePump::quitNow() {
        MessagePump* pump = getCurrent();
        if (!pump) {
            jour_e("MessagePump::create() wasn't called on this thread!");
            return;
        }

        pump->context_.top().quit_imm_ = true;
        pump->wakeup();
    }

    // static
    void MessagePump::destroy() {
        cur_pump_.reset();
    }

    // static
    MessagePump* MessagePump::getMain() {
        std::lock_guard<std::mutex> lk(sync_);
        return main_pump_;
    }

    // static
    MessagePump* MessagePump::getCurrent() {
        assert(cur_pump_);
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

    bool MessagePump::cosumeDelayed(int64_t* delay_ns) {
        for (;;) {
            Message msg;
            if (!msg_queue_->dequeueDelayed(delay_ns, &msg)) {
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
