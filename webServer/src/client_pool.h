
#ifndef _CLIENT_POOL_H
#define _CLIENT_POOL_H

#include <stdio.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#include "utility/general.h"
#include "log/log.h"


typedef struct {
	fd_set readFds;	// read fd set
	fd_set writeFds;	// write fd set
	fd_set backup; // backup for next loop
	int maxFds1;	// max fd plus 1
	int readyNum;	// number of ready fd
	int size;	// current fd number in set
	int arr[FD_SETSIZE];	// hold all fds
} client_pool;

void init_pool(client_pool *p, int fd);
void add_to_pool(client_pool *p, int fd);
void remove_from_pool(client_pool *p, int fd);

#endif
