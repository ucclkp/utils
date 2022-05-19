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
        : message_(nullptr),
          delayed_(nullptr),
          has_barrier_(false) {}

    MessageQueue::~MessageQueue() {
        clear();
    }

    bool MessageQueue::enqueue(Message* msg) {
        if (msg->is_barrier) {
            msg->reset();
            LOG(Log::ERR) << "Illegal msg!";
            return false;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);

        for (auto it = message_; it != nullptr; it = it->next) {
            if (it->is_barrier ||
                msg->time_ns == 0 ||
                msg->time_ns < it->time_ns)
            {
                break;
            }
        }

        auto ptr = message_;

        if (!ptr ||
            ptr->is_barrier ||
            msg->time_ns == 0 ||
            msg->time_ns < ptr->time_ns)
        {
            // push_front
            msg->next = message_;
            message_ = msg;
        } else {
            decltype(ptr) prev;
            for (;;) {
                prev = ptr;
                ptr = ptr->next;
                if (!ptr ||
                    ptr->is_barrier ||
                    msg->time_ns < ptr->time_ns)
                {
                    break;
                }
            }

            // insert_after prev
            msg->next = prev->next;
            prev->next = msg;
        }

        return true;
    }

    void MessageQueue::enqueueDelayed(Message* msg) {
        assert(!msg->is_barrier);

        auto ptr = delayed_;

        if (!ptr ||
            msg->time_ns == 0 ||
            msg->time_ns < ptr->time_ns)
        {
            msg->next = delayed_;
            delayed_ = msg;
        } else {
            decltype(ptr) prev;
            for (;;) {
                prev = ptr;
                ptr = ptr->next;
                if (!ptr || msg->time_ns < ptr->time_ns) {
                    break;
                }
            }

            // insert_after prev
            msg->next = prev->next;
            prev->next = msg;
        }
    }

    bool MessageQueue::dequeue(Message** out) {
        std::lock_guard<std::mutex> lk(queue_sync_);

        auto ptr = message_;

        // find barrier
        while (ptr && !ptr->is_barrier) {
            ptr = ptr->next;
        }

        uthrow_if(!ptr, "Cannot find barrier!\n");

        // over the barrier
        auto prev = ptr;
        ptr = ptr->next;

        auto cur = Cycler::now().count();
        while (ptr) {
            // 从队列中摘除该消息
            // erase_after prev
            prev->next = ptr->next;

            // 发现可以执行的消息就立即返回，即使后面可能存在延时消息。
            // 因此外部调用应循环调用 dequeue() 直到消息队列枯竭为止，
            // 否则队列后面的消息可能永远无法执行。
            if (ptr->time_ns <= uint64_t(cur)) {
                *out = ptr;
                return true;
            }

            enqueueDelayed(ptr);
            ptr = prev->next;
        }

        return false;
    }

    bool MessageQueue::dequeueDelayed(int64_t* delay_ns, Message** out) {
        std::lock_guard<std::mutex> lk(queue_sync_);

        auto ptr = delayed_;
        decltype(ptr) prev = nullptr;

        if (ptr) {
            auto cur = Cycler::now().count();
            if (ptr->time_ns <= uint64_t(cur)) {
                // 从队列中摘除该消息
                *out = ptr;
                if (prev) {
                    // erase_after prev
                    prev->next = ptr->next;
                } else {
                    // pop_front
                    delayed_ = ptr->next;
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

    void MessageQueue::remove(Message** head, Cycler* c) {
        auto ptr = *head;
        decltype(ptr) prev = nullptr;
        for (; ptr; ) {
            if (ptr->target == c) {
                if (prev) {
                    // erase_after prev
                    prev->next = ptr->next;
                    ptr->reset();

                    ptr = prev->next;
                } else {
                    // pop_front
                    *head = ptr->next;
                    ptr->reset();

                    ptr = *head;
                }
                continue;
            }

            prev = ptr;
            ptr = ptr->next;
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

    void MessageQueue::remove(Message** head, Cycler* c, int id) {
        auto ptr = *head;
        decltype(ptr) prev = nullptr;
        for (; ptr; ) {
            if (ptr->target == c &&
                ptr->id == id)
            {
                if (prev) {
                    // erase_after prev
                    prev->next = ptr->next;
                    ptr->reset();

                    ptr = prev->next;
                } else {
                    // pop_front
                    *head = ptr->next;
                    ptr->reset();

                    ptr = *head;
                }
                continue;
            }

            prev = ptr;
            ptr = ptr->next;
        }
    }

    void MessageQueue::clear() {
        std::lock_guard<std::mutex> lk(queue_sync_);

        auto msg = message_;
        while (msg) {
            auto tmp = msg;
            msg = msg->next;
            tmp->reset();
        }

        msg = delayed_;
        while (msg) {
            auto tmp = msg;
            msg = msg->next;
            tmp->reset();
        }
    }

    bool MessageQueue::contains(Cycler* c, int id) {
        if (!c) {
            return false;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);
        return contains(message_, c, id) || contains(delayed_, c, id);
    }

    bool MessageQueue::contains(Message* head, Cycler* c, int id) {
        for (auto it = head; it; it = it->next) {
            if (it->target == c &&
                it->id == id)
            {
                return true;
            }
        }
        return false;
    }

    bool MessageQueue::hasMessages(unsigned int lists) {
        std::lock_guard<std::mutex> lk(queue_sync_);
        bool result = false;
        if (lists & ML_NORMAL) {
            result |= !!message_;
        }
        if (lists & ML_DELAYED) {
            result |= !!delayed_;
        }
        return result;
    }

    int64_t MessageQueue::getDelayedTime() {
        std::lock_guard<std::mutex> lk(queue_sync_);

        auto ptr = delayed_;
        if (ptr) {
            auto cur = Cycler::now().count();
            if (ptr->time_ns <= uint64_t(cur)) {
                return 0;
            }
            return ptr->time_ns - cur;
        }
        return -1;
    }

    void MessageQueue::addBarrier() {
        std::lock_guard<std::mutex> lk(queue_sync_);

        if (has_barrier_) {
            return;
        }

        Message* barrier = Message::get();
        barrier->is_barrier = true;
        // push_front
        barrier->next = message_;
        message_ = barrier;

        has_barrier_ = true;
    }

    void MessageQueue::removeBarrier() {
        std::lock_guard<std::mutex> lk(queue_sync_);

        auto ptr = message_;
        Message* prev = nullptr;
        for (; ptr; ptr = ptr->next) {
            if (ptr->is_barrier) {
                if (prev) {
                    // erase_after prev
                    prev->next = ptr->next;
                } else {
                    // pop_front
                    message_ = ptr->next;
                }
                ptr->reset();
                break;
            }

            prev = ptr;
        }

        has_barrier_ = false;
    }

}
