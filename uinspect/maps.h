#pragma once

#define _GNU_SOURCE /* See feature_test_macros(7) */
#include <dlfcn.h>
#include <link.h>

#define PATH_MAX 256
#define SEG_MAX 1024

struct Segment {
  // seg attr
  ElfW(Word) type;
  ElfW(Word) flags;
  ElfW(Addr) start;
  ElfW(Addr) end;
  ElfW(Xword) align;

  // so attr
  ElfW(Addr) base_addr;
  char dl_name[PATH_MAX];
};

extern Segment segments[SEG_MAX];
extern int seg_count;

void SetupMaps();

void DumpMaps();
