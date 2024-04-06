#pragma once

#include "frida-gum.h"

namespace uinspect {

void do_hook(const char *slot, void (*enter)(const char *, GumCpuContext *),
             void (*exit)(const char *, GumCpuContext *));
void hook_init();
void hook_deinit();

struct HookRegister {
  HookRegister(const char *slot, void (*enter)(const char *, GumCpuContext *),
               void (*exit)(const char *, GumCpuContext *)) {
    do_hook(slot, enter, exit);
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
