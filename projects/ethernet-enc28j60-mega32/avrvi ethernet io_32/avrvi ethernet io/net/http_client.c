/*
,-----------------------------------------------------------------------------------------.
| net/http_client
|-----------------------------------------------------------------------------------------
| this file describes a very simple http client.
| - not yet finished, implemented only for some tests ...
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
//#include "http_client.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"
#include "http_client.h"
#include "httpd.h"


void http_client_init(void){
}

//DO NOT COMPILE THIS... IT WAS JUST A TEST... 
#if 0
void http_client_connect(unsigned char *buffer){
	//shoutcast test:
	// http://64.236.34.196:80/stream/1040
	tcp_sockets[0].state       = TCP_STATE_ESTABLISHED;
	tcp_sockets[0].source_port = 80;
	tcp_sockets[0].source_ip   = (64L<<24) + (236L<<16) + (34L<<8) + 196; //nic_ip - 5; //(192L<<24) + (168L<<16) + (0L<<8) + 1;//
	tcp_sockets[0].dest_port   = 40080;
	tcp_sockets[0].ack         = 0;
	tcp_sockets[0].seq         = 0;
	tcp_sockets[0].ttl         = TCP_TTL_TIMEOUT;
	
	//send SYN Packet
	tcp_send_packet(buffer, 0, (TCP_SYN_FLAG), 0);

	//init seq counter:
	tcp_sockets[0].seq = 1;

}

unsigned int http_client_data_in(unsigned char *buffer, unsigned int datapos, unsigned int datalen, unsigned char socketnum, unsigned char *appstate){
	softuart_puts_progmem("http client\r\n");
	tcp_sockets[0].ack = nic_buffer_to_seq(&buffer[TCP_POS_SEQ])+1;
	tcp_sockets[0].seq = 1;
	//*appstate = TCP_APPSTATE_EMPTYACK;
	unsigned long pos = TCP_POS_TXDATA;
	pos = httpd_add_progmem_data(PSTR("GET /stream/1040 HTTP/1.1\r\nHost: 64.236.34.196:80\r\nAccept: text/html\r\n\r\n"), buffer, pos);
	*appstate = TCP_APPSTATE_HAS_TXDATA;
	return (pos-TCP_POS_TXDATA);	
}
#endif

