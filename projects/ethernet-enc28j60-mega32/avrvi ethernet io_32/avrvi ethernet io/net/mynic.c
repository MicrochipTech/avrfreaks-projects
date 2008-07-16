/*
,-----------------------------------------------------------------------------------------.
| net/nic
|-----------------------------------------------------------------------------------------
| this file implements some basic nic function
| - wrapper for enc28j60 ethernet control lib
| - ip/mac buffer conversion tools
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
#include "mynic.h"
#include "../io/enc28j60.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

uint32_t nic_ip;
uint8_t  nic_mac[6];
uint32_t nic_gateway_ip; 

void nic_init(){
	//init enc28j60
	#if NIC_DEBUG
		softuart_puts_progmem("NIC : enc28j60 init... ");
	#endif
	enc28j60_init(); 
  //enc28j60Init();
	#if NIC_DEBUG
		softuart_puts_progmem("done\r\n");
	#endif

	//set mac:
	nic_copy_mac_to_buf(nic_mac);
	//enc28j60_set_mac_address(&nic_mac[0]);

	//set ip:
	nic_ip = NIC_IP_ADDRESS;

	#if NIC_DEBUG
		softuart_puts_progmem("NIC : ");
		softuart_put_ip(nic_ip);
		softuart_putnewline();
	#endif
	
	//setup default gw ip:
	nic_gateway_ip = NIC_GATEWAY_IP_ADDRESS;	
}

inline void nic_send_packet(unsigned char *buffer, unsigned int len){
	enc28j60_send_packet(buffer,len);
	//enc28j60PacketSend(len,buffer);
}

inline unsigned int nic_receive_packet(unsigned char *buffer, unsigned int maxlen){
	return enc28j60_receive_packet(buffer,maxlen);
	//return enc28j60PacketReceive(maxlen,buffer);
}

void nic_copy_mac_to_buf(unsigned char * buffer){
	buffer[0] = NIC_MAC0;
	buffer[1] = NIC_MAC1;
	buffer[2] = NIC_MAC2;
	buffer[3] = NIC_MAC3;
	buffer[4] = NIC_MAC4;
	buffer[5] = NIC_MAC5;
}

unsigned int nic_buffer_to_unsigned_int(unsigned char *buffer){
	unsigned int res = ((unsigned int)buffer[20]<<8) | buffer[21];
	return res;
}

uint32_t nic_buffer_to_ip(unsigned char *buffer){
	uint32_t res = ((uint32_t)buffer[0]<<24) | ((uint32_t)buffer[1]<<16) | ((uint32_t)buffer[2]<<8) | ((uint32_t)buffer[3]<<0);
	return res;
}

void nic_ip_to_buffer(unsigned char *buffer, uint32_t ip){
	buffer[0] = ip>>24 & 0xFF;
	buffer[1] = ip>>16 & 0xFF;
	buffer[2] = ip>> 8 & 0xFF;
	buffer[3] = ip>> 0 & 0xFF;
}

unsigned long nic_buffer_to_seq(unsigned char *buffer){
	unsigned long res = ((unsigned long)buffer[0]<<24) | ((unsigned long)buffer[1]<<16) | ((unsigned long)buffer[2]<<8) | ((unsigned long)buffer[3]<<0);
	return res;
}

void nic_seq_to_buffer(unsigned char *buffer, unsigned long seq){
	buffer[0] = seq>>24 & 0xFF;
	buffer[1] = seq>>16 & 0xFF;
	buffer[2] = seq>> 8 & 0xFF;
	buffer[3] = seq>> 0 & 0xFF;
}


