#ifndef NET_H
#define NET_H

#include <SDL2/SDL_net.h>

typedef struct {
  Bool in_use;
  uint8_t origin_cell;
  uint8_t target_cell;
} Client;

typedef enum {
  QUIT_FLAG,
  READY_FLAG,
  MOVE_FLAG
} NetFlag;

static const uint8_t MAX_PACKETS = 0xff;
static const uint16_t PORT_NUMBER = 4531;

#endif
