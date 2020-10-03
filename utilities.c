#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "utilities.h"

char* LOG_PATH = "./Bulltricker_Data/Logs/";

void log_text(char *format, ...) {
  char *path = malloc(100);
  char *time = malloc(100);
  itoa(START_TIME, time, 10);
  char *name = malloc(105);
  name = strcpy(name, "Game_");
  strcat(name, time); strcat(name, ".log");
  FILE *fptr = fopen(strcat(strcpy(path, LOG_PATH), name), "a");
  va_list args;
  va_start(args, format);
  vfprintf(fptr, format, args);
  va_end(args);
  fclose(fptr);
  free(path); free(time); free(name);
}

char *trim(char *s) {
  char *ptr;
  if (!s)
    return NULL;
  if (!*s)
    return s;
  for (ptr = s + strlen(s) - 1; (ptr >= s) && isspace(*ptr); --ptr);
  ptr[1] = '\0';
  return s;
}