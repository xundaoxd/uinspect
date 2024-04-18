#include "hook0.h"

#include "FunctionListener.h"
#include "frida-gum.h"
#include "hook1.h"
#include "spdlog/spdlog.h"
#include "utils.h"

static bool uinspect_inited = false;
static bool hook_inited = false;

static char hook_type = 'p';
static GumAddress hook_addr = 0;

GumInterceptor *interceptor;
static UinspectFunctionListener *listener;

namespace uinspect {

static bool parse_entry() {
  const char *entry_sym = getenv("UINSPECT_ENTRY");
  if (entry_sym == NULL) {
    entry_sym = "p:main";
  }
  const char *delim = index(entry_sym, ':');
  if (delim != NULL) {
    if (delim - entry_sym == 1) {
      hook_type = entry_sym[0];
      entry_sym = delim + 1;
    } else {
      spdlog::error("uinspect entry invalid, entry: {}", entry_sym);
      return false;
    }
  }
  hook_addr = ResolveAddr(entry_sym);
  if (!hook_addr) {
    return false;
  }
  return true;
}

void hook0_init() {
  if (!parse_entry()) {
    spdlog::warn("cannot find entry slot");
    return;
  }

  gum_init_embedded();
  interceptor = gum_interceptor_obtain();
  listener = UINSPECT_FUNCTION_LISTENER(
      g_object_new(UINSPECT_TYPE_FUNCTION_LISTENER, NULL));
  if (hook_type == 'p') {
    listener->on_enter = [](GumInvocationListener *, GumInvocationContext *) {
      if (hook_inited) {
        spdlog::warn("call init multiple times, ignore");
        return;
      }
      uinspect::hook1_init();
      hook_inited = true;
    };
  } else if (hook_type == 'r') {
    listener->on_leave = [](GumInvocationListener *, GumInvocationContext *) {
      if (hook_inited) {
        spdlog::warn("call init multiple times, ignore");
        return;
      }
      uinspect::hook1_init();
      hook_inited = true;
    };
  } else {
    spdlog::error("unknown hook type: {}", hook_type);
    g_object_unref(listener);
    g_object_unref(interceptor);
    gum_deinit_embedded();
    return;
  }

  gum_interceptor_begin_transaction(interceptor);
  gum_interceptor_attach(interceptor, GSIZE_TO_POINTER(hook_addr),
                         GUM_INVOCATION_LISTENER(listener), nullptr);
  gum_interceptor_end_transaction(interceptor);
  uinspect_inited = true;
}

void hook0_deinit() {
  if (uinspect_inited) {
    if (hook_inited) {
      uinspect::hook1_deinit();
    }
    gum_interceptor_detach(interceptor, GUM_INVOCATION_LISTENER(listener));
    g_object_unref(listener);
    g_object_unref(interceptor);
    gum_deinit_embedded();
  }
}

}  // namespace uinspect
