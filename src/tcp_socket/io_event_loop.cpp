//
// Created by Kamesh Rao Yeduvakula on 9/9/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//

#include <bijlee/io_event_loop.h>

bjl::io_event_loop::io_event_loop(int workers) : workers_ { workers } {
}

void bjl::io_event_loop::add_connection(address&& addr, int&& connection_fd) {
    connections.insert(std::make_pair(connection_fd, addr));
}

void bjl::io_event_loop::run() {

}

