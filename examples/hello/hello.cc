#include <cstdint>
#include <iostream>

int main(int argc, char *argv[]) {
  std::cout << "hello" << std::endl;
  std::cout << "main addr: 0x" << std::hex << (std::uint64_t)&main << std::endl;
  return 0;
}
