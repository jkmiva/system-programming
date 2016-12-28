#include "handle_conn.h"

void handle_incoming_connection(client_pool *cp) {
	int listen_sock = cp->arr[0].fd;
	int cli_sock;
	struct sockaddr_in cli_addr;
	socklen_t len = sizeof(cli_addr);
	if ((cli_sock = accept(listen_sock, (struct sockaddr *) &cli_addr, &len)) == -1) {
		fprintf(stderr, "Error accepting connection.\n");
		log_record("handle_incoming_connection: Failed accepting new incoming client.\n");
		return;
	}
	if (add_to_pool(cp, cli_sock, &cli_addr) == 0) {
		log_record("new connection from %s\n", cp->arr[cp->size].addr);
	}
}

void handle_io(client_pool *cp) {
	int i, fd;
	for (i = 1; i < cp->size; i++) {
		fd = cp->arr[i].fd;
		if (FD_ISSET(fd, &cp->readFds)) {
			handle_i(cp, i);
		}
		if (FD_ISSET(fd, &cp->writeFds)) {
			handle_o(cp, i);
		}
	}
}

void handle_i(client_pool *cp, int i) {
}

void handle_o(client_pool *cp, int i) {
}
