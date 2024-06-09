#ifndef EVENT_H
#define EVENT_H

typedef struct {
  int epoch;
} DateTime;

typedef struct {
  int seconds;
} Duration;

typedef struct Event {
  char *name;
  DateTime start;
  Duration duration;
} Event;

void add_event(char *name, DateTime start, Duration duration);

#endif
