#include "liso.h"

/*
 * p1. echo server with I/O multiplexing
 * 
 * Huang Jiaming, 2016/12/15
 */
const char *usage = "USAGE: ./liso <PORT>";

/*
 * from APUE figure 3.12
 * */
//void
//set_fl(int fd, int flags) /* flags are file status flags to turn on */
//{
//	int val;
//	if ((val = fcntl(fd, F_GETFL, 0)) < 0)
//		fprintf(stderr, "fcntl F_GETFL error");
//	val |= flags;
//	/* turn on flags */
//	if (fcntl(fd, F_SETFL, val) < 0)
//		fprintf(stderr, "fcntl F_SETFL error");
//}

int close_socket(int sock)
{
    if (close(sock))
    {
        fprintf(stderr, "Failed closing socket.\n");
        return 1;
    }
    return 0;
}

int main(int argc, char** argv) {

	if (argc < 2) {
		fprintf(stdout, "%s", usage);
		return EXIT_FAILURE;
	}
	
	int port = atoi(argv[1]);
	int listen_sock, client_sock;
    //ssize_t readret;
    socklen_t cli_size;
    struct sockaddr_in addr, cli_addr;
    char buf[BUF_SIZE];
    
    
    
    fprintf(stdout, "----- Echo Server -----\n");
    
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "Failed creating socket.\n");
        return EXIT_FAILURE;
	}
	
	/* bind listening socket */
	int optval = 1;
	addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR,&optval, sizeof(int));
	if (bind(listen_sock, (struct sockaddr *) &addr, sizeof(addr))) {
        close_socket(listen_sock);
        fprintf(stderr, "Failed binding socket.\n");
        return EXIT_FAILURE;
    }
    
    if (listen(listen_sock, 10)) {
        close_socket(listen_sock);
        fprintf(stderr, "Error listening on socket.\n");
        return EXIT_FAILURE;
    }
    
    /* initialize fd set */
    int avail_fd;
    client_pool cp;
	init_pool(&cp, listen_sock);
    
    

    while(1) {
		avail_fd = select(cp.maxfds1, &cp.fds, NULL, NULL, NULL);
		if(avail_fd == -1) {
			fprintf(stderr, "Failed select fd.\n");
			return EXIT_FAILURE;
		}
		int i;
		for (i = 0; i < cp.maxfds1 && avail_fd > 0; i++) {
			if (FD_ISSET(i, &cp.fds)) {
				avail_fd -= 1;
				if (i == listen_sock) {	// deal with new incoming connection
					cli_size = sizeof(cli_addr);
					if ((client_sock = accept(listen_sock, (struct sockaddr *) &cli_addr,
                                 &cli_size)) == -1) {
						close_socket(listen_sock);
						remove_from_pool(&cp, listen_sock);
						fprintf(stderr, "Error accepting connection.\n");
						continue;
					}
					add_to_pool(&cp, client_sock);
					fprintf(stdout, "number of clients is %d\n", cp.size);
				} else {	// deal with echo infomation
					int readret = 0;
					readret = recv(i, buf, BUF_SIZE, 0);
					if (readret == 0) {	// end of file, close the socket
						close_socket(i);
						remove_from_pool(&cp, i);
						fprintf(stdout, "number of clients is %d\n", cp.size);
						continue;
					}
					if (send(i, buf, readret, 0) != readret) {
						close_socket(i);
						remove_from_pool(&cp, i);
						close_socket(listen_sock);
						fprintf(stderr, "Error sending to client.\n");
						return EXIT_FAILURE;
					}
					memset(buf, 0, BUF_SIZE);				 
				}
			}
		}
		cp.fds = cp.backup;	// restore the listen fds;
	}
	return 0;
}
