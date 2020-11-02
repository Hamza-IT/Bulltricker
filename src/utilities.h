#ifndef UTILITIES_H
#define UTILITIES_H

#include <time.h>

typedef enum {
  false,
  true
} Bool;

typedef struct {
  int x;
  int y;
} IntTuple;

typedef struct {
  float x;
  float y;
} FloatTuple;

time_t START_TIME, CURRENT_TIME;

void debug_log(Bool show_call, int line, const char *file, char *, ...);
char *trim(char *);
char *pop_back(char *);
char *get_local_time();
int mirror_position(int);
char *ip_to_string(__int32);
#if defined(_WIN32) || defined(WIN32)
char *get_win_ip();
#endif

#endif
