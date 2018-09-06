//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//

#include <netdb.h>
#include <unistd.h>

#include <bijlee/tcp_socket.h>
#include <bijlee/util.h>
#include "bijlee/tcp_socket.h"

bjl::tcp_socket::tcp_socket(bjl::address addr) : address_{addr} {
    // validate the address

}

void bjl::tcp_socket::bind() {
    struct addrinfo hints;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;

    struct addrinfo *addrs;
    util::trySysCall([&]() {
        ::getaddrinfo(address_.host_ip(), address_.port(), &hints, &addrs);
    });

    int sockFd = -1;

    for (struct addrinfo *addr = addrs; addr; addr = addr->ai_next) {
        sockFd = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

        if (sockFd < 0)
            continue;

        if (::bind(sockFd, addr->ai_addr, addr->ai_addrlen) < 0) {
            close(sockFd);
            continue;
        }

        TRY(::listen(sockFd, backlog_));
        break;
    }
}

void bjl::tcp_socket::connect() {

}

void bjl::tcp_socket::serve() {

}

void bjl::tcp_socket::shutdown() {

}
