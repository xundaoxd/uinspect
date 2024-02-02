#include "maps.h"

__attribute__((constructor(200))) void so_main() {
  SetupMaps();
  DumpMaps();
}
