#ifndef P2P_structures
#define P2P_structures

#include "T-110_5150_reference_c_header.h"

struct P2P_join_response_packet {
    struct P2P_h header;
    uint16_t status;
};

#endif
