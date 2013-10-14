#ifndef P2P_CORE_HELPERS
#define P2P_CORE_HELPERS

#include "structures.h"
#include "helpers.h"
#include "data.h"

/**
 * Return 0 on success, -1 otherwise.
 **/
int getJoinResponseBody(int socket, struct P2P_h * h, struct P2P_join_response_packet * p) {
	p->header = *h;
	bytes_recv = recv(socket, (void *)&p->body, 2, 0);
	if (bytes_recv != 2) {
		print_debug("!!! Error on recieving packet body.");
		return -1;
	}
	p->body.status = ntohs(p->body.status);
	return 0;
}

#endif
