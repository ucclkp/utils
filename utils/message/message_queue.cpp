// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/message/message_queue.h"

#include "utils/log.h"
#include "utils/message/cycler.h"
#include "utils/message/message.h"


namespace utl {

    MessageQueue::MessageQueue()
        : has_barrier_(false),
          message_(nullptr),
          delayed_(nullptr) {}

    MessageQueue::~MessageQueue() {
    }

    void MessageQueue::clear() {
        std::lock_guard<std::mutex> lk(queue_sync_);
        removeAll(&message_);
        removeAll(&delayed_);
    }

    bool MessageQueue::hasMessage() {
        std::lock_guard<std::mutex> lk(queue_sync_);
        return message_ != nullptr;
    }

    bool MessageQueue::enqueue(Message* msg) {
        if (msg->is_barrier) {
            DCHECK(false) << "Illegal msg!";
            msg->recycle();
            return false;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);

        Message* ptr = message_;

        if (!ptr ||
            ptr->is_barrier ||
            msg->when == 0 ||
            msg->when < ptr->when)
        {
            msg->next = ptr;
            message_ = msg;
        } else {
            Message* prev;
            for (;;) {
                prev = ptr;
                ptr = ptr->next;
                if (!ptr || ptr->is_barrier || msg->when < ptr->when) {
                    break;
                }
                DCHECK(ptr != ptr->next);
            }

            msg->next = ptr;
            prev->next = msg;
            DCHECK(prev != msg);
        }

        return true;
    }

    void MessageQueue::enqueueDelayed(Message* msg) {
        DCHECK(!msg->is_barrier);

        Message* ptr = delayed_;

        if (!ptr ||
            msg->when == 0 ||
            msg->when < ptr->when)
        {
            msg->next = ptr;
            delayed_ = msg;
        } else {
            Message* prev;
            for (;;) {
                prev = ptr;
                ptr = ptr->next;
                if (!ptr || msg->when < ptr->when) {
                    break;
                }
                DCHECK(ptr != ptr->next);
            }

            msg->next = ptr;
            prev->next = msg;
            DCHECK(prev != msg);
        }
    }

    Message* MessageQueue::dequeue() {
        std::lock_guard<std::mutex> lk(queue_sync_);

        Message* ptr = message_;

        // find barrier
        while (ptr && !ptr->is_barrier) {
            ptr = ptr->next;
        }

        CHECK(ptr) << "Cannot find barrier!";

        // over the barrier
        Message* prev = ptr;
        ptr = ptr->next;

        auto cur = Cycler::now();
        while (ptr) {
            // 从队列中摘除该消息
            prev->next = ptr->next;

            // 发现可以执行的消息就立即返回，即使后面可能存在延时消息。
            // 因此外部调用应循环调用 dequeue() 直到消息队列枯竭为止，
            // 否则队列后面的消息可能永远无法执行。
            if (ptr->when <= cur) {
                return ptr;
            }

            enqueueDelayed(ptr);
            ptr = prev->next;
        }

        return nullptr;
    }

    Message* MessageQueue::dequeueDelayed(int64_t* delay) {
        std::lock_guard<std::mutex> lk(queue_sync_);

        Message* ptr = delayed_;
        Message* prev = nullptr;

        if (ptr) {
            auto cur = Cycler::now();
            if (ptr->when <= cur) {
                // 从队列中摘除该消息
                if (prev) {
                    prev->next = ptr->next;
                } else {
                    delayed_ = ptr->next;
                }
                return ptr;
            }

            *delay = ptr->when - cur;
        } else {
            *delay = -1;
        }

        return nullptr;
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
        Message* ptr = *head;
        Message* prev = nullptr;
        while (ptr) {
            if (ptr->target == c) {
                if (prev) {
                    prev->next = ptr->next;
                } else {
                    *head = ptr->next;
                }

                Message* msg = ptr;
                ptr = ptr->next;

                msg->recycle();
                continue;
            }

            prev = ptr;
            ptr = ptr->next;
        }
    }

    void MessageQueue::remove(Cycler* c, int what) {
        if (!c) {
            return;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);
        remove(&message_, c, what);
        remove(&delayed_, c, what);
    }

    void MessageQueue::remove(Message** head, Cycler* c, int what) {
        Message* ptr = *head;
        Message* prev = nullptr;
        while (ptr) {
            if (ptr->target == c &&
                ptr->what == what)
            {
                if (prev) {
                    prev->next = ptr->next;
                } else {
                    *head = ptr->next;
                }

                Message* msg = ptr;
                ptr = ptr->next;

                msg->recycle();
                continue;
            }

            prev = ptr;
            ptr = ptr->next;
        }
    }

    void MessageQueue::removeAll(Message** head) {
        auto ptr = *head;
        while (ptr) {
            auto msg = ptr;
            ptr = ptr->next;
            msg->recycle();
        }
        *head = nullptr;
    }

    bool MessageQueue::contains(Cycler* c, int what) {
        if (!c) {
            return false;
        }

        std::lock_guard<std::mutex> lk(queue_sync_);
        return contains(&message_, c, what) || contains(&delayed_, c, what);
    }

    bool MessageQueue::contains(Message** head, Cycler* c, int what) {
        Message* ptr = *head;
        while (ptr) {
            if (ptr->target == c &&
                ptr->what == what)
            {
                return true;
            }
            ptr = ptr->next;
        }
        return false;
    }

    void MessageQueue::addBarrier() {
        std::lock_guard<std::mutex> lk(queue_sync_);

        if (has_barrier_) {
            return;
        }

        auto barrier = Message::obtain();
        barrier->is_barrier = true;
        if (message_) {
            barrier->next = message_;
        }

        message_ = barrier;
        has_barrier_ = true;
    }

    void MessageQueue::removeBarrier() {
        std::lock_guard<std::mutex> lk(queue_sync_);

        Message* ptr = message_;
        Message* prev = nullptr;
        while (ptr) {
            if (ptr->is_barrier) {
                if (prev) {
                    prev->next = ptr->next;
                } else {
                    message_ = ptr->next;
                }
                ptr->recycle();
                break;
            }

            prev = ptr;
            ptr = ptr->next;
        }

        has_barrier_ = false;
    }

}