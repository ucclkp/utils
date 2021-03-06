// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/message/message.h"

#include "utils/log.h"


namespace utl {

    Message* Message::pool = nullptr;
    uint64_t Message::pool_size = 0;
    bool Message::is_initialized = false;
    std::mutex Message::pool_sync;


    Message::Message()
        : what(-1),
          when(0),
          target(nullptr),
          callback(nullptr),
          data(nullptr),
          next(nullptr),
          is_barrier(false) {
    }

    Message::~Message() {
    }

    void Message::init(uint64_t pool_capacity) {
        if (is_initialized) {
            LOG(Log::ERR) << "can only init once.";
            return;
        }

        Message* ptr = nullptr;

        for (uint64_t i = 0; i < pool_capacity; ++i) {
            Message* msg = new Message();
            if (pool) {
                ptr->next = msg;
                ptr = msg;
            } else {
                pool = msg;
                ptr = pool;
            }
        }

        is_initialized = true;
        pool_size = pool_capacity;
    }

    void Message::clean() {
        while (pool) {
            Message* msg = pool;
            pool = msg->next;
            delete msg;
        }

        is_initialized = false;
        pool_size = 0;
    }

    Message* Message::obtain() {
        std::lock_guard<std::mutex> lk(pool_sync);

        if (pool) {
            Message* msg = pool;
            pool = msg->next;
            msg->next = nullptr;
            --pool_size;

            return msg;
        }

        return new Message();
    }

    void Message::recycle() {
        std::lock_guard<std::mutex> lk(pool_sync);

        this->next = pool;
        pool = this;
        ++pool_size;

        what = -1;
        when = 0;
        target = nullptr;
        callback = nullptr;
        func = nullptr;
        data = nullptr;
        is_barrier = false;
    }

}