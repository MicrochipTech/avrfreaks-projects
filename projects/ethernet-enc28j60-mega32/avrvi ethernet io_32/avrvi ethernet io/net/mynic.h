#ifndef _NIC_H_
#define _NIC_H_

#include "../config.h"
#include "ip.h"
#include "tcp.h"
#include "udp.h"
#include "arp.h"
#include <stdint.h>

#define NIC_BUFFERSIZE 1054

extern uint32_t nic_ip;
extern uint8_t  nic_mac[6];
extern uint32_t nic_gateway_ip; 

void nic_init(void);
inline void nic_send_packet(unsigned char *buffer, unsigned int len);
inline unsigned int nic_receive_packet(unsigned char *buffer, unsigned int maxlen);
void nic_copy_mac_to_buf(unsigned char * buffer);
unsigned int nic_buffer_to_unsigned_int(unsigned char *buffer);
uint32_t nic_buffer_to_ip(unsigned char *buffer);
void nic_ip_to_buffer(unsigned char *buffer, uint32_t ip);
unsigned long nic_buffer_to_seq(unsigned char *buffer);
void nic_seq_to_buffer(unsigned char *buffer, unsigned long seq);
#endif

