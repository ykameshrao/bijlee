//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//
#ifndef BIJLEE_SOCKET_H
#define BIJLEE_SOCKET_H

#include "net.h"

namespace bjl {
    class tcp_socket {
    public:
        tcp_socket(address addr);

        void bind();

        void connect();

        void serve();

        void shutdown();

    private:
        address address_;
    };
}

#endif //BIJLEE_SOCKET_H
