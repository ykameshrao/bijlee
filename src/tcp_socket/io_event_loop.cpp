//
// Created by Kamesh Rao Yeduvakula on 9/9/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//

#include <bijlee/io_event_loop.h>
#include <unistd.h>
#include <memory>
#include <sstream>

bjl::io_event_loop::io_event_loop(int workers) : workers_ { workers } {
    workers_ = std::max(workers, 2);
    current_epoller_ = 0;
}

bjl::io_event_loop::~io_event_loop() {
    for (auto& t : worker_threads_) {
        t->join();
    }
}

void bjl::io_event_loop::add_connection(sockaddr&& addr, int connection_fd) {
    connections_.insert(std::make_pair(connection_fd, addr));
    epollers_[worker_threads_idx_[current_epoller_]]->add_fd(connection_fd, EPOLLIN | EPOLLHUP | EPOLLOUT);
    current_epoller_++;
    if ( current_epoller_ >= workers_ )
        current_epoller_ = 0;
}

void bjl::io_event_loop::run() {
    std::cout << "[I] Starting socket connection io event loop " << std::this_thread::get_id() << std::endl;

    for ( int i = 0; i < workers_ ; i++ ) {
        worker_threads_.push_back(std::make_unique<std::thread>([this, i]() {
            std::cout << "[I] Input/Output event loop " << std::this_thread::get_id() << std::endl;
            worker_threads_idx_.insert(std::make_pair(i, std::this_thread::get_id()));
            epollers_.insert(std::make_pair(std::this_thread::get_id(), std::make_unique<epoller>()));

            for (;;) {
                std::vector<epoll_event> events;
                int ready_fds = epollers_[std::this_thread::get_id()]->poll(events, 1024, std::chrono::milliseconds(-1));

                if (ready_fds > 0) {
                    for (const auto &event: events) {
                        close_on_error(event);

                        if (event.events & EPOLLIN) {
                            if(!read_data(event.data.fd)) {
                                epollers_[std::this_thread::get_id()]->rearm_fd(event.data.fd, EPOLLOUT | EPOLLET);
                            }
                        } else if (event.events & EPOLLOUT) {
                            send_data(event.data.fd);
                        }
                    }
                }
            }
        }));
    }
}

void bjl::io_event_loop::close_on_error(epoll_event e) {
    if (e.events & EPOLLERR || e.events & EPOLLHUP || ( !(e.events & EPOLLIN) && !(e.events & EPOLLOUT) ) ) {
        std::cerr << "[E] " << std::this_thread::get_id() << " epoll event error\n";
        epollers_[std::this_thread::get_id()]->remove_fd(e.data.fd);
        ::close(e.data.fd);
        connections_.erase(e.data.fd);
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
        std::cout << "[I] " << std::this_thread::get_id() << " Close " << fd << "\n";
        epollers_[std::this_thread::get_id()]->remove_fd(fd);
        ::close(fd);
        connections_.erase(fd);
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
        std::cout << "[E] " << std::this_thread::get_id() << " Write failed" << "\n";
    } else if (count == wsss.str().size()) {
        std::cout << "[I] " << std::this_thread::get_id() << " Done writing...closing connection!" << "\n";
        epollers_[std::this_thread::get_id()]->remove_fd(fd);
        ::close(fd);
        connections_.erase(fd);
    }
}


