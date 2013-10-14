#ifndef P2P_CORE
#define P2P_CORE

#include "structures.h"
#include "helpers.h"
#include "core_helpers.h"
#include "data.h"

void recievePacket(int socket) {
	struct P2P_h header;
	bytes_recv = recv(socket, (void *)&header, 16, 0);
	if (bytes_recv != 16) { // TODO check basic packet integrity
		print_debug("!!! Error on recieving packet header.");
	} else {
		//print_debug("Recieved packet. Parsing...");
		
		process_from_network(&header);
		
		switch (header.msg_type) {
			case MSG_PING:
				if (header.ttl == 1) {
					print_debug("\t\t\t<- PING:A");
					// Reply with PONG:A
					struct P2P_pong_A_packet reply = createPongPacket_A(header.msg_id);
					bytes_sent = send(socket, (void *)&reply, 16, 0); // MAYBE should set to nonblocking and forget about it...
					if (bytes_sent != 16) { 
						print_debug("!!! Error sending Pong.");
					} else {
						print_debug("PONG:A ->");
					}
				} else {
					print_debug("\t\t\t<- PING:B");
					//Reply with PONG:B
				}
				break;
			case MSG_PONG:
				print_debug("\t\t\t<- PONG:?");
				break;
			case MSG_BYE:
				print_debug("\t\t\t<- BYE");
				break;
			case MSG_JOIN:
				if (header.length == 0) { // JOIN REQUEST
					// TODO process join request
					print_debug("\t\t\t<- JOIN:REQ");
				} else if (header.length == 2) { // JOIN RESPONSE
					print_debug("\t\t\t<- JOIN:RES");
					struct P2P_join_response_packet join_response;
					
					if (getJoinResponseBody(socket, &header, &join_response) == 0) {
						if (join_response.body.status == 0x0200) {
							// Actually do sth!
							print_debug("\t\t\t   Join request accepted.");
						} else {
							// Maybe do sth as well?
							print_debug("\t\t\t   Unknown status code.");
						}
					}
				} else {
					print_debug("!!! Error: Unexpected length of JOIN packet.");
				}
				break;
			case MSG_QUERY:
				print_debug("\t\t\t<- QUERY");
				break;
			case MSG_QHIT:
				print_debug("\t\t\t<- QHIT");
				break;
			default:
				printf("!!! Unknown packet header:\n");
				print_header(&header);
				printf("Dropping packet.\n");
		}
	}
}

void *sendPings() {
	int i = 0;
	while (1) {
		if (sockets[i] != -1) {
			struct P2P_ping_packet p = createPingPacket_A();
			bytes_sent = send(sockets[i], (void *)&p, 16, 0);
			//print_debug("PING:A ->");
			debug_message_to("PING:A", i);
		}
		i += 1;
		if (i >= MAX_PEERS) {
			i = 0;
			sleep(5);
		}
	} 
}

//~ void *listenConnections() {
	//~ 
//~ }

#endif
