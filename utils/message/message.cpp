// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/message/message.h"


namespace utl {

    // static
    Message::MessagePool Message::s_pool_;

    Message::Message()
        : id(-1),
          time_ns(0),
          target(nullptr),
          callback(nullptr),
          ui1(0),
          ui2(0),
          data(nullptr),
          next(nullptr),
          is_barrier(false) {
    }

    Message::~Message() {
    }

    // static
    Message* Message::get() {
        return s_pool_.get();
    }

    void Message::reset() {
        id = -1;
        time_ns = 0;
        target = nullptr;
        callback = nullptr;
        func = nullptr;
        ui1 = 0;
        ui2 = 0;
        data = nullptr;
        shared_data.reset();
        is_barrier = false;

        s_pool_.put(this);
    }


    // Message::MessagePool
    Message::MessagePool::MessagePool()
        : pool_(nullptr)
    {
        reserve(100);
    }

    Message::MessagePool::~MessagePool() {
        clean();
    }

    void Message::MessagePool::reserve(size_t cnt) {
        if (cnt == 0) {
            return;
        }

        std::lock_guard<std::mutex> lg(pool_sync_);
        if (!pool_) {
            pool_ = new Message();
        }

        auto it = pool_;
        size_t cur = 0;
        while (it) {
            ++cur;
            if (!it->next) {
                break;
            }
            it = it->next;
        }

        for (; cur < cnt; ++cur) {
            auto msg = new Message();
            it->next = msg;
            it = msg;
        }
    }

    void Message::MessagePool::clean() {
        std::lock_guard<std::mutex> lg(pool_sync_);
        auto it = pool_;
        while (it) {
            auto tmp = it;
            it = it->next;
            delete tmp;
        }
        pool_ = nullptr;
    }

    Message* Message::MessagePool::get() {
        {
            std::lock_guard<std::mutex> lg(pool_sync_);
            auto msg = pool_;
            if (msg) {
                pool_ = msg->next;
                msg->next = nullptr;
                return msg;
            }
        }

        return new Message();
    }

    void Message::MessagePool::put(Message* m) {
        std::lock_guard<std::mutex> lg(pool_sync_);
        m->next = pool_;
        pool_ = m;
    }

}