#include <iostream>

#include "uinspect.h"

static void print_enter() {
  std::cout << "[uinspect] print enter" << std::endl;
}
static void print_exit() { std::cout << "[uinspect] print leave" << std::endl; }

HOOK(":_Z5printv", print_enter, print_exit)