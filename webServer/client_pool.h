
#ifndef _CLIENT_POOL_H
#define _CLIENT_POOL_H

#include "liso.h"

#define MAX_CLIENT 50

typedef struct {
	fd_set fds;	// fd set
	fd_set backup; // backup for next loop
	int maxfds1;	// max fd plus 1
	int size;	// current fd number in set
	int arr[MAX_CLIENT];	// hold all fds
} client_pool;

void init_pool(client_pool *p, int fd);
void add_to_pool(client_pool *p, int fd);
void remove_from_pool(client_pool *p, int fd);

#endif
