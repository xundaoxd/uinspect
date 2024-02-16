#pragma once
#include <cstring>

#define _GNU_SOURCE /* See feature_test_macros(7) */
#include <link.h>

inline ElfW(Shdr) * elf_get_section(ElfW(Ehdr) * hdr, ElfW(Half) idx) {
  if (idx >= hdr->e_shnum) {
    return nullptr;
  }
  return (ElfW(Shdr)*)((char*)hdr + hdr->e_shoff + hdr->e_shentsize * idx);
}

inline const char* elf_get_section_name(ElfW(Ehdr) * hdr, ElfW(Shdr) * shdr) {
  ElfW(Shdr)* str = elf_get_section(hdr, hdr->e_shstrndx);
  return (const char*)(hdr) + str->sh_offset + shdr->sh_name;
}

inline ElfW(Shdr) * elf_find_section(ElfW(Ehdr) * hdr, const char* name) {
  for (int i = 0; i < hdr->e_shnum; i++) {
    ElfW(Shdr)* elf_shdr = elf_get_section(hdr, i);
    if (strcmp(name, elf_get_section_name(hdr, elf_shdr)) == 0) {
      return elf_shdr;
    }
  }
  return nullptr;
}

inline ElfW(Sym) * elf_find_symbol(ElfW(Ehdr) * hdr, const char* name) {
  ElfW(Shdr)* str_section = elf_find_section(hdr, ".strtab");
  ElfW(Shdr)* tab_section = elf_find_section(hdr, ".symtab");
  if (!tab_section) {
    return nullptr;
  }
  for (int j = 0; j < tab_section->sh_size / tab_section->sh_entsize; j++) {
    ElfW(Sym)* sym = (ElfW(Sym)*)((char*)hdr + tab_section->sh_offset +
                                  j * tab_section->sh_entsize);
    if (strcmp(name, (const char*)(hdr) + str_section->sh_offset +
                         sym->st_name) == 0) {
      return sym;
    }
  }
  return nullptr;
}
