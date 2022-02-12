// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MESSAGE_MAC_MESSAGE_PUMP_UI_MAC_H_
#define UTILS_MESSAGE_MAC_MESSAGE_PUMP_UI_MAC_H_

#include "utils/message/message_pump.h"

#import <CoreFoundation/CoreFoundation.h>


namespace utl {
namespace mac {

    class MessagePumpUIMac : public MessagePump {
    public:
        ~MessagePumpUIMac();

        void wakeup() override;
        void loop() override;

    private:
        friend class MessagePump;

        MessagePumpUIMac();

        static void onSourcePerform(void* info);
        static void onTimerPerform(CFRunLoopTimerRef timer, void* info);
        static void runLoopCalback(
            CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info);

        void wait(int64_t delay);
        bool doWork();

        CFRunLoopRef cf_loop_;
        CFRunLoopSourceRef source_;
        CFRunLoopTimerRef timer_;
        CFRunLoopObserverRef observer_;
    };

}
}

#endif  // UTILS_MESSAGE_MAC_MESSAGE_PUMP_UI_MAC_H_
