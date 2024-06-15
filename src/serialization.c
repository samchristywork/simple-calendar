#define _GNU_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "event.h"

extern Event *events;
extern int n_events;

char *file_contents = NULL;

bool file_exists(char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    return false;
  }

  fclose(file);
  return true;
}

void make_file(char *filename) {
  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    printf("Failed to create file.\n");
    return;
  }

  fclose(file);
}

char *read_file(char *filename) {
  if (!file_exists(filename)) {
    make_file(filename);
  }

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

bool check_modified() {
  char *new_contents = serialize_events_to_string();
  bool modified = strcmp(file_contents, new_contents) != 0;
  free(new_contents);
  return modified;
}

void deserialize_events(char *filename) {
  file_contents = read_file(filename);

  if (file_contents == NULL) {
    return;
  }

  FILE *file = fopen(filename, "r");
  if (file != NULL) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, file)) != -1) {
      char *start = strtok(line, "	");
      char *duration = strtok(NULL, "	");
      char *name = strtok(NULL, "	");
      name[strlen(name) - 1] = '\0';

      DateTime dt;
      dt.epoch = atoi(start);

      Duration d;
      d.seconds = atoi(duration) * 60;

      add_event(name, dt, d);
    }

    free(line);
    fclose(file);
  }
}
