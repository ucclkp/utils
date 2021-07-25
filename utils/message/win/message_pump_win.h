// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MESSAGE_WIN_MESSAGE_PUMP_WIN_H_
#define UTILS_MESSAGE_WIN_MESSAGE_PUMP_WIN_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "utils/message/message_pump.h"


namespace utl {

    class MessagePumpWin : public MessagePump {
    public:
        void wakeup() override;
        void loop() override;

    private:
        friend class MessagePump;

        MessagePumpWin();

        void wait(int64_t delay_ns);
        bool platformWork();

        HANDLE event_;
    };

}

#endif  // UTILS_MESSAGE_WIN_MESSAGE_PUMP_WIN_H_