#include <stdio.h>

#include "frida-gum.h"

#define FUNC_TYPE_LISTENER (func_listener_get_type())
G_DECLARE_FINAL_TYPE(FuncListener, func_listener, FUNC, LISTENER, GObject)

struct _FuncListener {
  GObject parent;
};

static void func_listener_iface_init(gpointer g_iface, gpointer iface_data);
G_DEFINE_TYPE_EXTENDED(FuncListener, func_listener, G_TYPE_OBJECT, 0,
                       G_IMPLEMENT_INTERFACE(GUM_TYPE_INVOCATION_LISTENER,
                                             func_listener_iface_init))

static void func_listener_class_init(FuncListenerClass *klass) {}
static void func_listener_init(FuncListener *inst) {}

static void func_listener_on_enter(GumInvocationListener *listener,
                                   GumInvocationContext *ic) {
  const char *func = GUM_IC_GET_FUNC_DATA(ic, const char *);
  printf("[%s] enter\n", func);
}
static void func_listener_on_leave(GumInvocationListener *listener,
                                   GumInvocationContext *ic) {
  const char *func = GUM_IC_GET_FUNC_DATA(ic, const char *);
  printf("[%s] leave\n", func);
}

static void func_listener_iface_init(gpointer g_iface, gpointer iface_data) {
  GumInvocationListenerInterface *iface = g_iface;
  iface->on_enter = func_listener_on_enter;
  iface->on_leave = func_listener_on_leave;
}

GumInterceptor *interceptor;
GumInvocationListener *listener;

__attribute__((constructor)) void uinspect_enter() {
  gum_init_embedded();
  interceptor = gum_interceptor_obtain();
  listener = g_object_new(FUNC_TYPE_LISTENER, NULL);

  gum_interceptor_begin_transaction(interceptor);
  gum_interceptor_attach(
      interceptor,
      GSIZE_TO_POINTER(gum_module_find_export_by_name(NULL, "_Z5printv")),
      listener, GSIZE_TO_POINTER("print"));
  gum_interceptor_end_transaction(interceptor);
}

__attribute__((destructor)) void uinspect_exit() {
  g_object_unref(listener);
  g_object_unref(interceptor);
  gum_deinit_embedded();
}
