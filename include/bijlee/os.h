//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//
#ifndef BIJLEE_LINUX_OS_H
#define BIJLEE_LINUX_OS_H

namespace Bijlee {
    class OS {
    public:
        virtual int processor_core_count() = 0;

        virtual bool make_non_blocking(int fd) = 0;
    };

    class Linux : public OS {
    public:
        int processor_core_count() override;

        bool make_non_blocking(int fd) override;
    };
}

#endif //BIJLEE_LINUX_OS_H
