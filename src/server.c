#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "init.h"
#include "net.h"
#include "thread.h"
#include "server.h"

int cc;
const uint8_t MAX_SOCKETS = 0x02;

TCPsocket server_socket;
Client *clients;
SDLNet_SocketSet socket_set;
TCPsocket *sockets;

Bool server_quit;

Bool tcp_add_socket(TCPsocket socket_to_add) {
  if (cc >= MAX_SOCKETS) {
    debug_log(true, __LINE__, __FILE__, "\tFailed to add socket to the socket set: maximum capacity attained.\n\n");
    return false;
  }
  else if (SDLNet_TCP_AddSocket(socket_set, socket_to_add) < 0) {
    debug_log(true, __LINE__, __FILE__, "\tFailed to add socket to the socket set: %s.\n\n", SDLNet_GetError());
    return false;
  }
  return true;
}

Bool tcp_remove_socket(TCPsocket socket_to_remove) {
  if (SDLNet_TCP_DelSocket(socket_set, socket_to_remove) == -1) {
    debug_log(true, __LINE__, __FILE__, "\tServer failed to remove socket from the socket set: %s.\n\n", SDLNet_GetError());
    return false;
  }
  return true;
}

Bool init_net() {
  if (SDLNet_Init() < 0) {
    debug_log(true, __LINE__, __FILE__, "\tFailed to initialize SDLNet: %s.\n\n", SDLNet_GetError());
    return false;
  }
  clients = malloc(sizeof(Client)*MAX_SOCKETS);
  sockets = malloc(sizeof(TCPsocket)*MAX_SOCKETS);
  socket_set = SDLNet_AllocSocketSet(MAX_SOCKETS+1);
  if (socket_set == NULL) {
    debug_log(true, __LINE__, __FILE__, "\tFailed to allocate memory to the socket set: %s.\n\n", SDLNet_GetError());
    return false;
  }
  cc = 0;
  server_quit = false;
  return true;
}

Bool tcp_add_server_socket(const char *host) {
  IPaddress ip;
  SDL_LockMutex(ip_mutex);
  if (SDLNet_ResolveHost(&ip, host, PORT_NUMBER) < 0) {
    debug_log(true, __LINE__, __FILE__, "\tFailed to resolve host: %s.\n\n", SDLNet_GetError());
    return false;
  }
  server_socket = SDLNet_TCP_Open(&ip);
  ip_updated = true;
  SDL_CondSignal(ip_cond);
  SDL_UnlockMutex(ip_mutex);
  if (server_socket == NULL) {
    debug_log(true, __LINE__, __FILE__, "\tFailed to open server TCP socket: %s.\n\n", SDLNet_GetError());
    return false;
  }
  return tcp_add_socket(server_socket);
}

int check_sockets(int interval) {
  return SDLNet_CheckSockets(socket_set, interval);
}

int accept_socket(int index) {
  if (index < MAX_SOCKETS && cc < MAX_SOCKETS) {
    sockets[index] = SDLNet_TCP_Accept(server_socket);
    if (sockets[index] == NULL)
      return 0;
    if (tcp_add_socket(sockets[index]) == false)
      return -1;
    clients[index].in_use = true;
    return 1;
  }
  return -2;
}

void tcp_close_socket(int index) {
  if (sockets[index] == NULL) {
    debug_log(true, __LINE__, __FILE__, "\tAttempted to delete a NULL socket.\n\n");
    return;
  }
  else if (tcp_remove_socket(sockets[index]) == true) {
    memset(&clients[index], 0x00, sizeof(Client));
    SDLNet_TCP_Close(sockets[index]);
    sockets[index] = NULL;
  }
}

void send_data(int index, uint8_t *data, uint16_t length, NetFlag flag) {
  uint8_t temp_data[MAX_PACKETS];
  int offset = 0;
  memcpy(temp_data+offset, &flag, sizeof(NetFlag));
  offset += sizeof(NetFlag);
  memcpy(temp_data+offset, data, length);
  offset += length;
  int sent_count = SDLNet_TCP_Send(sockets[index], temp_data, offset);
  if (sent_count <= 0) {
    debug_log(true, __LINE__, __FILE__, "\tServer failed to send data: %s.\n\n", SDLNet_GetError());
    tcp_close_socket(index);
  }
  else if (flag == QUIT_FLAG)
    server_quit = true;
}

void broadcast(int index, uint8_t *data, uint16_t length, NetFlag flag) {
  for (int i = 0; i < cc; i++) {
    if (i != index)
      send_data(i, data, length, flag);
  }
}

uint8_t *receive_data(int index, uint16_t *length) {
  uint8_t temp_data[MAX_PACKETS];
  int received_count = SDLNet_TCP_Recv(sockets[index], temp_data, MAX_PACKETS);
  if (received_count <= 0) {
    tcp_close_socket(index);
    const char *error = SDLNet_GetError();
    if (strlen(error) == 0)
      debug_log(true, __LINE__, __FILE__, "\tClient with id %d has disconnected.\n\n", index+1);
    else
      debug_log(true, __LINE__, __FILE__, "\tServer failed to receive data: %s.\n\n", error);
    return NULL;
  }
  *length = received_count;
  uint8_t *data = malloc(received_count*sizeof(uint8_t));
  memcpy(data, temp_data, received_count);
  return data;
}

void process_client_data(int index, uint8_t *data, uint16_t *offset) {
  if (data != NULL) {
    NetFlag flag = *(NetFlag*) &data[*offset];
    *offset += sizeof(NetFlag);
    switch (flag) {
      case MOVE_FLAG: {
        uint16_t send_offset = 0;
        uint8_t broadcast_data[MAX_PACKETS];
        memcpy(broadcast_data+send_offset, data+*offset+send_offset, sizeof(uint8_t));
        send_offset += sizeof(uint8_t);
        memcpy(broadcast_data+send_offset, data+*offset+send_offset, sizeof(uint8_t));
        send_offset += sizeof(uint8_t);
        *offset += send_offset;
        broadcast(index, broadcast_data, send_offset, MOVE_FLAG);
        break;
      }
      case QUIT_FLAG: {
        debug_log(true, __LINE__, __FILE__, "\tClient with id %d has quit.\n\n", index+1);
        broadcast(-1, data+*offset, sizeof(uint8_t), QUIT_FLAG);
        *offset += sizeof(uint8_t);
        break;
      }
    }
  }
}

void process_sockets(int *ready_count) {
  for (int index = 0; index < MAX_SOCKETS && *ready_count > 0; index++) {
    if (sockets[index] != NULL && SDLNet_SocketReady(sockets[index])) {
      uint16_t length = 0;
      uint8_t *data = receive_data(index, &length);
      if (length > 0 && data != NULL) {
        debug_log(true, __LINE__, __FILE__, "\tServer received data with length: %d\n", length);
        debug_log(false, __LINE__, __FILE__, "\tData: ");
        for (int i = 0; i < length; i++)
          debug_log(false, __LINE__, __FILE__, "\t%d", data[i]);
        debug_log(false, __LINE__, __FILE__, "\n\n");
      }
      if (data == NULL)
        (*ready_count)--;
      else {
        int processed_count = 0;
        uint16_t offset = 0;
        while (offset < length) {
          process_client_data(index, data, &offset);
          processed_count++;
        }
        debug_log(true, __LINE__, __FILE__, "\t%d proccessed socket(s) for client with id: %d.\n\n", processed_count, index+1);
        free(data);
        (*ready_count)--;
      }
    }
  }
}

void server_start() {
  if (init_net() == true) {
    char *host = NULL;
    if (tcp_add_server_socket(host) == false)
      return;
    int next_index = 0;
    while (server_quit == false) {
      int ready_count = check_sockets(0);
      if (ready_count > 0) {
        if (SDLNet_SocketReady(server_socket)) {
          int got_socket = accept_socket(next_index);
          if (got_socket <= 0)
            ready_count--;
          else {
            debug_log(true, __LINE__, __FILE__, "\tServer has detected a new connection. Client Id: %d.\n\n", ++cc);
            if (cc == MAX_SOCKETS) {
              uint8_t data[] = { 0 };
              broadcast(-1, data, sizeof(uint8_t), READY_FLAG);
            }
            ready_count--;
          }
        }
        process_sockets(&ready_count);
        next_index = (next_index+1)%MAX_SOCKETS;
      }
      if (local_ready == false && (quit == true || quit_local == true))
        break;
      SDL_Delay(1);
    }
    close_net();
    SDL_LockMutex(quit_mutex);
    quit_ready = true;
    SDL_CondSignal(quit_cond);
    SDL_UnlockMutex(quit_mutex);
  }
}

void tcp_close_sockets() {
  SDLNet_TCP_Close(server_socket);
  for (int i = 0; i < cc; i++) {
    if (sockets[i] != NULL)
      tcp_close_socket(i);
  }
}

void close_net() {
  if (sockets != NULL) {
    tcp_close_sockets();
    SDLNet_FreeSocketSet(socket_set);
    SDLNet_Quit();
    free(clients); free(sockets);
    cc = 0;
    clients = NULL; sockets = NULL;
    debug_log(true, __LINE__, __FILE__, "\tServer network closed successfully.\n\n");
  }
}
