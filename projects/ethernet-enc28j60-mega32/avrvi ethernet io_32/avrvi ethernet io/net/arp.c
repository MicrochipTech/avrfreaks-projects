/*
,-----------------------------------------------------------------------------------------.
| net/arp
|-----------------------------------------------------------------------------------------
| this file implements a very basic arp protocol
| - we store the last <ARP_TABLE_SIZE> mac<->ip mapping in an table
| - received arp request & answer with reply
| - send arp request & read replay -> add to table
|
| Author   : {{removed according to contest rules}}
|            -> circuitcellar.com avr design contest 2006
|            -> Entry #AT2616
|
|-----------------------------------------------------------------------------------------
| License:
| This program is free software; you can redistribute it and/or modify it under
| the terms of the GNU General Public License as published by the Free Software
| Foundation; either version 2 of the License, or (at your option) any later
| version.
| This program is distributed in the hope that it will be useful, but
|
| WITHOUT ANY WARRANTY;
|
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with
| this program; if not, write to the Free Software Foundation, Inc., 51
| Franklin St, Fifth Floor, Boston, MA 02110, USA
|
| http://www.gnu.de/gpl-ger.html
`-----------------------------------------------------------------------------------------*/

#include "arp.h"
#include "mynic.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

struct arp_entry arp_table[ARP_TABLE_SIZE];
volatile int arp_table_pointer;

//param initialise arp table:
void arp_init(void){
	for(unsigned char i=0; i<ARP_TABLE_SIZE; i++){
		arp_table[i].ip  = 0;
		//arp_table[i].ttl = 0;
	}
	arp_table_pointer = 0;
}

//decode arp packet & send reply
void arp_packet_in(unsigned char *buffer, unsigned int len){
	uint32_t src_ip;
	uint32_t dst_ip;
	int pos;
	unsigned char c;

	src_ip = nic_buffer_to_ip(&buffer[ARP_POS_SRC_IP]);
	dst_ip = nic_buffer_to_ip(&buffer[ARP_POS_DST_IP]);

	//is this an arp request ?
	if (nic_buffer_to_unsigned_int(buffer) == ARP_OP_REQUEST){
		//is this arp request for us ?
		if (dst_ip == nic_ip){
			//yes -> send ARP REPLY:
			//reuse buffer for the reply !!!
			#if ARP_DEBUG
			softuart_puts_progmem("ARP : who has ");
			softuart_put_ip(dst_ip);
			softuart_puts_progmem(" tell ");
			softuart_put_ip(src_ip);
			softuart_putnewline();
			#endif
			
			//get entry pos in arp table.
			pos = arp_search_by_ip(src_ip);

			if (pos < 0){
				//ip not found -> get next insert id:
				pos = arp_table_pointer;
				
				//update ringbuffer pointer
				arp_table_pointer++;
				if (arp_table_pointer >= ARP_TABLE_SIZE)
					arp_table_pointer=0; 
			}

			//already known -> update
			arp_table[pos].ip = src_ip; //not necessary ...
			for (c=0; c<6; c++){
				arp_table[pos].mac[c] = buffer[ARP_POS_SRC_MAC+c];
			}

			//setup the eth packet:
			//packet source mac:
			nic_copy_mac_to_buf(&buffer[6]);
				
			//packet target mac:
			for (c=0; c<6; c++){
				buffer[c] = buffer[ARP_POS_SRC_MAC+c];
			}

			//now setup the arp packet:
			//change OP to REPLY
			buffer[ARP_POS_OP+0] = ARP_OP_REPLY>>8 & 0xFF;
			buffer[ARP_POS_OP+1] = ARP_OP_REPLY    & 0xFF;
			
			//copy src mac address to dst:
			for(c=0; c<6; c++){
				buffer[ARP_POS_DST_MAC+c] = buffer[ARP_POS_SRC_MAC+c];
			}

			//set src mac to our mac
			nic_copy_mac_to_buf(&buffer[ARP_POS_SRC_MAC]);

			//setup source ip:
			nic_ip_to_buffer(&buffer[ARP_POS_SRC_IP], nic_ip);
			//setup target ip:
			nic_ip_to_buffer(&buffer[ARP_POS_DST_IP], src_ip);

			//send packet
			nic_send_packet(buffer, len);
		}	
	}else if (nic_buffer_to_unsigned_int(buffer) == ARP_OP_REPLY){
		//handle ARP REPLY:
	
		//is this for us ?
		if (nic_buffer_to_ip(&buffer[ARP_POS_DST_IP]) == nic_ip){
			#if ARP_DEBUG
			softuart_puts_progmem("ARP : got reply for us");
			softuart_putnewline();
			#endif

			//if unknown -> add
			if( arp_search_by_ip(nic_buffer_to_ip(&buffer[ARP_POS_SRC_IP])) == -1)
				arp_add_mac2ip(&buffer[ARP_POS_SRC_MAC], nic_buffer_to_ip(&buffer[ARP_POS_SRC_IP]));
		}
		
		//show tale
		//arp_print_table();
	}
}

int arp_add_mac2ip(unsigned char *buffer, unsigned long ip){
	unsigned char c;
	int res;
	
	#if ARP_DEBUG
	softuart_puts_progmem("ARP : adding ip ");
	softuart_put_ip(ip);
	softuart_puts_progmem(" to mac table");
	softuart_putnewline();
	#endif	

	//set ip
	arp_table[arp_table_pointer].ip = ip;
	
	//set mac:
	for (c=0; c<6; c++){
		arp_table[arp_table_pointer].mac[c] = buffer[c];
	}

	res = arp_table_pointer;	

	//update ringbuffer pointer
	arp_table_pointer++;
	if (arp_table_pointer >= ARP_TABLE_SIZE)
		arp_table_pointer=0;
	
	return res;
}

//send an arp request
void arp_send_request(unsigned char *buffer, uint32_t *dest_ip){
	unsigned char c;	
	
	//generate ethernet packet:
	for (c=0; c<6; c++){
		//set destination mac address:
		buffer[c]   = 0xFF; //BROADCAST!
		//set source mac address:
		buffer[c+6] = nic_mac[c];
	}

	//set ethernet packettype to ARP
	buffer[12] = 0x08;
	buffer[13] = 0x06;

	//set hardware type to ethernet:
	buffer[ARP_POS_HARDWARE_TYPE+0] = 0x00;
	buffer[ARP_POS_HARDWARE_TYPE+1] = 0x01;

	//set protocol to IP:
	buffer[ARP_POS_PROTOCOL+0] = 0x08;
	buffer[ARP_POS_PROTOCOL+1] = 0x00;

	//set hw size:
	buffer[ARP_POS_HARDWARE_SIZE] = 0x06;

	//set protocol size
	buffer[ARP_POS_PROTOCOL_SIZE] = 0x04;

	//set ARP type to REQUEST
	buffer[ARP_POS_OP+0] = ARP_OP_REQUEST>>8;
	buffer[ARP_POS_OP+1] = ARP_OP_REQUEST;

	//setup sender mac & ip:
	nic_copy_mac_to_buf(&buffer[ARP_POS_SRC_MAC]);
	nic_ip_to_buffer(&buffer[ARP_POS_SRC_IP], nic_ip);

	//set target mac to 0xFF:
	for(c=0; c<6; c++)
		buffer[ARP_POS_DST_MAC+c] = 0xFF;
	
	//set target ip:
	nic_ip_to_buffer(&buffer[ARP_POS_DST_IP], *dest_ip);

	//send arp request:
	nic_send_packet(buffer, 14+28);	
}

void arp_print_table(){
	#if ARP_DEBUG
	int i,j;

	softuart_puts_progmem("ARP TABLE:");
	softuart_putnewline();
	softuart_puts_progmem("    | MAC                    | IP");
	softuart_putnewline();
	for (i=0; i<ARP_TABLE_SIZE; i++){
		softuart_put_uint8(i);
		softuart_puts_progmem(" | ");
	
		softuart_put_uint8(arp_table[i].mac[0]);
		for (j=1; j<6; j++){
			softuart_putc(':');
			softuart_put_uint8(arp_table[i].mac[j]);
		}
		softuart_puts_progmem(" | ");
		softuart_put_ip(arp_table[i].ip);
		softuart_putnewline();
	}
	softuart_puts_progmem("ptr: ");
	softuart_put_uint8(arp_table_pointer);
	softuart_putnewline();
	softuart_putnewline();
	#endif
}

int arp_search_by_ip(uint32_t ip){
	int i;
	for (i=0; i<ARP_TABLE_SIZE; i++){
		if (arp_table[i].ip == ip)
			return i;
	}
	//if we get here, ip is unknown
	return -1;
}	
