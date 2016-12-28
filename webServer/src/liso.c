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
	int listen_sock;
    
    
    log_init();
    log_record("----- Echo Server starting -----\n");
    
    listen_sock = set_listen_socket(port);
    
    /* initialize client pool */
    client_pool *cp = malloc(sizeof(client_pool));
	init_pool(cp, listen_sock);

    while(1) {
		cp->readFds = cp->backup;
		cp->writeFds = cp->backup;
		int readyNum = select(cp->maxFds1, &cp->readFds, &cp->writeFds, NULL, NULL);
		if(readyNum == -1) {
			fprintf(stderr, "Failed select fd.\n");
			log_record("main: select function failed.Quit!\n");
			return EXIT_FAILURE;	// fatal error, close program
		}
		// handle incoming connection
		if (FD_ISSET(listen_sock, &cp->readFds)) {
			handle_incoming_connection(cp);
		}
		
		handle_io(cp);
		
//		for (i = 0; i < cp->maxFds1 && cp->readyNum > 0; i++) {
//		
//	// deal with echo infomation
//		int readret = 0;
//		readret = recv(i, buf, BUF_SIZE, 0);
//		if (readret == 0) {	// end of file, close the socket
//			remove_from_pool(cp, i);
//			fprintf(stdout, "number of clients is %d\n", cp->size);
//			continue;
//		}
//		if (send(i, buf, readret, 0) != readret) {
//			remove_from_pool(cp, i);
//			fprintf(stderr, "Error sending to client.\n");
//			return EXIT_FAILURE;
//		}
//		memset(buf, 0, BUF_SIZE);
//	}
	}
	
	log_close();
	return 0;
}
