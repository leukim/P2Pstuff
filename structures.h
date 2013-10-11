#ifndef P2P_structures
#define P2P_structures

#include "T-110_5150_reference_c_header.h"

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

struct P2P_pong_packet {
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
