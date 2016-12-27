
#include "client_pool.h"

/*
 * typedef struct {
 *	fd_set readFds;	// read fd set
 *	fd_set writeFds;	// write fd set
 * 	fd_set backup;	// backup
 *	int maxFds1;	// max fd + 1
 *	int readyNum;	// number of ready fd
 *	int size;	// current fd number in set
 *	int arr[FD_SETSIZE];	// hold all fds
 * }client_pool;
 */
/* initial client pool and add listen socket */
void init_pool(client_pool *p, int fd) {
	FD_ZERO(&p->readFds);
	FD_ZERO(&p->writeFds);
	FD_ZERO(&p->backup);
	FD_SET(fd,&p->backup);
	p->maxFds1 = fd + 1;
	p->readyNum = 0;
	p->size = 1;
	p->arr[0] = fd;
	int i;
	for (i = 1; i < FD_SETSIZE; i++) {
		p->arr[i] = -1;
	}
}

/* add i/o socket to client pool */
void add_to_pool(client_pool *p, int fd) {
	if (p->size >= FD_SETSIZE) {
		fprintf(stderr, "add_to_pool: Failed adding client to pool.\n");
		log_record("add_to_pool: Failed adding client to pool.\n");
		return;
	}
	FD_SET(fd, &p->backup);
	p->arr[p->size] = fd;
	p->size++;
    p->maxFds1 = MAX(fd+1, p->maxFds1);
}

/* remove i/o socket from pool and close it */
void remove_from_pool(client_pool *p, int fd) {
	FD_CLR(fd, &p->backup);
	int i;
	for (i = 0; i < FD_SETSIZE; i++) {
		if (p->arr[i] == fd) {
			p->arr[i] = -1;
			break;
		}
	}
	p->size--;
	Close(fd);
	if (fd + 1 == p->maxFds1) {
		p->maxFds1 = fd;
	}
}
