#include "SegmentManager.h"

#include <cstdlib>
#include <cstring>

namespace uinspect {

SegmentManager::SegmentManager() { Refresh(); }

SegmentManager* SegmentManager::Instance() {
  static SegmentManager inst;
  return &inst;
}

void SegmentManager::Refresh() {
  dl_iterate_phdr(
      [](struct dl_phdr_info* info, size_t, void* self) {
        SegmentManager* manager = (SegmentManager*)(self);
        for (ElfW(Half) i = 0; i < info->dlpi_phnum; i++) {
          if (info->dlpi_phdr[i].p_type == PT_LOAD) {
            auto file_addr = info->dlpi_phdr[i].p_offset;
            auto file_sz = info->dlpi_phdr[i].p_filesz;

            auto mem_addr = info->dlpi_addr + info->dlpi_phdr[i].p_vaddr;
            auto mem_sz = info->dlpi_phdr[i].p_memsz;

            std::string name;
            if (strlen(info->dlpi_name)) {
              char real_path[1024];
              if (realpath(info->dlpi_name, real_path) == NULL) {
                name = info->dlpi_name;
              } else {
                name = real_path;
              }
            }

            manager->name2segment[name].emplace(
                file_addr, SegmentInfo{info->dlpi_name, file_addr, file_sz,
                                       mem_addr, mem_sz});
          }
        }
        return 0;
      },
      this);
}

const SegmentInfo* SegmentManager::FindSegmentByFile(const char* name,
                                                     ElfW(Off) addr) {
  auto iter = name2segment.find(name);
  if (iter == name2segment.end()) {
    char real_path[1024];
    if (realpath(name, real_path) == NULL) {
      return nullptr;
    }
    iter = name2segment.find(real_path);
    if (iter == name2segment.end()) {
      return nullptr;
    }
  }
  auto seg_iter = iter->second.lower_bound(addr);
  if (seg_iter == iter->second.end()) {
    return nullptr;
  }
  if (seg_iter->first > addr && seg_iter != iter->second.begin()) {
    seg_iter = std::prev(seg_iter);
  }
  const SegmentInfo* info = &seg_iter->second;
  if (info->file_addr <= addr && info->file_addr + info->file_sz > addr) {
    return info;
  }
  return nullptr;
}

}  // namespace uinspect
