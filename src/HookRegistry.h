#pragma once

#include <cstring>
#include <string>
#include <vector>

namespace uinspect {

struct HookEntry {
  std::string slot;
  std::string soname;
  std::string sym;
  void (*enter)();
  void (*exit)();
};

struct HookRegistry {
  std::vector<HookEntry> hooks;
  static HookRegistry *Instance();
};

inline void new_hook(const char *slot, void (*enter)(), void (*exit)()) {
  const char *so_end = index(slot, ':');
  if (so_end == NULL) {
    fprintf(stderr, "[uinspect] hook format invalid, hook: %s\n", slot);
    return;
  }
  HookEntry entry;
  entry.slot = slot;
  entry.soname = std::string(slot, so_end - slot);
  entry.sym = std::string(so_end + 1, strlen(so_end + 1));
  entry.enter = enter;
  entry.exit = exit;
  HookRegistry::Instance()->hooks.emplace_back(std::move(entry));
}

}  // namespace uinspect

#define UINSPECT__CAT(x, y) x##y
#define UINSPECT_CAT(x, y) UINSPECT__CAT(x, y)

#define UINSPECT_HOOK(...)                             \
  static int UINSPECT_CAT(uinspect_hook_register_,     \
                          __COUNTER__) = []() -> int { \
    uinspect::new_hook(__VA_ARGS__);                   \
    return 0;                                          \
  }();
