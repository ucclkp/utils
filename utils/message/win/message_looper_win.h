// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MESSAGE_WIN_MESSAGE_LOOPER_WIN_H_
#define UTILS_MESSAGE_WIN_MESSAGE_LOOPER_WIN_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "utils/message/message_looper.h"


namespace utl {

    class MessageLooperWin : public MessageLooper {
    public:
        void wakeup() override;
        void loop() override;

    private:
        friend class MessageLooper;

        MessageLooperWin();

        void wait(int64_t delay);
        bool platformWork();

        HANDLE event_;
    };

}

#endif  // UTILS_MESSAGE_WIN_MESSAGE_LOOPER_WIN_H_