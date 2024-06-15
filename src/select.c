#include <gtk/gtk.h>

#include "event.h"

extern int selected_event;
extern Event *events;
extern int n_events;

void select_next_event(GtkWidget *widget) {
  int lowerTimeBound = time(NULL);
  if (selected_event != -1) {
    lowerTimeBound = events[selected_event].start.epoch;
  }

  int lowestEventIdx = -1;
  int lowestEventTime = INT_MAX;
  for (int i = 0; i < n_events; i++) {
    Event event = events[i];
    if (event.start.epoch > lowerTimeBound &&
        event.start.epoch < lowestEventTime) {
      lowestEventTime = event.start.epoch;
      lowestEventIdx = i;
    }
  }

  if (lowestEventIdx != -1) {
    selected_event = lowestEventIdx;
  }

  gtk_widget_queue_draw(widget);
}

void select_previous_event(GtkWidget *widget) {
  int upperTimeBound = time(NULL);
  if (selected_event != -1) {
    upperTimeBound = events[selected_event].start.epoch;
  }

  int highestEventIdx = -1;
  int highestEventTime = INT_MIN;
  for (int i = 0; i < n_events; i++) {
    Event event = events[i];
    if (event.start.epoch < upperTimeBound &&
        event.start.epoch > highestEventTime) {
      highestEventTime = event.start.epoch;
      highestEventIdx = i;
    }
  }

  if (highestEventIdx != -1) {
    selected_event = highestEventIdx;
  }

  gtk_widget_queue_draw(widget);
}
