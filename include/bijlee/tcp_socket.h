//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//
#ifndef BIJLEE_SOCKET_H
#define BIJLEE_SOCKET_H

#include <thread>
#include "net.h"
#include "epoller.h"

namespace bjl {
    class tcp_socket {
    public:
        tcp_socket(address addr);

        tcp_socket(address addr, int conn_queue_length);

        void start();

        void startThreaded();

        void shutdown();

        void connect();

        void handle_new_connection();

    private:
        address address_;
        int socket_fd_;
        int conn_queue_length_;
        std::unique_ptr<std::thread> acceptor_thread_;
        epoller epoller_;
        constexpr static int DefaultConnQueueLength = 128;

        void bind();

        void run();
    };
}

#endif //BIJLEE_SOCKET_H
