#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
