#include "column.h"
#include "dialog.h"
#include "draw.h"
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

int get_event(float x, float y) {
  float column_width = (float)(width - time_column_width) / 7;
  CellPos c = get_cell(x, y);
  int day_start_time = get_start_of_week() + c.column * 24 * 60 * 60;
  for (int i = 0; i < n_events; i++) {
    Event event = events[i];
    float column_height = (float)height - header_height;
    Rect event_rect = get_event_rect(event, column_width, day_start_time,
                                     column_height, c.column);
    if (x >= event_rect.x && x <= event_rect.x + event_rect.w &&
        y >= event_rect.y && y <= event_rect.y + event_rect.h) {
      return i;
    }
  }

  return -1;
}

      }
    }
  }

}
