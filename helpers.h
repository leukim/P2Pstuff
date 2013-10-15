#ifndef P2P_HELPERS
#define P2P_HELPERS

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "structures.h"
#include "data.h"
#include <sys/fcntl.h>

void print_debug(const char * msg) {
	if (DEBUG) {
		printf("DBG: ");
		printf("%s", msg);
		printf("\n");
	}
}

void debug_message_to(const char * msg, int socket) {
	if (DEBUG) {
		printf("DBG: %s --> %d\n", msg, socket);
	}
}

void debug_message_from(const char * msg, int socket) {
	if (DEBUG) {
		printf("DBG:\t\t\t%s <-- %d\n", msg, socket);
	}
}

void ip_hex_to_dec(uint32_t * ip, int * b1, int * b2, int * b3, int * b4) {
    *b1 = (int) (*ip & 0xff);
    *b2 = (int) ((*ip >> 8) & 0xff);
    *b3 = (int) ((*ip >> 16) & 0xff);
    *b4 = (int) ((*ip >> 24) & 0xff);
}

void print_header_inner(struct P2P_h * h, int complete) {
	if (complete == 0) {
		printf("\tMSG:\t\t0x0%x\n", h->msg_type);
		printf("\tLEN:\t\t%d\n", h->length);
		printf("\tMSGID:\t\t0x%x\n", h->msg_id);
	} else {
		int b1, b2, b3, b4;
		ip_hex_to_dec(&h->org_ip, &b1, &b2, &b3, &b4);
		printf("\tVersion:\t%x\n", h->version);
		printf("\tTTL:\t\t%x\n", h->ttl);
		printf("\tMSG:\t\t0x0%x\n", h->msg_type);
		printf("\tRES:\t\t%x\n", h->reserved);
		printf("\tSND PORT:\t%d\n", h->org_port);
		printf("\tLEN:\t\t%d\n", h->length);
		printf("\tIP:\t\t%d.%d.%d.%d (0x%x)\n", b1, b2, b3, b4, h->org_ip);
		printf("\tMSGID:\t\t0x%x\n", h->msg_id);
	}
}

void print_header(struct P2P_h * h) {
	print_header_inner(h, 1);
}

void print_header_simple(struct P2P_h *	 h) {
	print_header_inner(h, 0);
}

void process_from_network(struct P2P_h * h) {
    h->org_port = ntohs(h->org_port);
    h->length = ntohs(h->length);
    h->org_ip = ntohl(h->org_ip);
    h->msg_id = ntohl(h->msg_id);
}

void process_to_network(struct P2P_h * h) {
    h->org_port = htons(h->org_port);
    h->length = htons(h->length);
    h->org_ip = htonl(h->org_ip);
    h->msg_id = htonl(h->msg_id);
}

void print_header_to_network(struct P2P_h * h) {
	process_from_network(h);
	print_header_inner(h, 1);
	process_to_network(h);
}

uint32_t getMessageID() {
    uint32_t temp = (uint32_t)my_port;
    CURRENT_MSG += 1;
    return ((my_ip ^ my_port) ^ (temp << 16)) + CURRENT_MSG;
}

int newPeerID() {
	int i = 0;
	for(i = 0; i < MAX_PEERS; ++i) {
		if (sockets[i] == -1) return i;
	}
	return -1;
}

//*************************************************************
//                                                            *
//                      BOOTSTRAPPING                         *
//                                                            *
//*************************************************************

int getBootstrapSocket(char * ip, char * port) {
    struct addrinfo hints;
    struct addrinfo *res;  // will point to the results
    
    printf("Querying for %s:%s...\n",ip, port); 
    
    //Fill server info
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    int status = getaddrinfo(ip, port, &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return -1;
    }
    
    //Create socket
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) printf("Socket error\n");
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    connect(sockfd, res->ai_addr, res->ai_addrlen);
    
    unsigned int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags|O_NDELAY);
    
    return sockfd;
}

int getListenSocket() {
	struct addrinfo hints;
    struct addrinfo *res;  // will point to the results
    
    //Fill server info
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    int status = getaddrinfo(NULL, "12345", &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return -1;
    }
    
    //Create socket
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) printf("Socket error\n");
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    
    unsigned int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags|O_NDELAY);
    
    return sockfd;
}

//*************************************************************
//                                                            *
//                PACKET FORGERY FUNCTIONS                    *
//                                                            *
//*************************************************************

struct P2P_h getHeader(uint32_t org_ip, uint16_t org_port, uint8_t ttl, uint8_t msg_type, uint32_t msgid, uint16_t len) {
    struct P2P_h h;
    h.version = 0x1;
    h.ttl = ttl;
    h.msg_type = msg_type;
    h.reserved = 0x0;
    h.org_port = htons(org_port);
    h.length = htons(len);
    h.org_ip = htonl(org_ip);
    h.msg_id = htonl(msgid);
    return h;
}

////////////////////////////////////////////////////////////////////////

void fillPongBody(struct P2P_pong_B_packet * p) {
	// TODO For now we assume no neighours
	memset(&p->body, 0, sizeof p->body);
	// Set header length!
	p->header.length = htons(4); // For now, because it is empty.
}

void processPongBodyToNetwork(struct P2P_pong_B_packet * p) {
	p->body.front.entry_size = htons(p->body.front.entry_size);
	p->body.e1.ip = htonl(p->body.e1.ip);
	p->body.e2.ip = htonl(p->body.e2.ip);
	p->body.e3.ip = htonl(p->body.e3.ip);
	p->body.e4.ip = htonl(p->body.e4.ip);
	p->body.e5.ip = htonl(p->body.e5.ip);
	p->body.e1.port = htons(p->body.e1.port);
	p->body.e2.port = htons(p->body.e2.port);
	p->body.e3.port = htons(p->body.e3.port);
	p->body.e4.port = htons(p->body.e4.port);
	p->body.e5.port = htons(p->body.e5.port);
}

////////////////////////////////////////////////////////////////////////

struct P2P_join_request_packet createJoinRequest() {
    struct P2P_join_request_packet p;
    p.header = getHeader(my_ip, my_port, MAX_TTL, MSG_JOIN, getMessageID(), NO_BODY);
    return p;
}

struct P2P_join_response_packet createJoinResponse(uint32_t msgid) {
    struct P2P_join_response_packet p;
    p.header = getHeader(my_ip, my_port, MAX_TTL, MSG_JOIN, msgid, 2);
    p.body.status = htons(0x0200);
    return p;
}

struct P2P_ping_packet createPingPacket_A() {
    struct P2P_ping_packet p;
    p.header = getHeader(my_ip, my_port, 0x01, MSG_PING, getMessageID(), NO_BODY);
    return p;
}

struct P2P_ping_packet createPingPacket_B() {
    struct P2P_ping_packet p;
    p.header = getHeader(my_ip, my_port, MAX_TTL, MSG_PING, getMessageID(), NO_BODY);
    return p;
}

struct P2P_pong_A_packet createPongPacket_A(uint32_t msgid) {
    struct P2P_pong_A_packet p;
    p.header = getHeader(my_ip, my_port, 0x01, MSG_PONG, msgid, NO_BODY);
    return p;
}

struct P2P_pong_B_packet createPongPacket_B(uint32_t msgid) {
    struct P2P_pong_B_packet p;
    p.header = getHeader(my_ip, my_port, MAX_TTL, MSG_PONG, msgid, NO_BODY);
    fillPongBody(&p);
    processPongBodyToNetwork(&p);
    return p;
}

struct P2P_bye_packet createByePacket() {
    struct P2P_bye_packet p;
    p.header = getHeader(my_ip, my_port, MAX_TTL, MSG_BYE, getMessageID(), NO_BODY);
    return p;
}

#endif
