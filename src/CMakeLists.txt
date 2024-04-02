add_library(uinspect SHARED uinspect.cc hook.cc)
target_link_libraries(uinspect frida::frida)

