/*
,-----------------------------------------------------------------------------------------.
| main
|-----------------------------------------------------------------------------------------
| this file implements the main loop of the whole webserver project
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
#include "main.h"

//#define F_CPU F_OSC

#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdint.h>
#include "update/flashloader.h"


#include "io/dataflash.h"
#include "io/plot.h"
#include "io/filesystem.h"
#include "io/lm75.h"
#include "io/mca25.h"
#include "io/mpf10.h"
#include "io/led.h"

//ACTIVATE DEBUG by editing this file:
#include "debug.h"

#include "io/clock.h"
#include "io/plot.h"
#include "io/servo.h"

#include "net/mynic.h"
#include "net/icmp.h"
#include "net/ip.h"
#include "net/arp.h"
#include "net/tcp.h"
#include "net/httpd.h"
#include "net/ntp_client.h"
#include "net/http_client.h"
#include "net/base64.h"
#include "util/string.h"
#include "main.h"

//interrupt:
#include <avr/interrupt.h>
#include <inttypes.h>
//#include <avr/signal.h>
#define NOP() asm volatile ("nop"::);
#include "config.h"

//global ethernetbuffer
unsigned char buffer[NIC_BUFFERSIZE];

int main(void){
	int len;
	unsigned int type;
	unsigned int port;
	
	DDRB  = 0xFF;
	DDRC  = 0xFF;
	PORTB = 0xFF;

	//flash status led 5 times
	led_init();
	for(unsigned b=0; b<5; b++){
		LED_STATUS_ON(); //PORTC |= (1<<3);
		for(int t=0; t<8; t++)
			_delay_ms(10);
		LED_STATUS_OFF(); //PORTC &= ~(1<<3);
		for(int t=0; t<8; t++)
			_delay_ms(10);
	}	


	//initialise everything:
	softuart_init();
	softuart_puts_progmem("init:");
	softuart_putnewline();
	
	clock_init();
//	mpf10_init();
	servo_init();

//	mca25_init();
//	mca25_configure();
	nic_init();
	ntp_client_init();
//	lm75_init();
//	logger_init();
	tcp_init();
//	http_client_init();

//	dataflash_init();
//	filesystem_init();
//	filesystem_format(); //erase filesystem! takes a long time !!
//	filesystem_list_files();

	//wait some time (800ms)
	for(int t=0; t<80; t++)
			_delay_ms(10);

	//send an ntp time request
	ntp_client_send_request(buffer);

	//main loop, never ends !
	while(1){
		//is there a packet ?
		len = nic_receive_packet(&buffer[0],NIC_BUFFERSIZE);
		
    if (len>0){
			//softuart_puts_progmem("RX\n");
			//packet type
			type = (buffer[12]<<8) + buffer[13];

			
			if(type == ETHERNET_TYPE_ARP){
				///ARP packet
				arp_packet_in(buffer,len);
				//check if we have to call ntp_client:
				if (ntp_client_call_again == 1)
					ntp_client_send_request(buffer);

			}else if (type == ETHERNET_TYPE_IP){
				///IP packet

				//ip protocol:
				unsigned char ip_protocol = buffer[IP_POS_IPTYPE];

				switch (ip_protocol){
					///icmp packet:
					case (IP_PROTOCOL_ICMP):
						icmp_packet_in(&buffer[0],len);
						break;
	
					///tcp packet:
					case(IP_PROTOCOL_TCP):
						//port = (buffer[TCP_POS_DST_PORT]<<8) + buffer[TCP_POS_DST_PORT+1];
						tcp_packet_in(buffer,len);
						break;

					///udp packet:
					case(IP_PROTOCOL_UDP):
						port = (buffer[UDP_POS_DEST_PORT]<<8) + buffer[UDP_POS_DEST_PORT+1];
						
						switch (port){
							//packet for udp client ?
							case(IP_PORT_NTP_CLIENT):
								ntp_client_decode_packet(buffer, len);
								break;

							//unknown udp packet:
							default:
								#if UDP_DEBUG
								softuart_puts_progmem("UDP : ?\r\n");
								#endif
								break;
						}
						
						break;

					///unknown:
					default:
						softuart_puts("??? : ip proto ");
						softuart_put_uint8(ip_protocol);
						softuart_putnewline();
				}
			}
			
		}

		//calc new time:
		clock_do();

		//every new second:
		if (CLOCK_TEST_FLAG(CLOCK_SEC)){
			if (clock[CLOCK_SEC]&0x01)
				LED_STATUS_ON();
			else
				LED_STATUS_OFF();
			//call tcp cleanup:
			tcp_ttl_cleanup();

			//call mca25 cleanup:
//			mca25_cleanup();

			//call templogger:
//			logger_do();

			//clear flag:
			CLOCK_CLEAR_FLAG(CLOCK_SEC);
		}
	}
}


