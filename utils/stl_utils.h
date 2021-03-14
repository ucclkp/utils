// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STL_UTILS_H_
#define UTILS_STL_UTILS_H_


namespace utl {

    template <class T>
    void STLDeleteElements(T* container) {
        for (auto it = container->begin(); it != container->end(); ++it) {
            delete *it;
        }
        container->clear();
    }

}

#endif  // UTILS_STL_UTILS_H_
