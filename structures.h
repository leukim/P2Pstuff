#ifndef P2P_structures
#define P2P_structures

#include "T-110_5150_reference_c_header.h"

#define PROGRAM_STOP 0
#define PROGRAM_RUN 1

#define PEER_OK 0
#define PEER_JOIN_PENDING 1
#define PEER_OFFLINE 2

struct P2P_client_info {
	uint32_t ip;
	uint16_t port;
	int status;
	int missed_pings;
};

//////////////////////////////////////////////////////////

struct P2P_pong {
    struct P2P_pong_front front;
    struct P2P_pong_entry e1;
    struct P2P_pong_entry e2;
    struct P2P_pong_entry e3;
    struct P2P_pong_entry e4;
    struct P2P_pong_entry e5;
};

//////////////////////////////////////////////////////////

struct P2P_ping_packet {
    struct P2P_h header;
};

struct P2P_pong_A_packet {
    struct P2P_h header;
};

struct P2P_pong_B_packet {
    struct P2P_h header;
    struct P2P_pong body;
};

struct P2P_join_request_packet {
    struct P2P_h header;
};

struct P2P_join_response_packet {
    struct P2P_h header;
    struct P2P_join body;
};

struct P2P_bye_packet {
    struct P2P_h header;
};

#endif
