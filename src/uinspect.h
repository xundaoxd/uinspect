#pragma once

namespace uinspect {

void uinspect_hook(const char* slot, void (*enter)(), void (*exit)());

struct HookRegister {
  HookRegister(const char* slot, void (*enter)(), void (*exit)()) {
    uinspect_hook(slot, enter, exit);
  }
};

}  // namespace uinspect

#define UINSPECT__CAT(x, y) x##y
#define UINSPECT_CAT(x, y) UINSPECT__CAT(x, y)

#define HOOK(slot, enter, exit)                                               \
  namespace {                                                                 \
  static ::uinspect::HookRegister UINSPECT_CAT(uinspect_hook_register_,       \
                                               __COUNTER)(slot, enter, exit); \
  }
