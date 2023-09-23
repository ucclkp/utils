// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/command_line.h"

#import <Foundation/Foundation.h>


namespace utl {

    // static
    void CommandLine::initialize() {
        NSArray* args = [[NSProcessInfo processInfo] arguments];
        NSInteger str_num = args.count;
        if (str_num <= 0) {
            return;
        }

        char16_t** c_strs = new char16_t*[str_num];
        for (NSInteger i = 0; i < str_num; ++i) {
            NSString* ns_str = args[i];
            char16_t* c_str = new char16_t[ns_str.length + 1];
            for (NSUInteger j = 0; j < ns_str.length; ++j) {
                c_str[j] = [ns_str characterAtIndex:j];
            }
            c_str[ns_str.length] = u'\0';
            c_strs[i] = c_str;
        }

        initialize(int(str_num), c_strs);

        for (NSInteger i = 0; i < str_num; ++i) {
            delete[] c_strs[i];
        }
        delete[] c_strs;
    }

}
