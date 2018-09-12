# bijlee
C++ 11/14/17 only lightweight libraries for tcp, raft, etc

## Goal
* To provide lightweight, simple, C++11+ only libraries for multiple use cases like networking, consensus, file I/O, etc utilizing the direct interfacing with Linux System APIs.
* To provide these libraries based on Linux systems to begin with. Cross platform support is NOT a goal at the moment.

## Components
* tcp_socket - tcp socket networking library using Linux epoll interface.
* raft - consensus algorithm implementation

## Benchmarks
* tcp_socket - some very very initial benchmarks of current code are as follows -
  * MBP 2017 model, Ubuntu 18.04, Core i7, 16 GB RAM

  * ./wrk -c40 -t4 -d10s -Htest:test http://0.0.0.0:1983/
  > Running 10s test @ http://0.0.0.0:1983/
  4 threads and 40 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.72ms    1.72ms  22.31ms   91.53%
    Req/Sec     6.45k   561.43     7.79k    72.00%
  256917 requests in 10.01s, 2.79GB read
  Socket errors: connect 0, read 255793, write 1118, timeout 0
Requests/sec:  25666.72
Transfer/sec:    285.36MB

  * ./wrk -c320 -t64 -d60s -Htest:test http://0.0.0.0:1983/
  > Running 1m test @ http://0.0.0.0:1983/
  64 threads and 320 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     9.67ms   37.37ms   1.67s    98.12%
    Req/Sec   354.25    189.18     3.06k    68.44%
  1281477 requests in 1.00m, 13.91GB read
  Socket errors: connect 0, read 1281394, write 56, timeout 24
Requests/sec:  21323.41
Transfer/sec:    237.07MB