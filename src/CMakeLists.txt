find_package(PkgConfig REQUIRED)
pkg_check_modules(gobject REQUIRED IMPORTED_TARGET gobject-2.0)

find_package(Threads REQUIRED)
add_library(uinspect SHARED uinspect.c)
target_link_libraries(uinspect PRIVATE PkgConfig::gobject frida Threads::Threads dl)

add_executable(hello hello.cc)
target_link_libraries(hello PRIVATE PkgConfig::gobject frida Threads::Threads dl)

