#include "structures.h"
#include "helpers.h"
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

int main() {
    //Declarations
    char * ip = "130.233.43.39";
    char * port = "8317";
    
    uint16_t org_port = 12345;
    uint32_t org_ip  = 0x82E92937;
    void *buf_in;
    int flags = 0x00;
    //Structs
    struct addrinfo hints;
    struct addrinfo *res;  // will point to the results
    struct P2P_h header_out;
    struct P2P_join_response_packet response;
    
    
    printf("%s\n",ip); 
    printf("%s\n",port);
    
    //Fill server info
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    int status = getaddrinfo(ip, port, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    }
    
    //Create socket
    int sockfd;
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) printf("Socket error\n");
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    connect(sockfd, res->ai_addr, res->ai_addrlen);
    header_out.version = 0x1;
    header_out.ttl = 0x5;
    header_out.msg_type = MSG_JOIN;
    header_out.reserved = 0x0;
    header_out.org_port = org_port;
    header_out.length = 0;
    header_out.org_ip = htonl(org_ip);
    header_out.msg_id = 0xb61effff;
    
    printf("Packet sent!\n");
    print_header(header_out);
    
    int bytes_sent;
    process_to_network(&header_out);
    bytes_sent = send(sockfd, (void *)&header_out, 16, 0);
    
    
    //listen(sockfd, 5);
    //accept(sockfd, (struct sockaddr *)&header_in, &addr_size);
    int bytes_recv = 0;
    bytes_recv = recv(sockfd, (void *)&response, 18, flags);
    process_from_network(&response.header);
    printf("Recieved packet!\n");
    print_header(response.header);
    printf("\tBODY:\t\t%x\n", response.status);
    if (response.header.msg_id == MSG_JOIN) printf("OK!\n");
    printf("Bytes: %d\n", bytes_recv);
    return 0;
}

