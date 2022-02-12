// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/message/mac/message_pump_mac.h"

#include <numeric>

#include "utils/log.h"
#include "utils/message/message_queue.h"


namespace utl {
namespace mac {

    MessagePumpMac::MessagePumpMac() {
        int ret = pthread_cond_init(&cv_, nullptr);
        if (ret != 0) {
            LOG(Log::ERR) << "Cannot create pthread cv: " << ret;
            return;
        }

        ret = pthread_mutex_init(&cv_mutex_, nullptr);
        if (ret != 0) {
            pthread_cond_destroy(&cv_);
            LOG(Log::ERR) << "Cannot create pthread mutex: " << ret;
            return;
        }

        is_initialized_ = true;
    }

    MessagePumpMac::~MessagePumpMac() {
        if (is_initialized_) {
            pthread_cond_destroy(&cv_);
            pthread_mutex_destroy(&cv_mutex_);
            is_initialized_ = false;
        }
    }

    void MessagePumpMac::wakeup() {
        if (is_initialized_) {
            pthread_mutex_lock(&cv_mutex_);
            cv_pred_ = true;
            pthread_mutex_unlock(&cv_mutex_);

            pthread_cond_signal(&cv_);
        }
    }

    bool MessagePumpMac::platformWork() {
        return false;
    }

    void MessagePumpMac::wait(int64_t delay) {
        if (!is_initialized_) {
            return;
        }

        pthread_mutex_lock(&cv_mutex_);

        if (delay != -1) {
            timespec pt;
            pt.tv_sec = delay / 1000000000;
            pt.tv_nsec = delay % 1000000000;

            for (; !cv_pred_; ) {
                pthread_cond_timedwait_relative_np(&cv_, &cv_mutex_, &pt);
            }
        } else {
            for (; !cv_pred_; ) {
                pthread_cond_wait(&cv_, &cv_mutex_);
            }
        }

        cv_pred_ = false;
        pthread_mutex_unlock(&cv_mutex_);
    }

    void MessagePumpMac::loop() {
        for (;;) {
            bool has_more_work = platformWork();
            if (context_.top().quit_imm_) {
                break;
            }

            has_more_work |= cosume();
            if (context_.top().quit_imm_) {
                break;
            }

            int64_t delay_ns;
            has_more_work |= cosumeDelayed(&delay_ns);
            if (context_.top().quit_imm_) {
                break;
            }

            if (has_more_work) {
                continue;
            }

            if (context_.top().quit_when_idle_) {
                break;
            }

            wait(delay_ns);
        }
    }

}
}
