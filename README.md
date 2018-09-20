# bijlee [currently being implemented]
C++ 11/14/17 only lightweight libraries for tcp, raft, etc

## Goal
* To provide lightweight, simple, C++11+ only libraries for multiple use cases like networking, consensus, file I/O, etc utilizing the direct interfacing with Linux System APIs.
* To provide these libraries based on Linux systems to begin with. Cross platform support is NOT a goal at the moment.

## Components
* tcp_socket - tcp socket networking library using Linux epoll interface.
* raft - consensus algorithm implementation

## Benchmarks
* tcp_socket - will put after fixing the code