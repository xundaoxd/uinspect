#pragma once
#include <unistd.h>

#include <chrono>
#include <cstdint>
#include <fstream>
#include <string>

#include "spdlog/spdlog.h"
#include "uinspect/HookRegistry.h"
#include "uinspect/PerfMonitor.h"

struct CommInfo {};
template <>
struct fmt::formatter<CommInfo> : fmt::formatter<std::string> {
  auto format(const CommInfo &, format_context &ctx) const
      -> decltype(ctx.out()) {
    thread_local const std::string comm = []() {
      std::fstream fs("/proc/self/comm");
      std::string tmp;
      fs >> tmp;
      return tmp;
    }();
    thread_local const auto tid = gettid();
    return format_to(ctx.out(), "{}:{}", comm, tid);
  }
};

struct CurrentTimeInfo {};
template <>
struct fmt::formatter<CurrentTimeInfo> : fmt::formatter<std::string> {
  auto format(const CurrentTimeInfo &, format_context &ctx) const
      -> decltype(ctx.out()) {
    return format_to(
        ctx.out(), "{}",
        std::chrono::system_clock::now().time_since_epoch().count());
  }
};

struct PerfCounter {
  uinspect::PerfMonitor monitor;
  std::uint64_t cycle;
  std::uint64_t inst;

  PerfCounter() {
    if (monitor.Monitor(PERF_COUNT_HW_CPU_CYCLES, &cycle) == -1) {
      spdlog::error("cannot monitor cpu cycle, errno: {}, info: {}", errno,
                    sys_errlist[errno]);
    }
    if (monitor.Monitor(PERF_COUNT_HW_INSTRUCTIONS, &inst) == -1) {
      spdlog::error("cannot monitor cpu inst, errno: {}, info: {}", errno,
                    sys_errlist[errno]);
    }
    monitor.Reset();
    monitor.Enable();
  }

  void Update() { monitor.Update(); }

  static PerfCounter *Instance() {
    static PerfCounter inst;
    return &inst;
  }
};

static void func_enter(uinspect::HookEntry *entry) {
  PerfCounter *counter = PerfCounter::Instance();
  counter->Update();
  spdlog::info("enter {} {} {} {} {}", entry->slot, CommInfo(),
               CurrentTimeInfo(), counter->cycle, counter->inst);
}

static void func_exit(uinspect::HookEntry *entry) {
  PerfCounter *counter = PerfCounter::Instance();
  counter->Update();
  spdlog::info("exit {} {} {} {} {}", entry->slot, CommInfo(),
               CurrentTimeInfo(), counter->cycle, counter->inst);
}
