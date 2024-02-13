#include "init.h"

__attribute__((constructor(200))) void uinspect_main() { UinspectInit(); }
