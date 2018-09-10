//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//
#include <bijlee/net.h>
#include <bijlee/tcp_socket.h>

using namespace bjl;

int main() {
    address server_address { "0.0.0.0", "1983", proto::ipv4 };
    tcp_socket server { server_address };

    server.start();

    return 0;
}