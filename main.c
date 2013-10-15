#include "structures.h"
#include "helpers.h"
#include "core.h"
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "data.h"
#include <pthread.h>

void sig_handler(int signo) {
  if (signo == SIGINT) {
	  print_debug("Interrupt signal caught: finishing connection.");
	  STATE = PROGRAM_STOP;
  }
}

void init_peers() {
	int i = 0;
	for (i = 0; i < MAX_PEERS; i++) {
		sockets[i] = -1;
	}
}

int main() {
	signal(SIGINT, sig_handler);
	
	init_peers();
	
    // Bootstrap socket to the server
    sockets[0] = getBootstrapSocket(bootstrap_ip, bootstrap_port);
    peers[0].ip = bootstrap_ip_hex;
    peers[0].port =  bootstrap_port_hex;
    peers[0].missed_pings = 0;
    peers[0].status = PEER_JOIN_PENDING;
    
    // Join request packet (just a header)
    struct P2P_join_request_packet join_request = createJoinRequest();
    
    // Send join request
    bytes_sent = send(sockets[0], (void *)&join_request, 16, 0);
    if (bytes_sent != 16) {
        print_debug("!!! Error sending join request.");
    }
    /**/print_debug("JOIN:REQ ->");
    
    pthread_t threads[1];
    int rc = pthread_create(&threads[0], NULL, sendPings, NULL);
    
    // Main program loop
    while(STATE != PROGRAM_STOP) {
		recievePacket(sockets[0]);
	}
	
    //Send bye packet!
	struct P2P_bye_packet bye = createByePacket();
	//print_header(&bye.header);
	process_to_network(&bye.header);
	int i = 0;
	for (i = 0; i < MAX_PEERS; i++) {
		if (sockets[i] != -1) {
			bytes_sent = send(sockets[i], (void *)&bye, 16, 0);
			debug_message_to("BYE", i);
		}
	}
	if (bytes_sent == 16) print_debug("Program terminated successfully. Bye.");
    return 0;
}

