find_package(frida REQUIRED)

file(GLOB SRCS *.cc)
add_library(uinspect SHARED ${SRCS})
target_include_directories(uinspect PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(uinspect frida::frida)
