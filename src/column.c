#include <stdbool.h>

#include "column.h"
#include "draw.h"
#include "event.h"
#include "hsv.h"

extern int width;
extern int height;

extern int time_column_width;
extern int header_height;
extern int num_days;

extern Events events;

void draw_column_header(cairo_t *cr, int i, char *text, char *subtext) {
  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 16);

  float column_width = (float)(width - time_column_width) / num_days;
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
  float column_width = (float)(width - time_column_width) / num_days;
  cairo_set_source_shade(cr, 0.2);
  cairo_set_line_width(cr, 0.5);

  cairo_move_to(cr, time_column_width + i * column_width, 0);
  cairo_line_to(cr, time_column_width + i * column_width, height);
  cairo_stroke(cr);

  cairo_move_to(cr, time_column_width + i * column_width, header_height);
  cairo_line_to(cr, time_column_width + (i + 1) * column_width, header_height);
  cairo_stroke(cr);
}

void draw_time_text(cairo_t *cr, int x, int y, int hour) {
  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 10);

  char t[6];
  snprintf(t, 6, "%02d:00", hour);
  cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.2);
  cairo_move_to(cr, x + 6, y - 3);
  cairo_show_text(cr, t);
}

void draw_column_grids(cairo_t *cr, int i) {
  float column_width = (float)(width - time_column_width) / num_days;
  float column_height = (float)height - header_height;

  for (int j = events.start_hour; j < events.end_hour; j++) {
    int x = time_column_width + i * column_width;
    int y = hour_to_y_offset(j);

    cairo_set_source_shade(cr, 0.5);
    cairo_set_line_width(cr, 0.5);
    cairo_move_to(cr, x, y);
    cairo_line_to(cr, x + column_width, y);
    cairo_stroke(cr);

    int num_hours = events.end_hour - events.start_hour;
    y += column_height / num_hours / 2;
    cairo_set_source_shade(cr, 0.8);
    cairo_set_line_width(cr, 0.5);
    cairo_move_to(cr, x, y);
    cairo_line_to(cr, x + column_width, y);
    cairo_stroke(cr);

    draw_time_text(cr, x, y, j);
  }
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
  Event event = events.events[idx];
  for (int i = 0; i < events.n; i++) {
    if (i == idx) {
      continue;
    }

    Event other = events.events[i];
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

  int num_hours = events.end_hour - events.start_hour;
  int day_start_time = get_start_of_week() + column * num_hours * 60 * 60;
  if (event_start_time >= day_start_time &&
      event_start_time < day_start_time + 24 * 60 * 60) {
    float hour = (float)(event_start_time - day_start_time) / (60 * 60);
    float y = hour_to_y_offset(hour);
    float h =
        (float)event.duration.seconds / (num_hours * 60 * 60) * column_height;

    return (Rect){time_column_width + column * column_width + 5, y,
                  column_width - 10, h};
  }

  return (Rect){0, 0, 0, 0};
}

void draw_overlap_indicator(cairo_t *cr, int i, int idx, int overlap) {
  float column_height = (float)height - header_height;
  float column_width = (float)(width - time_column_width) / num_days;
  Event event = events.events[idx];
  Event other = events.events[overlap];

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
  for (int j = 0; j < events.n; j++) {
    Event event = events.events[j];

    if (draw_selected_event && events.selected != j) {
      continue;
    }

    float hue = string_to_hue(event.name);
    if (events.selected == j) {
      cairo_set_source_hsva(cr, hue, 0.1, 0.8, 0.8);
    } else {
      cairo_set_source_hsva(cr, hue, 0.1, 0.9, 0.8);
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

void draw_column(cairo_t *cr, int i) {
  char *days[] = {"Sunday",   "Monday", "Tuesday", "Wednesday",
                  "Thursday", "Friday", "Saturday"};

  char date[11];
  time_t t = get_start_of_week() + i * 24 * 60 * 60;
  struct tm tm = *localtime(&t);
  strftime(date, 11, "%Y-%m-%d", &tm);
  char *day = days[tm.tm_wday];
  draw_column_header(cr, i, day, date);
  draw_column_grids(cr, i);
  draw_column_borders(cr, i);
  draw_column_events(cr, i, false);
  draw_column_events(cr, i, true);
}
