// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MULTI_CALLBACKS_HPP_
#define UTILS_MULTI_CALLBACKS_HPP_

#include "utils/log.h"


namespace utl {

    template<typename Mu>
    bool addCallbackTo(Mu& multi, typename Mu::value_type& cb) {
        if (!cb) {
            DLOG(Log::WARNING) << "Invalid callback!";
            return false;
        }

        auto it = std::find(multi.begin(), multi.end(), cb);
        if (it != multi.end()) {
            DLOG(Log::WARNING) << "A callback can only be added once!";
            return false;
        }
        multi.push_back(cb);
        return true;
    }

    template<typename Mu>
    bool removeCallbackFrom(Mu& multi, typename Mu::value_type& cb) {
        if (!cb) {
            DLOG(Log::WARNING) << "Invalid callback!";
            return false;
        }

        auto it = std::find(multi.begin(), multi.end(), cb);
        if (it != multi.end()) {
            multi.erase(it);
        } else {
            DLOG(Log::WARNING) << "Cannot find specific callback!";
            return false;
        }
        return true;
    }

}

#endif  // UTILS_MULTI_CALLBACKS_HPP_