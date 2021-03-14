// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_FILES_FILE_UTILS_H_
#define UTILS_FILES_FILE_UTILS_H_

#include <filesystem>


namespace utl {

    std::filesystem::path getExecFileName(bool dir);

}

#endif  // UTILS_FILES_FILE_UTILS_H_
