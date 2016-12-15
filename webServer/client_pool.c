
#include <stdio.h>
#include <sys/select.h>
#include "liso.h"

/*
 * typedef struct {
 *	fd_set fds;	// fd set
 *	int ndfs;	// max fd + 1
 *	int size;	// current fd number in set
 *	int arr[MAX_CLIENT];	// hold all fds
 * }client_pool;
 */
void init_pool(client_pool *p, int fd) {
	FD_ZERO(&p->fds);
	FD_ZERO(&p->backup);
	FD_SET(fd,&p->fds);
	p->backup = p->fds;	// set backup
	p->maxfds1 = fd + 1;
	p->size = 0;
	int i;
	for (i = 0; i < MAX_CLIENT; i++) {
		p->arr[i] = -1;
	}
}
void add_to_pool(client_pool *p, int fd) {
	if (p->size >= MAX_CLIENT) {
		fprintf(stderr, "Exceed MAX number of connection");
		return;
	}
	FD_SET(fd, &p->backup);
	p->fds = p->backup;
	p->arr[p->size] = fd;
	p->size++;
    if (fd >= p->maxfds1) {
		p->maxfds1 = fd + 1;
	}
}
void remove_from_pool(client_pool *p, int fd) {
	FD_CLR(fd, &p->backup);
	p->fds = p->backup;
	p->size--;
	if (fd + 1 == p->maxfds1) {
		p->maxfds1 = fd;
	}
}
