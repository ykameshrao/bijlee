//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//

#include <bijlee/net.h>
#include <arpa/inet.h>
#include <cstring>

#include "bijlee/net.h"

using namespace bjl;

address::address(std::string host_ip, std::string port, bjl::proto protocl) :
    host_ip_ { std::move(host_ip) }, port_ { std::move(port) }, protocol_ { protocl } {
    if(std::stoi(port_) < 1024)
        throw std::runtime_error("used port");

    // validate host name format as per protocol
    struct sockaddr_in sa;
    int result;
    switch(protocol_) {
        case proto::ipv4:
            result = inet_pton(AF_INET, host_ip_.c_str(), &(sa.sin_addr));
            break;
        case proto::ipv6:
            result = inet_pton(AF_INET6, host_ip_.c_str(), &(sa.sin_addr));
            break;
    }

    if(result == 0)
        throw std::runtime_error("invalid ip address");

    if(result < 0)
        throw std::runtime_error(strerror(errno));

}

address::address(std::string host_dns) {
    throw std::runtime_error("not yet implemented");
}

const char *address::host_ip() const {
    return host_ip_.c_str();
}

const char *address::port() const {
    return port_.c_str();
}
