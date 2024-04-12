#include <iostream>
void print() { std::cout << "print" << std::endl; }

static void static_print() { std::cout << "static print" << std::endl; }

int main(int argc, char *argv[]) {
  print();
  static_print();
  return 0;
}
