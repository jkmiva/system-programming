#ifndef _HANDLE_CONN_H_
#define _HANDLE_CONN_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>

#include "client_pool.h"

void handle_incoming_connection(client_pool *cp);
void handle_io(client_pool *cp);
void handle_i(client_pool *cp, int i);
void handle_o(client_pool *cp, int i);

#endif
