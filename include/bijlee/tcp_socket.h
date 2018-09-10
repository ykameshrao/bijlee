//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//
#ifndef BIJLEE_SOCKET_H
#define BIJLEE_SOCKET_H

#include <thread>
#include "net.h"
#include "epoller.h"
#include "io_event_loop.h"

namespace bjl {
    class tcp_socket {
    public:
        tcp_socket(address addr);

        tcp_socket(address addr, int conn_queue_length);

        ~tcp_socket();

        void start();

        void shutdown();

        void connect();

        void handle_new_connection();

    private:
        address address_;
        int socket_fd_;
        int conn_queue_length_;
        std::unique_ptr<std::thread> acceptor_thread_;
        std::unique_ptr<std::thread> io_thread_;
        epoller epoller_;
        io_event_loop io_evt_loop_;

        constexpr static int DefaultConnQueueLength = 128;

        void bind();

        void run();

        void run_io_epoller();

        void start_threaded();
    };
}

#endif //BIJLEE_SOCKET_H
