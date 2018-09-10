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
#include <arpa/inet.h>
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
                                    address_{addr}, conn_queue_length_{conn_queue_length}, io_evt_loop_{util::processor_core_count()} {
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
    epoller_.add_fd(sockFd, EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP);
    socket_fd_ = sockFd;
    volatile_listen_fd = sockFd;
}

void bjl::tcp_socket::run() {
    for (;;) {
        std::cout << "polling...";
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
    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len = sizeof(peer_addr);
    address peer_address { std::string { inet_ntoa(peer_addr.sin_addr) }, ntohs(peer_addr.sin_port) , proto::ipv4 };

    int client_fd = util::try_sys_call(::accept(socket_fd_, (struct sockaddr *)&peer_addr, &peer_addr_len));
    util::make_non_blocking(client_fd);

    std::cout << "received connection from : "  << peer_address.host_ip();
    io_evt_loop_.add_connection(std::move(peer_address), std::move(client_fd));
}

void bjl::tcp_socket::connect() {

}

void bjl::tcp_socket::start() {
   // start_threaded();
   bind();
   run();
}

void bjl::tcp_socket::start_threaded() {
    acceptor_thread_.reset(new std::thread([=]() {
        this->bind();
        this->run();
    }));
    //acceptor_thread_.get()->join();
}

void bjl::tcp_socket::shutdown() {

}
