#include "socket_wrapper.h"

/*
 * set up listen socket for server
 * on success, return socket descriptor
 * on failure, return -1
 */
int set_listen_socket(int port) {
	int listenSock;
	struct sockaddr_in servAddr;
	int optval = 1;
	/* create listen socket */
	if ((listenSock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "set_listen_socket: Failed creating listen socket for server.\n");
        log_record("set_listen_socket: Failed creating listen socket for server.\n");
        return EXIT_FAILURE;
	}
	/* Eliminates "Address already in use" error from bind */
	if (setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0) {
		fprintf(stderr, "set_listen_socket: Failed set listen port reusable.\n");
		log_record("set_listen_socket: Failed set listen port reusable.\n");
		return EXIT_FAILURE;
	}
	/* bing server socket to its address */
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	servAddr.sin_addr.s_addr = INADDR_ANY;
	if ((bind(listenSock, (struct sockaddr *)&servAddr, sizeof(servAddr))) == -1) {
		Close(listenSock);
		fprintf(stderr, "set_listen_socket: Failed binding listen socket.\n");
		log_record("set_listen_socket: Failed binding listen socket.\n");
		return EXIT_FAILURE;
	}
	/* listen on listenSock */
	if (listen(listenSock, 5) == -1) {
		Close(listenSock);
		fprintf(stderr, "set_listen_socket: Failed listening on socket.\n");
		log_record("set_listen_socket: Failed listening on socket.\n");
		return EXIT_FAILURE;
	}
	return listenSock;
}



/*
 * close socket 
 */
void Close(int sock)
{
    if (close(sock)) {
        fprintf(stderr, "Failed closing socket.\n");
        log_record("Close: Failed closing socket.\n");
        exit(EXIT_FAILURE);
    }
}

/*
 * mainly for setting sockets non-blocking 
 * 
 * */
void set_fl(int fd, int flags) {
	int val;
	if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
		fprintf(stderr, "fcntl F_GETFL error");
		log_record("set_fl: fcntl F_GETFL error.\n");
	}
	val |= flags;
	/* turn on flags */
	if (fcntl(fd, F_SETFL, val) < 0) {
		fprintf(stderr, "fcntl F_SETFL error");
		log_record("set_fl: fcntl F_SETFL error.\n");
	}
}
