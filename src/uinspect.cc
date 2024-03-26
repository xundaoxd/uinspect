#include <stdio.h>

__attribute__((constructor)) void uinspect_main() {
  printf("uinspect hello\n");
}
