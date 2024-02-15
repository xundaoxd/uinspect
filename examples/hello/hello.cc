#include <thread>

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  std::this_thread::sleep_for(std::chrono::seconds(10));
  return 0;
}
