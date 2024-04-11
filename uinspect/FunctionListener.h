#pragma once

#include "frida-gum.h"

G_BEGIN_DECLS

#define UINSPECT_TYPE_FUNCTION_LISTENER (uinspect_function_listener_get_type())
G_DECLARE_FINAL_TYPE(UinspectFunctionListener, uinspect_function_listener,
                     UINSPECT, FUNCTION_LISTENER, GObject)

struct _UinspectFunctionListener {
  GObject parent;
  void (*on_enter)(GumInvocationListener *, GumInvocationContext *);
  void (*on_leave)(GumInvocationListener *, GumInvocationContext *);
};

G_END_DECLS
