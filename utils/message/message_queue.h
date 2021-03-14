// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MESSAGE_MESSAGE_QUEUE_H_
#define UTILS_MESSAGE_MESSAGE_QUEUE_H_

#include <mutex>


namespace utl {

    class Cycler;
    class Message;

    class MessageQueue {
    public:
        MessageQueue();
        ~MessageQueue();

        void clear();
        bool hasMessage();

        bool enqueue(Message* msg);

        /**
         * 尝试从即时队列中获取一个消息。
         * 如果发现延时消息，则将该消息放入延时队列，直到发现一个可用消息或到达队列尾部。
         * 因此调用该方法时应使用循环，直到方法返回 nullptr 以防止尾部消息永远不会被执行到。
         * @return 如果队列中无消息，或没有达到指定时间的消息，返回 nullptr，否则返回消息。
         */
        Message* dequeue();

        /**
         * 尝试从延时队列中获取一个消息。
         * @param delay 如果返回值不为 nullptr，该参数的值未定义。
         *              否则，当存在延时消息时，该参数的值为剩余延时；
         *              当不存在任何消息时，该参数的值为 -1。
         * @return 如果队列中无消息，或没有达到指定时间的消息，返回 nullptr，否则返回消息。
         */
        Message* dequeueDelayed(int64_t* delay);

        void remove(Cycler* c);
        void remove(Cycler* c, int what);
        bool contains(Cycler* c, int what);

        void addBarrier();
        void removeBarrier();

    private:
        void enqueueDelayed(Message* msg);

        void remove(Message** head, Cycler* c);
        void remove(Message** head, Cycler* c, int what);
        bool contains(Message** head, Cycler* c, int what);
        void removeAll(Message** head);

        bool has_barrier_;
        Message* message_;
        Message* delayed_;
        std::mutex queue_sync_;
    };

}

#endif  // UTILS_MESSAGE_MESSAGE_QUEUE_H_