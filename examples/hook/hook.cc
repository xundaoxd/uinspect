struct HookEntry {
  enum class Type {
    PROBE,
    RETPROBE,
  };
  const char* entry;
  void* func;
};

#define _CAT(x, y) x##y
#define CAT(x, y) _CAT(x, y)
#define HOOK(e, f)                             \
  __attribute__((section("hook_array"), used)) \
  HookEntry CAT(hook, __COUNTER__) = {.entry = e, .func = (void*)&f};

extern "C" HookEntry *__section_hook_array_start, *__section_hook_array_end;
void hook() {}
HOOK("probe:/lib/x86_64-linux-gnu/libc.so.6:0x84420", hook);
HOOK("probe:/lib/x86_64-linux-gnu/libc.so.6:0x82ce0", hook);
