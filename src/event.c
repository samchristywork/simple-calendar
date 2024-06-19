#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "draw.h"
#include "event.h"

extern int time_column_width;
extern int header_height;
extern int height;
extern int width;
extern int num_days;

Events events = {
    .events = NULL, .n = 0, .selected = -1, .start_hour = 0, .end_hour = 24};

float hour_to_y_offset(float hour) {
  int num_hours = events.end_hour - events.start_hour;
  return header_height +
         (hour - events.start_hour) / num_hours * (height - header_height);
}

void add_event(char *name, DateTime start, Duration duration) {
  if (events.events == NULL) {
    events.events = malloc(sizeof(Event));
    events.n = 1;
  } else {
    events.events = realloc(events.events, (events.n + 1) * sizeof(Event));
    events.n++;
  }

  Event *e = &events.events[events.n - 1];
  e->name = strdup(name);
  e->start = start;
  e->duration = duration;
}

void draw_outlined_rectangle(cairo_t *cr, float x, float y, float w, float h) {
  cairo_set_line_width(cr, 0.5);
  cairo_rectangle(cr, x, y, w, h);
  cairo_fill(cr);

  cairo_set_source_shade(cr, 0.3);
  cairo_rectangle(cr, x, y, w, h);
  cairo_stroke(cr);
}

void draw_event_name(cairo_t *cr, Event event, int column, float column_width,
                     float y) {
  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 10);

  cairo_set_source_shade(cr, 0.2);
  cairo_move_to(cr, time_column_width + column * column_width + 10, y + 12);
  cairo_show_text(cr, event.name);
}

void draw_event_duration(cairo_t *cr, Event event, int column,
                         float column_width, float y) {
  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 10);

  int duration = event.duration.seconds;
  char t[6];
  snprintf(t, 6, "%02d:%02d", duration / 3600, (duration % 3600) / 60);
  float justify = 0;
  cairo_text_extents_t extents;
  cairo_text_extents(cr, t, &extents);
  justify = column_width - extents.width - 20;

  cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.4);
  cairo_move_to(cr, justify + time_column_width + column * column_width + 10,
                y + 12);
  cairo_show_text(cr, t);
}

void draw_event(cairo_t *cr, Event event, int column) {
  int day_start_time = get_start_of_week() + column * 24 * 60 * 60;
  float column_height = (float)height - header_height;
  float column_width = (float)(width - time_column_width) / num_days;
  float hour = (float)(event.start.epoch - day_start_time) / (60 * 60);
  float y = hour_to_y_offset(hour);
  int num_hours = events.end_hour - events.start_hour;
  float h =
      (float)event.duration.seconds / (num_hours * 60 * 60) * column_height;

  if (y < header_height) {
    h -= header_height - y;
    y = header_height;
  }

  draw_outlined_rectangle(cr, time_column_width + column * column_width + 5, y,
                          column_width - 10, h);

  draw_event_name(cr, event, column, column_width, y);
  draw_event_duration(cr, event, column, column_width, y);
}
