#ifndef MOUSE_H
#define MOUSE_H

#include <gtk/gtk.h>

gboolean handle_mouse_press(GtkWidget *widget, GdkEventButton *event,
                            gpointer data);

gboolean handle_mouse_release(GtkWidget *widget, GdkEventButton *event,
                              gpointer data);

gboolean handle_mouse_drag(GtkWidget *widget, GdkEventMotion *event,
                           gpointer data);

#endif
