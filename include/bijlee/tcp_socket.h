//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//
#ifndef BIJLEE_SOCKET_H
#define BIJLEE_SOCKET_H

namespace bjl {
    class tcp_socket {
    public:
        void bind_and_listen();

        void connect();

        void serve();

        void shutdown();
    private:

    };
}

#endif //BIJLEE_SOCKET_H
