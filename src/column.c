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

  cairo_text_extents(cr, subtext, &extents);
  cairo_set_source_shade(cr, 0.2);
  cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cr, 10);
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
