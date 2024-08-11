// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_PLATFORM_ENTRANCES_H_
#define UTILS_PLATFORM_ENTRANCES_H_

#include "platform_utils.h"

#ifdef OS_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#define GUI_MAIN()  \
    int APIENTRY wWinMain(  \
        HINSTANCE hInstance, HINSTANCE hPrevInstance,  \
        LPWSTR lpCmdLine, int nShowCmd)
#else
#define GUI_MAIN()  \
    int main()
#endif

#endif  // UTILS_PLATFORM_ENTRANCES_H_