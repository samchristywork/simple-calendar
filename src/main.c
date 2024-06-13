#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <stdbool.h>

#include "handle.h"
#include "serialization.h"

char *filename = "./calendar.txt";

time_t current_time = 0;

int width = 1400;
int height = 800;

int main(int argc, char *argv[]) {
  current_time = time(NULL);

  deserialize_events(filename);

  gtk_init(&argc, &argv);

  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 100, 100);
  gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
  gtk_window_set_title(GTK_WINDOW(window), "Calendar");

  GdkDisplay *display = gdk_display_get_default();
  if (display == NULL) {
    g_print("No display found.\n");
    return -1;
  }

  GdkMonitor *monitor = gdk_display_get_primary_monitor(display);
  if (monitor == NULL) {
    monitor = gdk_display_get_monitor_at_point(display, 0, 0);
    if (monitor == NULL) {
      g_print("No monitor found.\n");
      return -1;
    }
  }

  GdkRectangle geometry;
  gdk_monitor_get_geometry(monitor, &geometry);

  GtkWidget *drawing_area = gtk_drawing_area_new();
  gtk_widget_set_size_request(drawing_area, width, height);
  gtk_container_add(GTK_CONTAINER(window), drawing_area);

  connect_signals(window, drawing_area);

  gtk_widget_show_all(window);
  gtk_main();
}
