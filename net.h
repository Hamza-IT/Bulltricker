#ifndef NET_H
#define NET_H

#include <SDL2/SDL_net.h>

uint8_t const MAX_POCKET = 0xff;
uint8_t const MAX_SOCKET = 0x02;

int next_ind;
TCPsocket server_socket;
Client clients[MAX_SOCKETS];
SDLNet_SocketSet socket_set;
TCPsocket sockets[MAX_SOCKETS];

#endif 