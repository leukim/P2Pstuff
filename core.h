#ifndef P2P_CORE
#define P2P_CORE

#include "structures.h"
#include "helpers.h"
#include "core_helpers.h"
#include "data.h"

void processPacket(int socket, struct P2P_h header, int id) {
	process_from_network(&header);
		
	switch (header.msg_type) {
		case MSG_PING:
			if (header.ttl == 1) {
				debug_message_from("PING:A", id);
				// Reply with PONG:A
				struct P2P_pong_A_packet reply = createPongPacket_A(header.msg_id);
				bytes_sent = send(socket, (void *)&reply, 16, 0); // MAYBE should set to nonblocking and forget about it...
				if (bytes_sent != 16) { 
					print_debug("!!! Error sending Pong.");
				} else {
					debug_message_to("PONG:A", id);
				}
			} else {
				debug_message_from("PING:B",id);
				//Reply with PONG:B
			}
			break;
		case MSG_PONG:
			debug_message_from("PONG:?", id);
			break;
		case MSG_BYE:
			debug_message_from("BYE", id);
			break;
		case MSG_JOIN:
			if (header.length == 0) { // JOIN REQUEST
				// TODO process join request
				debug_message_from("JOIN:REQ", id);
			} else if (header.length == 2) { // JOIN RESPONSE
				debug_message_from("JOIN:RES", id);
				struct P2P_join_response_packet join_response;
				
				if (getJoinResponseBody(socket, &header, &join_response) == 0) {
					if (join_response.body.status == 0x0200) {
						// Actually do sth!
						debug_message_from("Join request accepted", id);
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
			debug_message_from("QUERY", id);
			break;
		case MSG_QHIT:
			debug_message_from("QHIT", id);
			break;
		default:
			printf("!!! Unknown packet header:\n");
			print_header(&header);
			printf("Dropping packet.\n");
	}
}

void *recievePacket() {
	struct P2P_h header[MAX_PEERS];
	int i = 0;
	while (STATE != PROGRAM_STOP) {
		if (sockets[i] != -1) {
			bytes_recv = recv(sockets[i], (void *)&header[i], 16, 0);
			if (bytes_recv != -1) {
				processPacket(sockets[i], header[i], i);
			}
		}
		i += 1;
		if (i >= MAX_PEERS) i = 0;
	}
		
}

void *sendPings() {
	int i = 0;
	while (STATE != PROGRAM_STOP) {
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
