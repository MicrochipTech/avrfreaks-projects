/*
,-----------------------------------------------------------------------------------------.
| net/tcp
|-----------------------------------------------------------------------------------------
| this file implements a very basic tcp protocol
| - very basic
|
| KNOWN PROBLEMS:
| - it does not yet check the tcp checksum of incoming packets!!
| - something with the seq/ack calc might be buggy
|   -> must be checked by sending bad tcp packets/re-request etc [[TODO!]]
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

#include "tcp.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

struct tcp_socket tcp_sockets[TCP_SOCKET_COUNT];

//initialise all sockets to closed state:
void tcp_init(){
	unsigned char i = 0;
	for(i=0; i<TCP_SOCKET_COUNT; i++){
		tcp_close_socket(i);
	}
}

unsigned char tcp_find_free_socket(){
	unsigned char i;
	for(i=0; i<TCP_SOCKET_COUNT; i++){
		if (tcp_sockets[i].state == TCP_STATE_CLOSED)
			return i;
	}

	//no free closed socket fount! -> kick an TIMED_WAIT socket
	for(i=0; i<TCP_SOCKET_COUNT; i++){
		if (tcp_sockets[i].state == TCP_STATE_TIMED_WAIT){
			tcp_close_socket(i);
			return i;
		}
	}

	//no more free sockets ... return invalid val	
	return(TCP_SOCKET_COUNT);
}

void tcp_packet_in(unsigned char *buffer, unsigned int len){
	unsigned char tcp_header_flags;
	unsigned int  source_port;
	unsigned int  dest_port;
	unsigned int  datalen;
	unsigned int  datapos;
	unsigned long source_ip;
	unsigned long longtmp;
	unsigned char socketnum;
	//unsigned char i;

	//read header:
	//source ip:
	source_ip   = nic_buffer_to_ip(&buffer[IP_POS_SRC_IP]);
	//source port:
	source_port = (buffer[TCP_POS_SRC_PORT]<<8) | buffer[TCP_POS_SRC_PORT+1];
	//dest port
	dest_port   = (buffer[TCP_POS_DST_PORT]<<8) | buffer[TCP_POS_DST_PORT+1];
	//flags:
	tcp_header_flags = buffer[TCP_POS_HEADERFLAGS] & 0x3F;

	//check if target ip matches:
	if (nic_buffer_to_ip(&buffer[IP_POS_DST_IP]) != nic_ip){
		#if TCP_DEBUG
		softuart_puts_progmem("TCP : ignoring packet for ip ");
		softuart_put_ip(nic_buffer_to_ip(&buffer[IP_POS_DST_IP]));
		softuart_putnewline();
		#endif
		return; //no, not for us !
	}

	//do we have an open socket for this connection ?!
	for(socketnum=0; socketnum<TCP_SOCKET_COUNT; socketnum++){
		//check every not closed socket
		if (tcp_sockets[socketnum].state != TCP_STATE_CLOSED){
			//compare source port (faster check than source ip
			if(tcp_sockets[socketnum].source_port == source_port){
				//now compare the source ip:
				if(tcp_sockets[socketnum].source_ip == source_ip){
					/// yes, this is for us !
					//handle packet
					#if TCP_DEBUG
					softuart_puts_progmem("TCP : socket[");
					softuart_put_uint8(socketnum);
					softuart_puts_progmem("] ACK: ");
					softuart_put_uint16(nic_buffer_to_seq(&buffer[TCP_POS_ACK])&0xFFFF);
					softuart_puts_progmem(" SEQ: ");
					softuart_put_uint16(nic_buffer_to_seq(&buffer[TCP_POS_SEQ])&0xFFFF);
					softuart_puts_progmem(" -> ");
					#endif

					//reset TTL:
					tcp_sockets[socketnum].ttl = TCP_TTL_TIMEOUT;

					//check for connection abort (RST flag)
					if (tcp_header_flags & TCP_RST_FLAG){
						//client wants to terminate ! accept this...
						tcp_close_socket(socketnum);
						//tcp_sockets[socketnum].ack = nic_buffer_to_seq(&buffer[TCP_POS_SEQ]) + 1;
						//tcp_sockets[socketnum].seq = nic_buffer_to_seq(&buffer[TCP_POS_ACK]);
						//FIXME: what is the correct response to an RST packet ?!

						#if TCP_DEBUG
						softuart_puts_progmem(" RST FLAG received -> socket CLOSED.\r\n");
						#endif
						return;
					}

					switch (tcp_sockets[socketnum].state){
						///SNY_RECEIVED
						case(TCP_STATE_SYN_RECEIVED):
							if (tcp_header_flags == TCP_ACK_FLAG){
								//ok, this connection is established:
								//we do not need to send any data now)
								tcp_sockets[socketnum].state = TCP_STATE_ESTABLISHED;

								#if TCP_DEBUG
								softuart_puts_progmem("ESTABLISHED");
								#endif
							}else{
								//invalid packet or TCP_RST_FLAG for example ...
								//connection reset -> close socket
								tcp_close_socket(socketnum);
								#if TCP_DEBUG
								softuart_puts_progmem("CLOSED");
								#endif
							}
							break;
	
						///ESTABLISHED
						case(TCP_STATE_ESTABLISHED):
							if (tcp_header_flags & TCP_FIN_FLAG){
								//connection close request
								//-------send ACK & goto CLOSE_WAIT---- no do this:
								//send ACK+FIN and goto LAST ACK:
								tcp_sockets[socketnum].state = TCP_STATE_LAST_ACK;
								tcp_sockets[socketnum].ack = nic_buffer_to_seq(&buffer[TCP_POS_SEQ]) + 1;
								tcp_sockets[socketnum].seq = nic_buffer_to_seq(&buffer[TCP_POS_ACK]);

								tcp_send_packet(buffer, socketnum, (TCP_FIN_FLAG|TCP_ACK_FLAG), 0);
								#if TCP_DEBUG
								softuart_puts_progmem("LAST_ACK");
								#endif
							}else{
								//this is data for application!
								//check for error & send packet to correct application

								///OUTGOING DATA HANDLING:
								//which databyte was acked ?
								longtmp = nic_buffer_to_seq(&buffer[TCP_POS_ACK]);
								//the last packet we have sent was tcp_sockets[socketnum].seq,
								//check it:
								if ((longtmp != tcp_sockets[socketnum].seq) && (longtmp != 1)){
									#if TCP_DEBUG
									softuart_puts_progmem("TCP : ack != last seqnum! retransmit : ");
									softuart_put_uint16(longtmp&0xFFFF);
									softuart_putc(' ');
									softuart_put_uint16(tcp_sockets[socketnum].seq&0xFFFF);
									softuart_putc(' ');
									softuart_putnewline();
									#endif
									//overwrite seq num -> send acked data !
									tcp_sockets[socketnum].seq = longtmp;
								}

								//update our seq counter, we should send now
								//databyte longtmp-1 (1 = initval of seq counter)
								//tcp_sockets[socketnum].seq = longtmp;


								///INCOMING DATA HANDLING:
								//check if this is the data we expect:
								longtmp = nic_buffer_to_seq(&buffer[TCP_POS_SEQ]);

								//extract data pos:
								datapos = (14 + ((buffer[IP_POS_VERSION] & 0x0F) << 2) + ((buffer[TCP_POS_DATA_OFFSET] & 0xF0) >>2));
								datalen = ((buffer[IP_POS_PKTLEN+0]<<8) | buffer[IP_POS_PKTLEN+1])-datapos+14;

								//we are waiting for a packet with seq = <tcp_sockets[socketnum].ack>
								//-> we sent an ack for packet byte n, now the seq counter of incoming
								//   packet has to be <n>
								if ((longtmp != tcp_sockets[socketnum].ack) && (tcp_sockets[socketnum].ack != 0)){
									//there was an error, check what to do next:
									if (longtmp < tcp_sockets[socketnum].ack){
										//we expected data x to y but we got (x-n) to y
										//we onlny need x to y, so discard n bytes:
										longtmp = (tcp_sockets[socketnum].ack - longtmp);
										datapos = datapos + longtmp;
										datalen = datalen - longtmp;
										#if TCP_DEBUG
										softuart_puts_progmem(" WARN: tcp data dup! ignoring duplicate data. ");
										#endif
									}else{ //longtmp > tcp....
										//uups one packet was lost during transfer !
										//re request this packet, send an ack for expected <seq>:
										#if TCP_DEBUG
										softuart_puts_progmem(" ERR : tcp packet lost! re-requesting. ");
										#endif
										tcp_send_packet(buffer, socketnum, (TCP_ACK_FLAG), 0);
										return;
									}
								}

								//protect buffer:
								if (datapos > NIC_BUFFERSIZE)
									datapos = NIC_BUFFERSIZE;
								if ((datapos + datalen) > NIC_BUFFERSIZE)
									datalen = 0;

								//limit datalength to length that fits into our buffer:
								if (datalen > (NIC_BUFFERSIZE-datapos))
									datalen = (NIC_BUFFERSIZE-datapos);

								//next ack packet we will send must ack data byte n+datalen:
								tcp_sockets[socketnum].ack = tcp_sockets[socketnum].ack + (datalen);

								#if TCP_DEBUG
								softuart_put_uint16(datalen);
								softuart_puts_progmem(" bytes in ");
								/*if (datalen>0)
									softuart_putnewline();
								for(int d=datapos; d<datapos+datalen; d++){
									softuart_putc(buffer[d]);
								}*/
								softuart_putnewline();
								#endif

								//now call the application
								unsigned char appstate = TCP_APPSTATE_NONE;
								unsigned int  data_tx_count = 0;
								if ((tcp_sockets[socketnum].dest_port == IP_PORT_HTTPD)||(tcp_sockets[socketnum].dest_port == 3333)){
									data_tx_count = httpd_data_in(buffer, datapos, datalen, socketnum, &appstate);
								//}else{
								//	data_tx_count = http_client_data_in(buffer, datapos, datalen, socketnum, &appstate);
								}

								//application modified appstate
								switch(appstate){
									case (TCP_APPSTATE_EMPTYACK):
										tcp_send_packet(buffer, socketnum, (TCP_ACK_FLAG), 0);
										tcp_sockets[socketnum].seq = tcp_sockets[socketnum].seq;

										break;
									case (TCP_APPSTATE_HAS_TXDATA):
										//send data
										tcp_send_packet(buffer, socketnum, (TCP_ACK_FLAG), data_tx_count);
										//seq counter update:
										tcp_sockets[socketnum].seq = tcp_sockets[socketnum].seq + data_tx_count;
										#if TCP_DEBUG
										softuart_puts_progmem("TCP : last byte sent = ");
										softuart_put_uint16(tcp_sockets[socketnum].seq&0xFFFF);
										softuart_putc(' ');
										#endif
									
										break;
									case (TCP_APPSTATE_FINISHED):
										#if TCP_DEBUG
										softuart_puts_progmem("TCP : app closed socket ");
										softuart_put_uint8(socketnum);
										#endif
										tcp_sockets[socketnum].state = TCP_STATE_FIN_WAIT1;
										tcp_send_packet(buffer, socketnum, (TCP_FIN_FLAG|TCP_PSH_FLAG|TCP_ACK_FLAG), 0);
										//seq counter update:
										tcp_sockets[socketnum].seq = tcp_sockets[socketnum].seq + 1;
										break;
									default:
										//do nothing... should not happen!
										break;
								}
							}
							break;

						///CLOSE WAIT
						case(TCP_STATE_CLOSE_WAIT):
							//goto last_ack & send FIN packet
							tcp_sockets[socketnum].state = TCP_STATE_LAST_ACK;
							tcp_sockets[socketnum].ack = nic_buffer_to_seq(&buffer[TCP_POS_SEQ])+1;
							tcp_sockets[socketnum].seq = nic_buffer_to_seq(&buffer[TCP_POS_ACK]);

							tcp_send_packet(buffer, socketnum, (TCP_FIN_FLAG), 0);
							#if TCP_DEBUG
							softuart_puts_progmem("LAST_ACK");
							#endif
							break;

						///LAST_ACK
						case(TCP_STATE_LAST_ACK):
							//socket is closed
							tcp_close_socket(socketnum);
							#if TCP_DEBUG
							softuart_puts_progmem("LAST_ACK->CLOSED");
							#endif
							break;

						///FIN WAIT 1
						case(TCP_STATE_FIN_WAIT1):
							//if we receive FIN
							tcp_sockets[socketnum].ack = nic_buffer_to_seq(&buffer[TCP_POS_SEQ])+1;
							
							#if TCP_DEBUG
							softuart_puts_progmem("FIN_WAIT1->");
							#endif
							if (tcp_header_flags == TCP_FIN_FLAG){
								//goto CLOSING & send an ack:
								tcp_sockets[socketnum].state = TCP_STATE_CLOSING;
								tcp_send_packet(buffer, socketnum, (TCP_ACK_FLAG), 0);
								#if TCP_DEBUG
								softuart_puts_progmem("CLOSING");
								#endif
							}else if(tcp_header_flags == (TCP_FIN_FLAG | TCP_ACK_FLAG)){
								//FIN&ACK -> send ACK & close !
								if (nic_buffer_to_seq(&buffer[TCP_POS_ACK]) == tcp_sockets[socketnum].seq){
									//close socket!
									tcp_close_socket(socketnum);
									#if TCP_DEBUG
									softuart_puts_progmem("CLOSED !");
									#endif
								}else{
									//both have sent the connection close request, we must wait for an last ack
									tcp_sockets[socketnum].state = TCP_STATE_LAST_ACK;
									#if TCP_DEBUG
									softuart_puts_progmem("LAST ACK !");
									#endif
								}
								tcp_send_packet(buffer, socketnum, (TCP_ACK_FLAG), 0);

							}else{
								//ACK -> goto FIN WAIT2
								tcp_sockets[socketnum].state = TCP_STATE_FIN_WAIT2;
								#if TCP_DEBUG
								softuart_puts_progmem("FIN_WAIT2");
								#endif
							}
							break;
						
						///FIN WAIT 2
						case(TCP_STATE_FIN_WAIT2):
							//if we receive FIN
							tcp_sockets[socketnum].ack = nic_buffer_to_seq(&buffer[TCP_POS_SEQ])+1;
							//tcp_sockets[socketnum].seq = nic_buffer_to_seq(&buffer[TCP_POS_ACK]);

							if (tcp_header_flags & TCP_FIN_FLAG){
								//FIN -> goto TIMED WAIT
								tcp_sockets[socketnum].state = TCP_STATE_TIMED_WAIT;

								tcp_send_packet(buffer, socketnum, (TCP_ACK_FLAG), 0);
								#if TCP_DEBUG
								softuart_puts_progmem("FIN_WAIT2->TIMED_WAIT");
								#endif
							}
							break;

						///TIMED WAIT:
						case(TCP_STATE_TIMED_WAIT):
							#if TCP_DEBUG
							softuart_puts_progmem("TIMED_WAIT: packet in ?! ");
							#endif
							break;

						///CLOSING
						case(TCP_STATE_CLOSING):
							if (tcp_header_flags & TCP_ACK_FLAG){
								tcp_sockets[socketnum].state = TCP_STATE_TIMED_WAIT;
								#if TCP_DEBUG
								softuart_puts_progmem("CLOSING->TIMED_WAIT");
								#endif
							}
							break;

						default:
							#if TCP_DEBUG
							softuart_puts_progmem("ERROR: invalid tcp state ! socket closed!");
							#endif
							tcp_close_socket(socketnum);
					}
					#if TCP_DEBUG
					softuart_putnewline();
					#endif

					//we must set timed wait TTL here because timed wait is not packet triggered
					if (tcp_sockets[socketnum].state == TCP_STATE_TIMED_WAIT){
						tcp_sockets[socketnum].ttl = 5; //5 seconds timeout
						#if TCP_DEBUG
						softuart_puts_progmem("TIMED_WAIT: timeout set to 5s");
						softuart_putnewline();
						#endif
					}

					return;
				}
			}
		}

	}

	//there is no active socket! -> conn request ?
	if (tcp_header_flags == TCP_SYN_FLAG){
		//yes, connection request. find a free socket:
		socketnum = tcp_find_free_socket();

		if (socketnum < TCP_SOCKET_COUNT){
			//free socket found (:= socketnum) -> handle request
			tcp_sockets[socketnum].state       = TCP_STATE_SYN_RECEIVED;
			tcp_sockets[socketnum].source_port = source_port;
			tcp_sockets[socketnum].source_ip   = source_ip;
			tcp_sockets[socketnum].dest_port   = dest_port;		
			tcp_sockets[socketnum].ack         = nic_buffer_to_seq(&buffer[TCP_POS_SEQ])+1;
			tcp_sockets[socketnum].seq         = 0;
			tcp_sockets[socketnum].ttl         = TCP_TTL_TIMEOUT;
			tcp_sockets[socketnum].misc_state  = 0;

			//send SYN+ACK Packet
			tcp_send_packet(buffer, socketnum, (TCP_SYN_FLAG|TCP_ACK_FLAG), 0);

			//init seq counter:
			tcp_sockets[socketnum].seq = 1;

			//cleanup/initialise httpd connection:
			httpd_cleanup_conn(socketnum);

			#if TCP_DEBUG
			softuart_puts_progmem("TCP : connection request. [OK]");
			softuart_putnewline();
			#endif
			return;
		}
	}

	/////////////////////////////////////////////////////
	//if we get here something went wrong ...
	// - we have no open socket for this packet
	// - this is not an connection request
	// - we have no more free sockets left (!)
	//tcp_send_packet(buffer, socketnum, TCP_RST_FLAG, 0);//NO! WE HAVE NO SOCKNUM!
	#if TCP_DEBUG
	softuart_puts_progmem("TCP : WARN: ignoring packet (no socket open)");
	softuart_putnewline();
	#endif
	return;
}

//called ~every second, removes dead tcp sockets
// - decrement ttl (starts with TCP_TTL_TIMEOUT -> TCP_TTL_TIMEOUT seconds inactivity = timeout)
// - if ttl==0 -> mark socket as closed !
void tcp_ttl_cleanup(){
	unsigned char i;
	for(i=0; i<TCP_SOCKET_COUNT; i++){
		//decrement ttl:
		if (tcp_sockets[i].state != TCP_STATE_CLOSED){
			tcp_sockets[i].ttl--;
			
			//if socket TTL count is zero, close this socket!
			if (tcp_sockets[i].ttl == 0){
				tcp_close_socket(i);

				#if TCP_DEBUG
				softuart_puts_progmem("TCP : TTL timeout for socket [");
				softuart_put_uint8(i);
				softuart_puts_progmem("]. socket closed.");
				softuart_putnewline();
				#endif
			}
		}
	}
}

void tcp_close_socket(unsigned char socketnum){
	tcp_sockets[socketnum].state = TCP_STATE_CLOSED;
	httpd_cleanup_conn(socketnum);
}

//send a tcp packet:
void tcp_send_packet(unsigned char *buffer, unsigned char socketnum, unsigned char flags, unsigned int len){
	unsigned int arp_idx;
	unsigned int tmp;
	unsigned long *ip;

	if ((tcp_sockets[socketnum].source_ip & NIC_IP_NETMASK) != (nic_ip & NIC_IP_NETMASK)){
		#if TCP_DEBUG
		softuart_puts_progmem("TCP : ip ");
		softuart_put_ip(tcp_sockets[socketnum].source_ip);
		softuart_puts_progmem(" outside home net -> use gateway\r\n");
		#endif
		ip = &nic_gateway_ip;
	}else{
		ip = &tcp_sockets[socketnum].source_ip;
	}

	arp_idx = arp_search_by_ip(*ip);

	if (arp_idx == -1){
		//target ip (gateway) unknown
		
		#if TCP_DEBUG
		softuart_puts_progmem("TCP : ERR ARP UKNWN: ");
		softuart_put_ip(tcp_sockets[socketnum].source_ip);
		#endif
		
		//normal solution:
		//arp_send_request(buffer, ip);
		//return; //<- must be called again !!!

		///FIXME
		#if TCP_DEBUG
		softuart_puts_progmem("-> calling arp'hack to add mac<->ip mapping");
		softuart_putnewline();
		#endif
		
		//but we do not want to wait for arp request etc
		//we received an ip packet and we did not touch the mac header etc yet!
		//-> the source mac is still in buffer! -> use this !
		arp_idx = arp_add_mac2ip(&buffer[ETHERNET_POS_SRC_MAC], tcp_sockets[socketnum].source_ip);
	}
	///softuart_puts_progmem("<tcp packet sent> ");
	

	//generate ip packet:
	ip_generate_packet(buffer, 
		&(tcp_sockets[socketnum].source_ip), 
		(arp_table[arp_idx].mac),
		tcp_sockets[socketnum].dest_port,
		tcp_sockets[socketnum].source_port,
		IP_PROTOCOL_TCP,
		20+len
	);

	//setup source port (our port)
	buffer[TCP_POS_SRC_PORT+0] = (tcp_sockets[socketnum].dest_port)>>8;
	buffer[TCP_POS_SRC_PORT+1] = (tcp_sockets[socketnum].dest_port) & 0xFF;

	//setup destination port (client port)
	buffer[TCP_POS_DST_PORT+0] = (tcp_sockets[socketnum].source_port)>>8;
	buffer[TCP_POS_DST_PORT+1] = (tcp_sockets[socketnum].source_port) & 0xFF;

	//setup seq nr: FIXME
	nic_seq_to_buffer(&buffer[TCP_POS_SEQ], (tcp_sockets[socketnum].seq));
	/*buffer[TCP_POS_SEQ+0] = 0x00;
	buffer[TCP_POS_SEQ+1] = 0x00;
	buffer[TCP_POS_SEQ+2] = 0x00;
	buffer[TCP_POS_SEQ+3] = 0x00;*/

	//setup ack nr: FIXME
	nic_seq_to_buffer(&buffer[TCP_POS_ACK], (tcp_sockets[socketnum].ack));

	//setup header offset
	buffer[TCP_POS_DATA_OFFSET] = 0x05<<4;

	//setup flags:
	buffer[TCP_POS_HEADERFLAGS] = flags;
	
	//setup window
	buffer[TCP_POS_WINDOWSIZE+0] = (NIC_BUFFERSIZE-20-14)>>8;
	buffer[TCP_POS_WINDOWSIZE+1] = (NIC_BUFFERSIZE-20-14)&0xFF;

	//setup checksum -> set 0x0000, will be replaced later!
	buffer[TCP_POS_CHECKSUM+0] = 0x00;
	buffer[TCP_POS_CHECKSUM+1] = 0x00;

	//setup urgend pointer (not used -> 0)
	buffer[TCP_POS_URGENT_PTR+0] = 0x00;
	buffer[TCP_POS_URGENT_PTR+1] = 0x00;


	//calc checksum: we use some tricks to do it faster:
	//step0: calculate checksum for pseudoheader (0x00 TYPE TCPLEN)
	tmp = IP_PROTOCOL_TCP + 20 + len;
	//step1: calculate checksum IP_SRC+IP_DST + TCP header&data
	//       (-2*4 -> include the 8 bytes source+dest ip)
	// -> we calculated the checksum over the "pseudoheder" and tcp header+data
	tmp = nethelp_checksum(&buffer[TCP_POS_SRC_PORT-2*4], 20+2*4 + len, tmp);

	//set checksum:
	buffer[TCP_POS_CHECKSUM+0] = tmp>>8;
	buffer[TCP_POS_CHECKSUM+1] = tmp & 0xFF;

	//send packet
	nic_send_packet(buffer, TCP_POS_URGENT_PTR+2+len);	
	
}



