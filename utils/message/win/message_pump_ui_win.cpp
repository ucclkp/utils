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
#define WM_PUSHLOOP  (WM_APP + 2)


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
         * 在消息发送到窗口过程前收到通知。
         * 用于监听应用程序切换到前台/后台的消息。
         */
        cwp_hook_ = ::SetWindowsHookExW(WH_CALLWNDPROC, ModalLoopCWPHookProc, nullptr, ui_thread_id_);
        if (!cwp_hook_) {
            LOG(Log::ERR) << "Cannot create hook: " << ::GetLastError();
        }

        /**
         * 在调用 GetMessage / PeekMessage 耗尽系统队列中的消息，且程序位于前台时收到通知。
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
            wt_finished_.store(true, std::memory_order_relaxed);
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
        if (cwp_hook_) {
            ::UnhookWindowsHookEx(cwp_hook_);
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
        if (isInAnyModalLoop()) {
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
        is_in_sml_.store(in_sml, std::memory_order_relaxed);
    }

    void MessagePumpUIWin::setInMoveModalLoop(bool in_mml) {
        is_in_mml_.store(in_mml, std::memory_order_relaxed);
    }

    void MessagePumpUIWin::setInDialogModalLoop(bool in_dml) {
        is_in_dml_.store(in_dml, std::memory_order_relaxed);
    }

    bool MessagePumpUIWin::isInSizeModalLoop() const {
        return is_in_sml_.load(std::memory_order_relaxed);
    }

    bool MessagePumpUIWin::isInMoveModalLoop() const {
        return is_in_mml_.load(std::memory_order_relaxed);
    }

    bool MessagePumpUIWin::isInDialogModalLoop() const {
        return is_in_dml_.load(std::memory_order_relaxed);
    }

    bool MessagePumpUIWin::isInAnyModalLoop() const {
        return is_in_sml_.load(std::memory_order_relaxed) ||
            is_in_mml_.load(std::memory_order_relaxed) ||
            is_in_dml_.load(std::memory_order_relaxed);
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

    bool MessagePumpUIWin::needPolling() const {
        return (!is_app_active_.load(std::memory_order_relaxed) &&
                is_in_dml_.load(std::memory_order_relaxed));
    }

    // static
    LRESULT CALLBACK MessagePumpUIWin::ModalLoopMsgHookProc(int code, WPARAM wParam, LPARAM lParam) {
        /**
         * 在调整窗口位置或大小时，嵌套消息循环每获得特定的消息，就跑一下我们自己的消息队列。
         * 仅对特定的消息做处理，以防止卡顿。
         */
        auto mp_ptr = getCurrent();
        auto This = static_cast<MessagePumpUIWin*>(mp_ptr.get());
        if (code == HC_ACTION &&
            UINT(wParam) == PM_REMOVE)
        {
            bool should_process = false;
            MSG* msg = reinterpret_cast<MSG*>(lParam);
            switch (msg->message) {
            case WM_PUSHLOOP:
                should_process |= This->isInAnyModalLoop();
                break;

            default:
                break;
            }

            if (should_process) {
                int64_t delay_ns;
                This->cosume();
                This->cosumeDelayed(&delay_ns);
            }
        }

        return ::CallNextHookEx(This->msg_hook_, code, wParam, lParam);
    }

    // static
    LRESULT CALLBACK MessagePumpUIWin::ModalLoopCWPHookProc(int code, WPARAM wParam, LPARAM lParam) {
        /**
         * 当程序位于前台（即属于该程序的某个窗口位于前台）时，可依靠 WH_FOREGROUNDIDLE 获得
         * 处理我们自己的消息队列的机会。但当程序切换到后台时，WH_FOREGROUNDIDLE 不再起作用。
         * 目前没有比较好的办法来解决这个问题，只能借用 waiting_thread_ 轮询处理了。
         */
        auto mp_ptr = getCurrent();
        auto This = static_cast<MessagePumpUIWin*>(mp_ptr.get());
        if (code == HC_ACTION) {
            auto msg = reinterpret_cast<CWPSTRUCT*>(lParam);
            switch (msg->message) {
            case WM_ACTIVATEAPP:
                if (msg->wParam == TRUE) {
                    This->is_app_active_.store(true, std::memory_order_relaxed);
                } else {
                    This->is_app_active_.store(false, std::memory_order_relaxed);
                    ::SetEvent(This->waiting_event_);
                }
                break;

            case WM_MOVE:
                // 移动窗口时维持我们的消息队列
                if (msg->hwnd && This->is_in_mml_.load(std::memory_order_relaxed)) {
                    ::PostThreadMessageW(This->ui_thread_id_, WM_PUSHLOOP, 0, 0);
                }
                break;

            case WM_SIZE:
                // 调整窗口大小时维持我们的消息队列
                if (msg->hwnd && This->is_in_sml_.load(std::memory_order_relaxed)) {
                    ::PostThreadMessageW(This->ui_thread_id_, WM_PUSHLOOP, 0, 0);
                }
                break;

            default:
                break;
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
        auto mp_ptr = getCurrent();
        auto This = static_cast<MessagePumpUIWin*>(mp_ptr.get());
        if (code == HC_ACTION) {
            if (This->isInAnyModalLoop()) {
                /**
                 * 不要在这里直接处理队列消息。
                 * WH_FOREGROUNDIDLE 对于回调中的函数调用有一些限制，参见微软文档：
                 * https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644980
                 */
                auto delayed_time = This->getDelayedTime();
                if (This->hasMessages(MessageQueue::ML_NORMAL) || delayed_time == 0) {
                    ::PostThreadMessageW(This->ui_thread_id_, WM_PUSHLOOP, 0, 0);
                } else {
                    This->waiting_ns_.store(delayed_time, std::memory_order_relaxed);
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
            int64_t timeout;
            if (This->needPolling()) {
                timeout = 1;
            } else {
                timeout = This->waiting_ns_.exchange(-1, std::memory_order_relaxed);
            }

            if (timeout == -1) {
                timeout = INFINITE;
            }

            DWORD ret = ::WaitForSingleObject(This->waiting_event_, DWORD(timeout));
            if (This->wt_finished_.load(std::memory_order_relaxed)) {
                break;
            }

            if (ret == WAIT_TIMEOUT) {
                ::PostThreadMessageW(This->ui_thread_id_, WM_PUSHLOOP, 0, 0);
            }

            if (This->wt_finished_.load(std::memory_order_relaxed)) {
                break;
            }
        }
        return 0;
    }

}
}