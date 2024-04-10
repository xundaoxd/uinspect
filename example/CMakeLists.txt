add_library(print_hook SHARED print_hook.cc)
target_link_libraries(print_hook uinspect)

add_executable(print_demo print_demo.cc)
target_link_options(print_demo PRIVATE -export-dynamic)

