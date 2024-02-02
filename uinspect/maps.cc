#include "maps.h"

#include <stdio.h>

#include <algorithm>
#include <cstring>

Segment segments[SEG_MAX];
int seg_count;

static int dl_iterator_func(struct dl_phdr_info *info, size_t size,
                            void *data) {
  for (int i = 0; i < info->dlpi_phnum; i++) {
    if (info->dlpi_phdr[i].p_type & PT_LOAD &&
        info->dlpi_phdr[i].p_flags & PF_X) {
      ElfW(Addr) seg_start = info->dlpi_addr + info->dlpi_phdr[i].p_vaddr;
      ElfW(Addr) seg_end = seg_start + info->dlpi_phdr[i].p_memsz;
      segments[seg_count] = Segment{info->dlpi_phdr[i].p_type,
                                    info->dlpi_phdr[i].p_flags,
                                    seg_start,
                                    seg_end,
                                    info->dlpi_phdr[i].p_align,
                                    info->dlpi_addr};
      strncpy(segments[seg_count].dl_name, info->dlpi_name,
              sizeof(Segment::dl_name));
      seg_count++;
    }
  }
  return 0;
}

void SetupMaps() {
  seg_count = 0;
  dl_iterate_phdr(&dl_iterator_func, 0);
  std::sort(segments, segments + seg_count,
            [](const auto &a, const auto &b) { return a.start < b.start; });
}

void DumpMaps() {
  for (int i = 0; i < seg_count; i++) {
    printf("name: %s\n", segments[i].dl_name);
    printf("start: 0x%lx\n", segments[i].start);
    printf("end: 0x%lx\n", segments[i].end);
  }
}
