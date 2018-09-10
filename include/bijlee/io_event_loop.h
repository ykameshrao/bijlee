//
// Created by Kamesh Rao Yeduvakula on 9/9/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//
#ifndef BIJLEE_IO_EVENT_LOOP_H
#define BIJLEE_IO_EVENT_LOOP_H

#include <unordered_map>
#include <sys/socket.h>
#include "epoller.h"
#include "net.h"

namespace bjl {
    class io_event_loop {
    public:
        io_event_loop(int workers);

        void add_connection(sockaddr&& addr, int connection_fd);

        void run();

    private:
        int workers_;
        //epoller io_epoller_;
        std::unordered_map<int, sockaddr> connections;
    };
}

#endif //BIJLEE_IO_EVENT_LOOP_H
