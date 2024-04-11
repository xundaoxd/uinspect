cmake_minimum_required(VERSION 3.17)
project(uinspect VERSION 0.0.1)

include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pipe")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -rdynamic -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-inline")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

add_subdirectory(3rdparty)
find_package(frida REQUIRED)

file(GLOB SRCS uinspect/*.cc)
add_library(uinspect SHARED ${SRCS})
target_include_directories(uinspect PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(uinspect PUBLIC frida::frida spdlog)

install(TARGETS uinspect spdlog
    LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
)

option(UINSPECT_BUILD_EXAMPLE "Build Example" ON)
if(${UINSPECT_BUILD_EXAMPLE})
    add_subdirectory(example)
endif()

