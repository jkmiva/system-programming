#include "liso.h"

/*
 * p1. echo server with I/O multiplexing, 2016/12/15
 * p2, log module, parse HTTP1.1
 * 
 * Author: Huang Jiaming
 */
const char *usage = "USAGE: ./liso <PORT> <logfile>";
int port;
char *logfile;

int main(int argc, char** argv) {

	if (argc < 3) {
		fprintf(stdout, "%s", usage);
		return EXIT_FAILURE;
	}
	
	port = atoi(argv[1]);	// server port
	logfile = argv[2];		// logfile
	int listenSock, client_sock;
    //ssize_t readret;
    socklen_t cli_size;
    struct sockaddr_in cli_addr;
    char buf[BUF_SIZE];
    
    
    log_init();
    log_record("----- Echo Server starting -----\n");
    
    listenSock = set_listen_socket(port);
    
    /* initialize client pool */
    client_pool *cp = malloc(sizeof(client_pool));
	init_pool(cp, listenSock);
    
    

    while(1) {
		cp->readFds = cp->backup;
		cp->writeFds = cp->backup;
		cp->readyNum = select(cp->maxFds1, &cp->readFds, &cp->writeFds, NULL, NULL);
		if(cp->readyNum == -1) {
			fprintf(stderr, "Failed select fd.\n");
			log_record("main: select function failed.\n");
			return EXIT_FAILURE;
		}
		int i;
		for (i = 0; i < cp->maxFds1 && cp->readyNum > 0; i++) {
			if (FD_ISSET(i, &cp->readFds)) {
				cp->readyNum -= 1;
				if (i == listenSock) {	// deal with new incoming connection
					cli_size = sizeof(cli_addr);
					if ((client_sock = accept(listenSock, (struct sockaddr *) &cli_addr,
                                 &cli_size)) == -1) {
						remove_from_pool(cp, listenSock);
						fprintf(stderr, "Error accepting connection.\n");
						log_record("main: Failed accepting new conncetion.\n");
						continue;
					}
					add_to_pool(cp, client_sock);
					fprintf(stdout, "number of clients is %d\n", cp->size);
				} else {	// deal with echo infomation
					int readret = 0;
					readret = recv(i, buf, BUF_SIZE, 0);
					if (readret == 0) {	// end of file, close the socket
						remove_from_pool(cp, i);
						fprintf(stdout, "number of clients is %d\n", cp->size);
						continue;
					}
					if (send(i, buf, readret, 0) != readret) {
						remove_from_pool(cp, i);
						fprintf(stderr, "Error sending to client.\n");
						return EXIT_FAILURE;
					}
					memset(buf, 0, BUF_SIZE);
				}
			}
		}
	}
	
	log_close();
	return 0;
}
