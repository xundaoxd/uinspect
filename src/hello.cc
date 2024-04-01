#include <iostream>

#include "frida-gum.h"

void print() { std::cout << "hello world" << std::endl; }
int main(int argc, char *argv[]) {
  print();
  printf("addr: %lx\n", gum_module_find_export_by_name(NULL, "_Z5printv"));
  return 0;
}
