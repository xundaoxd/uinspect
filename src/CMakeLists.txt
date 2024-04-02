add_library(uinspect uinspect.cc hook.cc)
target_link_libraries(uinspect frida::frida)
set_target_properties(uinspect PROPERTIES
    POSITION_INDEPENDENT_CODE True)

