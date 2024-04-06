#include <iostream>

#include "uinspect.h"

static void print_enter(const char *, GumCpuContext *) {
  std::cout << "[uinspect] print enter" << std::endl;
}
static void print_exit(const char *, GumCpuContext *) {
  std::cout << "[uinspect] print leave" << std::endl;
}

UINSPECT_HOOK(":_Z5printv", print_enter, print_exit)
