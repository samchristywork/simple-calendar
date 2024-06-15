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

extern int day_offset;

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
    {GDK_KEY_q, "quit", "Quit"},
    {GDK_KEY_Escape, "quit", "Quit"},
    {GDK_KEY_d, "delete-event", "Delete Event"},
    {GDK_KEY_o, "add-event", "Add Event"},
    {GDK_KEY_s, "save", "Save"},
    {GDK_KEY_n, "next-event", "Next Event"},
    {GDK_KEY_p, "previous-event", "Previous Event"},
    {GDK_KEY_h, "left", "Move Event Left"},
    {GDK_KEY_j, "down", "Move Event Down"},
    {GDK_KEY_k, "up", "Move Event Up"},
    {GDK_KEY_l, "right", "Move Event Right"},
    {GDK_KEY_Left, "left", "Move Event Left"},
    {GDK_KEY_Down, "down", "Move Event Down"},
    {GDK_KEY_Up, "up", "Move Event Up"},
    {GDK_KEY_Right, "right", "Move Event Right"},
    {GDK_KEY_H, "previous-week", "Previous Week"},
    {GDK_KEY_L, "next-week", "Next Week"},
    {GDK_KEY_c, "copy-event", "Copy Event"},
    {GDK_KEY_plus, "increase-duration", "Increase Duration"},
    {GDK_KEY_minus, "decrease-duration", "Decrease Duration"},
    {GDK_KEY_r, "rename-event", "Rename Event"},
    {GDK_KEY_question, "help", "Help"},
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
    if (bindings[i].keyval == keyval) {
      return strdup(bindings[i].name);
    }
  }

  return strdup("unknown");
}

int get_event(float x, float y) {
  float column_width = (float)(width - time_column_width) / 7;
  CellPos c = get_cell(x, y);
  for (int i = 0; i < n_events; i++) {
    Event event = events[i];
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

void show_help_menu() {
  GtkWidget *dialog = gtk_dialog_new_with_buttons(
      "Help", NULL, GTK_DIALOG_MODAL, "Ok", GTK_RESPONSE_OK, NULL);

  GtkGrid *grid = GTK_GRID(gtk_grid_new());

  gtk_container_add(
      GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
      GTK_WIDGET(grid));

  GtkWidget *description_label = gtk_label_new("Key");
  GtkWidget *name_label = gtk_label_new("Name");
  GtkWidget *key_label = gtk_label_new("Key");

  gtk_label_set_xalign(GTK_LABEL(description_label), 0);
  gtk_label_set_xalign(GTK_LABEL(name_label), 0);
  gtk_label_set_xalign(GTK_LABEL(key_label), 1);

  PangoAttrList *attr_list = pango_attr_list_new();
  PangoAttribute *attr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
  attr->start_index = 0;
  attr->end_index = 3;

  pango_attr_list_insert(attr_list, attr);
  gtk_label_set_attributes(GTK_LABEL(description_label), attr_list);
  gtk_label_set_attributes(GTK_LABEL(name_label), attr_list);
  gtk_label_set_attributes(GTK_LABEL(key_label), attr_list);

  gtk_grid_attach(grid, description_label, 0, 0, 1, 1);
  gtk_grid_attach(grid, name_label, 1, 0, 1, 1);
  gtk_grid_attach(grid, key_label, 2, 0, 1, 1);

  int n_bindings = sizeof(bindings) / sizeof(Binding);
  for (int i = 0; i < n_bindings; i++) {
    Binding binding = bindings[i];
    GtkWidget *description = gtk_label_new(binding.description);
    GtkWidget *label = gtk_label_new(binding.name);
    GtkWidget *key = gtk_label_new(gdk_keyval_name(binding.keyval));

    gtk_label_set_xalign(GTK_LABEL(label), 0);
    gtk_label_set_xalign(GTK_LABEL(description), 0);
    gtk_label_set_xalign(GTK_LABEL(key), 1);

    gtk_grid_attach(grid, description, 0, i + 1, 1, 1);
    gtk_grid_attach(grid, label, 1, i + 1, 1, 1);
    gtk_grid_attach(grid, key, 2, i + 1, 1, 1);
  }

  gtk_widget_show_all(dialog);
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
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
      selected_event = -1;
    }
    gtk_widget_queue_draw(widget);
  } else if (event->keyval == GDK_KEY_o) {
    char *name = ask_for_string("Event Name");
    if (name != NULL) {
      add_event(name,
                (DateTime){
                    time(NULL) / (60 * 60) * 60 * 60,
                },
                (Duration){60 * 60});
      selected_event = n_events - 1;
    }
  } else if (event->keyval == GDK_KEY_s) {
    serialize_events(filename);
    gtk_widget_queue_draw(widget);
  } else if (event->keyval == GDK_KEY_n) {
    select_next_event(widget);
  } else if (event->keyval == GDK_KEY_p) {
    select_previous_event(widget);
  } else if (event->keyval == GDK_KEY_Up || event->keyval == GDK_KEY_k) {
    if (selected_event != -1) {
      events[selected_event].start.epoch -= 60 * 30;
      gtk_widget_queue_draw(widget);
    }
  } else if (event->keyval == GDK_KEY_Down || event->keyval == GDK_KEY_j) {
    if (selected_event != -1) {
      events[selected_event].start.epoch += 60 * 30;
      gtk_widget_queue_draw(widget);
    }
  } else if (event->keyval == GDK_KEY_Right || event->keyval == GDK_KEY_l) {
    if (selected_event != -1) {
      events[selected_event].start.epoch += 24 * 60 * 60;
      gtk_widget_queue_draw(widget);
    }
  } else if (event->keyval == GDK_KEY_Left || event->keyval == GDK_KEY_h) {
    if (selected_event != -1) {
      events[selected_event].start.epoch -= 24 * 60 * 60;
      gtk_widget_queue_draw(widget);
    }
  } else if (event->keyval == GDK_KEY_H) {
    day_offset--;
    gtk_widget_queue_draw(widget);
  } else if (event->keyval == GDK_KEY_L) {
    day_offset++;
    gtk_widget_queue_draw(widget);
  } else if (event->keyval == GDK_KEY_c) {
    if (selected_event != -1) {
      add_event(events[selected_event].name, events[selected_event].start,
                events[selected_event].duration);
    }
  } else if (event->keyval == GDK_KEY_plus) {
    if (selected_event != -1) {
      events[selected_event].duration.seconds += 60 * 30;
      gtk_widget_queue_draw(widget);
    }
  } else if (event->keyval == GDK_KEY_minus) {
    if (selected_event != -1) {
      events[selected_event].duration.seconds -= 60 * 30;
      if (events[selected_event].duration.seconds < 60 * 30) {
        events[selected_event].duration.seconds = 60 * 30;
      }
      gtk_widget_queue_draw(widget);
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
  } else if (event->keyval == GDK_KEY_question) {
    show_help_menu();
  }

  return FALSE;
}

gboolean handle_mouse_press(GtkWidget *widget, GdkEventButton *event,
                            gpointer data) {
  (void)data;
  if (event->button == 1 && event->type == GDK_BUTTON_PRESS) {
    g_print("Mouse left click at (%f, %f)\n", event->x, event->y);
    CellPos c = get_cell(event->x, event->y);
    g_print("Cell: (%d, %d)\n", c.column, c.row);

    int EventIdx = get_event(event->x, event->y);
    g_print("Event: %d\n", EventIdx);
    selected_event = EventIdx;

    if (selected_event == -1) {
      char *name = ask_for_string("Event Name");
      if (name != NULL) {
        add_event(name,
                  (DateTime){get_start_of_week() + c.column * 24 * 60 * 60 +
                             c.row * 60 * 60 / 2},
                  (Duration){60 * 60});
        selected_event = n_events - 1;
      }
    }

    gtk_widget_queue_draw(widget);
  }

}
