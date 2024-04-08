#include "Hook.h"

#include <strings.h>

#include "HookRegistry.h"
#include "frida-gum.h"
#include "spdlog/spdlog.h"

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
    entry->enter(entry);
  };
  iface->on_leave = [](GumInvocationListener *, GumInvocationContext *ic) {
    uinspect::HookEntry *entry =
        GUM_IC_GET_FUNC_DATA(ic, uinspect::HookEntry *);
    entry->exit(entry);
  };
}

G_DEFINE_TYPE_EXTENDED(FuncListener, func_listener, G_TYPE_OBJECT, 0,
                       G_IMPLEMENT_INTERFACE(GUM_TYPE_INVOCATION_LISTENER,
                                             func_listener_iface_init))

extern GumInterceptor *interceptor;
static GumInvocationListener *hook_listener;

namespace uinspect {

void HookInit() {
  hook_listener =
      (GumInvocationListener *)g_object_new(FUNC_TYPE_LISTENER, NULL);
  gum_interceptor_begin_transaction(interceptor);
  for (auto &&entry : uinspect::HookRegistry::Instance()->hooks) {
    GumAddress entry_addr =
        gum_module_find_export_by_name(NULL, entry.sym.c_str());
    if (!entry_addr) {
      spdlog::warn("cannot find sym address, sym: {}", entry.sym);
      continue;
    }
    gum_interceptor_attach(interceptor, GSIZE_TO_POINTER(entry_addr),
                           hook_listener, &entry);
  }
  gum_interceptor_end_transaction(interceptor);
}
void HookDeinit() {
  gum_interceptor_detach(interceptor, hook_listener);
  g_object_unref(hook_listener);
}

}  // namespace uinspect
