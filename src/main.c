#define _GNU_SOURCE
#include <gtk/gtk.h>
#include <stdbool.h>

#include "args.h"
#include "handle.h"
#include "serialization.h"

#define VERSION_STRING "simple-calendar-1.0.0"

#define LICENSE_STRING                                                         \
  "Copyright (C) 2024 Sam Christy.\n"                                          \
  "License GPLv3+: GNU GPL version 3 or later "                                \
  "<http://gnu.org/licenses/gpl.html>\n"                                       \
  "\n"                                                                         \
  "This is free software; you are free to change and redistribute it.\n"       \
  "There is NO WARRANTY, to the extent permitted by law."

time_t current_time = 0;
char *filename;

int width = 1400;
int height = 800;
int num_days = 7;

int main(int argc, char *argv[]) {
  add_arg('f', "filename", "The file to read and write events to",
          ARG_REQUIRED);
  add_arg('h', "help", "Show this help message", ARG_NONE);
  add_arg('v', "version", "Show the version number and license info", ARG_NONE);
  add_arg('x', "width", "The width of the window", ARG_REQUIRED);
  add_arg('y', "height", "The height of the window", ARG_REQUIRED);
  add_arg('n', "days", "The number of days to show", ARG_REQUIRED);

  bool help = get_arg_bool(argc, argv, 'h', false);
  bool version = get_arg_bool(argc, argv, 'v', false);
  filename = get_arg_string(argc, argv, 'f', "./calendar.txt");
  width = get_arg_int(argc, argv, 'x', 1400);
  height = get_arg_int(argc, argv, 'y', 800);
  num_days = get_arg_int(argc, argv, 'n', 7);

  if (version) {
    printf("%s\n\n%s\n", VERSION_STRING, LICENSE_STRING);
    return EXIT_SUCCESS;
  }

  if (help) {
    usage(argv[0]);
    return EXIT_SUCCESS;
  }

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
