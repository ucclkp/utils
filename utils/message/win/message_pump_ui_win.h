// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MESSAGE_WIN_MESSAGE_PUMP_UI_WIN_H_
#define UTILS_MESSAGE_WIN_MESSAGE_PUMP_UI_WIN_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <atomic>
#include <vector>

#include "utils/message/message_pump.h"


namespace utl {

    class WinMessageIntercepter {
    public:
        virtual ~WinMessageIntercepter() = default;

        virtual void onPrePeekMessage() {}
        virtual void onPostPeekMessage() {}

        virtual bool onPreProcessMessage(MSG* msg) = 0;
        virtual bool onAfterTranslateMessage(MSG* msg) = 0;
        virtual void onPostProcessMessage(MSG* msg) {}
    };

    class MessagePumpUIWin : public MessagePump {
    public:
        ~MessagePumpUIWin();

        void wakeup() override;
        void loop() override;

        void setInSizeModalLoop(bool in_sml);
        void setInMoveModalLoop(bool in_mml);
        void setInDialogModalLoop(bool in_dml);

        bool isInSizeModalLoop() const;
        bool isInMoveModalLoop() const;

        void addIntercepter(WinMessageIntercepter* i);
        void removeIntercepter(WinMessageIntercepter* i);

    private:
        friend class MessagePump;

        MessagePumpUIWin();

        void initWaitingObjs();

        void wait(int64_t delay);
        bool platformWork();

        void notifyPrePeekMessage();
        void notifyPostPeekMessage();
        bool notifyPreProcessMessage(MSG* msg);
        bool notifyAfterTranslateMessage(MSG* msg);
        void notifyPostProcessMessage(MSG* msg);

        static LRESULT CALLBACK ModalLoopMsgHookProc(int code, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK ModalLoopIdleHookProc(int code, WPARAM wParam, LPARAM lParam);
        static DWORD WINAPI waitingThreadProc(LPVOID param);

        HANDLE waiting_thread_ = nullptr;
        HANDLE waiting_event_;
        DWORD ui_thread_id_;
        std::atomic_bool wt_finished_ = false;
        std::atomic_int64_t waiting_ns_ = -1;

        HHOOK msg_hook_;
        HHOOK idle_hook_;
        HANDLE event_;
        bool is_in_sml_ = false;
        bool is_in_mml_ = false;
        bool is_in_dml_ = false;
        std::vector<WinMessageIntercepter*> intercepters_;
    };

}

#endif  // UTILS_MESSAGE_WIN_MESSAGE_PUMP_UI_WIN_H_