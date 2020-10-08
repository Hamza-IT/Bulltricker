#include "net.h"
#include "utilities.h"

void init_net() {
    if (SDLNet_Init() < 0) {
        log_text("\tFailed to initialize SDLNet: %s.\n", SDLNet_GetError());
        return;
    }
    next_ind = 0;
}

void tcp_open_server_socket(uint16_t port, char *host) {
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, host, port) < 0) {
        log_text("\tFailed to resolve host: %s.\n", SDLNet_GetError());
        return;
    }
    server_socket = SDLNet_TCP_Open(&ip);
    if (server_socket == NULL) {
        log_text("\tFailed to open server TCP socket: %s.\n", SDLNet_GetError());
        return;
    }
}
