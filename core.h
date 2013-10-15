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
				bytes_sent = send(socket, (void *)&reply, 16, 0);
				debug_message_to("PONG:A", id);
			} else {
				debug_message_from("PING:B",id);
				//Reply with PONG:B
				struct P2P_pong_B_packet reply = createPongPacket_B(header.msg_id);
				bytes_sent = send(socket, (void *)&reply, sizeof reply, 0);
				debug_message_to("PONG:B", id);
				//print_header_to_network(&reply.header);
			}
			break;
		case MSG_PONG:
			if (header.ttl == 1) {
				peers[id].missed_pings -= 1;
				char message[25];
				sprintf(&message[0], "PONG:A (%d)", peers[id].missed_pings);
				debug_message_from(&message[0], id);
			} else {
				debug_message_from("PONG:B", id);
			}
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
						peers[id].status = PEER_OK;
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
		if (sockets[i] != -1 && peers[i].status == PEER_OK) {
			struct P2P_ping_packet p = createPingPacket_A();
			bytes_sent = send(sockets[i], (void *)&p, 16, 0);
			peers[i].missed_pings += 1;
			char message[25];
			sprintf(&message[0], "PING:A (%d)", peers[i].missed_pings);
			debug_message_to(&message[0], i);
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
