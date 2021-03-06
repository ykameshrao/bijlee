//
// Created by Kamesh Rao Yeduvakula on 9/9/18.
// Copyright (c) 2018, Kaleidosoft Labs. All rights reserved.
//

#include <bijlee/io_event_loop.h>
#include <unistd.h>
#include <memory>
#include <sstream>
#include <cstring>

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
    //connections_.insert(std::make_pair(connection_fd, addr));
    epollers_[worker_threads_idx_[current_epoller_]]->add_fd(connection_fd, EPOLLIN | EPOLLERR | EPOLLPRI | EPOLLHUP | EPOLLRDHUP | EPOLLET);
    current_epoller_++;
    if ( current_epoller_ >= workers_ )
        current_epoller_ = 0;
}

void bjl::io_event_loop::run() {
    std::cout << "[I] Starting socket connection io event loop; thread_name: " << std::this_thread::get_id() << std::endl;

    for ( int i = 0; i < workers_ ; i++ ) {
        worker_threads_.push_back(std::make_unique<std::thread>([this, i]() {
            std::cout << "[I] Input/Output event loop " << i << "; thread_name: " << std::this_thread::get_id() << std::endl;
            worker_threads_idx_.insert(std::make_pair(i, std::this_thread::get_id()));
            epollers_.insert(std::make_pair(std::this_thread::get_id(), std::make_unique<epoller>()));

            for (;;) {
                std::vector<epoll_event> events;
                int ready_fds = epollers_[std::this_thread::get_id()]->poll(events, 1024, std::chrono::milliseconds(-1));

                if (ready_fds > 0) {
                    for (const auto &event: events) {
                        close_on_error(event);

                        if (event.events & EPOLLIN) {
                            if(read_data(event.data.fd)) {
                                epollers_[std::this_thread::get_id()]->rearm_fd(event.data.fd, EPOLLOUT | EPOLLERR | EPOLLPRI | EPOLLHUP | EPOLLRDHUP | EPOLLET);
                                std::cout << "[I] thread_name: " << std::this_thread::get_id() << "; fd: " << event.data.fd << " Done reading...now writing!" << std::endl;
                            }
                        } else if (event.events & EPOLLOUT) {
                            std::cout << "[I] thread_name: " << std::this_thread::get_id() << "; fd: " << event.data.fd << " Sending data ..";
                            send_data(event.data.fd);
                        } else {
                            check_for_errors(event);
                        }
                    }
                }
            }
        }));
    }
}

void bjl::io_event_loop::close_on_error(epoll_event e) {
    if (e.events & EPOLLERR || e.events & EPOLLHUP || ( !(e.events & EPOLLIN) && !(e.events & EPOLLOUT) ) ) {
        std::cerr << "[E] thread_name: " << std::this_thread::get_id() << "; epoll event error" << std::endl;
        epollers_[std::this_thread::get_id()]->remove_fd(e.data.fd);
        ::close(e.data.fd);
        //connections_.erase(e.data.fd);
    }
}

bool bjl::io_event_loop::read_data(int fd) {
    char buf[8192];
    memset(buf, 0, sizeof buf);

    ssize_t totalBytes = 0;
    for (;;) {
        ssize_t bytes;
        bytes = ::recv(fd, buf + totalBytes, 8192 - totalBytes, 0);
        if (bytes == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                if (totalBytes > 0) {
                    std::cout << fd << " says: " <<  buf;
                    return true;
                }
            } else {
                if (errno == ECONNRESET) {
                    std::cout << "[I] thread_name: " << std::this_thread::get_id() << "; ECONNRESET Disconnected " << fd << std::endl;
                } else {
                    throw std::runtime_error(strerror(errno));
                }
            }
            break;
        } else if (bytes == 0) {
            std::cout << "[I] thread_name: " << std::this_thread::get_id() << "; Disconnected " << fd << std::endl;
            epollers_[std::this_thread::get_id()]->remove_fd(fd);
            ::close(fd);
            break;
        } else {
            totalBytes += bytes;
            if (totalBytes >= 8192) {
                std::cerr << "Too long packet" << std::endl;
                break;
            }
        }
    }

    return false;
}

bool bjl::io_event_loop::send_data(int fd) {
    std::string response = "[\n"
                           "  {\n"
                           "    \"_id\": \"5b98b3826a5c59509d2d6b38\",\n"
                           "    \"index\": 0,\n"
                           "    \"guid\": \"674352a8-d68e-4c17-adbb-e2b131a740f3\",\n"
                           "    \"isActive\": false,\n"
                           "    \"balance\": \"$1,038.01\",\n"
                           "    \"picture\": \"http://placehold.it/32x32\",\n"
                           "    \"age\": 28,\n"
                           "    \"eyeColor\": \"green\",\n"
                           "    \"name\": {\n"
                           "      \"first\": \"Chapman\",\n"
                           "      \"last\": \"Wilcox\"\n"
                           "    },\n"
                           "    \"company\": \"UTARA\",\n"
                           "    \"email\": \"chapman.wilcox@utara.tv\",\n"
                           "    \"phone\": \"+1 (904) 450-2191\",\n"
                           "    \"address\": \"660 Kossuth Place, Escondida, North Dakota, 6915\",\n"
                           "    \"about\": \"Minim velit nisi qui deserunt magna. Ipsum id officia deserunt veniam irure pariatur ullamco voluptate voluptate. Consequat minim tempor velit est culpa cillum pariatur officia nisi.\",\n"
                           "    \"registered\": \"Thursday, August 16, 2018 6:55 PM\",\n"
                           "    \"latitude\": \"75.328025\",\n"
                           "    \"longitude\": \"-139.145889\",\n"
                           "    \"tags\": [\n"
                           "      \"adipisicing\",\n"
                           "      \"elit\",\n"
                           "      \"minim\",\n"
                           "      \"quis\",\n"
                           "      \"cupidatat\"\n"
                           "    ],\n"
                           "    \"range\": [\n"
                           "      0,\n"
                           "      1,\n"
                           "      2,\n"
                           "      3,\n"
                           "      4,\n"
                           "      5,\n"
                           "      6,\n"
                           "      7,\n"
                           "      8,\n"
                           "      9\n"
                           "    ],\n"
                           "    \"friends\": [\n"
                           "      {\n"
                           "        \"id\": 0,\n"
                           "        \"name\": \"Edna Oneal\"\n"
                           "      },\n"
                           "      {\n"
                           "        \"id\": 1,\n"
                           "        \"name\": \"Millicent Kline\"\n"
                           "      },\n"
                           "      {\n"
                           "        \"id\": 2,\n"
                           "        \"name\": \"Gena Jarvis\"\n"
                           "      }\n"
                           "    ],\n"
                           "    \"greeting\": \"Hello, Chapman! You have 6 unread messages.\",\n"
                           "    \"favoriteFruit\": \"banana\"\n"
                           "  },\n"
                           "  {\n"
                           "    \"_id\": \"5b98b382c4c339a0fc8c47cc\",\n"
                           "    \"index\": 1,\n"
                           "    \"guid\": \"ad68d72b-8842-44ea-a8fe-27d0bc3fdccb\",\n"
                           "    \"isActive\": false,\n"
                           "    \"balance\": \"$3,722.80\",\n"
                           "    \"picture\": \"http://placehold.it/32x32\",\n"
                           "    \"age\": 38,\n"
                           "    \"eyeColor\": \"blue\",\n"
                           "    \"name\": {\n"
                           "      \"first\": \"Ella\",\n"
                           "      \"last\": \"Wallace\"\n"
                           "    },\n"
                           "    \"company\": \"REALMO\",\n"
                           "    \"email\": \"ella.wallace@realmo.me\",\n"
                           "    \"phone\": \"+1 (876) 524-2150\",\n"
                           "    \"address\": \"651 Cox Place, Hobucken, Illinois, 8156\",\n"
                           "    \"about\": \"Enim in est incididunt labore enim elit adipisicing eu sunt cillum esse anim magna. Reprehenderit nostrud excepteur veniam do aute consequat et ullamco. Exercitation pariatur mollit anim elit. Magna commodo est duis veniam.\",\n"
                           "    \"registered\": \"Saturday, August 20, 2016 8:10 PM\",\n"
                           "    \"latitude\": \"10.319829\",\n"
                           "    \"longitude\": \"176.19461\",\n"
                           "    \"tags\": [\n"
                           "      \"et\",\n"
                           "      \"sunt\",\n"
                           "      \"eiusmod\",\n"
                           "      \"dolor\",\n"
                           "      \"dolore\"\n"
                           "    ],\n"
                           "    \"range\": [\n"
                           "      0,\n"
                           "      1,\n"
                           "      2,\n"
                           "      3,\n"
                           "      4,\n"
                           "      5,\n"
                           "      6,\n"
                           "      7,\n"
                           "      8,\n"
                           "      9\n"
                           "    ],\n"
                           "    \"friends\": [\n"
                           "      {\n"
                           "        \"id\": 0,\n"
                           "        \"name\": \"Crane Coffey\"\n"
                           "      },\n"
                           "      {\n"
                           "        \"id\": 1,\n"
                           "        \"name\": \"Hurley Padilla\"\n"
                           "      },\n"
                           "      {\n"
                           "        \"id\": 2,\n"
                           "        \"name\": \"Georgina Emerson\"\n"
                           "      }\n"
                           "    ],\n"
                           "    \"greeting\": \"Hello, Ella! You have 8 unread messages.\",\n"
                           "    \"favoriteFruit\": \"apple\"\n"
                           "  },\n"
                           "  {\n"
                           "    \"_id\": \"5b98b382d2a1b809d71db212\",\n"
                           "    \"index\": 2,\n"
                           "    \"guid\": \"f2f6aea2-9559-46e7-a1fd-8ae387b6ecac\",\n"
                           "    \"isActive\": true,\n"
                           "    \"balance\": \"$1,838.58\",\n"
                           "    \"picture\": \"http://placehold.it/32x32\",\n"
                           "    \"age\": 29,\n"
                           "    \"eyeColor\": \"brown\",\n"
                           "    \"name\": {\n"
                           "      \"first\": \"Hayden\",\n"
                           "      \"last\": \"Pena\"\n"
                           "    },\n"
                           "    \"company\": \"PROSELY\",\n"
                           "    \"email\": \"hayden.pena@prosely.co.uk\",\n"
                           "    \"phone\": \"+1 (939) 576-3344\",\n"
                           "    \"address\": \"913 Garden Place, Mulberry, Maryland, 1831\",\n"
                           "    \"about\": \"Voluptate do esse aliquip sint laboris velit eu aliquip esse magna velit dolor dolore proident. Veniam nostrud ea exercitation dolor. Anim amet consectetur aute ut consectetur consectetur.\",\n"
                           "    \"registered\": \"Sunday, March 30, 2014 12:53 AM\",\n"
                           "    \"latitude\": \"-74.743171\",\n"
                           "    \"longitude\": \"31.6763\",\n"
                           "    \"tags\": [\n"
                           "      \"esse\",\n"
                           "      \"ullamco\",\n"
                           "      \"consequat\",\n"
                           "      \"amet\",\n"
                           "      \"Lorem\"\n"
                           "    ],\n"
                           "    \"range\": [\n"
                           "      0,\n"
                           "      1,\n"
                           "      2,\n"
                           "      3,\n"
                           "      4,\n"
                           "      5,\n"
                           "      6,\n"
                           "      7,\n"
                           "      8,\n"
                           "      9\n"
                           "    ],\n"
                           "    \"friends\": [\n"
                           "      {\n"
                           "        \"id\": 0,\n"
                           "        \"name\": \"Byrd Haley\"\n"
                           "      },\n"
                           "      {\n"
                           "        \"id\": 1,\n"
                           "        \"name\": \"Kimberly Reyes\"\n"
                           "      },\n"
                           "      {\n"
                           "        \"id\": 2,\n"
                           "        \"name\": \"Barton Webb\"\n"
                           "      }\n"
                           "    ],\n"
                           "    \"greeting\": \"Hello, Hayden! You have 5 unread messages.\",\n"
                           "    \"favoriteFruit\": \"strawberry\"\n"
                           "  },\n"
                           "  {\n"
                           "    \"_id\": \"5b98b3829530dcf26222a9b5\",\n"
                           "    \"index\": 3,\n"
                           "    \"guid\": \"4837ada3-9611-4e94-ba0f-b8c92c0d8379\",\n"
                           "    \"isActive\": true,\n"
                           "    \"balance\": \"$3,875.09\",\n"
                           "    \"picture\": \"http://placehold.it/32x32\",\n"
                           "    \"age\": 20,\n"
                           "    \"eyeColor\": \"green\",\n"
                           "    \"name\": {\n"
                           "      \"first\": \"Woodward\",\n"
                           "      \"last\": \"Weeks\"\n"
                           "    },\n"
                           "    \"company\": \"KYAGURU\",\n"
                           "    \"email\": \"woodward.weeks@kyaguru.us\",\n"
                           "    \"phone\": \"+1 (905) 497-2604\",\n"
                           "    \"address\": \"530 Ruby Street, Staples, Vermont, 1259\",\n"
                           "    \"about\": \"Aute non magna amet quis est cupidatat consequat. Culpa tempor elit non cupidatat laboris qui occaecat occaecat dolor. Incididunt culpa aute nisi esse consequat voluptate dolor. Mollit dolore excepteur eu do in velit incididunt voluptate fugiat enim reprehenderit. Veniam veniam eu adipisicing sint.\",\n"
                           "    \"registered\": \"Friday, July 1, 2016 6:08 AM\",\n"
                           "    \"latitude\": \"-55.091042\",\n"
                           "    \"longitude\": \"-7.945398\",\n"
                           "    \"tags\": [\n"
                           "      \"incididunt\",\n"
                           "      \"magna\",\n"
                           "      \"veniam\",\n"
                           "      \"excepteur\",\n"
                           "      \"dolore\"\n"
                           "    ],\n"
                           "    \"range\": [\n"
                           "      0,\n"
                           "      1,\n"
                           "      2,\n"
                           "      3,\n"
                           "      4,\n"
                           "      5,\n"
                           "      6,\n"
                           "      7,\n"
                           "      8,\n"
                           "      9\n"
                           "    ],\n"
                           "    \"friends\": [\n"
                           "      {\n"
                           "        \"id\": 0,\n"
                           "        \"name\": \"Watkins Harper\"\n"
                           "      },\n"
                           "      {\n"
                           "        \"id\": 1,\n"
                           "        \"name\": \"England Duran\"\n"
                           "      },\n"
                           "      {\n"
                           "        \"id\": 2,\n"
                           "        \"name\": \"Alexander Burt\"\n"
                           "      }\n"
                           "    ],\n"
                           "    \"greeting\": \"Hello, Woodward! You have 10 unread messages.\",\n"
                           "    \"favoriteFruit\": \"strawberry\"\n"
                           "  },\n"
                           "  {\n"
                           "    \"_id\": \"5b98b382fdc37a4c90892ed6\",\n"
                           "    \"index\": 4,\n"
                           "    \"guid\": \"b817a1df-5a3a-4830-8fc8-f9a0533e914e\",\n"
                           "    \"isActive\": false,\n"
                           "    \"balance\": \"$3,625.57\",\n"
                           "    \"picture\": \"http://placehold.it/32x32\",\n"
                           "    \"age\": 27,\n"
                           "    \"eyeColor\": \"brown\",\n"
                           "    \"name\": {\n"
                           "      \"first\": \"Johnnie\",\n"
                           "      \"last\": \"Massey\"\n"
                           "    },\n"
                           "    \"company\": \"ETERNIS\",\n"
                           "    \"email\": \"johnnie.massey@eternis.ca\",\n"
                           "    \"phone\": \"+1 (893) 525-3982\",\n"
                           "    \"address\": \"846 Dwight Street, Jessie, Oregon, 8929\",\n"
                           "    \"about\": \"Pariatur qui pariatur dolor eu do sit dolor. Occaecat consequat mollit exercitation ex mollit. Ut anim aute nisi dolor. Sunt qui cupidatat nulla non esse.\",\n"
                           "    \"registered\": \"Wednesday, September 16, 2015 1:02 PM\",\n"
                           "    \"latitude\": \"-18.444964\",\n"
                           "    \"longitude\": \"-79.135692\",\n"
                           "    \"tags\": [\n"
                           "      \"nostrud\",\n"
                           "      \"nulla\",\n"
                           "      \"veniam\",\n"
                           "      \"amet\",\n"
                           "      \"exercitation\"\n"
                           "    ],\n"
                           "    \"range\": [\n"
                           "      0,\n"
                           "      1,\n"
                           "      2,\n"
                           "      3,\n"
                           "      4,\n"
                           "      5,\n"
                           "      6,\n"
                           "      7,\n"
                           "      8,\n"
                           "      9\n"
                           "    ],\n"
                           "    \"friends\": [\n"
                           "      {\n"
                           "        \"id\": 0,\n"
                           "        \"name\": \"Kara Noel\"\n"
                           "      },\n"
                           "      {\n"
                           "        \"id\": 1,\n"
                           "        \"name\": \"Prince Moore\"\n"
                           "      },\n"
                           "      {\n"
                           "        \"id\": 2,\n"
                           "        \"name\": \"Vickie Potter\"\n"
                           "      }\n"
                           "    ],\n"
                           "    \"greeting\": \"Hello, Johnnie! You have 8 unread messages.\",\n"
                           "    \"favoriteFruit\": \"banana\"\n"
                           "  },\n"
                           "  {\n"
                           "    \"_id\": \"5b98b382a7c9ae9975a2733c\",\n"
                           "    \"index\": 5,\n"
                           "    \"guid\": \"7ea79dfa-cc29-49ec-bf89-152e97585cfb\",\n"
                           "    \"isActive\": false,\n"
                           "    \"balance\": \"$1,825.95\",\n"
                           "    \"picture\": \"http://placehold.it/32x32\",\n"
                           "    \"age\": 35,\n"
                           "    \"eyeColor\": \"green\",\n"
                           "    \"name\": {\n"
                           "      \"first\": \"Pickett\",\n"
                           "      \"last\": \"Waller\"\n"
                           "    },\n"
                           "    \"company\": \"BITREX\",\n"
                           "    \"email\": \"pickett.waller@bitrex.biz\",\n"
                           "    \"phone\": \"+1 (817) 483-3901\",\n"
                           "    \"address\": \"426 Argyle Road, Catharine, West Virginia, 299\",\n"
                           "    \"about\": \"Irure cupidatat aute cillum elit duis qui pariatur aute qui consectetur deserunt. Velit nisi magna quis qui culpa incididunt consequat. Sint aute quis ut duis mollit cupidatat anim labore nisi deserunt quis. Proident aliquip incididunt proident commodo reprehenderit. Ea eiusmod ipsum pariatur do voluptate deserunt pariatur aute consequat magna deserunt labore id id. Incididunt enim adipisicing Lorem qui incididunt adipisicing aliquip.\",\n"
                           "    \"registered\": \"Sunday, July 17, 2016 4:18 PM\",\n"
                           "    \"latitude\": \"-45.423635\",\n"
                           "    \"longitude\": \"48.866546\",\n"
                           "    \"tags\": [\n"
                           "      \"ullamco\",\n"
                           "      \"pariatur\",\n"
                           "      \"pariatur\",\n"
                           "      \"incididunt\",\n"
                           "      \"ullamco\"\n"
                           "    ],\n"
                           "    \"range\": [\n"
                           "      0,\n"
                           "      1,\n"
                           "      2,\n"
                           "      3,\n"
                           "      4,\n"
                           "      5,\n"
                           "      6,\n"
                           "      7,\n"
                           "      8,\n"
                           "      9\n"
                           "    ],\n"
                           "    \"friends\": [\n"
                           "      {\n"
                           "        \"id\": 0,\n"
                           "        \"name\": \"Benson Grimes\"\n"
                           "      },\n"
                           "      {\n"
                           "        \"id\": 1,\n"
                           "        \"name\": \"Floyd Molina\"\n"
                           "      },\n"
                           "      {\n"
                           "        \"id\": 2,\n"
                           "        \"name\": \"Blair Pearson\"\n"
                           "      }\n"
                           "    ],\n"
                           "    \"greeting\": \"Hello, Pickett! You have 7 unread messages.\",\n"
                           "    \"favoriteFruit\": \"strawberry\"\n"
                           "  },\n"
                           "  {\n"
                           "    \"_id\": \"5b98b382c49e2a6f5dcc4c2f\",\n"
                           "    \"index\": 6,\n"
                           "    \"guid\": \"1761e356-e23f-4574-8071-30e47a79a479\",\n"
                           "    \"isActive\": true,\n"
                           "    \"balance\": \"$1,915.54\",\n"
                           "    \"picture\": \"http://placehold.it/32x32\",\n"
                           "    \"age\": 23,\n"
                           "    \"eyeColor\": \"brown\",\n"
                           "    \"name\": {\n"
                           "      \"first\": \"Cruz\",\n"
                           "      \"last\": \"Thompson\"\n"
                           "    },\n"
                           "    \"company\": \"ZILLACOM\",\n"
                           "    \"email\": \"cruz.thompson@zillacom.info\",\n"
                           "    \"phone\": \"+1 (911) 476-3964\",\n"
                           "    \"address\": \"923 Louise Terrace, Layhill, Arkansas, 1498\",\n"
                           "    \"about\": \"Enim eiusmod quis occaecat cupidatat eu. Labore quis officia do nulla. Non magna dolore in laborum proident consequat voluptate incididunt aute. Ex anim incididunt minim labore. Nulla fugiat eiusmod dolor deserunt non proident Lorem aliquip. Officia aliquip elit commodo commodo deserunt officia ex in laboris minim duis deserunt officia officia.\",\n"
                           "    \"registered\": \"Monday, April 28, 2014 6:46 AM\",\n"
                           "    \"latitude\": \"75.774709\",\n"
                           "    \"longitude\": \"-23.088559\",\n"
                           "    \"tags\": [\n"
                           "      \"aute\",\n"
                           "      \"mollit\",\n"
                           "      \"consectetur\",\n"
                           "      \"sit\",\n"
                           "      \"officia\"\n"
                           "    ],\n"
                           "    \"range\": [\n"
                           "      0,\n"
                           "      1,\n"
                           "      2,\n"
                           "      3,\n"
                           "      4,\n"
                           "      5,\n"
                           "      6,\n"
                           "      7,\n"
                           "      8,\n"
                           "      9\n"
                           "    ],\n"
                           "    \"friends\": [\n"
                           "      {\n"
                           "        \"id\": 0,\n"
                           "        \"name\": \"Rita Parsons\"\n"
                           "      },\n"
                           "      {\n"
                           "        \"id\": 1,\n"
                           "        \"name\": \"Marisa Hammond\"\n"
                           "      },\n"
                           "      {\n"
                           "        \"id\": 2,\n"
                           "        \"name\": \"Short Barnes\"\n"
                           "      }\n"
                           "    ],\n"
                           "    \"greeting\": \"Hello, Cruz! You have 5 unread messages.\",\n"
                           "    \"favoriteFruit\": \"apple\"\n"
                           "  },\n"
                           "  {\n"
                           "    \"_id\": \"5b98b3829682958a8b5944fc\",\n"
                           "    \"index\": 7,\n"
                           "    \"guid\": \"ac32374c-e84e-43e2-b81f-0d926b533f75\",\n"
                           "    \"isActive\": true,\n"
                           "    \"balance\": \"$2,285.98\",\n"
                           "    \"picture\": \"http://placehold.it/32x32\",\n"
                           "    \"age\": 34,\n"
                           "    \"eyeColor\": \"green\",\n"
                           "    \"name\": {\n"
                           "      \"first\": \"Singleton\",\n"
                           "      \"last\": \"Shepard\"\n"
                           "    },\n"
                           "    \"company\": \"TELEPARK\",\n"
                           "    \"email\": \"singleton.shepard@telepark.net\",\n"
                           "    \"phone\": \"+1 (858) 452-3607\",\n"
                           "    \"address\": \"987 Langham Street, Chapin, South Dakota, 8779\",\n"
                           "    \"about\": \"Cillum et exercitation reprehenderit qui sint nulla pariatur veniam ex elit. Amet aliqua cillum est tempor officia eu culpa aliquip pariatur aute. Anim eu adipisicing labore exercitation qui consectetur ea laboris ipsum consectetur. Reprehenderit ullamco non cupidatat elit exercitation fugiat ex dolor voluptate amet ut et. Minim in do nostrud nulla dolore elit ad pariatur duis dolore excepteur dolore pariatur incididunt. Anim consequat sint reprehenderit eiusmod labore deserunt aliqua id consequat adipisicing esse velit aliqua incididunt. Non minim eu ipsum consectetur anim ut.\",\n"
                           "    \"registered\": \"Tuesday, January 28, 2014 10:08 AM\",\n"
                           "    \"latitude\": \"-16.194585\",\n"
                           "    \"longitude\": \"-9.874218\",\n"
                           "    \"tags\": [\n"
                           "      \"exercitation\",\n"
                           "      \"laboris\",\n"
                           "      \"minim\",\n"
                           "      \"consequat\",\n"
                           "      \"ex\"\n"
                           "    ],\n"
                           "    \"range\": [\n"
                           "      0,\n"
                           "      1,\n"
                           "      2,\n"
                           "      3,\n"
                           "      4,\n"
                           "      5,\n"
                           "      6,\n"
                           "      7,\n"
                           "      8,\n"
                           "      9\n"
                           "    ],\n"
                           "    \"friends\": [\n"
                           "      {\n"
                           "        \"id\": 0,\n"
                           "        \"name\": \"Blanchard Holden\"\n"
                           "      },\n"
                           "      {\n"
                           "        \"id\": 1,\n"
                           "        \"name\": \"Fischer Wynn\"\n"
                           "      },\n"
                           "      {\n"
                           "        \"id\": 2,\n"
                           "        \"name\": \"Angeline Dejesus\"\n"
                           "      }\n"
                           "    ],\n"
                           "    \"greeting\": \"Hello, Singleton! You have 10 unread messages.\",\n"
                           "    \"favoriteFruit\": \"banana\"\n"
                           "  }\n"
                           "]";
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
        std::cout << "[E] thread_name: " << std::this_thread::get_id() << "; fd: " << fd << " Write failed" << std::endl;
    } else if (count == wsss.str().size()) {
        std::cout << "[I] thread_name: " << std::this_thread::get_id() << "; fd: " << fd << " Done writing...closing connection!" << std::endl;
        epollers_[std::this_thread::get_id()]->remove_fd(fd);
        ::close(fd);
        //connections_.erase(fd);
    }
}

void bjl::io_event_loop::check_for_errors(const epoll_event &event) {
    if(event.events & EPOLLRDHUP) {
        std::cout   << "[I] thread_name: " << std::this_thread::get_id()
                    << "; fd: " << event.data.fd
                    << "; event: EPOLLRDHUP";
    } else if(event.events & EPOLLHUP) {
        std::cout   << "[I] thread_name: " << std::this_thread::get_id()
                    << "; fd: " << event.data.fd
                    << "; event: EPOLLHUP";
    } else if(event.events & EPOLLERR) {
        std::cout   << "[I] thread_name: " << std::this_thread::get_id()
                    << "; fd: " << event.data.fd
                    << "; event: EPOLLERR";
    } else if(event.events & EPOLLPRI) {
        std::cout   << "[I] thread_name: " << std::this_thread::get_id()
                    << "; fd: " << event.data.fd
                    << "; event: EPOLLPRI";
    }
}
