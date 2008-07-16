//see http://de.wikipedia.org/wiki/Address_Resolution_Protocol

#ifndef _ARP_H_
#define _ARP_H_
#include <stdint.h>

#define ARP_TABLE_SIZE 4

struct arp_entry {
	uint32_t ip;
	uint8_t mac[6];
	//uint8_t ttl;
};

extern volatile int arp_table_pointer;
extern struct arp_entry arp_table[ARP_TABLE_SIZE];

void arp_packet_in(unsigned char *buffer, unsigned int len);
void arp_print_table(void);
int arp_search_by_ip(uint32_t ip);
void arp_send_request(unsigned char *buffer, uint32_t *dest_ip);
int arp_add_mac2ip(unsigned char *buffer, unsigned long ip);

#define ARP_OP_REQUEST 0x0001
#define ARP_OP_REPLY   0x0002

#define ARP_POS_HARDWARE_TYPE 14
#define ARP_POS_PROTOCOL      16
#define ARP_POS_HARDWARE_SIZE 18
#define ARP_POS_PROTOCOL_SIZE 19
#define ARP_POS_OP            20
#define ARP_POS_SRC_MAC       22
#define ARP_POS_SRC_IP        28
#define ARP_POS_DST_MAC       32
#define ARP_POS_DST_IP        38


#endif
