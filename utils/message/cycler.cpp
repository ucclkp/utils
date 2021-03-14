// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/message/cycler.h"

#include <chrono>

#include "utils/executable.h"
#include "utils/message/message.h"
#include "utils/message/message_pump.h"
#include "utils/message/message_queue.h"


namespace utl {

    Cycler::Cycler()
        : pump_(MessagePump::getCurrent()),
          listener_(nullptr) {
    }

    Cycler::Cycler(MessagePump* pump)
        : pump_(pump),
          listener_(nullptr) {
    }

    Cycler::~Cycler() {
        pump_->getQueue()->remove(this);
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
        Message msg;
        msg.callback = exec;

        postAtTime(&msg, at_time);
    }

    void Cycler::post(const std::function<void()>& func, int id) {
        postDelayed(func, 0, id);
    }

    void Cycler::postDelayed(const std::function<void()>& func, uint64_t delay, int id) {
        postAtTime(func, delay + now(), id);
    }

    void Cycler::postAtTime(const std::function<void()>& func, uint64_t at_time, int id) {
        Message msg;
        msg.func = func;
        msg.id = id;

        postAtTime(&msg, at_time);
    }

    void Cycler::post(int what) {
        postDelayed(what, 0);
    }

    void Cycler::postDelayed(int id, uint64_t delay) {
        postAtTime(id, delay + now());
    }

    void Cycler::postAtTime(int id, uint64_t at_time) {
        Message msg;
        msg.id = id;

        postAtTime(&msg, at_time);
    }

    void Cycler::post(Message* msg) {
        postDelayed(msg, 0);
    }

    void Cycler::postDelayed(Message* msg, uint64_t delay) {
        postAtTime(msg, delay + now());
    }

    void Cycler::postAtTime(Message* msg, uint64_t at_time) {
        msg->time = at_time;
        enqueueMessage(msg);
    }

    void Cycler::enqueueMessage(Message* msg) {
        msg->target = this;
        pump_->getQueue()->enqueue(*msg);
        pump_->wakeup();
    }

    bool Cycler::hasMessages(int id) {
        return pump_->getQueue()->contains(this, id);
    }

    void Cycler::removeMessages(int id) {
        pump_->getQueue()->remove(this, id);
    }

    void Cycler::dispatchMessage(const Message& msg) {
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