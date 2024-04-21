macro(add_hook name)
    add_library(${name} SHARED ${ARGN})
    target_link_libraries(${name} uinspect)
    install(TARGETS ${name}
        LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
    )
endmacro()

add_executable(print_demo print_demo.cc)
target_link_options(print_demo PRIVATE -export-dynamic)

add_hook(print_hook print_hook.cc)
add_hook(planning_hook planning_hook.cc)
add_hook(perception_hook perception_hook.cc)
add_hook(map_engine_hook map_engine_hook.cc)

