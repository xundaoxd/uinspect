#include "hook.h"

#include <strings.h>

#include <string>
#include <vector>

#include "common.h"

namespace uinspect {

struct HookEntry {
  std::string slot;
  std::string soname;
  std::string sym;
  void (*enter)(const char *, GumCpuContext *);
  void (*exit)(const char *, GumCpuContext *);
};
std::vector<HookEntry> entries;
void do_hook(const char *slot, void (*enter)(const char *, GumCpuContext *),
             void (*exit)(const char *, GumCpuContext *)) {
  const char *so_end = index(slot, ':');
  if (so_end == NULL) {
    fprintf(stderr, "[uinspect] hook format invalid, hook: %s\n", slot);
    return;
  }
  HookEntry entry;
  entry.slot = slot;
  entry.soname = std::string(slot, so_end - slot);
  entry.sym = std::string(so_end + 1, so_end + 1 + strlen(so_end + 1));
  entry.enter = enter;
  entry.exit = exit;
  entries.emplace_back(std::move(entry));
}

}  // namespace uinspect

#define FUNC_TYPE_LISTENER (func_listener_get_type())
G_DECLARE_FINAL_TYPE(FuncListener, func_listener, FUNC, LISTENER, GObject)

struct _FuncListener {
  GObject parent;
};

static void func_listener_class_init(FuncListenerClass *) {}
static void func_listener_init(FuncListener *) {}
static void func_listener_iface_init(gpointer g_iface, gpointer) {
  GumInvocationListenerInterface *iface =
      (GumInvocationListenerInterface *)g_iface;
  iface->on_enter = [](GumInvocationListener *, GumInvocationContext *ic) {
    uinspect::HookEntry *entry =
        GUM_IC_GET_FUNC_DATA(ic, uinspect::HookEntry *);
    entry->enter(entry->slot.c_str(), ic->cpu_context);
  };
  iface->on_leave = [](GumInvocationListener *, GumInvocationContext *ic) {
    uinspect::HookEntry *entry =
        GUM_IC_GET_FUNC_DATA(ic, uinspect::HookEntry *);
    entry->exit(entry->slot.c_str(), ic->cpu_context);
  };
}

G_DEFINE_TYPE_EXTENDED(FuncListener, func_listener, G_TYPE_OBJECT, 0,
                       G_IMPLEMENT_INTERFACE(GUM_TYPE_INVOCATION_LISTENER,
                                             func_listener_iface_init))

GumInvocationListener *hook_listener;

void hook_init() {
  hook_listener =
      (GumInvocationListener *)g_object_new(FUNC_TYPE_LISTENER, NULL);
  gum_interceptor_begin_transaction(interceptor);
  for (auto &&entry : uinspect::entries) {
    GumAddress entry_addr =
        gum_module_find_export_by_name(NULL, entry.sym.c_str());
    if (!entry_addr) {
      fprintf(stderr, "[uinspect] cannot find sym address, sym: %s\n",
              entry.sym.c_str());
      continue;
    }
    gum_interceptor_attach(interceptor, GSIZE_TO_POINTER(entry_addr),
                           hook_listener, &entry);
  }
  gum_interceptor_end_transaction(interceptor);
}
void hook_deinit() {
  gum_interceptor_detach(interceptor, hook_listener);
  g_object_unref(hook_listener);
}
