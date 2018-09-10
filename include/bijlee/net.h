//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//
#ifndef BIJLEE_NET_H
#define BIJLEE_NET_H

#include <iostream>

namespace bjl {
    enum class proto { ipv4, ipv6 };
    class address {
    public:
        address(std::string host_ip, uint16_t port, proto protocl);
        address(std::string host_dns);

        address(const address& other) = default;
        address(address&& other) = default;

        address &operator=(const address& other) = default;
        address &operator=(address&& other) = default;

        const char* host_ip() const;
        const char* port() const;
    private:
        std::string _host_ip;
        std::string _host_dns;
        uint16_t _port;
        proto _protocol;
    };
}

#endif //BIJLEE_NET_H
