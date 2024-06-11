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

gboolean handle_key(GtkWidget *widget, GdkEventKey *event, gpointer data) {
  (void)data;
  if (event->keyval == GDK_KEY_q) {
    // TODO: Ask if we want to save before quitting
    gtk_main_quit();
  } else if (event->keyval == GDK_KEY_Escape) {
    gtk_main_quit();
  } else if (event->keyval == GDK_KEY_d) {
    if (selected_event != -1) {
      free(events[selected_event].name);
      for (int i = selected_event; i < n_events - 1; i++) {
        events[i] = events[i + 1];
      }
      n_events--;
    }
  } else if (event->keyval == GDK_KEY_s) {
    serialize_events(filename);
    gtk_widget_queue_draw(widget);
  } else if (event->keyval == GDK_KEY_n) {
    select_next_event(widget);
  } else if (event->keyval == GDK_KEY_p) {
    select_previous_event(widget);
  } else if (event->keyval == GDK_KEY_c) {
    if (selected_event != -1) {
      add_event(events[selected_event].name, events[selected_event].start,
                events[selected_event].duration);
    }
  } else if (event->keyval == GDK_KEY_r) {
    if (selected_event != -1) {
      char *newName = ask_for_string("New Name");
      if (newName != NULL) {
        free(events[selected_event].name);
        events[selected_event].name = newName;
        gtk_widget_queue_draw(widget);
      }
    }
  }

  return FALSE;
}

  }

}
