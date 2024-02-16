#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

#include "elf.h"

static void* do_mmap_self(size_t* size);
static void do_unmmap_self(void* addr, size_t size);

static void main_hook();

__attribute__((constructor)) void uinspect_init() {
  void* self_addr;
  size_t self_size;

  self_addr = do_mmap_self(&self_size);
  if (self_addr == MAP_FAILED) {
    perror("mmap self failed");
    goto mmap_err;
  }

  // do hook begin
  struct SymEntry {
    ElfW(Sym) * sym;
    ElfW(Addr) virt_addr;
    ElfW(Word) prot;
    ElfW(Addr) virt_begin;
    ElfW(Xword) virt_size;
  };
  SymEntry esym;
  esym.sym = elf_find_symbol((ElfW(Ehdr)*)self_addr, "main");

  if (!dl_iterate_phdr(
          [](struct dl_phdr_info* info, size_t size, void* data) -> int {
            (void)size;
            SymEntry* entry = (SymEntry*)data;
            if (!strlen(info->dlpi_name)) {
              return 0;
            }
            for (ElfW(Half) i = 0; i < info->dlpi_phnum; i++) {
              const ElfW(Phdr)& phdr = info->dlpi_phdr[i];
              if (phdr.p_offset <= entry->sym->st_value &&
                  (phdr.p_offset + phdr.p_filesz > entry->sym->st_value)) {
                entry->virt_addr = info->dlpi_addr + phdr.p_vaddr +
                                   entry->sym->st_value - phdr.p_offset;
                entry->prot = phdr.p_flags;
                entry->virt_begin = info->dlpi_addr + phdr.p_vaddr;
                entry->virt_size = phdr.p_memsz;
                return 1;
              }
            }
            return 0;
          },
          &esym)) {
    perror("can't find virt address");
    goto hook_err;
  }
  if (mprotect((void*)esym.virt_begin, esym.virt_size,
               esym.prot | PROT_WRITE)) {
    perror("can't set virt page prot");
    goto hook_err;
  }
  // TODO: hook main func
  // do hook end

hook_err:
  do_unmmap_self(self_addr, self_size);
mmap_err:
  return;
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

static void main_hook() {}
