#include <gtk/gtk.h>

#include "dialog.h"
#include "draw.h"
#include "event.h"
#include "select.h"
#include "serialization.h"

extern char *filename;

extern Event *events;
extern int n_events;
extern int selected_event;

extern int time_column_width;
extern int header_height;

extern int width;
extern int height;

typedef struct CellPos {
  int column;
  int row;
} CellPos;

CellPos get_cell(float x, float y) {
  return (CellPos){
      (x - time_column_width) / ((float)(width - time_column_width) / 7),
      2 * (y - header_height) / ((float)height - header_height) * 24};
}

// Refactor with get_event_rect
int get_event(float x, float y) {
  CellPos c = get_cell(x, y);
  int day_start_time = get_start_of_week() + c.column * 24 * 60 * 60;
  for (int i = n_events - 1; i >= 0; i--) {
    Event event = events[i];

    int event_start_time = event.start.epoch;
    if (event_start_time >= day_start_time &&
        event_start_time < day_start_time + 24 * 60 * 60) {
      float event_y = (float)(event_start_time - day_start_time) /
                      (24 * 60 * 60) * (height - header_height);
      float event_h = (float)event.duration.seconds / (24 * 60 * 60) *
                      (height - header_height);

      if (y >= event_y + header_height &&
          y <= event_y + header_height + event_h) {
        return i;
      }
    }
  }

  return -1;
}
