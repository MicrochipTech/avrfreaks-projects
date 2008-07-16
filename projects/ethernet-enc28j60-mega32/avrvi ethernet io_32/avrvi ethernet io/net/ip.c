/*
,-----------------------------------------------------------------------------------------.
| net/ip
|-----------------------------------------------------------------------------------------
| this file implements a very basic ip protocol
| - ethernet packet generation is done
| - ip packet is constructed
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

#include "ip.h"
#include "mynic.h"
#include "nethelp.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

volatile unsigned int ip_identification;

void ip_generate_packet(unsigned char *buffer,
                        uint32_t *dest_ip,
                        unsigned char *dest_mac, 
                        unsigned int source_port, unsigned int dest_port, 
                        unsigned char ip_packettype,
                        unsigned int data_length){
	unsigned int res16;
	unsigned int c;
	
	//datalength = datalength + ip header:
	data_length = data_length + (IP_HEADER_LENGTH<<2);
	
	//generate ethernet packet:
	for (c=0; c<6; c++){
		//set destination mac address:
		buffer[c]   = dest_mac[c];
		//set source mac address:
		buffer[c+6] = nic_mac[c];
	}

	//set ethernet packettype to IP
	buffer[12] = 0x08;
	buffer[13] = 0x00;

	//now generate ip packet:

	//set ipv4 & set length:
	buffer[IP_POS_VERSION] = IP_V4 | IP_HEADER_LENGTH;

	//set TOS to default 0x00 (?)
	buffer[IP_POS_TOS] = 0x00;

	//set total length:
	buffer[IP_POS_PKTLEN+0] = (data_length>>8)&0xFF;
	buffer[IP_POS_PKTLEN+1] = (data_length   )&0xFF;

	//set packet identification
	buffer[IP_POS_IDENTIFICATIION+0] = (ip_identification>>8)&0xFF;
	buffer[IP_POS_IDENTIFICATIION+1] = (ip_identification   )&0xFF;
	ip_identification++;

	//set dont fragment flags:
	buffer[IP_POS_FRAGMENTATION_FLAGS+0] = 0x40;
	buffer[IP_POS_FRAGMENTATION_FLAGS+1] = 0x00;
	
	//set TTL to 128
	buffer[IP_POS_TTL] = 0x80;
	
	//set ip packettype to tcp/udp/...
	buffer[IP_POS_IPTYPE] = ip_packettype;

	//set dest ip:
	nic_ip_to_buffer(&buffer[IP_POS_DST_IP], *dest_ip);
	
	//set source ip:
	nic_ip_to_buffer(&buffer[IP_POS_SRC_IP], nic_ip);

	//set checksum to zero before running crc calc:
	buffer[IP_POS_CHECKSUM+0] = 0x00;
	buffer[IP_POS_CHECKSUM+1] = 0x00;

	//calculate checksum:
	res16 = nethelp_checksum(&buffer[IP_POS_VERSION], IP_HEADER_LENGTH<<2, 0);
	
	//store checksum:
	buffer[IP_POS_CHECKSUM+0] = (res16>>8)&0xFF;
	buffer[IP_POS_CHECKSUM+1] = res16 & 0xFF;

	//finished 

}

