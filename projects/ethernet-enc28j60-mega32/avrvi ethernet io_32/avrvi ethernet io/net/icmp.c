/*
,-----------------------------------------------------------------------------------------.
| net/icmp
|-----------------------------------------------------------------------------------------
| this file implements a very basic icmp protocol
| - ethernet packet generation is done
| - received ping request is answered with reply
| - thats it! we can not send a ping request for example...
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

#include "icmp.h"
#include "mynic.h"
#include "ip.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

//called if icmp packet comes in
void icmp_packet_in(unsigned char *buffer,unsigned int len){
	int i;
	int pos;

	//we only support ICMP echo request (ping) packets:
	if (buffer[ICMP_POS_TYPE] == ICMP_TYPE_ECHO_REQUEST){
		#if ICMP_DEBUG
		softuart_puts_progmem("ICMP: echo reply sent");
		softuart_putnewline();
		#endif
		//reuse buffer for the ICMP echo reply:

		//*******************************
		//setup the eth packet:
		//get packet target mac from arp table:
		pos = arp_search_by_ip(nic_buffer_to_ip(&buffer[IP_POS_SRC_IP]));
		if (pos<0){
			//no match, copy src to target ...
			for(i=0; i<6; i++)
				buffer[i] = buffer[6+i];
		}else{
			//match, copy mac from arp table:
			for(i=0; i<6; i++)
				buffer[i] = arp_table[pos].mac[i];
		}

		//packet source mac:
		nic_copy_mac_to_buf(&buffer[6]);

		//change type to ECHO REPLY
		buffer[ICMP_POS_TYPE] = ICMP_TYPE_ECHO_REPLY;
		
		//"calculate" checksum:
		//use quick & dirty hack, we change only the icmp type
		//so we can calculate the new checksum like this:
		if (buffer[ICMP_POS_CHECKSUM+0] > 0xFF-0x08)
			buffer[ICMP_POS_CHECKSUM+1] = buffer[ICMP_POS_CHECKSUM+1] + 1;

		buffer[ICMP_POS_CHECKSUM+0] = buffer[ICMP_POS_CHECKSUM+0] + 0x08;


		//set up dst ip (copy src to dest ip):
		for(i=0; i<4; i++){
			buffer[IP_POS_DST_IP+i] = buffer[IP_POS_SRC_IP+i];
		}
		
		//set up src ip:
		nic_ip_to_buffer(&buffer[IP_POS_SRC_IP], nic_ip);

		
		nic_send_packet(buffer,len);
	}
}
