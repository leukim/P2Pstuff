#ifndef P2P_HELPERS
#define P2P_HELPERS

#include <stdio.h>

void print_header(struct P2P_h h) {
    printf("\tVersion:\t%x\n", h.version);
    printf("\tTTL:\t\t%x\n", h.ttl);
    printf("\tMSG:\t\t0x0%x\n", h.msg_type);
    printf("\tRES:\t\t%x\n", h.reserved);
    printf("\tSND PORT:\t%d\n", h.org_port);
    printf("\tLEN:\t\t%d\n", h.length);
    printf("\tIP:\t\t%x\n", h.org_ip);
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

#endif
