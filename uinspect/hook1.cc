#include "hook1.h"

#include <strings.h>

#include "FunctionListener.h"
#include "HookRegistry.h"
#include "frida-gum.h"
#include "spdlog/spdlog.h"
#include "utils.h"

extern GumInterceptor *interceptor;
static UinspectFunctionListener *listener;

namespace uinspect {

void hook1_init() {
  listener = UINSPECT_FUNCTION_LISTENER(
      g_object_new(UINSPECT_TYPE_FUNCTION_LISTENER, NULL));
  listener->on_enter = [](GumInvocationListener *, GumInvocationContext *ic) {
    uinspect::HookEntry *entry =
        GUM_IC_GET_FUNC_DATA(ic, uinspect::HookEntry *);
    if (entry->on_enter) {
      entry->on_enter(entry);
    }
  };
  listener->on_leave = [](GumInvocationListener *, GumInvocationContext *ic) {
    uinspect::HookEntry *entry =
        GUM_IC_GET_FUNC_DATA(ic, uinspect::HookEntry *);
    if (entry->on_leave) {
      entry->on_leave(entry);
    }
  };

  gum_interceptor_begin_transaction(interceptor);
  for (auto &&entry : uinspect::HookRegistry::Instance()->hooks) {
    GumAddress entry_addr = ResolveAddr(entry.slot.c_str());
    if (!entry_addr) {
      spdlog::error("cannot find sym address, slot: {}", entry.slot);
      continue;
    }
    gum_interceptor_attach(interceptor, GSIZE_TO_POINTER(entry_addr),
                           GUM_INVOCATION_LISTENER(listener), &entry);
  }
  gum_interceptor_end_transaction(interceptor);
}
void hook1_deinit() {
  gum_interceptor_detach(interceptor, GUM_INVOCATION_LISTENER(listener));
  g_object_unref(listener);
}

}  // namespace uinspect
