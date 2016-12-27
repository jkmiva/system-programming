#ifndef _SOCKET_WRAPPER_H
#define _SOCKET_WRAPPER_H

#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>

#include "../log/log.h"
void Close(int sock);
int set_listen_socket(int port);
void set_fl(int fd, int flags);

#endif
