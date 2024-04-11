#include "FunctionListener.h"

static void uinspect_function_listener_class_init(
    UinspectFunctionListenerClass *klass) {
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->set_property = [](GObject *object, guint property_id,
                                  const GValue *value, GParamSpec *pspec) {
    UinspectFunctionListener *self = UINSPECT_FUNCTION_LISTENER(object);
    switch (property_id) {
      case 1:
        self->on_enter =
            (void (*)(GumInvocationListener *,
                      GumInvocationContext *))g_value_get_pointer(value);
        break;
      case 2:
        self->on_leave =
            (void (*)(GumInvocationListener *,
                      GumInvocationContext *))g_value_get_pointer(value);
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
  };
  object_class->get_property = [](GObject *object, guint property_id,
                                  GValue *value, GParamSpec *pspec) {
    UinspectFunctionListener *self = UINSPECT_FUNCTION_LISTENER(object);
    switch (property_id) {
      case 1:
        g_value_set_pointer(value, GSIZE_TO_POINTER(self->on_enter));
        break;
      case 2:
        g_value_set_pointer(value, GSIZE_TO_POINTER(self->on_leave));
        break;
      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
  };
  g_object_class_install_property(
      object_class, 1,
      g_param_spec_pointer("on_enter", "", "", G_PARAM_READWRITE));
  g_object_class_install_property(
      object_class, 2,
      g_param_spec_pointer("on_leave", "", "", G_PARAM_READWRITE));
}
static void uinspect_function_listener_init(UinspectFunctionListener *self) {
  self->on_enter = nullptr;
  self->on_leave = nullptr;
}

static void uinspect_function_listener_iface_init(
    GumInvocationListenerInterface *iface) {
  iface->on_enter = [](GumInvocationListener *listener,
                       GumInvocationContext *ic) {
    UinspectFunctionListener *self = UINSPECT_FUNCTION_LISTENER(listener);
    if (self->on_enter) {
      self->on_enter(listener, ic);
    }
  };
  iface->on_leave = [](GumInvocationListener *listener,
                       GumInvocationContext *ic) {
    UinspectFunctionListener *self = UINSPECT_FUNCTION_LISTENER(listener);
    if (self->on_leave) {
      self->on_leave(listener, ic);
    }
  };
}

G_DEFINE_TYPE_WITH_CODE(
    UinspectFunctionListener, uinspect_function_listener, G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(GUM_TYPE_INVOCATION_LISTENER,
                          uinspect_function_listener_iface_init))
