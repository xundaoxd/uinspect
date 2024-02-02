#include <iostream>

__attribute__((constructor)) void so_main() {
  std::cout << "uinspect" << std::endl;
}

