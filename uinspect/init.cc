#include <unistd.h>

#include <fstream>
#include <sstream>
#include <string>

#include "hook0.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include "utils.h"

UINSPECT_CONSTRUCTOR([]() {
  std::string log_file = getenv("UINSPECT_LOG");
  if (log_file.empty()) {
    log_file = []() {
      std::stringstream tmp;
      if (char* p = getenv("UINSPECT_LOG_PREFIX")) {
        tmp << p << '/';
      }
      tmp <<
          []() {
            std::string tmp;
            std::fstream fs("/proc/self/comm");
            fs >> tmp;
            return tmp;
          }()
          << '-' << std::to_string(gettid()) << ".log";
      return tmp.str();
    }();
  }
  auto logger = spdlog::basic_logger_mt("uinspect", log_file, true);
  spdlog::set_default_logger(logger);
})

UINSPECT_CONSTRUCTOR([]() { uinspect::hook0_init(); })
UINSPECT_DESTRUCTOR([]() { uinspect::hook0_deinit(); })
