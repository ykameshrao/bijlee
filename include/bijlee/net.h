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
        address(std::string host_ip, std::string port, proto protocl);
        address(std::string host_dns);

        address(const address& other) = default;
        address(address&& other) = default;

        address &operator=(const address& other) = default;
        address &operator=(address&& other) = default;

        const char* host_ip() const;
        const char* port() const;
    private:
        std::string host_ip_;
        std::string host_dns_;
        std::string port_;
        proto protocol_;
    };
}

#endif //BIJLEE_NET_H
