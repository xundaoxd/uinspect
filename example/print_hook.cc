#include <cstdint>

#include "spdlog/spdlog.h"
#include "uinspect/HookRegistry.h"
#include "uinspect/PerfMonitor.h"

std::uint64_t page_fault = 0;
std::uint64_t inst = 0;
std::uint64_t cycle = 0;
thread_local uinspect::PerfMonitor monitor = []() {
  uinspect::PerfMonitor monitor;
  monitor.Monitor(PERF_COUNT_SW_PAGE_FAULTS, &page_fault);
  monitor.Monitor(PERF_COUNT_HW_CPU_CYCLES, &cycle);
  monitor.Monitor(PERF_COUNT_HW_INSTRUCTIONS, &inst);
  monitor.Reset();
  monitor.Enable();
  return monitor;
}();

static void print_enter(uinspect::HookEntry*) {
  monitor.Update();
  spdlog::info("enter {} {} {}", cycle, inst, page_fault);
}
static void print_exit(uinspect::HookEntry*) {
  monitor.Update();
  spdlog::info("exit {} {} {}", cycle, inst, page_fault);
}

UINSPECT_HOOK(":_Z5printv", print_enter, print_exit)
