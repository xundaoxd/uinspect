#pragma once

struct HookCtx {
  char code[12];
  void *func;
  void *addr;
  void *segment_beg;
  void *segment_end;
  int segment_prot;
};

int HookFunc(HookCtx *ctx);
void UnHookFunc(HookCtx *ctx);
