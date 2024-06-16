#include "column.h"
#include "dialog.h"
#include "draw.h"
#include "select.h"
#include "serialization.h"

extern char *filename;

extern Events events;

extern int time_column_width;
extern int header_height;

extern int width;
extern int height;

extern int day_offset;

#define BIND(name) strcmp(binding, name) == 0

typedef struct CellPos {
  int column;
  int row;
} CellPos;

typedef struct Binding {
  guint keyval;
  char *name;
  char *description;
} Binding;

Binding bindings[] = {
    {0, "", ""},
    {0, "", "Events"},
    {GDK_KEY_o, "add-event", "Add Event"},
    {GDK_KEY_d, "delete-event", "Delete Event"},
    {GDK_KEY_n, "next-event", "Next Event"},
    {GDK_KEY_p, "previous-event", "Previous Event"},
    {GDK_KEY_c, "copy-event", "Copy Event"},
    {GDK_KEY_r, "rename-event", "Rename Event"},
    {GDK_KEY_plus, "increase-duration", "Increase Duration"},
    {GDK_KEY_minus, "decrease-duration", "Decrease Duration"},

    {0, "", ""},
    {0, "", "Movement"},
    {GDK_KEY_h, "left", "Move Event Left"},
    {GDK_KEY_j, "down", "Move Event Down"},
    {GDK_KEY_k, "up", "Move Event Up"},
    {GDK_KEY_l, "right", "Move Event Right"},
    {GDK_KEY_Left, "left", "Move Event Left"},
    {GDK_KEY_Down, "down", "Move Event Down"},
    {GDK_KEY_Up, "up", "Move Event Up"},
    {GDK_KEY_Right, "right", "Move Event Right"},
    {GDK_KEY_H, "previous-day", "Previous Day"},
    {GDK_KEY_L, "next-day", "Next Day"},

    {0, "", ""},
    {0, "", "Misc."},
    {GDK_KEY_question, "help", "Help"},
    {GDK_KEY_q, "quit", "Quit"},
    {GDK_KEY_Escape, "quit", "Quit"},
    {GDK_KEY_s, "save", "Save"},
};

CellPos get_cell(float x, float y) {
  CellPos c = {0, 0};

  if (x < time_column_width) {
    return c;
  }

  c.column = (x - time_column_width) / ((float)(width - time_column_width) / 7);
  c.row = 2 * (y - header_height) / ((float)height - header_height) * 24;

  return c;
}

char *get_binding(guint keyval) {
  int n_bindings = sizeof(bindings) / sizeof(Binding);
  for (int i = 0; i < n_bindings; i++) {
    if (bindings[i].keyval == 0) {
      continue;
    }
    if (bindings[i].keyval == keyval) {
      return strdup(bindings[i].name);
    }
  }

  return strdup("unknown");
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

void show_help_dialog() {
  GtkWidget *dialog = gtk_dialog_new_with_buttons(
      "Help", NULL, GTK_DIALOG_MODAL, "Ok", GTK_RESPONSE_OK, NULL);

  GtkGrid *grid = GTK_GRID(gtk_grid_new());

  gtk_grid_set_column_spacing(grid, 10);

  gtk_container_add(
      GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
      GTK_WIDGET(grid));

  int n_bindings = sizeof(bindings) / sizeof(Binding);
  for (int i = 0; i < n_bindings; i++) {
    Binding binding = bindings[i];
    GtkWidget *description = gtk_label_new(binding.description);
    GtkWidget *label = gtk_label_new(binding.name);
    GtkWidget *key = gtk_label_new(gdk_keyval_name(binding.keyval));

    if (binding.keyval == 0) {
      PangoAttrList *attr_list = pango_attr_list_new();
      {
        PangoAttribute *attr = pango_attr_size_new(16 * PANGO_SCALE);
        attr->start_index = 0;
        attr->end_index = strlen(binding.description);
        pango_attr_list_insert(attr_list, attr);
      }
      gtk_label_set_attributes(GTK_LABEL(description), attr_list);
    }

    gtk_label_set_xalign(GTK_LABEL(label), 0);
    gtk_label_set_xalign(GTK_LABEL(description), 0);
    gtk_label_set_xalign(GTK_LABEL(key), 0);

    gtk_grid_attach(grid, description, 0, i, 1, 1);
    gtk_grid_attach(grid, label, 1, i, 1, 1);
    gtk_grid_attach(grid, key, 2, i, 1, 1);
  }

  gtk_widget_show_all(dialog);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

gboolean handle_key(GtkWidget *widget, GdkEventKey *event, gpointer data) {
  (void)data;

  char *binding = get_binding(event->keyval);

  if (BIND("unknown")) {
    g_print("Unknown key: %s\n", gdk_keyval_name(event->keyval));
    free(binding);
    return FALSE;
  } else if (BIND("quit")) {
    // TODO: Ask if we want to save before quitting
    gtk_main_quit();
  } else if (BIND("delete-event")) {
    if (events.selected != -1) {
      free(events.events[events.selected].name);
      for (int i = events.selected; i < events.n - 1; i++) {
        events.events[i] = events.events[i + 1];
      }
      events.n--;
      events.selected = -1;
    }
  } else if (BIND("add-event")) {
    char *name = ask_for_string("Event Name");
    if (name != NULL) {
      add_event(name,
                (DateTime){
                    time(NULL) / (60 * 60) * 60 * 60,
                },
                (Duration){60 * 60});
      events.selected = events.n - 1;
    }
  } else if (BIND("save")) {
    serialize_events(filename);
  } else if (BIND("next-event")) {
    select_next_event(widget);
  } else if (BIND("previous-event")) {
    select_previous_event(widget);
  } else if (BIND("up")) {
    if (events.selected != -1) {
      events.events[events.selected].start.epoch -= 60 * 30;
    }
  } else if (BIND("down")) {
    if (events.selected != -1) {
      events.events[events.selected].start.epoch += 60 * 30;
    }
  } else if (BIND("right")) {
    if (events.selected != -1) {
      events.events[events.selected].start.epoch += 24 * 60 * 60;
    }
  } else if (BIND("left")) {
    if (events.selected != -1) {
      events.events[events.selected].start.epoch -= 24 * 60 * 60;
    }
  } else if (BIND("previous-day")) {
    day_offset--;
  } else if (BIND("next-day")) {
    day_offset++;
  } else if (BIND("copy-event")) {
    if (events.selected != -1) {
      add_event(events.events[events.selected].name,
                events.events[events.selected].start,
                events.events[events.selected].duration);
    }
  } else if (BIND("increase-duration")) {
    if (events.selected != -1) {
      events.events[events.selected].duration.seconds += 60 * 30;
    }
  } else if (BIND("decrease-duration")) {
    if (events.selected != -1) {
      events.events[events.selected].duration.seconds -= 60 * 30;
      if (events.events[events.selected].duration.seconds < 60 * 30) {
        events.events[events.selected].duration.seconds = 60 * 30;
      }
    }
  } else if (BIND("rename-event")) {
    if (events.selected != -1) {
      char *newName = ask_for_string("New Name");
      if (newName != NULL) {
        free(events.events[events.selected].name);
        events.events[events.selected].name = newName;
      }
    }
  } else if (BIND("help")) {
    show_help_dialog();
  }

  gtk_widget_queue_draw(widget);

  free(binding);
  return FALSE;
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

gboolean handle_refresh(gpointer data) {
  gtk_widget_queue_draw(GTK_WIDGET(data));
  return TRUE;
}

gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data) {
  (void)data;
  (void)widget;

  draw(cr);

  return FALSE;
}

void connect_signals(GtkWidget *window, GtkWidget *drawing_area) {
  g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(draw_callback),
                   NULL);

  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit),
                   NULL);

  g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(handle_key),
                   NULL);

  g_signal_connect(G_OBJECT(window), "button-press-event",
                   G_CALLBACK(handle_mouse_press), NULL);

  g_signal_connect(G_OBJECT(window), "button-release-event",
                   G_CALLBACK(handle_mouse_release), NULL);

  g_signal_connect(G_OBJECT(window), "motion-notify-event",
                   G_CALLBACK(handle_mouse_drag), NULL);

  g_timeout_add(1000, handle_refresh, drawing_area);
}
