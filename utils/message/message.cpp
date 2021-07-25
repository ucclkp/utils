// Copyright (c) 2016 ucclkp <ucclkp@gmail.com>.
// This file is part of utils project.
//
// This program is licensed under GPLv3 license that can be
// found in the LICENSE file.

#include "utils/message/message.h"

#include "utils/log.h"


namespace utl {

    Message::Message()
        : id(-1),
          time_ns(0),
          target(nullptr),
          callback(nullptr),
          data(nullptr),
          is_barrier(false) {
    }

    Message::~Message() {
    }

}