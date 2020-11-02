#include "thread.h"
#include "server.h"
#include "client.h"

void init_mutexes() {
  log_mutex = SDL_CreateMutex();
  ip_mutex = SDL_CreateMutex(); join_mutex = SDL_CreateMutex(); move_mutex = SDL_CreateMutex(), quit_mutex = SDL_CreateMutex();
  ip_cond = SDL_CreateCond(); join_cond = SDL_CreateCond(); move_cond = SDL_CreateCond(); quit_cond = SDL_CreateCond();
}

void destroy_mutexes() {
  SDL_DestroyMutex(log_mutex);
  SDL_DestroyMutex(ip_mutex); SDL_DestroyMutex(join_mutex); SDL_DestroyMutex(move_mutex); SDL_DestroyMutex(quit_mutex);
  SDL_DestroyCond(ip_cond); SDL_DestroyCond(join_cond); SDL_DestroyCond(move_cond); SDL_DestroyCond(quit_cond);
  log_mutex = NULL; ip_mutex = NULL; join_mutex = NULL; move_mutex = NULL; quit_mutex = NULL;
  ip_cond = NULL; join_cond = NULL; move_cond = NULL; quit_cond = NULL;
}

int server_routine(void *arg) {
  server_start();
  return 0;
}

int client_routine(void *arg) {
  ClientData *data = (ClientData*)arg;
  client_join(data->server_ip);
  return 0;
}
