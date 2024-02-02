#include "maps.h"

__attribute__((constructor)) void so_main() {
  SetupMaps();
  DumpMaps();
}
