// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MESSAGE_WIN_MESSAGE_PUMP_UI_WIN_H_
#define UTILS_MESSAGE_WIN_MESSAGE_PUMP_UI_WIN_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "utils/message/message_pump.h"


namespace utl {

    class MessagePumpUIWin : public MessagePump {
    public:
        void wakeup() override;
        void loop() override;

    private:
        friend class MessagePump;

        MessagePumpUIWin();

        void wait(int64_t delay);
        bool platformWork();

        HANDLE event_;
    };

}

#endif  // UTILS_MESSAGE_WIN_MESSAGE_PUMP_UI_WIN_H_