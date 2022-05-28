// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#ifndef UTILS_STREAM_UTILS_H_
#define UTILS_STREAM_UTILS_H_

#include <istream>

#include "utils/endian.hpp"
#include "utils/endian_ml.hpp"

#define RET_FALSE  \
    { return false; }

#define READ_STREAM(var, size)  \
    s.read(reinterpret_cast<char*>(&var), size);  \
    if (!s.good()) RET_FALSE

#define READ_STREAM_BE(var, size)  \
    READ_STREAM(var, size)  \
    static_assert(size == sizeof(var), "The size of 'var' must be equal to 'size'");  \
    var = utl::fromToBE(var);

#define READ_STREAM_LE(var, size)  \
    READ_STREAM(var, size)  \
    static_assert(size == sizeof(var), "The size of 'var' must be equal to 'size'");  \
    var = utl::fromToLE(var);

#define READ_STREAM_MLBE(var, size)  \
    READ_STREAM(var, size)  \
    static_assert(size < sizeof(var), "The size of 'var' must be greater than 'size'");  \
    var = utl::fromMLBE<size>(var);

#define READ_STREAM_MLLE(var, size)  \
    READ_STREAM(var, size)  \
    static_assert(size < sizeof(var), "The size of 'var' must be greater than 'size'");  \
    var = utl::fromMLLE<size>(var);

#define PEEK_STREAM(buf)  \
    buf = s.peek();       \
    if (!s) RET_FALSE

#define GET_STREAM(buf)  \
    buf = s.get();       \
    if (!s) RET_FALSE

#define SKIP_BYTES(size)  \
    s.seekg(size, std::ios::cur);  \
    if (!s.good()) RET_FALSE

#define NEXT_EQUAL(str, size) {  \
    char l_buf[size];  \
    READ_STREAM(l_buf, size);  \
    if (std::strncmp(l_buf, str, size) != 0) RET_FALSE;  \
}


#define WRITE_STREAM(var, size)  \
    s.write(reinterpret_cast<const char*>(&var), size);  \
    if (!s.good()) RET_FALSE

#define WRITE_STREAM_STR(str)  \
    WRITE_STREAM(str.data()[0], str.length())

#define WRITE_STREAM_BE(var, size) {  \
    static_assert(size == sizeof(var), "The size of 'var' must be equal to 'size'");  \
    auto tmp = utl::fromToBE(var);  \
    WRITE_STREAM(tmp, size)  \
}

#define WRITE_STREAM_MLBE(var, size) {  \
    static_assert(size < sizeof(var), "The size of 'var' must be greater than 'size'");  \
    auto tmp = utl::toMLBE<size>(var);  \
    WRITE_STREAM(tmp, size)  \
}


#define PUT_STREAM(v)  \
    s.put(char(v));  \
    if (!s.good()) RET_FALSE

#define SEEKP_STREAM(pos)  \
    if(!s.seekp(pos).good()) RET_FALSE

#define SEEKG_STREAM(pos)  \
    if(!s.seekg(pos).good()) RET_FALSE


namespace utl {

    /**
     * 查看从当前位置开始的流数据是否等于 str。
     * 如果流中的数据足够，且等于 str，返回 1；
     * 如果中途读取到流末尾，或数据不等，则回退流指针，并返回 0；
     * 如果读取流出错，返回 -1。
     */
    inline int startWith(std::istream& s, const std::string_view& str) {
        auto saved = s.tellg();

        char ch;
        for (size_t i = 0; i < str.size(); ++i) {
            s.read(&ch, 1);
            if (!s) {
                if (s.bad() || !s.eof()) return -1;
            }
            if (s.eof() || str[i] != ch) {
                s.clear();
                s.seekg(saved);
                return 0;
            }
        }
        return 1;
    }

}

#endif  // UTILS_STREAM_UTILS_H_