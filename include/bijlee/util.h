//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//
#ifndef BIJLEE_UTIL_H
#define BIJLEE_UTIL_H

namespace bjl {
    class  util {
    public:
        int processor_core_count();

        bool make_non_blocking(int fd);
    };
}

#endif //BIJLEE_UTIL_H
