#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "elf.h"

void* self_addr;
size_t self_size;

static void* do_mmap_self(size_t* size);
static void do_unmmap_self(void* addr, size_t size);
static void do_main_hook();

__attribute__((constructor)) void uinspect_init() {
  self_addr = do_mmap_self(&self_size);
  if (self_addr == MAP_FAILED) {
    return;
  }
  do_main_hook();
}
__attribute__((destructor)) void uinspect_uninit() {
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

static void do_main_hook() {
  ElfW(Ehdr)* hdr = (ElfW(Ehdr)*)self_addr;
  // TODO: hook main func
}