//
// Created by Kamesh Rao Yeduvakula on 9/9/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//

#include <bijlee/io_event_loop.h>
#include <unistd.h>
#include <memory>
#include <sstream>

bjl::io_event_loop::io_event_loop(int workers) : workers_ { workers } {
//    for (int i=0; i < workers_; i++) {
//        auto thread_ = new std::thread([this](int fd) {
//            if (!read_data(fd)) {
//                send_data(fd);
//            }
//        });
//        worker_thread_.push_back(std::move(thread_));
//    }
}

bjl::io_event_loop::~io_event_loop() {
//    for(const auto& t : worker_thread_) {
//        t.join();
//    }
}

void bjl::io_event_loop::add_connection(sockaddr&& addr, int connection_fd) {
    connections_.insert(std::make_pair(connection_fd, addr));
    io_epoller_.add_fd(connection_fd, EPOLLIN | EPOLLHUP | EPOLLOUT);
}

void bjl::io_event_loop::run() {
    for (;;) {
        std::vector<epoll_event> events;
        int ready_fds = io_epoller_.poll(events, 1024, std::chrono::milliseconds(-1));

        if (ready_fds > 0) {
            for (const auto& event: events) {
                close_on_error(event);

                if (event.events & EPOLLIN) {
                    if(!read_data(event.data.fd)) {
                        io_epoller_.rearm_fd(event.data.fd, EPOLLOUT | EPOLLET);
                    }
                } else if (event.events & EPOLLOUT) {
                    send_data(event.data.fd);
                }

            }
//            for (int i=0; i < events.size(); i += workers_) {
//                for(int p = i; p < workers_; p++) {
//                    close_on_error(events[p]);
//
//                    if (events[p].events & EPOLLIN) {
//
//
//                    }
//                }
//            }
        }
    }
}

void bjl::io_event_loop::close_on_error(epoll_event e) {
    if (e.events & EPOLLERR || e.events & EPOLLHUP || ( !(e.events & EPOLLIN) && !(e.events & EPOLLOUT) ) ) {
        std::cerr << "[E] epoll event error\n";
        ::close(e.data.fd);
    }
}

bool bjl::io_event_loop::read_data(int fd) {
    char buf[512];
    auto count = ::read(fd, buf, 512);
    if (count == -1) {
        if (errno == EAGAIN) { // read all data
            return false;
        }
    } else if (count == 0) { // EOF - remote closed connection
        std::cout << "[I] Close " << fd << "\n";
        io_epoller_.remove_fd(fd);
        ::close(fd);
        return false;
    }

    std::cout << fd << " says: " <<  buf;
    return true;
}

bool bjl::io_event_loop::send_data(int fd) {
    std::string response = "{status: \"recieved\"}";

    std::stringstream wsss;
    wsss << "HTTP/1.1 200 OK\r\n"
         << "Connection: keep-alive\r\n"
         << "Content-Type: application/json\r\n"
         << "Content-Length: " << response.length() << "\r\n"
         << "\r\n"
         << response
         << "\r\n";

    auto count = ::write(fd, wsss.str().c_str(), wsss.str().size());
    if (count == -1) {
        std::cout << "[E] Write failed" << "\n";
    } else if (count == wsss.str().size()) {
        std::cout << "[I] Done writing...closing connection!" << "\n";
        io_epoller_.remove_fd(fd);
        ::close(fd);
    }
}


