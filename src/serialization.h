#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <stdbool.h>

char *serialize_events_to_string();
char *read_file(char *filename);
void serialize_events(char *filename);
bool check_modified();
void deserialize_events(char *filename);

#endif
