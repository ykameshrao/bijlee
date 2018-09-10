cmake_minimum_required(VERSION 3.12)
project(bijlee)

include(CheckCXXCompilerFlag)

option(B_BUILD_TESTS "build tests alongside the project" OFF)
option(BIJLEE_BUILD_EXAMPLES "build examples alongside the project" OFF)
option(BIJLEE_INSTALL "add bijlee as install target (recommended)" ON)

CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

# Clang exports C++17 in std::experimental namespace (tested on Clang 5 and 6).
# This gives an error on date.h external library.
# Following workaround forces Clang to compile at best with C++14
if(COMPILER_SUPPORTS_CXX17 AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
endif()

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory (src)

include_directories (src)

add_subdirectory (examples)

# Set version...

# Major and minor version...
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)

# Make available in a header file...
configure_file (
        "include/bijlee/version.h.in"
        "include/bijlee/version.h"
        @ONLY
)

# Install header...
install (
        FILES
        ${CMAKE_BINARY_DIR}/include/bijlee/version.h
        DESTINATION
        include/bijlee/
)

# Configure the pkg-config metadata...

# Initialize the metadata variables to support remote builds...
set(prefix         ${CMAKE_INSTALL_PREFIX})
set(exec_prefix    ${CMAKE_INSTALL_PREFIX}/bin)
set(libdir         ${CMAKE_INSTALL_PREFIX}/lib)
set(includedir     ${CMAKE_INSTALL_PREFIX}/include)
set(version        ${VERSION_MAJOR}.${VERSION_MINOR})

# Perform substitutions...
configure_file (
        "libbijlee.pc.in"
        "libbijlee.pc"
        @ONLY
)

# Install pkg-config metadata into standard location within the prefix...
install (
        FILES
        ${CMAKE_BINARY_DIR}/libbijlee.pc
        DESTINATION
        lib/pkgconfig/
)
