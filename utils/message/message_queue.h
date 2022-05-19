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
        enum MessageList {
            ML_NORMAL  = 1 << 0,
            ML_DELAYED = 1 << 1,
        };

        MessageQueue();
        ~MessageQueue();

        bool enqueue(Message* msg);

        /**
         * 尝试从即时队列中获取一个消息。
         * 如果发现延时消息，则将该消息放入延时队列，直到发现一个可用消息或到达队列尾部。
         * 因此调用该方法时应使用循环，直到方法返回 false 以防止尾部消息永远不会被执行到。
         * @param out 消息
         * @return 如果队列中无消息，或没有达到指定时间的消息，返回 false，否则返回消息。
         */
        bool dequeue(Message** out);

        /**
         * 尝试从延时队列中获取一个消息。
         * @param delay_ns 如果返回值不为 false，该参数的值未定义。
         *              否则，当存在延时消息时，该参数的值为剩余延时；
         *              当不存在任何消息时，该参数的值为 -1。
         * @param out 消息
         * @return 如果队列中无消息，或没有达到指定时间的消息，返回 false，否则返回 true。
         */
        bool dequeueDelayed(int64_t* delay_ns, Message** out);

        void remove(Cycler* c);
        void remove(Cycler* c, int id);
        void clear();

        bool contains(Cycler* c, int id);
        bool hasMessages(unsigned int lists);

        /**
         * 获取延时队列中最近将要执行的消息还有多长时间可以执行。
         * @return 如果延时队列中无消息，返回 -1;
         *         如果最近的消息已经可以执行了，返回 0;
         *         如果最近的消息还不到时间，返回距执行所需的时间，单位为纳秒。
         */
        int64_t getDelayedTime();

        void addBarrier();
        void removeBarrier();

    private:
        void enqueueDelayed(Message* msg);

        void remove(Message** head, Cycler* c);
        void remove(Message** head, Cycler* c, int id);
        bool contains(Message* head, Cycler* c, int id);

        Message* message_;
        Message* delayed_;
        bool has_barrier_;
        std::mutex queue_sync_;
    };

}

#endif  // UTILS_MESSAGE_MESSAGE_QUEUE_H_
