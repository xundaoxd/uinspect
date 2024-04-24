#include <chrono>
#include <iostream>

int fib(int n) {
  if (n == 0 || n == 1) {
    return 1;
  }
  return fib(n - 1) + fib(n - 2);
}

void do_payload() { fib(20); }

int main(int argc, char *argv[]) {
  do_payload();
  auto st = std::chrono::steady_clock::now();
  for (int i = 0; i < 100000; i++) {
    do_payload();
  }
  auto ed = std::chrono::steady_clock::now();
  std::cout << "time: " << (ed - st).count() / 100000 << " ns\n";
  return 0;
}
