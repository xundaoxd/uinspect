#include <cstdint>
#include <iostream>

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
  return monitor;
}();

static void print_enter(uinspect::HookEntry*) {
  std::cout << "[uinspect] print enter" << std::endl;
  monitor.Update();
  monitor.Enable();
}
static void print_exit(uinspect::HookEntry*) {
  monitor.Disable();
  auto prev_fault = page_fault;
  auto prev_cycle = cycle;
  auto prev_inst = inst;
  monitor.Update();
  std::cout << "[uinspect] print leave" << std::endl;
  std::cout << "[uinspect] page fault: " << page_fault - prev_fault
            << std::endl;
  std::cout << "[uinspect] inst: " << inst - prev_inst << std::endl;
  std::cout << "[uinspect] cycle: " << cycle - prev_cycle << std::endl;
}

UINSPECT_HOOK(":_Z5printv", print_enter, print_exit)
