#pragma once

#include "HookRegistry.h"
#include "frida-gum.h"

namespace uinspect {

struct HookCtx {
  const HookEntry* hook_ctx;
  const GumCpuContext* cpu_ctx;

  // thread local instance
  static HookCtx* Instance();
};

}  // namespace uinspect
