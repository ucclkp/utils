// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/message/win/message_pump_ui_win.h"

#include <cassert>

#include "utils/log.h"
#include "utils/message/message_queue.h"
#include "utils/multi_callbacks.hpp"

#define WM_WAKELOOP  (WM_APP + 1)


namespace utl {
namespace win {

    MessagePumpUIWin::MessagePumpUIWin() {
        event_ = ::CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!event_) {
            LOG(Log::ERR) << "Cannot create event: " << ::GetLastError();
        }

        ui_thread_id_ = ::GetCurrentThreadId();

        /**
         * 在调用 GetMessage / PeekMessage 获得消息时收到通知。
         * 消息泵初始化时即安装钩子，但只在 setInSizeModalLoop 或 setInMoveModalLoop 设置
         * 为 true 时做事。
         */
        msg_hook_ = ::SetWindowsHookExW(WH_GETMESSAGE, ModalLoopMsgHookProc, nullptr, ui_thread_id_);
        if (!msg_hook_) {
            LOG(Log::ERR) << "Cannot create hook: " << ::GetLastError();
        }

        /**
         * 在调用 GetMessage / PeekMessage 耗尽系统队列中的消息时收到通知。
         * 消息泵初始化时即安装钩子，但只在 setInSizeModalLoop 或 setInMoveModalLoop 设置
         * 为 true 时做事。
         */
        idle_hook_ = ::SetWindowsHookExW(WH_FOREGROUNDIDLE, ModalLoopIdleHookProc, nullptr, ui_thread_id_);
        if (!idle_hook_) {
            LOG(Log::ERR) << "Cannot create hook: " << ::GetLastError();
        }

        initWaitingObjs();
    }

    MessagePumpUIWin::~MessagePumpUIWin() {
        if (waiting_thread_) {
            wt_finished_ = true;
            ::SetEvent(waiting_event_);
            ::WaitForSingleObject(waiting_thread_, INFINITE);
            ::CloseHandle(waiting_thread_);
        }
        if (waiting_event_) {
            ::CloseHandle(waiting_event_);
        }
        if (event_) {
            ::CloseHandle(event_);
        }
        if (msg_hook_) {
            ::UnhookWindowsHookEx(msg_hook_);
        }
        if (idle_hook_) {
            ::UnhookWindowsHookEx(idle_hook_);
        }
    }

    void MessagePumpUIWin::initWaitingObjs() {
        /**
         * 创建线程，用于在系统对话框消息循环时支持我们自己的延时消息。
         */
        waiting_event_ = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!waiting_event_) {
            LOG(Log::ERR) << "Cannot create waiting event: " << ::GetLastError();
            return;
        }

        waiting_thread_ = ::CreateThread(nullptr, 0, waitingThreadProc, this, 0, nullptr);
        if (!waiting_thread_) {
            LOG(Log::ERR) << "Cannot create waiting thread: " << ::GetLastError();
        }
    }

    void MessagePumpUIWin::wakeup() {
        if (is_in_mml_ || is_in_sml_ || is_in_dml_) {
            ::PostThreadMessageW(ui_thread_id_, WM_WAKELOOP, 0, 0);
            return;
        }

        if (event_) {
            ::SetEvent(event_);
        }
    }

    void MessagePumpUIWin::loop() {
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

    void MessagePumpUIWin::setInSizeModalLoop(bool in_sml) {
        is_in_sml_ = in_sml;
    }

    void MessagePumpUIWin::setInMoveModalLoop(bool in_mml) {
        is_in_mml_ = in_mml;
    }

    void MessagePumpUIWin::setInDialogModalLoop(bool in_dml) {
        is_in_dml_ = in_dml;
    }

    bool MessagePumpUIWin::isInSizeModalLoop() const {
        return is_in_sml_;
    }

    bool MessagePumpUIWin::isInMoveModalLoop() const {
        return is_in_mml_;
    }

    void MessagePumpUIWin::addIntercepter(WinMessageIntercepter* i) {
        addCallbackTo(intercepters_, i);
    }

    void MessagePumpUIWin::removeIntercepter(WinMessageIntercepter* i) {
        removeCallbackFrom(intercepters_, i);
    }

    void MessagePumpUIWin::wait(int64_t delay) {
        DWORD timeout = delay < 0 ? INFINITE : DWORD(delay / 1000000);
        DWORD result = ::MsgWaitForMultipleObjectsEx(
            1, &event_, timeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
        if (result == WAIT_OBJECT_0) {
            //
        }
        ::ResetEvent(event_);
        ubassert(result != WAIT_FAILED);
    }

    bool MessagePumpUIWin::platformWork() {
        notifyPrePeekMessage();

        /*
         * 处理所有系统事件，以防止一些持续产生的事件（比如绘制事件）
         * 拖慢系统事件的处理。
         */
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                context_.top().quit_imm_ = true;
                return false;
            }

            auto ret = notifyPreProcessMessage(&msg);
            if (!ret) {
                ::TranslateMessage(&msg);
                ret = notifyAfterTranslateMessage(&msg);
                if (!ret) {
                    ::DispatchMessage(&msg);
                }
            }
            notifyPostProcessMessage(&msg);
        }

        notifyPostPeekMessage();
        return false;
    }

    void MessagePumpUIWin::notifyPrePeekMessage() {
        for (auto cb : intercepters_) {
            cb->onPrePeekMessage();
        }
    }

    void MessagePumpUIWin::notifyPostPeekMessage() {
        for (auto cb : intercepters_) {
            cb->onPostPeekMessage();
        }
    }

    bool MessagePumpUIWin::notifyPreProcessMessage(MSG* msg) {
        bool ret = false;
        for (auto cb : intercepters_) {
            ret |= cb->onPreProcessMessage(msg);
        }
        return ret;
    }

    bool MessagePumpUIWin::notifyAfterTranslateMessage(MSG* msg) {
        bool ret = false;
        for (auto cb : intercepters_) {
            ret |= cb->onAfterTranslateMessage(msg);
        }
        return ret;
    }

    void MessagePumpUIWin::notifyPostProcessMessage(MSG* msg) {
        for (auto cb : intercepters_) {
            cb->onPostProcessMessage(msg);
        }
    }

    // static
    LRESULT CALLBACK MessagePumpUIWin::ModalLoopMsgHookProc(int code, WPARAM wParam, LPARAM lParam) {
        /**
         * 在调整窗口位置或大小时，嵌套消息循环每获得特定的消息，就跑一下我们自己的消息队列。
         * 仅对特定的消息做处理，以防止卡顿。
         */
        auto This = static_cast<MessagePumpUIWin*>(getCurrent());
        if (code == HC_ACTION) {
            if (This->is_in_sml_ || This->is_in_mml_) {
                UINT remove_flag = UINT(wParam);
                MSG* msg = reinterpret_cast<MSG*>(lParam);
                //LOG(Log::INFO) << "============ MSG HOOK: " << msg->message;

                if (remove_flag == PM_REMOVE && msg->hwnd) {
                    bool should_process = false;
                    if (This->is_in_sml_) {
                        // 调整窗口大小时，似乎监听不到 WM_SIZE，最好的时机似乎只有 WM_MOUSEMOVE
                        should_process |= msg->message == WM_MOUSEMOVE;
                    }
                    if (This->is_in_mml_) {
                        // 移动窗口时，只监听 WM_MOVE 消息就足够了
                        should_process |= msg->message == WM_MOVE;
                    }

                    if (should_process) {
                        int64_t delay_ns;
                        This->cosume();
                        This->cosumeDelayed(&delay_ns);
                    }
                }
            }
        }

        return ::CallNextHookEx(This->msg_hook_, code, wParam, lParam);
    }

    // static
    LRESULT CALLBACK MessagePumpUIWin::ModalLoopIdleHookProc(int code, WPARAM wParam, LPARAM lParam) {
        /**
         * 在嵌套消息循环处理完消息，进入等待状态时，跑一下我们自己的消息队列。
         * 对于我们自己的消息队列中的延时消息，使用工作线程等待，超时后通知嵌套消息循环。
         */
        auto This = static_cast<MessagePumpUIWin*>(getCurrent());
        if (code == HC_ACTION) {
            if (This->is_in_sml_ || This->is_in_mml_ || This->is_in_dml_) {
                int64_t delay_ns;
                bool has_more_work = This->cosume();
                has_more_work |= This->cosumeDelayed(&delay_ns);
                if (has_more_work) {
                    ::PostThreadMessageW(This->ui_thread_id_, WM_WAKELOOP, 0, 0);
                } else {
                    This->waiting_ns_ = delay_ns;
                    ::SetEvent(This->waiting_event_);
                }
            }
        }

        return ::CallNextHookEx(This->msg_hook_, code, wParam, lParam);
    }

    // static
    DWORD WINAPI MessagePumpUIWin::waitingThreadProc(LPVOID param) {
        auto This = static_cast<MessagePumpUIWin*>(param);
        for (;;) {
            int64_t timeout = This->waiting_ns_.exchange(-1);
            if (timeout == -1) {
                timeout = INFINITE;
            }

            DWORD ret = ::WaitForSingleObject(This->waiting_event_, DWORD(timeout));
            if (This->wt_finished_) {
                break;
            }

            //LOG(Log::INFO) << "============ Waiting thread resumed: " << timeout;

            if (ret == WAIT_TIMEOUT) {
                ::PostThreadMessageW(This->ui_thread_id_, WM_WAKELOOP, 0, 0);
            }

            if (This->wt_finished_) {
                break;
            }
        }
        return 0;
    }

}
}