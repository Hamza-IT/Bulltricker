#ifndef CLIENT_H
#define CLIENT_H

#include "net.h"

Client client;

void client_join(const char*);
void client_send_data(uint8_t*, uint16_t, NetFlag);
void close_socket();

#endif 