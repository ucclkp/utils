// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STL_UTILS_H_
#define UTILS_STL_UTILS_H_

#define STL_VECTOR_ITERATORS(type, vec)     \
    std::vector<type>::iterator begin() { return vec.begin(); }                          \
    std::vector<type>::iterator end() { return vec.end(); }                              \
    std::vector<type>::const_iterator begin() const { return vec.begin(); }              \
    std::vector<type>::const_iterator end() const { return vec.end(); }                  \
    std::vector<type>::reverse_iterator rbegin() { return vec.rbegin(); }                \
    std::vector<type>::reverse_iterator rend() { return vec.rend(); }                    \
    std::vector<type>::const_reverse_iterator rbegin() const { return vec.rbegin(); }    \
    std::vector<type>::const_reverse_iterator rend() const { return vec.rend(); }        \
    std::vector<type>::const_iterator cbegin() const { return vec.cbegin(); }            \
    std::vector<type>::const_iterator cend() const { return vec.cend(); }                \
    std::vector<type>::const_reverse_iterator crbegin() const { return vec.crbegin(); }  \
    std::vector<type>::const_reverse_iterator crend() const { return vec.crend(); }


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
