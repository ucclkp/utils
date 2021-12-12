// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_SCOPE_UTILS_H_
#define UTILS_SCOPE_UTILS_H_

#include <utility>

#include "utils/define_utils.hpp"


namespace utl::internal {
    template <typename F>
    class ObjectXXX {
    public:
        explicit ObjectXXX(F&& f)
            : cancelled_(false),
              func_(std::forward<F>(f)) {}

        ~ObjectXXX() {
            invoke();
        }

        void invoke() {
            if (!cancelled_) {
                func_();
            }
        }

        void validate() {
            cancelled_ = false;
        }

        void cancel() {
            cancelled_ = true;
        }

        bool isCancelled() const {
            return cancelled_;
        }

    private:
        bool cancelled_;
        F func_;
    };

    class ObjectStub {
    public:
        ObjectStub() = default;
        template <typename F>
        ObjectXXX<F> operator+(F&& f) {
            return ObjectXXX<F>(std::forward<F>(f));
        }
    };
}

#define SCOPED_CONVERTER utl::internal::ObjectStub() + [&] ()
#define ESC_FROM_SCOPE auto ANONYMOUS_VAR(_anonymous_var_) = SCOPED_CONVERTER

#endif  // UTILS_SCOPE_UTILS_H_