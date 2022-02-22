// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include <thread>
#include <future>

#include "utils/message/cycler.h"
#include "utils/message/message.h"
#include "utils/message/message_pump.h"
#include "utils/message/message_queue.h"
#include "utils/unit_test/test_collector.h"


TEST_CASE(MessageUnitTest) {

    TEST_DEF("MessagePump tests.") {
        std::weak_ptr<utl::MessagePump> pump;
        std::promise<void> promise;

        std::thread worker([&pump, &promise]() {
            utl::MessagePump::create();
            pump = utl::MessagePump::getCurrent();
            promise.set_value();
            utl::MessagePump::run();
            utl::MessagePump::destroy();
            });

        promise.get_future().get();

        utl::Cycler cycler(pump);
        cycler.post(
            [pump]() {
                auto ptr = pump.lock();
                if (ptr) {
                    ptr->quit();
                }
            });

        worker.join();
        return true;
    };

}