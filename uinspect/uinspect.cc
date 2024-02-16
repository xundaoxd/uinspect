#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>

#include "elf.h"

static void* do_mmap_self(size_t* size);
static void do_unmmap_self(void* addr, size_t size);

static void do_main_hook(void* self_addr);

__attribute__((constructor)) void uinspect_init() {
  void* self_addr;
  size_t self_size;

  self_addr = do_mmap_self(&self_size);
  if (self_addr == MAP_FAILED) {
    perror("mmap self failed");
    return;
  }

  do_main_hook(self_addr);

  do_unmmap_self(self_addr, self_size);
}

static void* do_mmap_self(size_t* size) {
  int fd = -1;
  struct stat st;
  void* addr = MAP_FAILED;
  fd = open("/proc/self/exe", O_RDONLY);
  if (fd == -1) {
    goto open_err;
  }
  if (-1 == fstat(fd, &st)) {
    goto stat_err;
  }
  *size = st.st_size;
  addr = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

stat_err:
  close(fd);
open_err:
  return addr;
}

static void do_unmmap_self(void* addr, size_t size) {
  if (addr == MAP_FAILED || !size) {
    return;
  }
  munmap(addr, size);
}

#include <stdio.h>

static void do_main_hook(void* self_addr) {
  // TODO: hook main func
  ElfW(Ehdr)* hdr = (ElfW(Ehdr)*)self_addr;
  ElfW(Sym)* main_sym = elf_find_symbol(hdr, "main");
  printf("main func addr: 0x%lx\n", main_sym->st_value);
}
