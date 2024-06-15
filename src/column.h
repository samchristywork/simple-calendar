#ifndef COLUMN_H
#define COLUMN_H

#include <gtk/gtk.h>

#include "event.h"

typedef struct Rect {
  float x;
  float y;
  float w;
  float h;
} Rect;

Rect get_event_rect(Event event, float column_width, int column_height,
                    int column);

#endif
