#include <stdbool.h>

#include "column.h"
#include "draw.h"
#include "hsv.h"

extern int width;
extern int height;

extern int time_column_width;
extern int header_height;

// TODO: Turn this into a single struct
extern Event *events;
extern int n_events;
extern int selected_event;

void draw_column_header(cairo_t *cr, int i, char *text, char *subtext) {
  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 16);

  float column_width = (float)(width - time_column_width) / 7;
  cairo_set_source_shade(cr, 0.9);
  cairo_rectangle(cr, time_column_width + i * column_width, 0, column_width,
                  header_height);
  cairo_fill(cr);

  cairo_text_extents_t extents;
  cairo_text_extents(cr, text, &extents);
  cairo_set_source_shade(cr, 0.3);
  int x = time_column_width + i * column_width + column_width / 2 -
          extents.width / 2;
  cairo_move_to(cr, x, 22);
  cairo_show_text(cr, text);

  cairo_set_source_shade(cr, 0.2);
  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 10);
  cairo_text_extents(cr, subtext, &extents);
  x = time_column_width + i * column_width + column_width / 2 -
      extents.width / 2;
  cairo_move_to(cr, x, 40);
  cairo_show_text(cr, subtext);
}

void draw_column_borders(cairo_t *cr, int i) {
  float column_width = (float)(width - time_column_width) / 7;
  cairo_set_source_shade(cr, 0.2);
  cairo_set_line_width(cr, 0.5);

  cairo_move_to(cr, time_column_width + i * column_width, 0);
  cairo_line_to(cr, time_column_width + i * column_width, height);
  cairo_stroke(cr);

  cairo_move_to(cr, time_column_width + i * column_width, header_height);
  cairo_line_to(cr, time_column_width + (i + 1) * column_width, header_height);
  cairo_stroke(cr);
}

void draw_column_grids(cairo_t *cr, int i) {
  float column_width = (float)(width - time_column_width) / 7;
  float column_height = (float)height - header_height;

  for (int j = 0; j < 24; j++) {
    int x = time_column_width + i * column_width;
    int y = header_height + j * column_height / 24;

    cairo_set_source_shade(cr, 0.5);
    cairo_set_line_width(cr, 0.5);
    cairo_move_to(cr, x, y);
    cairo_line_to(cr, x + column_width, y);
    cairo_stroke(cr);

    y += column_height / 24 / 2;
    cairo_set_source_shade(cr, 0.8);
    cairo_set_line_width(cr, 0.5);
    cairo_move_to(cr, x, y);
    cairo_line_to(cr, x + column_width, y);
    cairo_stroke(cr);
  }
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
  cairo_move_to(cr, time_column_width + column * column_width + 10,
                y + header_height + 12);
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
                y + header_height + 12);
  cairo_show_text(cr, t);
}

void draw_event(cairo_t *cr, Event event, int column) {
  int day_start_time = get_start_of_week() + column * 24 * 60 * 60;
  float column_height = (float)height - header_height;
  float column_width = (float)(width - time_column_width) / 7;
  float y = (float)(event.start.epoch - day_start_time) / (24 * 60 * 60) *
            column_height;
  float h = (float)event.duration.seconds / (24 * 60 * 60) * column_height;

  draw_outlined_rectangle(cr, time_column_width + column * column_width + 5,
                          y + header_height, column_width - 10, h);

  draw_event_name(cr, event, column, column_width, y);
  draw_event_duration(cr, event, column, column_width, y);
}

float string_to_hue(char *s) {
  int sum = 0;
  for (size_t i = 0; i < strlen(s); i++) {
    sum += s[i];
  }

  return (float)(sum % 360) / 360;
}

bool check_if_ranges_overlap(int start_time1, int end_time1, int start_time2,
                             int end_time2) {
  if (start_time1 >= start_time2 && start_time1 < end_time2) {
    return true;
  }

  if (end_time1 > start_time2 && end_time1 <= end_time2) {
    return true;
  }

  if (start_time2 >= start_time1 && start_time2 < end_time1) {
    return true;
  }

  if (end_time2 > start_time1 && end_time2 <= end_time1) {
    return true;
  }

  return false;
}

int event_has_overlap(int idx) {
  Event event = events[idx];
  for (int i = 0; i < n_events; i++) {
    if (i == idx) {
      continue;
    }

    Event other = events[i];
    int event_start_time = event.start.epoch;
    int event_end_time = event.start.epoch + event.duration.seconds;
    int other_start_time = other.start.epoch;
    int other_end_time = other.start.epoch + other.duration.seconds;
    if (check_if_ranges_overlap(event_start_time, event_end_time,
                                other_start_time, other_end_time)) {
      return i;
    }
  }
  return -1;
}

Rect get_event_rect(Event event, float column_width, int column_height,
                    int column) {
  int event_start_time = event.start.epoch;

  int day_start_time = get_start_of_week() + column * 24 * 60 * 60;
  if (event_start_time >= day_start_time &&
      event_start_time < day_start_time + 24 * 60 * 60) {
    float y = (float)(event_start_time - day_start_time) / (24 * 60 * 60) *
              column_height;
    float h = (float)event.duration.seconds / (24 * 60 * 60) * column_height;

    return (Rect){time_column_width + column * column_width + 5,
                  y + header_height, column_width - 10, h};
  }

  return (Rect){0, 0, 0, 0};
}

void draw_overlap_indicator(cairo_t *cr, int i, int idx, int overlap) {
  float column_height = (float)height - header_height;
  float column_width = (float)(width - time_column_width) / 7;
  Event event = events[idx];
  Event other = events[overlap];

  Rect event_rect = get_event_rect(event, column_width, column_height, i);

  Rect other_rect = get_event_rect(other, column_width, column_height, i);

  cairo_set_source_hsva(cr, 0, 0.8, 0.8, 1.0);
  cairo_set_line_width(cr, 3.0);
  cairo_move_to(cr, event_rect.x, event_rect.y);
  cairo_line_to(cr, event_rect.x, event_rect.y + event_rect.h);
  cairo_stroke(cr);

  cairo_move_to(cr, other_rect.x, other_rect.y);
  cairo_line_to(cr, other_rect.x, other_rect.y + other_rect.h);
  cairo_stroke(cr);
}

void draw_column_events(cairo_t *cr, int i, bool draw_selected_event) {
  for (int j = 0; j < n_events; j++) {
    Event event = events[j];

    if (draw_selected_event && selected_event != j) {
      continue;
    }

    float hue = string_to_hue(event.name);
    if (selected_event == j) {
      cairo_set_source_hsva(cr, hue, 0.1, 0.8, 0.9);
    } else {
      cairo_set_source_hsva(cr, hue, 0.1, 0.9, 0.9);
    }

    int day_start_time = get_start_of_week() + i * 24 * 60 * 60;
    if (event.start.epoch >= day_start_time &&
        event.start.epoch < day_start_time + 24 * 60 * 60) {
      draw_event(cr, event, i);
    }

    int overlap = event_has_overlap(j);
    if (overlap != -1) {
      draw_overlap_indicator(cr, i, j, overlap);
    }
  }
}
