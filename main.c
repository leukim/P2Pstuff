#include "T-110_5150_reference_c_header.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <crypt.h>
#include <stdio.h>

int main() {
    //Declarations
	char * ip = "130.233.43.39";
	char * port = "8317";
	int status;
	int org_port = 12345;
	uint32_t org_ip  = 0x82E92937;
	void *buf_in;
	int flags = 0;
	//Structs
    struct addrinfo hints;
    struct addrinfo *res;  // will point to the results
    struct P2P_h header_out;
	struct P2P_h header_in;
	
	
	printf("%s\n",ip); 
	printf("%s\n",port);
	
	//Fill server info
	memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    if ((status = getaddrinfo(ip, port, &hints, &res)) != 0) {
        printf("Connection error\n");
    }
	
	//Create socket
    int sockfd;
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(sockfd, res->ai_addr, res->ai_addrlen);
	connect(sockfd, res->ai_addr, res->ai_addrlen);
	header_out.version=1;
	header_out.ttl = 5;
	header_out.msg_type = 0x03;
	header_out.reserved = 0;
	header_out.org_port = org_port;
	header_out.length = 0;
	header_out.org_ip = org_ip;
	header_out.msg_id = 0xb61d;
	
	int bytes_sent;
	bytes_sent = send(sockfd, (void *)&header_out, 0, 0);
	
	//listen(sockfd, 5);
	//accept(sockfd, (struct sockaddr *)&header_in, &addr_size);
	int bytes_recv;
	bytes_recv = recv(sockfd, (void *)&header_in, 0, flags);
	printf("Bien\n");
	printf("%d\n", header_in.msg_id);
	return 0;
}

