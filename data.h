#ifndef P2P_DATA
#define P2P_DATA

#define DEBUG 1

#define NO_BODY 0

// Own data
uint32_t my_ip  = 0x82E92937;
uint16_t my_port = 12345;

// Hardcoded data of the bootstrap server
char * bootstrap_ip = "130.233.43.39";
char * bootstrap_port = "8317";

// Internal state data
int STATE = PROGRAM_RUN;

int bytes_sent = 0;
int bytes_recv = 0;

uint32_t CURRENT_MSG = 0;

#endif
