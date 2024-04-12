#include <string>

#include "hook0.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include "utils.h"

UINSPECT_CONSTRUCTOR([]() {
  std::string log_file = "uinspect.log";
  if (const char *file = getenv("UINSPECT_LOG")) {
    log_file = file;
  }
  auto logger = spdlog::basic_logger_mt("uinspect", log_file, true);
  spdlog::set_default_logger(logger);
})

UINSPECT_CONSTRUCTOR([]() { uinspect::hook0_init(); })
UINSPECT_DESTRUCTOR([]() { uinspect::hook0_deinit(); })
