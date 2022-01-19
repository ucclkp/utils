// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/message/message_queue.h"

#include <cassert>

#include "utils/log.h"
#include "utils/message/cycler.h"
#include "utils/message/message.h"


namespace utl {

    MessageQueue::MessageQueue()
        : has_barrier_(false) {}

    MessageQueue::~MessageQueue() {
    }

    void MessageQueue::clear() {
        std::lock_guard<std::mutex> lk(queue_sync_);
        message_.clear();
        delayed_.clear();
    }

    bool MessageQueue::hasMessage() {
        std::lock_guard<std::mutex> lk(queue_sync_);
        return !message_.empty();
    }

    bool MessageQueue::enqueue(const Message& msg) {
        if (msg.is_barrier) {
            LOG(Log::ERR) << "Illegal msg!";
            return false;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);

        for (auto it = message_.begin(); it != message_.end(); ++it) {
            if (it->is_barrier || msg.time_ns == 0 || msg.time_ns < it->time_ns) {
                break;
            }
        }

        auto ptr = message_.begin();

        if (ptr == message_.end() ||
            ptr->is_barrier ||
            msg.time_ns == 0 ||
            msg.time_ns < ptr->time_ns)
        {
            message_.push_front(msg);
        } else {
            decltype(ptr) prev;
            for (;;) {
                prev = ptr;
                ++ptr;
                if (ptr == message_.end() || ptr->is_barrier || msg.time_ns < ptr->time_ns) {
                    break;
                }
            }

            message_.insert_after(prev, msg);
        }

        return true;
    }

    void MessageQueue::enqueueDelayed(const Message& msg) {
        assert(!msg.is_barrier);

        auto ptr = delayed_.begin();

        if (ptr == delayed_.end() ||
            msg.time_ns == 0 ||
            msg.time_ns < ptr->time_ns)
        {
            delayed_.push_front(msg);
        } else {
            decltype(ptr) prev;
            for (;;) {
                prev = ptr;
                ++ptr;
                if (ptr == delayed_.end() || msg.time_ns < ptr->time_ns) {
                    break;
                }
            }

            delayed_.insert_after(prev, msg);
        }
    }

    bool MessageQueue::dequeue(Message* out) {
        std::lock_guard<std::mutex> lk(queue_sync_);

        auto ptr = message_.begin();

        // find barrier
        while (ptr != message_.end() && !ptr->is_barrier) {
            ++ptr;
        }

        panic_if(ptr == message_.end(), "Cannot find barrier!\n");

        // over the barrier
        auto prev = ptr;
        ++ptr;

        auto cur = Cycler::now().count();
        while (ptr != message_.end()) {
            // 从队列中摘除该消息
            auto msg = *ptr;
            message_.erase_after(prev);

            // 发现可以执行的消息就立即返回，即使后面可能存在延时消息。
            // 因此外部调用应循环调用 dequeue() 直到消息队列枯竭为止，
            // 否则队列后面的消息可能永远无法执行。
            if (msg.time_ns <= uint64_t(cur)) {
                *out = msg;
                return true;
            }

            enqueueDelayed(msg);
            ptr = prev;
            ++ptr;
        }

        return false;
    }

    bool MessageQueue::dequeueDelayed(int64_t* delay_ns, Message* out) {
        std::lock_guard<std::mutex> lk(queue_sync_);

        auto ptr = delayed_.begin();
        auto prev = delayed_.end();

        if (ptr != delayed_.end()) {
            auto cur = Cycler::now().count();
            if (ptr->time_ns <= uint64_t(cur)) {
                // 从队列中摘除该消息
                *out = *ptr;
                if (prev != delayed_.end()) {
                    delayed_.erase_after(prev);
                } else {
                    delayed_.pop_front();
                }
                return true;
            }

            *delay_ns = ptr->time_ns - cur;
        } else {
            *delay_ns = -1;
        }

        return false;
    }

    void MessageQueue::remove(Cycler* c) {
        if (!c) {
            return;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);
        remove(&message_, c);
        remove(&delayed_, c);
    }

    void MessageQueue::remove(List* head, Cycler* c) {
        auto ptr = head->begin();
        auto prev = head->end();
        for (; ptr != head->end(); ++ptr) {
            if (ptr->target == c) {
                if (prev != head->end()) {
                    head->erase_after(prev);
                    ptr = prev;
                } else {
                    head->pop_front();
                    ptr = head->before_begin();
                }
                continue;
            }

            prev = ptr;
        }
    }

    void MessageQueue::remove(Cycler* c, int id) {
        if (!c) {
            return;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);
        remove(&message_, c, id);
        remove(&delayed_, c, id);
    }

    void MessageQueue::remove(List* head, Cycler* c, int id) {
        auto ptr = head->begin();
        auto prev = head->end();
        for (; ptr != head->end(); ++ptr) {
            if (ptr->target == c &&
                ptr->id == id)
            {
                if (prev != head->end()) {
                    head->erase_after(prev);
                    ptr = prev;
                } else {
                    head->pop_front();
                    ptr = head->before_begin();
                }
                continue;
            }

            prev = ptr;
        }
    }

    bool MessageQueue::contains(Cycler* c, int id) {
        if (!c) {
            return false;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);
        return contains(&message_, c, id) || contains(&delayed_, c, id);
    }

    bool MessageQueue::contains(List* head, Cycler* c, int id) {
        for (auto it = head->begin(); it != head->end(); ++it) {
            if (it->target == c &&
                it->id == id)
            {
                return true;
            }
        }
        return false;
    }

    void MessageQueue::addBarrier() {
        std::lock_guard<std::mutex> lk(queue_sync_);

        if (has_barrier_) {
            return;
        }

        Message barrier;
        barrier.is_barrier = true;
        message_.push_front(barrier);
        has_barrier_ = true;
    }

    void MessageQueue::removeBarrier() {
        std::lock_guard<std::mutex> lk(queue_sync_);

        auto ptr = message_.begin();
        auto prev = message_.end();
        for (; ptr != message_.end(); ++ptr) {
            if (ptr->is_barrier) {
                if (prev != message_.end()) {
                    message_.erase_after(prev);
                } else {
                    message_.pop_front();
                }
                break;
            }

            prev = ptr;
        }

        has_barrier_ = false;
    }

}
