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
#include <cstring>
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
}

bjl::tcp_socket::~tcp_socket() {
    if (acceptor_thread_) acceptor_thread_->join();
    if(io_thread_) io_thread_->join();

    if(socket_fd_) {
        epoller_.remove_fd(socket_fd_);
        ::close(socket_fd_);
    }
}

void bjl::tcp_socket::bind() {
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
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
    epoller_.add_fd(sockFd, EPOLLIN);
    socket_fd_ = sockFd;
    volatile_listen_fd = sockFd;
}

void bjl::tcp_socket::run() {
    for (;;) {
        std::vector<epoll_event> events;

        int ready_fds = epoller_.poll(events, 128, std::chrono::milliseconds(-1));
        if (ready_fds == -1) {
            if (errno == EINTR && volatile_listen_fd == -1) return;
                throw std::runtime_error("Polling interrupted");
        } else if (ready_fds > 0) {
            for (const auto& event: events) {
                if (event.events & EPOLLERR ||
                    event.events & EPOLLHUP ||
                    !(event.events & EPOLLIN)) {
                    std::cerr << "[E] thread_name: " << std::this_thread::get_id() << "; epoll event error" << std::endl;
                    ::close(event.data.fd);
                } else if (event.events & EPOLLIN) {
                    if (event.data.fd == socket_fd_)
                        handle_new_connection();
                }
            }
        }
    }
}

void bjl::tcp_socket::handle_new_connection() {
    struct sockaddr in_addr;
    socklen_t in_len = sizeof(in_addr);

    int client_fd = util::try_sys_call(::accept(socket_fd_, &in_addr, &in_len));
    util::make_non_blocking(client_fd);

    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    if (::getnameinfo(&in_addr, in_len,
                    host, NI_MAXHOST,
                    service, NI_MAXSERV,
                    NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
        std::cout << "[I] thread_name: " << std::this_thread::get_id() << "; Accepted connection on descriptor " << client_fd << " (host=" << host << ", port=" << service << ")"  << std::endl;
    }

    io_evt_loop_.add_connection(std::move(in_addr), client_fd);
}

void bjl::tcp_socket::connect() {

}

void bjl::tcp_socket::start() {
    std::cout << "[I] Starting main thread; thread_name: " << std::this_thread::get_id() << std::endl;
    start_threaded();
}

void bjl::tcp_socket::start_threaded() {
    acceptor_thread_.reset(new std::thread([this]() {
        std::cout << "[I] Socket connection acceptor thread; thread_name: " << std::this_thread::get_id() << std::endl;
        this->bind();
        this->run();
    }));

    run_io_epoller();
}

void bjl::tcp_socket::shutdown() {

}

void bjl::tcp_socket::run_io_epoller() {
    io_evt_loop_.run();
}
