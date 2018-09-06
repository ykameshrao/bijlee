//
// Created by Kamesh Rao Yeduvakula on 27/8/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//

#include <fstream>
#include <iterator>
#include <algorithm>

#include <unistd.h>
#include <fcntl.h>
#include <bijlee/util.h>
#include <sstream>
#include <cstring>


#include "bijlee/util.h"

using namespace std;
using namespace bjl;

int util::processor_core_count() {
    std::ifstream cpuinfo("/proc/cpuinfo");
    if (cpuinfo) {
        return std::count(std::istream_iterator<std::string>(cpuinfo),
                          std::istream_iterator<std::string>(),
                          std::string("processor"));
    }

    return sysconf(_SC_NPROCESSORS_ONLN);
}

bool util::make_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return false;

    flags |= O_NONBLOCK;
    int ret = fcntl(fd, F_SETFL, flags);
    if (ret == -1) return false;

    return true;
}

template<typename SysCall>
void util::trySysCall(SysCall sc) {
    auto ret = sc();
    if (ret < 0) {
        std::ostringstream oss;
        oss << "syscall failed: ";
        if (errno == 0) {
            oss << gai_strerror(ret);
        } else {
            oss << strerror(errno);
        }

        throw std::runtime_error(oss.str());
    }
}

template<typename SysCall, typename RetVal>
RetVal util::trySysCall(SysCall sc) {
    RetVal ret = sc();
    if (ret < 0) {
        std::ostringstream oss;
        oss << "syscall failed: ";
        if (errno == 0) {
            oss << gai_strerror(ret);
        } else {
            oss << strerror(errno);
        }

        throw std::runtime_error(oss.str());
    }

    return ret;
}