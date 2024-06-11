#ifndef SERIALIZATION_H
#define SERIALIZATION_H

char *serialize_events_to_string();
char *read_file(char *filename);
void serialize_events(char *filename);

#endif
