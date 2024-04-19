#pragma once

#include <string>
#include <vector>

#include "utils.h"

namespace uinspect {

struct HookEntry {
  std::string slot;
  void (*on_enter)(HookEntry *);
  void (*on_leave)(HookEntry *);
};

struct HookRegistry {
  std::vector<HookEntry> hooks;
  static HookRegistry *Instance();
};

inline void new_hook(const char *slot, void (*enter)(HookEntry *),
                     void (*exit)(HookEntry *)) {
  HookEntry entry;
  entry.slot = slot;
  entry.on_enter = enter;
  entry.on_leave = exit;
  HookRegistry::Instance()->hooks.emplace_back(std::move(entry));
}

inline void new_hook(void *addr, void (*enter)(HookEntry *),
                     void (*exit)(HookEntry *)) {
  char tmp[128];
  sprintf(tmp, "%p", addr);
  new_hook(tmp, enter, exit);
}

}  // namespace uinspect

#define UINSPECT_HOOK(...) \
  UINSPECT_CONSTRUCTOR([]() { ::uinspect::new_hook(__VA_ARGS__); })
