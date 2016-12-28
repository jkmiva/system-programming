
#ifndef _CLIENT_POOL_H
#define _CLIENT_POOL_H

#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "utility/general.h"
#include "log/log.h"

#define BUFSIZE 8192

typedef struct {
	int fd;
	char *addr;
	char *readBuf;
	char *writeBuf;
} client;

typedef struct {
	fd_set readFds;	// read fd set
	fd_set writeFds;	// write fd set
	fd_set backup; // backup for next loop
	int maxFds1;	// max fd plus 1
	int size;	// current fd number in set
	client arr[FD_SETSIZE];	// hold all clients
} client_pool;

void init_pool(client_pool *p, int fd);
int add_to_pool(client_pool *p, int fd, struct sockaddr_in *cli_addr);
void remove_from_pool(client_pool *p, int fd);

#endif
