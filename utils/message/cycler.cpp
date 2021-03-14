// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/message/cycler.h"

#include <chrono>

#include "utils/log.h"
#include "utils/executable.h"
#include "utils/message/message.h"
#include "utils/message/message_looper.h"
#include "utils/message/message_queue.h"


namespace utl {

    Cycler::Cycler()
        : looper_(MessageLooper::myLooper()),
          listener_(nullptr) {
    }

    Cycler::Cycler(MessageLooper* looper)
        : looper_(looper),
          listener_(nullptr) {
    }

    Cycler::~Cycler() {
        looper_->getQueue()->remove(this);
    }

    void Cycler::setListener(CyclerListener* l) {
        listener_ = l;
    }

    void Cycler::post(Executable* exec) {
        postDelayed(exec, 0);
    }

    void Cycler::postDelayed(Executable* exec, uint64_t delay) {
        postAtTime(exec, delay + now());
    }

    void Cycler::postAtTime(Executable* exec, uint64_t at_time) {
        Message* msg = Message::obtain();
        msg->callback = exec;

        sendMessageAtTime(msg, at_time);
    }

    void Cycler::post(const std::function<void()>& func, int what) {
        postDelayed(func, 0, what);
    }

    void Cycler::postDelayed(const std::function<void()>& func, uint64_t delay, int what) {
        postAtTime(func, delay + now(), what);
    }

    void Cycler::postAtTime(const std::function<void()>& func, uint64_t at_time, int what) {
        Message* msg = Message::obtain();
        msg->func = func;
        msg->what = what;

        sendMessageAtTime(msg, at_time);
    }

    void Cycler::sendEmptyMessage(int what) {
        sendEmptyMessageDelayed(what, 0);
    }

    void Cycler::sendEmptyMessageDelayed(int what, uint64_t delay) {
        sendEmptyMessageAtTime(what, delay + now());
    }

    void Cycler::sendEmptyMessageAtTime(int what, uint64_t at_time) {
        Message* msg = Message::obtain();
        msg->what = what;

        sendMessageAtTime(msg, at_time);
    }

    void Cycler::sendMessage(Message* msg) {
        sendMessageDelayed(msg, 0);
    }

    void Cycler::sendMessageDelayed(Message* msg, uint64_t delay) {
        sendMessageAtTime(msg, delay + now());
    }

    void Cycler::sendMessageAtTime(Message* msg, uint64_t at_time) {
        msg->when = at_time;
        enqueueMessage(msg);
    }

    void Cycler::enqueueMessage(Message* msg) {
        msg->target = this;
        looper_->getQueue()->enqueue(msg);
        looper_->wakeup();
    }

    bool Cycler::hasMessages(int what) {
        return looper_->getQueue()->contains(this, what);
    }

    void Cycler::removeMessages(int what) {
        looper_->getQueue()->remove(this, what);
    }

    void Cycler::dispatchMessage(Message* msg) {
        if (listener_) {
            listener_->onHandleMessage(msg);
        }
    }

    // static
    uint64_t Cycler::now() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }

}