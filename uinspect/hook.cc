#pragma once
#include "hook.h"

#include <cstring>

#include <sys/mman.h>
#define _GNU_SOURCE /* See feature_test_macros(7) */
#include <link.h>

int HookFunc(HookCtx *ctx) {
  if (int c = mprotect(ctx->segment_beg,
                       (char *)ctx->segment_end - (char *)ctx->segment_beg,
                       ctx->segment_prot | PROT_WRITE)) {
    return c;
  }
  memmove(ctx->code, ctx->addr, sizeof(HookCtx::code));
  ((char *)ctx->addr)[0] = 0x48;
  ((char *)ctx->addr)[1] = 0xb8;
  *(ElfW(Addr) *)&((char *)ctx->addr)[2] = (ElfW(Addr))ctx->func;
  ((char *)ctx->addr)[10] = 0xff;
  ((char *)ctx->addr)[11] = 0xe0;
  return 0;
}
void UnHookFunc(HookCtx *ctx) {
  memmove(ctx->addr, ctx->code, sizeof(HookCtx::code));
}
