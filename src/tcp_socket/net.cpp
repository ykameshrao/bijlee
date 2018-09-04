//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//

#include <bijlee/net.h>
#include <arpa/inet.h>
#include <cstring>

#include "bijlee/net.h"

using namespace bjl;

address::address(std::string& host_ip, uint16_t port, bjl::proto& protocl) :
    _host_ip { std::move(host_ip) }, _port { port }, _protocol { protocl } {
    if(_port < 1024)
        throw std::runtime_error("used port");

    // validate host name format as per protocol
    struct sockaddr_in sa;
    int result;
    switch(_protocol) {
        case proto::ipv4:
            result = inet_pton(AF_INET, host_ip.c_str(), &(sa.sin_addr));
            break;
        case proto::ipv6:
            result = inet_pton(AF_INET6, host_ip.c_str(), &(sa.sin_addr));
            break;
    }

    if(result == 0)
        throw std::runtime_error("invalid ip address");

    if(result < 0)
        throw std::runtime_error(strerror(errno));

}

address::address(std::string& host_dns) {
    throw std::runtime_error("not yet implemented");
}

const char *address::host_ip() const {
    return nullptr;
}

const char *address::port() const {
    return nullptr;
}
