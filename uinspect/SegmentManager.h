#pragma once

#include <link.h>

#include <map>
#include <unordered_map>

namespace uinspect {

struct SegmentInfo {
  const char* dlpi_name;

  ElfW(Off) file_addr;
  ElfW(Xword) file_sz;

  ElfW(Addr) mem_addr;
  ElfW(Xword) mem_sz;
};

class SegmentManager {
  std::unordered_map<std::string, std::map<ElfW(Off), SegmentInfo>>
      name2segment;

  SegmentManager();

 public:
  static SegmentManager* Instance();
  const SegmentInfo* FindSegmentByFile(const char* name, ElfW(Off) addr);
};

}  // namespace uinspect
