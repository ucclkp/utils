// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MESSAGE_MESSAGE_QUEUE_H_
#define UTILS_MESSAGE_MESSAGE_QUEUE_H_

#include <forward_list>
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

        bool enqueue(const Message& msg);

        /**
         * 尝试从即时队列中获取一个消息。
         * 如果发现延时消息，则将该消息放入延时队列，直到发现一个可用消息或到达队列尾部。
         * 因此调用该方法时应使用循环，直到方法返回 false 以防止尾部消息永远不会被执行到。
         * @param out 消息
         * @return 如果队列中无消息，或没有达到指定时间的消息，返回 false，否则返回消息。
         */
        bool dequeue(Message* out);

        /**
         * 尝试从延时队列中获取一个消息。
         * @param delay 如果返回值不为 false，该参数的值未定义。
         *              否则，当存在延时消息时，该参数的值为剩余延时；
         *              当不存在任何消息时，该参数的值为 -1。
         * @param out 消息
         * @return 如果队列中无消息，或没有达到指定时间的消息，返回 false，否则返回消息。
         */
        bool dequeueDelayed(int64_t* delay, Message* out);

        void remove(Cycler* c);
        void remove(Cycler* c, int id);
        bool contains(Cycler* c, int id);

        void addBarrier();
        void removeBarrier();

    private:
        using List = std::forward_list<Message>;

        void enqueueDelayed(const Message& msg);
        size_t size() const;

        void remove(List* head, Cycler* c);
        void remove(List* head, Cycler* c, int id);
        bool contains(List* head, Cycler* c, int id);

        List message_;
        List delayed_;
        bool has_barrier_;
        std::mutex queue_sync_;
    };

}

#endif  // UTILS_MESSAGE_MESSAGE_QUEUE_H_