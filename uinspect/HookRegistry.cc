#include "HookRegistry.h"

namespace uinspect {

HookRegistry* HookRegistry::Instance() {
  static HookRegistry inst;
  return &inst;
}

}  // namespace uinspect
