// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_DEFINE_UTILS_H_
#define UTILS_DEFINE_UTILS_H_

#define CONCAT_STR(left, right) left##right
#define ANONYMOUS_VAR(prefix) CONCAT_STR(prefix, __LINE__)

#define TO_WIDE_STRING(x) L##x
#define _TO_WIDE_STRING(x) TO_WIDE_STRING(x)
#define __WFILE__ _TO_WIDE_STRING(__FILE__)

#endif  // UTILS_DEFINE_UTILS_H_