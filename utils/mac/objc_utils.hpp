// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_MAC_OBJC_UTILS_HPP_
#define UTILS_MAC_OBJC_UTILS_HPP_

#include <objc/objc-runtime.h>

#ifdef __OBJC__
#define UTL_OBJC_CLASS(class_name) @class class_name
#else
#define UTL_OBJC_CLASS(class_name) typedef struct objc_object class_name;
#endif

#endif  // UTILS_MAC_OBJC_UTILS_HPP_
