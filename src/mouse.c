#include <gtk/gtk.h>

#include "column.h"
#include "dialog.h"
#include "draw.h"
#include "event.h"

extern Events events;

extern int time_column_width;
extern int header_height;

extern int width;
extern int height;

typedef struct CellPos {
  int column;
  int row;
} CellPos;

CellPos get_cell(float x, float y) {
  CellPos c = {0, 0};

  if (x < time_column_width) {
    return c;
  }

  c.column = (x - time_column_width) / ((float)(width - time_column_width) / 7);
  c.row = 2 * (y - header_height) / ((float)height - header_height) * 24;

  return c;
}

int get_event(float x, float y) {
  float column_width = (float)(width - time_column_width) / 7;
  CellPos c = get_cell(x, y);
  for (int i = 0; i < events.n; i++) {
    Event event = events.events[i];
    float column_height = (float)height - header_height;
    Rect event_rect =
        get_event_rect(event, column_width, column_height, c.column);
    if (x >= event_rect.x && x <= event_rect.x + event_rect.w &&
        y >= event_rect.y && y <= event_rect.y + event_rect.h) {
      return i;
    }
  }

  return -1;
}

gboolean handle_mouse_press(GtkWidget *widget, GdkEventButton *event,
                            gpointer data) {
  (void)data;
  if (event->button == 1 && event->type == GDK_BUTTON_PRESS) {
    g_print("Mouse left click at (%f, %f)\n", event->x, event->y);
    CellPos c = get_cell(event->x, event->y);
    g_print("Cell: (%d, %d)\n", c.column, c.row);

    int idx = get_event(event->x, event->y);
    g_print("Event: %d\n", idx);
    events.selected = idx;

    if (events.selected == -1) {
      char *name = ask_for_string("Event Name");
      if (name != NULL) {
        add_event(name,
                  (DateTime){get_start_of_week() + c.column * 24 * 60 * 60 +
                             c.row * 60 * 60 / 2},
                  (Duration){60 * 60});
        events.selected = events.n - 1;
      }
    }

    gtk_widget_queue_draw(widget);
  }

  return TRUE;
}

gboolean handle_mouse_release(GtkWidget *widget, GdkEventButton *event,
                              gpointer data) {
  (void)data;
  (void)widget;
  if (event->button == 1 && event->type == GDK_BUTTON_RELEASE) {
    g_print("Mouse left release at (%f, %f)\n", event->x, event->y);
  }

  return TRUE;
}

gboolean handle_mouse_drag(GtkWidget *widget, GdkEventMotion *event,
                           gpointer data) {
  (void)data;
  if (events.selected != -1) {
    CellPos c = get_cell(event->x, event->y);
    int day_start_time = get_start_of_week() + c.column * 24 * 60 * 60;
    events.events[events.selected].start.epoch =
        day_start_time + c.row * 60 * 60 / 2;

    gtk_widget_queue_draw(widget);
  }

  return TRUE;
}
