//
// Created by Kamesh Rao Yeduvakula on 8/9/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//
#include <bijlee/epoller.h>
#include <bijlee/util.h>
#include <sys/epoll.h>

bjl::epoller::epoller() {
    epoll_fd = util::try_sys_call(epoll_create1(0));
}

bjl::epoller::epoller(int max) {
    epoll_fd = util::try_sys_call(epoll_create(max));
}

void bjl::epoller::add_fd(int fd, uint32_t watches) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = watches;

    util::try_sys_call(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev));
}

void bjl::epoller::add_fd_one_shot(int fd, uint32_t watches) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = watches;
    ev.events |= EPOLLONESHOT;

    util::try_sys_call(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev));
}

void bjl::epoller::remove_fd(int fd) {
    struct epoll_event ev;
    util::try_sys_call(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev));
}

void bjl::epoller::rearm_fd(int fd, uint32_t watches) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = watches;

    util::try_sys_call(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev));
}

int bjl::epoller::poll(std::vector<epoll_event> &events, size_t maxEvents, std::chrono::milliseconds timeout) const {
    struct epoll_event evs[MaxEvents];

    int ready_fds = -1;
    do {
        ready_fds = epoll_wait(epoll_fd, evs, maxEvents, timeout.count());
    } while (ready_fds < 0 && errno == EINTR);

    if (ready_fds > 0) {
        for (int i = 0; i < ready_fds; ++i) {
            const struct epoll_event *ev = evs + i;

            epoll_event e;
            e.data = ev->data;
            e.events = ev->events;
            events.push_back(e);
        }
    }

    return ready_fds;
}