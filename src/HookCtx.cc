#include "HookCtx.h"

namespace uinspect {

HookCtx* HookCtx::Instance() {
  thread_local HookCtx inst;
  return &inst;
}

}  // namespace uinspect
