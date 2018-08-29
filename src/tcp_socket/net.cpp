//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//

#include "bijlee/net.h"

using namespace bjl;

address::address(std::string& host_ip, uint16_t port, bjl::proto& protocl) :
    _host_ip { std::move(host_ip) }, _port { port }, _protocol { protocl } {
    if(_port < 1024)
        throw std::runtime_error("used port");


}

address::address(std::string& host_dns) {
    throw std::runtime_error("not yet implemented");
}
