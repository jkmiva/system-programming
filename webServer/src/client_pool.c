
#include "client_pool.h"

/*
 * typedef struct {
 *	fd_set readFds;	// read fd set
 *	fd_set writeFds;	// write fd set
 * 	fd_set backup;	// backup
 *	int maxFds1;	// max fd + 1
 *	int size;	// current fd number in set
 *	client arr[FD_SETSIZE];	// hold all fds, arr[0] is listen_sock
 * }client_pool;
 */
/* initial client pool and add listen socket */
void init_pool(client_pool *p, int fd) {
	FD_ZERO(&p->readFds);
	FD_ZERO(&p->writeFds);
	FD_ZERO(&p->backup);
	FD_SET(fd,&p->backup);
	p->maxFds1 = fd + 1;
	p->size = 1;
	p->arr[0].fd = fd;
	int i;
	for (i = 1; i < FD_SETSIZE; i++) {
		p->arr[i].fd = -1;
	}
}

/* add i/o socket to client pool */
int add_to_pool(client_pool *p, int fd, struct sockaddr_in *cli_addr) {
	if (p->size >= FD_SETSIZE) {
		fprintf(stderr, "add_to_pool: Failed adding client to pool.\n");
		log_record("add_to_pool: Failed adding client to pool.\n");
		return -1;
	}
	FD_SET(fd, &p->backup);
    p->maxFds1 = MAX(fd+1, p->maxFds1);
    p->arr[p->size].fd = fd;
    char *addr_buf = malloc(sizeof(char) * INET_ADDRSTRLEN);
	if (inet_ntop(AF_INET, &(cli_addr->sin_addr), addr_buf, INET_ADDRSTRLEN) == NULL) {
		fprintf(stderr, "Failed allocting space for client address.\n");
		log_record("Failed allocting space for client address.\n");
		return -1;
	}
	p->arr[p->size].addr = addr_buf;
	p->size++;
	return 0;
}

/* remove i/o socket from pool and close it */
void remove_from_pool(client_pool *p, int fd) {
	FD_CLR(fd, &p->backup);
	int i;
	for (i = 0; i < FD_SETSIZE; i++) {
		if (p->arr[i].fd == fd) {
			p->arr[i].fd = -1;
			free(p->arr[i].addr);
			break;
		}
	}
	p->size--;
	Close(fd);
	if (fd + 1 == p->maxFds1) {
		p->maxFds1 = fd;
	}
}
