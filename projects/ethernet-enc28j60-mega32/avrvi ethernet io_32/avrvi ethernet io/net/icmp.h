#ifndef _ICMP_H_
#define _ICMP_H_
 
#define ICMP_POS_TYPE 34
#define ICMP_POS_CHECKSUM 36
#define ICMP_TYPE_ECHO_REPLY   0x00
#define ICMP_TYPE_ECHO_REQUEST 0x08

void icmp_packet_in(unsigned char *buffer,unsigned int len);

#endif
