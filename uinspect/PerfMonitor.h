#pragma once
#include <linux/perf_event.h>
#include <sched.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>
#include <unordered_map>
#include <vector>

namespace uinspect {

class PerfMonitor {
  int fd_{-1};

  std::unordered_map<std::uint64_t, std::uint64_t *> id2addr_;
  std::vector<char> buf_;

  int Monitor(struct perf_event_attr *hw_event, pid_t pid, int cpu,
              unsigned long flags) {
    int ret;
    ret = syscall(SYS_perf_event_open, hw_event, pid, cpu, fd_,
                  flags | PERF_FLAG_FD_CLOEXEC);
    if (fd_ == -1 && ret != -1) {
      fd_ = ret;
    }
    return ret;
  }

 public:
  int Monitor(struct perf_event_attr *hw_event, std::uint64_t *val) {
    int ret = Monitor(hw_event, 0, -1, 0);
    if (ret != -1) {
      std::uint64_t id;
      ioctl(ret, PERF_EVENT_IOC_ID, &id);
      id2addr_.emplace(id, val);
    }
    return ret;
  }
  int Monitor(perf_sw_ids e, std::uint64_t *val) {
    struct perf_event_attr pe;
    memset(&pe, 0, sizeof(pe));

    pe.size = sizeof(pe);
    pe.type = PERF_TYPE_SOFTWARE;
    pe.config = e;

    pe.disabled = 1;
    pe.inherit = 0;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;
    pe.exclude_guest = 1;
    pe.exclude_idle = 1;

    pe.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;

    return Monitor(&pe, val);
  }
  int Monitor(perf_hw_id e, std::uint64_t *val) {
    struct perf_event_attr pe;
    memset(&pe, 0, sizeof(pe));

    pe.size = sizeof(pe);
    pe.type = PERF_TYPE_HARDWARE;
    pe.config = e;

    pe.disabled = 1;
    pe.inherit = 0;
    pe.exclude_kernel = 1;
    pe.exclude_hv = 1;
    pe.exclude_guest = 1;
    pe.exclude_idle = 1;

    pe.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;

    return Monitor(&pe, val);
  }

  int Enable() {
    return ioctl(fd_, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);
  }
  int Disable() {
    return ioctl(fd_, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);
  }
  int Reset() { return ioctl(fd_, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP); }
  ssize_t Update() {
    struct read_format {
      std::uint64_t val;
      std::uint64_t id;
    };
    std::size_t size =
        sizeof(std::uint64_t) + id2addr_.size() * sizeof(read_format);
    buf_.resize(size);
    ssize_t len = read(fd_, buf_.data(), buf_.size());
    read_format *items =
        reinterpret_cast<read_format *>(buf_.data() + sizeof(std::uint64_t));
    for (std::uint64_t i = 0;
         i < *reinterpret_cast<std::uint64_t *>(buf_.data()); i++) {
      *id2addr_.at(items[i].id) = items[i].val;
    }
    return len;
  }
};

}  // namespace uinspect
