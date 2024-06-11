#define _GNU_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "event.h"

extern Event *events;
extern int n_events;

char *file_contents = NULL;

char *read_file(char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("Failed to open file.\n");
    return NULL;
  }

  size_t file_size = 0;
  fseek(file, 0, SEEK_END);
  file_size = ftell(file);
  rewind(file);

  char *buffer = malloc(file_size + 1);
  fread(buffer, 1, file_size, file);
  buffer[file_size] = '\0';

  fclose(file);

  return buffer;
}

char *serialize_events_to_string() {
  char *buffer = malloc(1);
  buffer[0] = '\0';

  for (int i = 0; i < n_events; i++) {
    Event event = events[i];
    char *line = NULL;
    asprintf(&line, "%d	%d	%s\n", event.start.epoch,
             event.duration.seconds / 60, event.name);
    buffer = realloc(buffer, strlen(buffer) + strlen(line) + 1);
    strcat(buffer, line);
    free(line);
  }

  return buffer;
}

void serialize_events(char *filename) {
  char *buffer = serialize_events_to_string();
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    printf("Failed to open file.\n");
    return;
  }

  fwrite(buffer, 1, strlen(buffer), file);
  fclose(file);

  file_contents = read_file(filename);
}
