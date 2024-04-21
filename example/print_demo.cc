#include <iostream>

int fib(int n) {
  if (n == 0 || n == 1) {
    return 1;
  }
  return fib(n - 1) + fib(n - 2);
}
__attribute__((noinline)) void do_payload(int n) {
  std::cout << fib(n) << std::endl;
}

int main(int argc, char *argv[]) {
  for (int i = 0; i < 1000; i++) {
    do_payload(rand() % 20 + 20);
  }
  return 0;
}
