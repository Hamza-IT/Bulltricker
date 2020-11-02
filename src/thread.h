#ifndef THREAD_H
#define THREAD_H

#include "utilities.h"
#include <SDL_thread.h>
#include <SDL_mutex.h>

typedef struct {
  char *server_ip;
} ClientData;

SDL_mutex *log_mutex, *ip_mutex, *join_mutex, *move_mutex, *quit_mutex;
SDL_cond *ip_cond, *join_cond, *move_cond, *quit_cond;

void init_mutexes();
void destroy_mutexes();
int server_routine(void *arg);
int client_routine(void *arg);

#endif 