#ifndef P2P_HELPERS
#define P2P_HELPERS

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "structures.h"

void ip_hex_to_dec(uint32_t * ip, int * b1, int * b2, int * b3, int * b4) {
    *b1 = (int) (*ip & 0xff);
    *b2 = (int) ((*ip >> 8) & 0xff);
    *b3 = (int) ((*ip >> 16) & 0xff);
    *b4 = (int) ((*ip >> 24) & 0xff);
}

void print_header(struct P2P_h h) {
    int b1, b2, b3, b4;
    ip_hex_to_dec(&h.org_ip, &b1, &b2, &b3, &b4);
    printf("\tVersion:\t%x\n", h.version);
    printf("\tTTL:\t\t%x\n", h.ttl);
    printf("\tMSG:\t\t0x0%x\n", h.msg_type);
    printf("\tRES:\t\t%x\n", h.reserved);
    printf("\tSND PORT:\t%d\n", h.org_port);
    printf("\tLEN:\t\t%d\n", h.length);
    printf("\tIP:\t\t%d.%d.%d.%d (0x%x)\n", b1, b2, b3, b4, h.org_ip);
    printf("\tMSGID:\t\t0x%x\n", h.msg_id);
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

//*************************************************************
//                                                            *
//                      BOOTSTRAPPING                         *
//                                                            *
//*************************************************************

int getBootstrapSocket(char * ip, char * port, uint16_t org_port) {
    uint32_t org_ip  = 0x82E92937;
    
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
    
    return sockfd;
}

//*************************************************************
//                                                            *
//                PACKET FORGERY FUNCTIONS                    *
//                                                            *
//*************************************************************

struct P2P_h joinRequestHeader(uint32_t org_ip, uint16_t org_port) {
    struct P2P_h h;
    h.version = 0x1;
    h.ttl = 0x5;
    h.msg_type = MSG_JOIN;
    h.reserved = 0x0;
    h.org_port = org_port;
    h.length = 0;
    h.org_ip = htonl(org_ip);
    h.msg_id = 0xb61effff;
    return h;
}

#endif
