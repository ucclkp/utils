// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/files/file_utils.h"

#include "utils/platform_utils.h"

#ifdef OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif defined OS_MAC
#include <mach-o/dyld.h>
#include <sys/param.h>
#endif


namespace utl {

    std::filesystem::path getExecFileName(bool dir) {
#ifdef OS_WINDOWS
        WCHAR buffer[MAX_PATH];
        DWORD result = ::GetModuleFileNameW(nullptr, buffer, MAX_PATH);
        if (result == 0) {
            return {};
        }

        DWORD size = MAX_PATH;
        std::unique_ptr<WCHAR[]> heap_buff;
        while (::GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            size *= 2;
            heap_buff.reset(new WCHAR[size]);
            result = ::GetModuleFileNameW(nullptr, heap_buff.get(), size);
            if (result == 0) {
                return {};
            }
        }

        std::u16string file_name;
        if (heap_buff) {
            file_name = reinterpret_cast<char16_t*>(heap_buff.get());
        } else {
            file_name = reinterpret_cast<char16_t*>(buffer);
        }
        std::filesystem::path file_path(file_name);

        if (dir) {
            file_path = file_path.parent_path();
        }

        return file_path;
#elif defined OS_MAC
        char buf[MAXPATHLEN];
        uint32_t size = MAXPATHLEN;
        std::filesystem::path path;
        if (_NSGetExecutablePath(buf, &size) != 0) {
            if (size <= MAXPATHLEN) {
                return {};
            }

            std::unique_ptr<char[]> d_buf(new char[size]);
            if (_NSGetExecutablePath(d_buf.get(), &size) != 0) {
                return {};
            }
            path = std::filesystem::path(realpath(d_buf.get(), nullptr));
        } else {
            path = std::filesystem::path(realpath(buf, nullptr));
        }

        if (dir) {
            path = path.parent_path();
        }
        return path;
#endif
    }

}
