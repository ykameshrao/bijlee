function(bijlee_example example_name)
    set(EXAMPLE_EXECUTABLE run_${example_name})
    set(EXAMPLE_SOURCE ${example_name}.cpp)

    add_executable(${EXAMPLE_EXECUTABLE} ${EXAMPLE_SOURCE})
    target_link_libraries(${EXAMPLE_EXECUTABLE} bijlee)
endfunction()

bijlee_example(tcp_server)
#bijlee_example(dtoma_epoll_example)