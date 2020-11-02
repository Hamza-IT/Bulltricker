#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "init.h"
#include "client.h"
#include "thread.h"

TCPsocket socket;
SDLNet_SocketSet client_socket_set;

Bool client_quit;

Bool client_tcp_remove_socket(TCPsocket socket_to_remove) {
  if (SDLNet_TCP_DelSocket(client_socket_set, socket_to_remove) == -1) {
    debug_log(true, __LINE__, __FILE__, "\tClient failed to remove a socket from the socket set: %s.\n\n", SDLNet_GetError());
    return false;
  }
  return true;
}

void close_socket() {
  if (socket != NULL) {
    if (client_tcp_remove_socket(socket) == true) {
      SDLNet_TCP_Close(socket);
      SDLNet_FreeSocketSet(client_socket_set);
      socket = NULL;
      memset(&client, 0x00, sizeof(Client));
      debug_log(true, __LINE__, __FILE__, "\tClient socket closed successfully.\n\n");
    }
  }
}

void client_send_data(uint8_t *data, uint16_t length, NetFlag flag) {
  uint8_t temp_data[MAX_PACKETS];
  int offset = 0;
  memcpy(temp_data+offset, &flag, sizeof(NetFlag));
  offset += sizeof(NetFlag);
  memcpy(temp_data+offset, data, length);
  offset += length;
  int sent_count = SDLNet_TCP_Send(socket, temp_data, offset);
  if (sent_count < offset) {
    debug_log(true, __LINE__, __FILE__, "\tClient failed to send data: %s.\n\n", SDLNet_GetError());
    close_socket();
  }
}

uint8_t *client_receive_data(uint16_t *length) {
  if (socket != NULL) {
    uint8_t temp_data[MAX_PACKETS];
    int received_count = SDLNet_TCP_Recv(socket, temp_data, MAX_PACKETS);
    if (received_count <= 0) {
      close_socket();
      const char *error = SDLNet_GetError();
      if (strlen(error) == 0)
        debug_log(true, __LINE__, __FILE__, "\tHosting server has shutdown.\n\n");
      else
        debug_log(true, __LINE__, __FILE__, "\tClient failed to receive data: %s.\n\n", error);
      return NULL;
    }
    *length = received_count;
    uint8_t *data = malloc(received_count*sizeof(uint8_t));
    memcpy(data, temp_data, received_count);
    return data;
  }
  return NULL;
}

void process_data(uint8_t *data, uint16_t *offset) {
  if (data == NULL)
    return;
  NetFlag flag = *(NetFlag*) &data[*offset];
  *offset += sizeof(NetFlag);
  switch (flag) {
    case READY_FLAG:
      *offset += sizeof(uint8_t);
      SDL_LockMutex(join_mutex);
      local_ready = true;
      SDL_CondSignal(join_cond);
      SDL_UnlockMutex(join_mutex);
      break;
    case MOVE_FLAG: {
      uint8_t origin_cell = *(uint8_t*) &data[*offset];
      *offset += sizeof(uint8_t);
      uint8_t target_cell = *(uint8_t*) &data[*offset];
      *offset += sizeof(uint8_t);
      SDL_LockMutex(move_mutex);
      client.origin_cell = origin_cell;
      client.target_cell = target_cell;
      move_applied = true;
      SDL_CondSignal(move_cond);
      SDL_UnlockMutex(move_mutex);
      break;
    }
    case QUIT_FLAG:
      *offset += sizeof(uint8_t);
      SDL_LockMutex(quit_mutex);
      quit_local = true;
      close_socket();
      SDL_CondBroadcast(quit_cond);
      SDL_UnlockMutex(quit_mutex);
      client_quit = true;
      break;
  }
}

Bool client_init_net(const char *server_ip) {
  if (SDLNet_Init() < 0) {
    debug_log(true, __LINE__, __FILE__, "\tFailed to initialize SDLNet: %s.\n\n", SDLNet_GetError());
    return false;
  }
  IPaddress ip;
  if (SDLNet_ResolveHost(&ip, server_ip, PORT_NUMBER) < 0) {
    debug_log(true, __LINE__, __FILE__, "\tClient failed to resolve host: %s.\n\n", SDLNet_GetError());
    return false;
  }
  socket = SDLNet_TCP_Open(&ip);
  if (socket == NULL) {
    debug_log(true, __LINE__, __FILE__, "\tClient failed to open tcp socket: %s.\n\n", SDLNet_GetError());
    return false;
  }
  client_socket_set = SDLNet_AllocSocketSet(1);
  if (client_socket_set == NULL) {
    debug_log(true, __LINE__, __FILE__, "\tFailed to allocate memory to the client's socket set: %s.\n\n", SDLNet_GetError());
    return false;
  }
  if (SDLNet_TCP_AddSocket(client_socket_set, socket) < 0) {
    debug_log(true, __LINE__, __FILE__, "\tFailed to add client's socket to the socket set: %s.\n\n", SDLNet_GetError());
    return false;
  }
  client_quit = false;
}

Bool check_socket() {
  if (socket != NULL) {
    if (SDLNet_CheckSockets(client_socket_set, 0) == -1) {
      debug_log(true, __LINE__, __FILE__, "\tFailed to check client's socket set state: %s.\n\n", SDLNet_GetError());
      return false;
    }
    return SDLNet_SocketReady(socket);
  }
  return false;
}

void client_join(const char *server_ip) {
  if (client_init_net(server_ip) == true) {
    while (client_quit == false) {
      if (check_socket() == true) {
        uint16_t length = 0;
        uint8_t *data = client_receive_data(&length);
        if (data != NULL && length > 0) {
          debug_log(true, __LINE__, __FILE__, "\tClient received data with length: %d\n", length);
          debug_log(false, __LINE__, __FILE__, "\tData: ");
          for (int i = 0; i < length; i++)
            debug_log(false, __LINE__, __FILE__, "\t%d", data[i]);
          debug_log(false, __LINE__, __FILE__, "\n\n");
        }
        uint16_t offset = 0;
        while (offset < length)
          process_data(data, &offset);
        free(data);
      }
      if (local_ready == false && (quit == true || quit_local == true))
        break;
      SDL_Delay(1);
    }
  }
  else {
    SDL_LockMutex(join_mutex);
    local_failed = true;
    SDL_CondSignal(join_cond);
    SDL_UnlockMutex(join_mutex);
  }
  close_socket();
}
