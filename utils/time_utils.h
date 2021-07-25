// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_TIME_UTILS_H_
#define UTILS_TIME_UTILS_H_

#include <chrono>
#include <cstdint>


namespace utl {

    class TimeUtils {
    public:
        using ns = std::chrono::nanoseconds;
        using nsp = const ns&;

        static ns upTime();

        static uint64_t upTimeMillis();
        static uint64_t upTimeMicros();
        static uint64_t upTimeNanos();
    };

}

#endif  // UTILS_TIME_UTILS_H_