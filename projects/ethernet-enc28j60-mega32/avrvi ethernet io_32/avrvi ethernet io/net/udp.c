/*
,-----------------------------------------------------------------------------------------.
| net/udp
|-----------------------------------------------------------------------------------------
| this file implements a very basic udp protocol
| - ethernet packet generation is done
| - udp packet is constructed with disabled checksum!
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
#include "udp.h"
//generate an udp packet.
//
//usage:
// - put udp data at position UDP_POS_DATA
// - call udp_generate_packet with all neccessary fields and 
//   datalength=how many databytes to tx
// - call nic_send_packet(buffer) to TX
void udp_generate_packet(unsigned char *buffer, 
                         uint32_t *dest_ip, 
                         unsigned char *dest_mac, unsigned int source_port, 
                         unsigned int dest_port, unsigned int data_length){

	//datalength = udpheader + datalen
	data_length = data_length + 8;

	//generate an ip packet (includes ethernet header)
	ip_generate_packet(buffer, dest_ip, dest_mac, source_port, dest_port, IP_PROTOCOL_UDP, data_length);

	//setup soure/dest port:
	buffer[IP_POS_DATA + 0] = (source_port>>8);
	buffer[IP_POS_DATA + 1] = (source_port   );
	buffer[IP_POS_DATA + 2] = (  dest_port>>8);
	buffer[IP_POS_DATA + 3] = (  dest_port   );

	//setup data length of udp
	buffer[IP_POS_DATA + 4] = (data_length>>8);
	buffer[IP_POS_DATA + 5] = (data_length   );
	
	//store checksum: (0x0000 -> checksum disable)
	buffer[IP_POS_DATA + 6] = 0x00;
	buffer[IP_POS_DATA + 7] = 0x00;

	//calculate checksum now: (for ip header + udp header + data !!!
	//add pseudo header (sourceip+targetip + 0x00 + PROTOCOL + LEN) after udp data:
	
/*	if (IP_POS_DATA+data_length+12 > 600){
		//no more space -> send 0x0000 as checksum -> checksum=off
		res16 = 0x0000;
	}else{
		//enough space to add checksum:
		if ((IP_POS_DATA+data_length)&0x01){
			//add 1byte padding
			buffer[IP_POS_DATA+data_length] = 0x00;
			data_length++;
		}

		//add source+dest ip:
		nic_ip_to_buffer(&buffer[IP_POS_DATA+data_length+4], *dest_ip);
		nic_ip_to_buffer(&buffer[IP_POS_DATA+data_length+0], nic_ip);

		//add 0x00;
		buffer[IP_POS_DATA+data_length+8] = 0x00;

		//add protocol:
		buffer[IP_POS_DATA+data_length+9] = IP_PACKETTYPE_UDP;

		//add UDP length:
		buffer[IP_POS_DATA+data_length+10] = (data_length>>8)&0xFF;
		buffer[IP_POS_DATA+data_length+11] = (data_length   )&0xFF;
			
		//add checksum over udpheader, udp data + udp pseudo header (after data)
		res16 = nethelp_checksum(&buffer[IP_POS_DATA], data_length+12, 0);
	}
	
	//store checksum:
	buffer[IP_POS_DATA+6] = (res16>>8)&0xFF;
	buffer[IP_POS_DATA+7] = res16 & 0xFF;
*/
}
