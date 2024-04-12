#pragma once

#include <exception>
#include <string>
#include <utility>

#include "SegmentManager.h"
#include "frida-gum.h"

#define UINSPECT__CAT(x, y) x##y
#define UINSPECT_CAT(x, y) UINSPECT__CAT(x, y)

namespace uinspect {

inline GumAddress ResolveAddr(const char* sym) {
  auto addr = gum_module_find_export_by_name(NULL, sym);
  if (addr) {
    return addr;
  }
  char tmp[512];
  strncpy(tmp, sym, sizeof(tmp));

  char* delim = index(tmp, ':');
  if (delim == NULL) {
    return 0;
  }
  *delim = '\0';
  addr = gum_module_find_export_by_name(tmp, delim + 1);
  if (addr) {
    return addr;
  }
  addr = gum_module_find_symbol_by_name(tmp, delim + 1);
  if (addr) {
    return addr;
  }

  try {
    auto offset = std::stoul(delim + 1, nullptr, 0);
    auto info = SegmentManager::Instance()->FindSegmentByFile(tmp, offset);
    if (info) {
      return info->mem_addr + offset - info->file_addr;
    }
  } catch (std::exception&) {
  };

  return 0;
}

template <typename F>
auto MakeConstructor(F&& f) -> decltype(auto) {
  struct Constructor {
    Constructor(F&& f) { std::forward<F>(f)(); }
  };

  return Constructor(std::forward<F>(f));
}

template <typename F>
auto MakeDestructor(F&& f) -> decltype(auto) {
  struct Destructor {
    std::decay_t<F> func;
    Destructor(F&& f) : func(std::forward<F>(f)) {}
    ~Destructor() { std::forward<F>(func)(); }
  };

  return Destructor(std::forward<F>(f));
}

}  // namespace uinspect

#define UINSPECT_CONSTRUCTOR(func)                                           \
  namespace {                                                                \
  auto UINSPECT_CAT(init_, __COUNTER__) = ::uinspect::MakeConstructor(func); \
  }

#define UINSPECT_DESTRUCTOR(func)                                             \
  namespace {                                                                 \
  auto UINSPECT_CAT(deinit_, __COUNTER__) = ::uinspect::MakeDestructor(func); \
  }
