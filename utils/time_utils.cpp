// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "time_utils.h"

#include <chrono>


namespace utl {

    // static
    TimeUtils::ns TimeUtils::upTime() {
        return std::chrono::high_resolution_clock::now().time_since_epoch();
    }

    // static
    uint64_t TimeUtils::upTimeMillis() {
        using namespace std::chrono;
        return duration_cast<milliseconds>(upTime()).count();
    }

    // static
    uint64_t TimeUtils::upTimeMicros() {
        using namespace std::chrono;
        return duration_cast<microseconds>(upTime()).count();
    }

    // static
    uint64_t TimeUtils::upTimeNanos() {
        return upTime().count();
    }

}
