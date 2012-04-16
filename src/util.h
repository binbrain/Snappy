#ifndef _UTIL_H
#define _UTIL_H

#include <gtk/gtk.h>

#define HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget * lookup_widget (GtkWidget *widget, const gchar *widget_name);

#endif /* _UTIL_H */
