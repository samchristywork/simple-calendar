#include <gtk/gtk.h>

#include "event.h"

extern Events events;

void select_next_event(GtkWidget *widget) {
  int lowerTimeBound = time(NULL);
  if (events.selected != -1) {
    lowerTimeBound = events.events[events.selected].start.epoch;
  }

  int lowestEventIdx = -1;
  int lowestEventTime = INT_MAX;
  for (int i = 0; i < events.n; i++) {
    Event event = events.events[i];
    if (event.start.epoch > lowerTimeBound &&
        event.start.epoch < lowestEventTime) {
      lowestEventTime = event.start.epoch;
      lowestEventIdx = i;
    }
  }

  if (lowestEventIdx != -1) {
    events.selected = lowestEventIdx;
  }

  gtk_widget_queue_draw(widget);
}

void select_previous_event(GtkWidget *widget) {
  int upperTimeBound = time(NULL);
  if (events.selected != -1) {
    upperTimeBound = events.events[events.selected].start.epoch;
  }

  int highestEventIdx = -1;
  int highestEventTime = INT_MIN;
  for (int i = 0; i < events.n; i++) {
    Event event = events.events[i];
    if (event.start.epoch < upperTimeBound &&
        event.start.epoch > highestEventTime) {
      highestEventTime = event.start.epoch;
      highestEventIdx = i;
    }
  }

  if (highestEventIdx != -1) {
    events.selected = highestEventIdx;
  }

  gtk_widget_queue_draw(widget);
}
