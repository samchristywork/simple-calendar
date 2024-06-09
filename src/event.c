#include <stdlib.h>
#include <string.h>

#include "event.h"

extern Event *events;
extern int n_events;

void add_event(char *name, DateTime start, Duration duration) {
  if (events == NULL) {
    events = malloc(sizeof(Event));
    n_events = 1;
  } else {
    events = realloc(events, (n_events + 1) * sizeof(Event));
    n_events++;
  }

  Event *e = &events[n_events - 1];
  e->name = strdup(name);
  e->start = start;
  e->duration = duration;
}
