#include <gtk/gtk.h>

#include "column.h"
#include "draw.h"
#include "serialization.h"

extern time_t current_time;

extern int width;
extern int height;

int header_height = 50;
int time_column_width = 50;

int get_start_of_week() {
  time_t t = current_time;
  struct tm tm = *localtime(&t);
  tm.tm_hour = 0;
  tm.tm_min = 0;
  tm.tm_sec = 0;
  tm.tm_mday -= tm.tm_wday;
  return mktime(&tm);
}

void cairo_set_source_shade(cairo_t *cr, double shade) {
  cairo_set_source_rgb(cr, shade, shade, shade);
}
