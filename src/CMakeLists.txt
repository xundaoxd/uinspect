add_library(uinspect SHARED uinspect.c)
target_link_libraries(uinspect frida::frida)

