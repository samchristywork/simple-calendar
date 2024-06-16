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

typedef struct Events {
  Event *events;
  int n;
  int selected;
  float start_hour;
  float end_hour;
} Events;

void add_event(char *name, DateTime start, Duration duration);

#endif
