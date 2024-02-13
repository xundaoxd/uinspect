#include "init.h"

#include "hook.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <string.h>
#include <sys/mman.h>
#define _GNU_SOURCE /* See feature_test_macros(7) */
#include <link.h>

HookCtx main_ctx;

static int iter_callback(struct dl_phdr_info *info, size_t size, void *data) {
  ElfW(Addr) file_offset = *(ElfW(Addr) *)data;

  if (!strlen(info->dlpi_name)) {
    for (int i = 0; i < info->dlpi_phnum; i++) {
      auto &phdr = info->dlpi_phdr[i];
      if (phdr.p_offset <= file_offset &&
          (phdr.p_offset + phdr.p_filesz) > file_offset) {
        main_ctx.segment_beg = (void *)(info->dlpi_addr + phdr.p_vaddr);
        main_ctx.segment_end =
            (void *)(info->dlpi_addr + phdr.p_vaddr + phdr.p_memsz);
        main_ctx.addr =
            (file_offset - phdr.p_offset) + (char *)main_ctx.segment_beg;
        main_ctx.segment_prot = phdr.p_flags;
        return 1;
      }
    }
  }
  return 2;
}

static int hook_main(int argc, char *argv[], char *envp[]) {
  // TODO: do init
  printf("fake main\n");

  // call code
  UnHookFunc(&main_ctx);
  return ((int (*)(int, char *[], char *[]))main_ctx.addr)(argc, argv, envp);
}

void UinspectInit() {
  if (getenv("UINSPECT_ADDR") == NULL) {
    return;
  }
  ElfW(Addr) file_offset = strtoul(getenv("UINSPECT_ADDR"), NULL, 0);
  if (2 == dl_iterate_phdr(&iter_callback, &file_offset)) {
    printf("iter phdr failed\n");
    return;
  }

  main_ctx.func = (void *)&hook_main;
  if (int code = HookFunc(&main_ctx)) {
    printf("hook main func failed, code: %d\n", code);
    return;
  }
}
