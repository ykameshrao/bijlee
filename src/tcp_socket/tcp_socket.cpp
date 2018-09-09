//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//

#include <netdb.h>
#include <unistd.h>

#include <bijlee/tcp_socket.h>
#include <bijlee/util.h>
#include <csignal>
#include <sys/epoll.h>
#include <vector>
#include "bijlee/tcp_socket.h"

namespace {
    volatile sig_atomic_t volatile_listen_fd = -1;

    void close_listener() {
        if (volatile_listen_fd != -1) {
            ::close(volatile_listen_fd);
            volatile_listen_fd = -1;
        }
    }

    void handle_sigint(int) {
        close_listener();
    }
}

bjl::tcp_socket::tcp_socket(bjl::address addr) : tcp_socket(addr, DefaultConnQueueLength) {
}

bjl::tcp_socket::tcp_socket(bjl::address addr, int conn_queue_length):
                                    address_{addr}, conn_queue_length_{conn_queue_length} {
    // validate the address
}

void bjl::tcp_socket::bind() {
    struct addrinfo hints;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;

    struct addrinfo *addrs;
    util::try_sys_call(::getaddrinfo(address_.host_ip(), address_.port(), &hints, &addrs));

    int sockFd = -1;

    for (struct addrinfo *addr = addrs; addr; addr = addr->ai_next) {
        sockFd = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

        if (sockFd < 0)
            continue;

        if (::bind(sockFd, addr->ai_addr, addr->ai_addrlen) < 0) {
            close(sockFd);
            continue;
        }

        util::try_sys_call(::listen(sockFd, conn_queue_length_));
        break;
    }

    util::make_non_blocking(sockFd);
    socket_fd_ = sockFd;
    volatile_listen_fd = sockFd;
}

void bjl::tcp_socket::run() {
    for (;;) {
        std::vector<epoll_event> events;

        int ready_fds = epoller_.poll(events, 128, std::chrono::milliseconds(-1));
        if (ready_fds == -1) {
            if (errno == EINTR && volatile_listen_fd == -1) return;
            throw std::runtime_error("polling interrupted");
        }
        else if (ready_fds > 0) {
            for (const auto& event: events) {
                if (event.events & EPOLLIN) {
                    if (event.data.fd == socket_fd_)
                        handle_new_connection();
                }
            }
        }
    }
}

void bjl::tcp_socket::handle_new_connection() {

}

void bjl::tcp_socket::connect() {

}

void bjl::tcp_socket::start() {
    bind();
    run();
}

void bjl::tcp_socket::startThreaded() {
    acceptor_thread_.reset(new std::thread([=]() {this->start();}));
}

void bjl::tcp_socket::shutdown() {

}
