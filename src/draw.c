#include <gtk/gtk.h>

#include "column.h"
#include "draw.h"
#include "serialization.h"

extern time_t current_time;

extern int width;
extern int height;

int header_height = 50;
int time_column_width = 50;
int week_number = 0;

int get_start_of_week() {
  time_t t = current_time;
  struct tm tm = *localtime(&t);
  tm.tm_hour = 0;
  tm.tm_min = 0;
  tm.tm_sec = 0;
  tm.tm_mday -= tm.tm_wday;
  return mktime(&tm) + week_number * 7 * 24 * 60 * 60;
}

void cairo_set_source_shade(cairo_t *cr, double shade) {
  cairo_set_source_rgb(cr, shade, shade, shade);
}

void draw_modified_indicator(cairo_t *cr) {
  if (check_modified()) {
    cairo_set_source_rgb(cr, 1.0, 0.7, 0.7);
    cairo_rectangle(cr, 0, 0, time_column_width, header_height);
    cairo_fill(cr);
  }
}

void draw_current_time(cairo_t *cr) {
  float column_width = (float)(width - time_column_width) / 7;
  float column_height = (float)height - header_height;

  time_t current_time = time(NULL);
  int day_start_time = get_start_of_week();
  int current_day = (current_time - day_start_time) / (24 * 60 * 60);
  int current_hour =
      (current_time - day_start_time - current_day * 24 * 60 * 60) / (60 * 60);
  int current_minute = (current_time - day_start_time -
                        current_day * 24 * 60 * 60 - current_hour * 60 * 60) /
                       60;

  float x = time_column_width + current_day * column_width;
  float y = header_height + current_hour * column_height / 24 +
            current_minute * column_height / 24 / 60;

  cairo_set_source_shade(cr, 0.0);
  cairo_set_line_width(cr, 2.0);
  cairo_move_to(cr, x, y);
  cairo_line_to(cr, x + column_width, y);
  cairo_stroke(cr);

  char *time_str = malloc(6);
  sprintf(time_str, "%02d:%02d", current_hour, current_minute);
  cairo_text_extents_t extents;
  cairo_text_extents(cr, time_str, &extents);

  cairo_move_to(cr, x + column_width - extents.width - 10,
                y + column_height / 24 / 2 - 2);
  cairo_show_text(cr, time_str);
  free(time_str);
}

void draw_time_column(cairo_t *cr) {
  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 10);

  float column_width = time_column_width;
  float column_height = (float)height - header_height;

  cairo_set_source_shade(cr, 0.9);
  cairo_rectangle(cr, 0, header_height, column_width, column_height);
  cairo_fill(cr);

  cairo_set_source_shade(cr, 0.2);
  cairo_set_line_width(cr, 0.5);
  for (int i = 0; i < 24; i++) {
    int x = 0;
    int y = header_height + i * column_height / 24;
    cairo_move_to(cr, x, y);
    cairo_line_to(cr, x + column_width, y);
    cairo_stroke(cr);

    char time[6];
    sprintf(time, "%02d:00", i);

    cairo_text_extents_t extents;
    cairo_text_extents(cr, time, &extents);

    cairo_move_to(cr, x + column_width / 2 - extents.width / 2,
                  y + column_height / 24 / 2 + extents.height / 2);
    cairo_show_text(cr, time);
  }
}

void draw(cairo_t *cr) {
  cairo_set_source_shade(cr, 1.0);
  cairo_paint(cr);

  draw_modified_indicator(cr);
  draw_time_column(cr);
  for (int i = 0; i < 7; i++) {
    draw_column(cr, i);
  }

  draw_current_time(cr);
}
