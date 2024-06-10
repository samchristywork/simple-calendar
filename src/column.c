#include <gtk/gtk.h>
#include <stdbool.h>

#include "draw.h"
#include "event.h"
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
