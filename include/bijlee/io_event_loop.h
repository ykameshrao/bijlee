//
// Created by Kamesh Rao Yeduvakula on 9/9/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//
#ifndef BIJLEE_IO_EVENT_LOOP_H
#define BIJLEE_IO_EVENT_LOOP_H

#include <unordered_map>
#include <sys/socket.h>
#include <thread>
#include "epoller.h"
#include "net.h"

namespace bjl {
    class io_event_loop {
    public:
        io_event_loop(int workers);
        ~io_event_loop();

        void add_connection(sockaddr&& addr, int connection_fd);

        bool read_data(int fd);

        bool send_data(int fd);

        void close_on_error(epoll_event e);

        void run();

    private:
        int workers_;
        int current_epoller_;
        std::vector<std::unique_ptr<std::thread>> worker_threads_;
        std::unordered_map<int, sockaddr> connections_;
        std::unordered_map<int, std::thread::id> worker_threads_idx_;
        std::unordered_map<std::thread::id, std::unique_ptr<epoller>> epollers_;

        void check_for_errors(const epoll_event &event);
    };
}

#endif //BIJLEE_IO_EVENT_LOOP_H
