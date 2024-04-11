#include "FunctionListener.h"
#include "frida-gum.h"
#include "hook.h"
#include "spdlog/spdlog.h"

static bool uinspect_inited = false;
static bool hook_inited = false;

static char type = 'p';
static const char *entry_sym = nullptr;

GumInterceptor *interceptor;
static UinspectFunctionListener *listener;

namespace uinspect {

static int parse_entry() {
  entry_sym = getenv("UINSPECT_ENTRY");
  if (entry_sym == NULL) {
    entry_sym = "p:main";
  }
  const char *delim = index(entry_sym, ':');
  if (delim == NULL || (delim - entry_sym) != 1) {
    spdlog::error("uinspect entry invalid, entry: {}", entry_sym);
    return 1;
  }
  type = entry_sym[0];
  entry_sym = delim + 1;
  return 0;
}

void uinspect_init() {
  if (parse_entry()) {
    return;
  }

  GumAddress entry_addr = gum_module_find_export_by_name(NULL, entry_sym);
  if (!entry_addr) {
    spdlog::error("cannot find entry address, entry: {}", entry_sym);
    return;
  }

  gum_init_embedded();
  interceptor = gum_interceptor_obtain();
  listener = UINSPECT_FUNCTION_LISTENER(
      g_object_new(UINSPECT_TYPE_FUNCTION_LISTENER, NULL));
  if (type == 'p') {
    listener->on_enter = [](GumInvocationListener *, GumInvocationContext *) {
      if (hook_inited) {
        spdlog::warn("call init multiple times, ignore");
        return;
      }
      uinspect::hook_init();
      hook_inited = true;
    };
  } else if (type == 'r') {
    listener->on_leave = [](GumInvocationListener *, GumInvocationContext *) {
      if (hook_inited) {
        spdlog::warn("call init multiple times, ignore");
        return;
      }
      uinspect::hook_init();
      hook_inited = true;
    };
  } else {
    spdlog::error("unknown hook type: {}", type);
    g_object_unref(listener);
    g_object_unref(interceptor);
    gum_deinit_embedded();
    return;
  }

  gum_interceptor_begin_transaction(interceptor);
  gum_interceptor_attach(interceptor, GSIZE_TO_POINTER(entry_addr),
                         GUM_INVOCATION_LISTENER(listener), nullptr);
  gum_interceptor_end_transaction(interceptor);
  uinspect_inited = true;
}

void uinspect_deinit() {
  if (uinspect_inited) {
    if (hook_inited) {
      uinspect::hook_deinit();
    }
    gum_interceptor_detach(interceptor, GUM_INVOCATION_LISTENER(listener));
    g_object_unref(listener);
    g_object_unref(interceptor);
    gum_deinit_embedded();
  }
}

}  // namespace uinspect
