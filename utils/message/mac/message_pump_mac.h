// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MESSAGE_MAC_MESSAGE_PUMP_MAC_H_
#define UTILS_MESSAGE_MAC_MESSAGE_PUMP_MAC_H_

#include "utils/message/message_pump.h"

#include <pthread.h>


namespace utl {
namespace mac {

    class MessagePumpMac : public MessagePump {
    public:
        ~MessagePumpMac();

        void wakeup() override;
        void loop() override;

    private:
        friend class MessagePump;

        MessagePumpMac();

        void wait(int64_t delay);
        bool platformWork();

        bool cv_pred_ = false;
        pthread_cond_t cv_;
        pthread_mutex_t cv_mutex_;
        bool is_initialized_ = false;
    };

}
}

#endif  // UTILS_MESSAGE_MAC_MESSAGE_PUMP_MAC_H_
