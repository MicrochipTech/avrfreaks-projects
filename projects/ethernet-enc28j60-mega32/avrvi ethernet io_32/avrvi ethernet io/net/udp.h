#ifndef _UDP_H_
#define _UDP_H_
#include <stdint.h>
#include "mynic.h"
#include "ip.h"

#define UDP_POS_DEST_PORT (IP_POS_DATA + 2)
#define UDP_POS_DATA (IP_POS_DATA+8)


void udp_generate_packet(unsigned char *buffer,
                         uint32_t *dest_ip, 
                         unsigned char *dest_mac, unsigned int source_port, 
                         unsigned int dest_port, unsigned int data_length);

#endif
