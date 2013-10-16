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
		//print_debug("Interrupt signal caught: finishing connection.");
		//STATE = PROGRAM_STOP;
		DEBUG = 0;
		int command = 1;
		printf("\nApplication menu\n");
		printf("-------------------------------------------------\n");
		printf("1: Continue (do nothing)\n");
		printf("2: Issue query\n");
		printf("0: Quit\n");
		printf("> ");
		scanf("%d",&command);
		switch(command) {
			case 0:
				STATE = PROGRAM_STOP;
				break;
			case 2:
				printf("Enter query: ");
				char query[MAX_QUERY_LEN];
				scanf("%s", &query[0]);
				struct P2P_query_packet query_packet = createQueryPacket(&query[0]);
				int i = 0;
				for (i = 0; i < MAX_PEERS; ++i) {
					if (sockets[i] != -1 && peers[i].status == PEER_OK) {
						bytes_sent = send(sockets[i], (void *)&query_packet, 16+strlen(&query[0]), 0);
						DEBUG = 1;
						insertQueryHistory(&query_packet, MYSELF, i);
						debug_message_to("QUERY", i);
					}
				}
				break;
			case 1:
				printf("Continuing...\n");
				break;
			default:
				printf("Unknown command.");
		}
		printf("-------------------------------------------------\n");
		DEBUG = 1;
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
	
	printf("Program started. Press Ctrl-C for menu.\n");
	
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
    
    pthread_t threads[2];
    int rc = pthread_create(&threads[0], NULL, sendPings, NULL);
    int rc2 = pthread_create(&threads[1], NULL, listenConnections, NULL);
    int rc3 = pthread_create(&threads[1], NULL, recievePacket, NULL);
    
    // Main program loop
    while(STATE != PROGRAM_STOP) {
		
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

