#pragma once

#include <cstring>
#include <string>
#include <vector>

#include "spdlog/spdlog.h"
#include "utils.h"

namespace uinspect {

struct HookEntry {
  std::string slot;
  std::string soname;
  std::string sym;
  void (*enter)(HookEntry *);
  void (*exit)(HookEntry *);
};

struct HookRegistry {
  std::vector<HookEntry> hooks;
  static HookRegistry *Instance();
};

inline void new_hook(const char *slot, void (*enter)(HookEntry *),
                     void (*exit)(HookEntry *)) {
  const char *so_end = index(slot, ':');
  if (so_end == NULL) {
    spdlog::warn("hook format invalid, hook: {}", slot);
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

#define UINSPECT_HOOK(...) \
  UINSPECT_CONSTRUCTOR([]() { ::uinspect::new_hook(__VA_ARGS__); })
