//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//

#include <bijlee/tcp_socket.h>
#include <netdb.h>

#include "bijlee/tcp_socket.h"

bjl::tcp_socket::tcp_socket(bjl::address addr) : address_ { addr } {
    // validate the address

}

void bjl::tcp_socket::bind() {
    struct addrinfo hints;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;

    static constexpr size_t MaxPortLenth = sizeof("65535");

}

void bjl::tcp_socket::connect() {

}

void bjl::tcp_socket::serve() {

}

void bjl::tcp_socket::shutdown() {

}
