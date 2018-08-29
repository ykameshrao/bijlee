//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//
#ifndef BIJLEE_OS_H
#define BIJLEE_OS_H

namespace bjl {
    class os {
    public:
        virtual int processor_core_count() = 0;

        virtual bool make_non_blocking(int fd) = 0;
    };

    class linux : public os {
    public:
        int processor_core_count() override;

        bool make_non_blocking(int fd) override;
    };
}

#endif //BIJLEE_OS_H
