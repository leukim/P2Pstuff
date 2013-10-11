#include "structures.h"
#include "helpers.h"
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

int main() {
    // Control stuff...
    int bytes_sent = 0;
    int bytes_recv = 0;
    
    // Own data
    uint32_t my_ip  = 0x82E92937;
    uint16_t my_port = 12345;
    
    // Hardcoded data of the bootstrap server
    char * ip = "130.233.43.39";
    char * port = "8317";
    
    // Bootstrap socket to the server
    int bootstrap_socket = getBootstrapSocket(ip, port, my_port);
    
    // Join request packet (just a header)
    struct P2P_join_request_packet join_request = createJoinRequest(my_ip, my_port);
    
    // Send join request
    /**/printf("Join request sent:\n");
    /**/print_header(join_request.header);
    process_to_network(&join_request.header);
    bytes_sent = send(bootstrap_socket, (void *)&join_request, 16, 0);
    if (bytes_sent != 16) {
        printf("!!! Error sending join request");
    }
    
    // Wait for join response
    struct P2P_join_response_packet join_response;
    bytes_recv = recv(bootstrap_socket, (void *)&join_response, 18, 0);
    if (bytes_recv != 18) {
        printf("!!! Error getting join response");
    }
    process_from_network(&join_response.header);
    
    if (join_response.header.msg_type == MSG_JOIN) {
        printf("Join response recieved:\n");
        /**/print_header(join_response.header);
        printf("\tBODY:\t\t%x\n", join_response.body.status);
        
        struct P2P_bye_packet bye = createByePacket(my_ip, my_port);
        /**/printf("Sending bye packet\n");
        /**/print_header(bye.header);
        process_to_network(&bye.header);
        bytes_sent = send(bootstrap_socket, (void *)&bye, 16, 0);
        if (bytes_sent == 16) printf("Send OK, Bye.\n");
    }
    return 0;
}

