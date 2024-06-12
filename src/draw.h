#ifndef DRAW_H
#define DRAW_H

#include <gtk/gtk.h>

int get_start_of_week();
void draw(cairo_t *cr);
void cairo_set_source_shade(cairo_t *cr, double shade);

#endif
