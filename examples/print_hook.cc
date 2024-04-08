#include <iostream>

#include "uinspect.h"

static void print_enter(uinspect::HookEntry* hook) {
  std::cout << "[uinspect] print enter" << std::endl;
}
static void print_exit(uinspect::HookEntry* hook) {
  std::cout << "[uinspect] print leave" << std::endl;
}

UINSPECT_HOOK(":_Z5printv", print_enter, print_exit)
