file (GLOB COMMON_SOURCE_FILES "common/*.cpp")
file (GLOB TCP_SOCKET_SOURCE_FILES "tcp_socket/*.cpp")
file (GLOB RAFT_SOURCE_FILES "raft/*.cpp")

file (GLOB INCLUDE_FILES ${PROJECT_SOURCE_DIR}/include/bijlee/*h)

set(SOURCE_FILES
        ${COMMON_SOURCE_FILES}
        ${TCP_SOCKET_SOURCE_FILES}
        ${RAFT_SOURCE_FILES}
        ${INCLUDE_FILES})

add_library(bijlee ${SOURCE_FILES})
set_target_properties(bijlee PROPERTIES
        OUTPUT_NAME "bijlee"
        VERSION ${GENERIC_LIB_VERSION}
        SOVERSION ${GENERIC_LIB_SOVERSION}
        )
add_definitions(-DONLY_C_LOCALE=1)

target_include_directories(bijlee PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        )

set(include_install_dir "../include")
set(lib_install_dir "lib/")
set(bin_install_dir "bin/")

target_link_libraries(bijlee pthread)

if (BIJLEE_INSTALL)
    install(
            TARGETS bijlee
            EXPORT ${targets_export_name}
            ARCHIVE DESTINATION ${lib_install_dir}
            LIBRARY DESTINATION ${lib_install_dir}
            RUNTIME DESTINATION ${bin_install_dir}
            INCLUDES DESTINATION ${include_install_dir})

    install(
            DIRECTORY "../include/bijlee"
            DESTINATION ${include_install_dir}
            FILES_MATCHING PATTERN "*.*h")
    install(TARGETS bijlee
            EXPORT BijleeTargets
            DESTINATION lib)
    install(EXPORT BijleeTargets
            DESTINATION "lib/cmake/bijlee"
            EXPORT_LINK_INTERFACE_LIBRARIES
            COMPONENT cmake-config
            )

    include(CMakePackageConfigHelpers)
    configure_package_config_file(
            "BijleeConfig.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/BijleeConfig.cmake"
            INSTALL_DESTINATION "lib/cmake/bijlee"
    )
    install(
            FILES "${CMAKE_CURRENT_BINARY_DIR}/BijleeConfig.cmake"
            DESTINATION "lib/cmake/bijlee"
            COMPONENT cmake-config
    )
endif()
