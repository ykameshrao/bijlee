//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//
#ifndef BIJLEE_UTIL_H
#define BIJLEE_UTIL_H

namespace bjl {
    class  util {
    public:
        static int processor_core_count();

        static bool make_non_blocking(int fd);

        static int try_sys_call(int ret);
    };
}

#endif //BIJLEE_UTIL_H
