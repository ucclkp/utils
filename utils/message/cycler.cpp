// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/message/cycler.h"

#include <chrono>

#include "utils/message/executable.h"
#include "utils/message/message.h"
#include "utils/message/message_pump.h"
#include "utils/message/message_queue.h"


namespace utl {

    Cycler::Cycler()
        : pump_(MessagePump::getCurrent()),
          listener_(nullptr),
          clear_when_destroy_(true) {}

    Cycler::Cycler(const std::weak_ptr<MessagePump>& pump)
        : pump_(pump),
          listener_(nullptr),
          clear_when_destroy_(true) {}

    Cycler::~Cycler() {
        if (clear_when_destroy_.load(std::memory_order_relaxed)) {
            clear();
        }
    }

    void Cycler::setListener(CyclerListener* l) {
        listener_ = l;
    }

    void Cycler::setClearWhenDestroy(bool val) {
        clear_when_destroy_.store(val, std::memory_order_relaxed);
    }

    void Cycler::post(Executable* exec, int id) {
        postDelayed(exec, ns(0), id);
    }

    void Cycler::postDelayed(Executable* exec, nsp delay, int id) {
        postAtTime(exec, delay + now(), id);
    }

    void Cycler::postAtTime(Executable* exec, nsp at_time, int id) {
        Message* msg = Message::get();
        msg->callback = exec;
        msg->id = id;

        postAtTime(msg, at_time);
    }

    void Cycler::post(const std::function<void()>& func, int id) {
        postDelayed(func, ns(0), id);
    }

    void Cycler::postDelayed(const std::function<void()>& func, nsp delay, int id) {
        postAtTime(func, delay + now(), id);
    }

    void Cycler::postAtTime(const std::function<void()>& func, nsp at_time, int id) {
        Message* msg = Message::get();
        msg->func = func;
        msg->id = id;

        postAtTime(msg, at_time);
    }

    void Cycler::post(int id) {
        postDelayed(id, ns(0));
    }

    void Cycler::postDelayed(int id, nsp delay) {
        postAtTime(id, delay + now());
    }

    void Cycler::postAtTime(int id, nsp at_time) {
        Message* msg = Message::get();
        msg->id = id;

        postAtTime(msg, at_time);
    }

    void Cycler::post(Message* msg) {
        postDelayed(msg, ns(0));
    }

    void Cycler::postDelayed(Message* msg, nsp delay) {
        postAtTime(msg, delay + now());
    }

    void Cycler::postAtTime(Message* msg, nsp at_time) {
        msg->time_ns = at_time.count();
        enqueueMessage(msg);
    }

    void Cycler::clear() {
        auto ptr = pump_.lock();
        if (ptr) {
            ptr->getQueue()->remove(this);
        }
    }

    void Cycler::enqueueMessage(Message* msg) {
        msg->target = this;
        auto ptr = pump_.lock();
        if (ptr) {
            ptr->getQueue()->enqueue(msg);
            ptr->wakeup();
        } else {
            msg->reset();
        }
    }

    bool Cycler::hasMessages(int id) {
        auto ptr = pump_.lock();
        if (ptr) {
            return ptr->getQueue()->contains(this, id);
        }
        return false;
    }

    void Cycler::removeMessages(int id) {
        auto ptr = pump_.lock();
        if (ptr) {
            ptr->getQueue()->remove(this, id);
        }
    }

    void Cycler::dispatchMessage(const Message& msg) {
        if (listener_) {
            listener_->onHandleMessage(msg);
        }
    }

    // static
    Cycler::ns Cycler::now() {
        return TimeUtils::upTime();
    }

}