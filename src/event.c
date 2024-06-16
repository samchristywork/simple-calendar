#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "event.h"

Events events = {
    .events = NULL, .n = 0, .selected = -1, .start_hour = 0, .end_hour = 24};

void add_event(char *name, DateTime start, Duration duration) {
  if (events.events == NULL) {
    events.events = malloc(sizeof(Event));
    events.n = 1;
  } else {
    events.events = realloc(events.events, (events.n + 1) * sizeof(Event));
    events.n++;
  }

  Event *e = &events.events[events.n - 1];
  e->name = strdup(name);
  e->start = start;
  e->duration = duration;
}
