// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/message/win/message_pump_win.h"

#include "utils/log.h"

#include "utils/message/message_queue.h"


namespace utl {

    MessagePumpWin::MessagePumpWin() {
        event_ = ::CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!event_) {
            LOG(Log::ERR) << "Cannot create event: " << ::GetLastError();
        }
    }

    void MessagePumpWin::wakeup() {
        ::SetEvent(event_);
    }

    void MessagePumpWin::loop() {
        for (;;) {
            bool has_more_work = platformWork();
            if (context_.top().quit_imm_) {
                break;
            }

            has_more_work |= cosume();
            if (context_.top().quit_imm_) {
                break;
            }

            int64_t delay;
            has_more_work |= cosumeDelayed(&delay);
            if (context_.top().quit_imm_) {
                break;
            }

            if (has_more_work) {
                continue;
            }

            if (context_.top().quit_when_idle_) {
                break;
            }

            wait(delay);
        }
    }

    void MessagePumpWin::wait(int64_t delay) {
        DWORD timeout = delay < 0 ? INFINITE : DWORD(delay);
        DWORD result = ::WaitForSingleObjectEx(event_, timeout, TRUE);
        if (result == WAIT_OBJECT_0) {
            //
        }
        ::ResetEvent(event_);
        DCHECK(result != WAIT_FAILED);
    }

    bool MessagePumpWin::platformWork() {
        return false;
    }

}
