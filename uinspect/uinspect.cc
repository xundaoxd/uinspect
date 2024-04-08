#include "Hook.h"
#include "frida-gum.h"
#include "spdlog/spdlog.h"

static bool uinspect_inited = false;
static bool hook_inited = false;

#define MAIN_TYPE_LISTENER (main_listener_get_type())
G_DECLARE_FINAL_TYPE(MainListener, main_listener, MAIN, LISTENER, GObject)

struct _MainListener {
  GObject parent;
};

static void main_listener_class_init(MainListenerClass *) {}
static void main_listener_init(MainListener *) {}
static void main_listener_iface_init(gpointer g_iface, gpointer) {
  GumInvocationListenerInterface *iface =
      (GumInvocationListenerInterface *)g_iface;
  iface->on_enter = [](GumInvocationListener *, GumInvocationContext *) {
    if (hook_inited) {
      spdlog::warn("call init multiple times, ignore");
      return;
    }
    uinspect::HookInit();
    hook_inited = true;
  };
}

G_DEFINE_TYPE_EXTENDED(MainListener, main_listener, G_TYPE_OBJECT, 0,
                       G_IMPLEMENT_INTERFACE(GUM_TYPE_INVOCATION_LISTENER,
                                             main_listener_iface_init))

GumInterceptor *interceptor;
static GumInvocationListener *main_listener;

namespace uinspect {

void uinspect_init() {
  const char *entry = getenv("UINSPECT_ENTRY");
  if (entry == NULL) {
    entry = "main";
  }
  GumAddress entry_addr = gum_module_find_export_by_name(NULL, entry);
  if (!entry_addr) {
    spdlog::warn("cannot find entry address, entry: {}", entry);
    return;
  }

  gum_init_embedded();
  interceptor = gum_interceptor_obtain();
  main_listener =
      (GumInvocationListener *)g_object_new(MAIN_TYPE_LISTENER, NULL);

  gum_interceptor_begin_transaction(interceptor);
  gum_interceptor_attach(interceptor, GSIZE_TO_POINTER(entry_addr),
                         main_listener, nullptr);
  gum_interceptor_end_transaction(interceptor);
  uinspect_inited = true;
}

void uinspect_deinit() {
  if (uinspect_inited) {
    if (hook_inited) {
      uinspect::HookDeinit();
    }
    gum_interceptor_detach(interceptor, main_listener);
    g_object_unref(main_listener);
    g_object_unref(interceptor);
    gum_deinit_embedded();
  }
}

}  // namespace uinspect