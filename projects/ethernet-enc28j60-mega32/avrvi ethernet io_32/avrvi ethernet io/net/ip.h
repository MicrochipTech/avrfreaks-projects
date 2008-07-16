#ifndef _IP_H_
#define _IP_H_
#include <stdint.h>

#define IP_V4 0x40
#define IP_HEADER_LENGTH 0x05

#define IP_PROTOCOL_ICMP 0x01
#define IP_PROTOCOL_TCP  0x06
#define IP_PROTOCOL_UDP  0x11


#define IP_PORT_HTTPD  80
#define IP_PORT_NTP   123

#define IP_CLIENT_PORT_START 40000UL
#define IP_PORT_NTP_CLIENT   (IP_CLIENT_PORT_START+IP_PORT_NTP)

#define IP_POS_VERSION  14
#define IP_POS_TOS      15
#define IP_POS_PKTLEN 16
#define IP_POS_IDENTIFICATIION 18
#define IP_POS_FRAGMENTATION_FLAGS 20
#define IP_POS_TTL 22
#define IP_POS_IPTYPE 23
#define IP_POS_CHECKSUM 24
#define IP_POS_SRC_IP 26
#define IP_POS_DST_IP 30
#define IP_POS_DATA 34


extern volatile unsigned int ip_identification;

void ip_generate_packet(unsigned char *buffer,
                        uint32_t *dest_ip,
                        unsigned char *dest_mac, 
                        unsigned int source_port, unsigned int dest_port, 
                        unsigned char ip_packettype,
                        unsigned int data_length);


#endif
