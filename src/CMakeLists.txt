add_library(uinspect SHARED uinspect.cc hook.cc)
target_include_directories(uinspect PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(uinspect frida::frida)

