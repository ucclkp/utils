// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/message/mac/message_pump_ui_mac.h"

#include <numeric>

#include "utils/log.h"
#include "utils/message/message_queue.h"

#import <Cocoa/Cocoa.h>


namespace utl {

    MessagePumpUIMac::MessagePumpUIMac() {
        auto ns_loop = [NSRunLoop currentRunLoop];
        cf_loop_ = [ns_loop getCFRunLoop];

        CFRunLoopObserverContext context = {0, this, nullptr, nullptr, nullptr};
        observer_ = CFRunLoopObserverCreate(
            kCFAllocatorDefault, kCFRunLoopAllActivities, YES, 0, &runLoopCalback, &context);
        if (observer_) {
            CFRunLoopAddObserver(cf_loop_, observer_, kCFRunLoopCommonModes);
        }

        CFRunLoopSourceContext source_context = {
            0, this, nullptr, nullptr, nullptr, nullptr, nullptr,
            nullptr, nullptr, &onSourcePerform
        };

        source_ = CFRunLoopSourceCreate(nullptr, 1, &source_context);
        if (source_) {
            CFRunLoopAddSource(cf_loop_, source_, kCFRunLoopCommonModes);
        }

        CFRunLoopTimerContext timer_context = {0, this, nullptr, nullptr, nullptr};
        timer_ = CFRunLoopTimerCreate(
                                      kCFAllocatorDefault,
                                      std::numeric_limits<double>::max(), std::numeric_limits<double>::max(),
                                      0, 0,
                                      &onTimerPerform, &timer_context);
        if (timer_) {
            CFRunLoopAddTimer(cf_loop_, timer_, kCFRunLoopCommonModes);
        }
    }

    MessagePumpUIMac::~MessagePumpUIMac() {
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

    void MessagePumpUIMac::onSourcePerform(void* info) {
        auto ptr = static_cast<utl::MessagePumpUIMac*>(info);
        if (!ptr->doWork()) {
            [NSApp stop:nil];
        }
    }

    void MessagePumpUIMac::onTimerPerform(CFRunLoopTimerRef timer, void* info) {
        auto ptr = static_cast<utl::MessagePumpUIMac*>(info);
        if (ptr->source_) {
            CFRunLoopSourceSignal(ptr->source_);
        }
    }

    void MessagePumpUIMac::runLoopCalback(
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

    void MessagePumpUIMac::wakeup() {
        if (source_ && cf_loop_) {
            CFRunLoopSourceSignal(source_);
            CFRunLoopWakeUp(cf_loop_);
        }
    }

    bool MessagePumpUIMac::doWork() {
        if (context_.top().quit_imm_) {
            return false;
        }

        bool has_more_work = cosume();
        if (context_.top().quit_imm_) {
            return false;
        }

        int64_t delay;
        has_more_work |= cosumeDelayed(&delay);
        if (context_.top().quit_imm_) {
            return false;
        }

        if (!has_more_work && context_.top().quit_when_idle_) {
            return false;
        }

        wait(delay);
        return true;
    }

    void MessagePumpUIMac::wait(int64_t delay) {
        if (delay != -1) {
            if (timer_) {
                auto next = CFAbsoluteTimeGetCurrent() + delay / 1000000000.0;
                CFRunLoopTimerSetNextFireDate(timer_, next);
            }
        }
    }

    void MessagePumpUIMac::loop() {
        [NSApp run];
    }

}
