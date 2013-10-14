#include "structures.h"
#include "helpers.h"
#include "core.h"
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "data.h"

void sig_handler(int signo) {
  if (signo == SIGINT) {
	  print_debug("Interrupt signal caught: finishing connection.");
	  STATE = PROGRAM_STOP;
  }
}

int main() {
	signal(SIGINT, sig_handler);
	
    // Bootstrap socket to the server
    int bootstrap_socket = getBootstrapSocket(bootstrap_ip, bootstrap_port);
    
    // Join request packet (just a header)
    struct P2P_join_request_packet join_request = createJoinRequest();
    
    // Send join request
    bytes_sent = send(bootstrap_socket, (void *)&join_request, 16, 0);
    if (bytes_sent != 16) {
        print_debug("!!! Error sending join request.");
    }
    /**/print_debug("JOIN:REQ ->");
    //print_header_to_network(&join_request.header);
    
    // Main program loop
    while(STATE != PROGRAM_STOP) {
		recievePacket(bootstrap_socket);
	}
	
    //Send bye packet!
	struct P2P_bye_packet bye = createByePacket();
	//print_header(&bye.header);
	process_to_network(&bye.header);
	bytes_sent = send(bootstrap_socket, (void *)&bye, 16, 0);
	/**/print_debug("BYE ->");
	if (bytes_sent == 16) print_debug("Program terminated successfully. Bye.");
    return 0;
}

