// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/message/mac/message_pump_mac.h"

#include <numeric>

#include "utils/log.h"
#include "utils/message/message_queue.h"

#import <Cocoa/Cocoa.h>


namespace utl {

    MessagePumpMac::MessagePumpMac() {
        NSRunLoop* ns_loop = [NSRunLoop currentRunLoop];
        CFRunLoopRef cf_loop = [ns_loop getCFRunLoop];

        CFRunLoopObserverContext context = {0, this, nullptr, nullptr, nullptr};
        observer_ = CFRunLoopObserverCreate(
            kCFAllocatorDefault, kCFRunLoopAllActivities, YES, 0, &runLoopCalback, &context);
        if (observer_) {
            CFRunLoopAddObserver(cf_loop, observer_, kCFRunLoopCommonModes);
        }

        CFRunLoopSourceContext source_context = {
            0, this, nullptr, nullptr, nullptr, nullptr, nullptr,
            nullptr, nullptr, &onSourcePerform
        };

        source_ = CFRunLoopSourceCreate(nullptr, 1, &source_context);
        if (source_) {
            CFRunLoopAddSource(cf_loop, source_, kCFRunLoopCommonModes);
        }

        CFRunLoopTimerContext timer_context = {0, this, nullptr, nullptr, nullptr};
        timer_ = CFRunLoopTimerCreate(
                                      kCFAllocatorDefault,
                                      std::numeric_limits<double>::max(), std::numeric_limits<double>::max(),
                                      0, 0,
                                      &onTimerPerform, &timer_context);
        if (timer_) {
            CFRunLoopAddTimer(cf_loop, timer_, kCFRunLoopCommonModes);
        }
    }

    MessagePumpMac::~MessagePumpMac() {
        if (timer_) {
            CFRelease(timer_);
        }
        if (source_) {
            CFRelease(source_);
        }
        if (observer_) {
            CFRelease(observer_);
        }
    }

    void MessagePumpMac::onSourcePerform(void* info) {
        auto ptr = static_cast<utl::MessagePumpMac*>(info);
        if (!ptr->doWork()) {
            [NSApp stop:nil];
        }
    }

    void MessagePumpMac::onTimerPerform(CFRunLoopTimerRef timer, void* info) {
        auto ptr = static_cast<utl::MessagePumpMac*>(info);
        if (ptr->source_) {
            CFRunLoopSourceSignal(ptr->source_);
        }
    }

    void MessagePumpMac::runLoopCalback(
        CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info)
    {
        //auto ptr = static_cast<utl::MessageLooperMac*>(info);
        switch (activity) {
        case kCFRunLoopEntry:
            //LOG(Log::INFO) << "[MessageLooper] Observer: Entry!";
            break;
        case kCFRunLoopBeforeTimers:
            //LOG(Log::INFO) << "[MessageLooper] Observer: Before Timers!";
            break;
        case kCFRunLoopBeforeSources:
            //LOG(Log::INFO) << "[MessageLooper] Observer: Before Sources!";
            break;
        case kCFRunLoopBeforeWaiting:
            //LOG(Log::INFO) << "[MessageLooper] Observer: Before Waiting!";
            break;
        case kCFRunLoopAfterWaiting:
            //LOG(Log::INFO) << "[MessageLooper] Observer: After Waiting!";
            break;
        case kCFRunLoopExit:
            //LOG(Log::INFO) << "[MessageLooper] Observer: Exit!";
            break;
        }
    }

    void MessagePumpMac::wakeup() {
        if (source_) {
            NSRunLoop* ns_loop = [NSRunLoop currentRunLoop];
            CFRunLoopRef cf_loop = [ns_loop getCFRunLoop];
            CFRunLoopSourceSignal(source_);
            CFRunLoopWakeUp(cf_loop);
        }
    }

    bool MessagePumpMac::doWork() {
        if (quit_imm_) {
            return false;
        }

        bool has_more_work = cosume();
        if (quit_imm_) {
            return false;
        }

        int64_t delay;
        has_more_work |= cosumeDelayed(&delay);
        if (quit_imm_) {
            return false;
        }

        if (!has_more_work && quit_when_idle_) {
            return false;
        }

        wait(delay);
        return true;
    }

    void MessagePumpMac::wait(int64_t delay) {
        if (delay != -1) {
            if (timer_) {
                auto next = CFAbsoluteTimeGetCurrent() + delay / 1000.0;
                CFRunLoopTimerSetNextFireDate(timer_, next);
            }
        }
    }

    void MessagePumpMac::loop() {
        [NSApp run];
    }

}
