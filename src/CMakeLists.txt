find_package(PkgConfig REQUIRED)
pkg_check_modules(gobject REQUIRED IMPORTED_TARGET gobject-2.0)

add_library(uinspect SHARED uinspect.c)
target_link_libraries(uinspect PRIVATE PkgConfig::gobject frida::frida)

add_executable(hello hello.cc)
target_link_libraries(hello frida::frida)

