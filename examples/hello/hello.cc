#include <stdio.h>

#include <thread>

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  printf("main function addr: 0x%p\n", &main);
  std::this_thread::sleep_for(std::chrono::seconds(10));
  return 0;
}
