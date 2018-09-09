//
// Created by Kamesh Rao Yeduvakula on 8/9/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//
#ifndef BIJLEE_EPOLL_H
#define BIJLEE_EPOLL_H

#include <iostream>
#include <sys/epoll.h>
#include <vector>
#include <chrono>

namespace bjl {
    class epoller {
    public:
        epoller();

        epoller(int max);

        void add_fd(int fd, uint32_t watches);

        void add_fd_one_shot(int fd, uint32_t watches);

        void remove_fd(int fd);

        void rearm_fd(int fd, uint32_t watches);

        int poll(std::vector<epoll_event>& events, size_t maxEvents, std::chrono::milliseconds timeout) const;

    private:
        int epoll_fd;
        constexpr static int DefaultMaxPollFileDescriptors = 128;
        constexpr static int MaxEvents = 1024;
    };
}

#endif //BIJLEE_EPOLL_H
