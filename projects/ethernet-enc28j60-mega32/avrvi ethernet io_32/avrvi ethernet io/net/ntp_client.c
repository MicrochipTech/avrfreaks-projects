/*
,-----------------------------------------------------------------------------------------.
| net/ntp_client
|-----------------------------------------------------------------------------------------
| this file describes a very simple ntp client.
| - it does not check the UDP checksum
| - it does not really do the ntp sequence, it justs sends ONE request
|   and uses the reply timestamp as valid clock (-> network delay -> wrong clock)
|   (i do not really need a very accurate clock...)
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
#include "ntp_client.h"

//ACTIVATE DEBUG by editing this file:
#include "../debug.h"

uint32_t ntp_client_server_ip;
char ntp_client_call_again;

void ntp_client_init(){
	ntp_client_server_ip = NTP_SERVER_IP_ADDRESS;
	ntp_client_call_again = 0;
}

void ntp_client_send_request(unsigned char *buffer){
	unsigned int i;

	//set NTP flags (client)
	buffer[NTP_POS_FLAGS] = 0xE3;

	//set peer clock stratum to unspecified
	buffer[NTP_POS_STRATUM] = 0x00;

	//set poll intervall to 4 (=16sec)
	buffer[NTP_POS_POLL_INTERVAL] = 0x04;

	//set precision (??)
	buffer[NTP_POS_PRECISION] = 0xFA;

	//clear data buffer:
	//-> we only need to set some values because many data is 0x00
	for(i=NTP_POS_ROOT_DELAY; i<NTP_POS_LAST_DATA; i++)
		buffer[i] = 0x00;

	//set root delay to 1s -> 0x00 0x01 0x00 0x00
	buffer[NTP_POS_ROOT_DELAY+1] = 0x01; //rest is zero, see above

	//set clock dispersion to 1s -> 0x00 0x01 0x00 0x00
	buffer[NTP_POS_CLOCK_DISP+1] = 0x01; //rest is zero, see above

	//reference clock id = 0x00 0x00 0x00 0x00
	//reference clock update time NULL = 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
	//originate timestamp         NULL= 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
	//receive timestamp           NULL= 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
	
	//tx timestamp:
	//send NULL ... MAYBE add timestamp calc from localclock here
	//tx timestamp           NULL= 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00

	//generate an udp packet:
	unsigned char *dest_mac;
	unsigned int arp_idx;
	unsigned long *server_ip;

	//target ip in the same network ?!
	if ((ntp_client_server_ip & NIC_IP_NETMASK) != (nic_ip & NIC_IP_NETMASK)){
		#if NTP_CLIENT_DEBUG
		softuart_puts_progmem("NTP : ntp server ");
		softuart_put_ip(ntp_client_server_ip);
		softuart_puts_progmem(" outside home net -> use gateway\r\n");
		#endif
		server_ip = &nic_gateway_ip;
	}else{
		server_ip = &ntp_client_server_ip;
	}
	
	arp_idx = arp_search_by_ip(*server_ip);

	if (arp_idx == -1){
		//target ip (gateway) unknown
		//->send arp request and exit,
		//this is called later again and we hopefully know the mac then
		arp_send_request(buffer, server_ip);
		
		//ntp client must be called again after we finished the arp comm!
		ntp_client_call_again = 1;
		return;
	}
	//no need to be called again, we know the mac address of gateway
	ntp_client_call_again = 0;

	dest_mac = (arp_table[arp_idx].mac);

	/*dest_mac[0] = 0x00;
	dest_mac[1] = 0xE0;
	dest_mac[2] = 0x4C;
	dest_mac[3] = 0x6A;
	dest_mac[4] = 0xBF;
	dest_mac[5] = 0xDE;*/

	udp_generate_packet(buffer, &ntp_client_server_ip, dest_mac , IP_PORT_NTP_CLIENT, IP_PORT_NTP, NTP_DATALENGTH);

	//send packet:
	nic_send_packet(buffer, 14+(IP_HEADER_LENGTH<<2)+8+NTP_DATALENGTH);
}

//decode an ntp packet & extract time data
//this is called after ip+udp decidet this packet is valid & is for us.
//-> ip+udp checks are done before (!)
void ntp_client_decode_packet(unsigned char *buffer, unsigned int len){
	unsigned long timestamp;
	//check for valid flags, should be:
	//- NTP Version 4
	//- Server Mode
	//-> ignore Leap indicator
	if ((buffer[NTP_POS_FLAGS]&0x3F) != NTP_FLAG_NTP4_SERVER)
		return;

	//extract timestamp
	//NOTE: because we do not really need the exact clock, we
	//      only use the TX timestamp and hope that the transfer
	//      did not take too long -> maybe fix this later
	//      (and we dont use the microseconds...)
	timestamp = buffer[NTP_POS_TX_TIMESTAMP+0]<<8;
	timestamp = (timestamp + buffer[NTP_POS_TX_TIMESTAMP+1])<<8;
	timestamp = (timestamp + buffer[NTP_POS_TX_TIMESTAMP+2])<<8;
	timestamp = (timestamp + buffer[NTP_POS_TX_TIMESTAMP+3]);

	#if NTP_CLIENT_DEBUG
	softuart_puts_progmem("NTP : timestamp is ");
	for(int i=0; i<4; i++){
		softuart_put_uint8(buffer[0x52+i]);
		softuart_putc(' ');
	}
	softuart_putnewline();
	#endif
	unsigned char date[6];
	ntp_client_timestamp_to_time(timestamp, date);

	#if NTP_CLIENT_DEBUG
	//german format !
	softuart_puts_progmem("DATE: ");
	softuart_put_uint8(date[NTP_TIME_DAY]);
	softuart_putc('.');
	softuart_put_uint8(date[NTP_TIME_MONTH]);
	softuart_putc('.');
	softuart_put_uint16(1900 + date[NTP_TIME_YEAR]);
	
	softuart_puts_progmem(" TIME: ");
	softuart_put_uint8(date[NTP_TIME_HOUR]);
	softuart_putc(':');
	softuart_put_uint8(date[NTP_TIME_MIN]);
	softuart_putc(':');
	softuart_put_uint8(date[NTP_TIME_SEC]);
	softuart_puts_progmem(" (UTC!)");
	softuart_putnewline();
	#endif

	clock_do();
	clock[CLOCK_HOUR] = date[NTP_TIME_HOUR]+2;
	clock[CLOCK_MIN] = date[NTP_TIME_MIN];
	clock[CLOCK_SEC] = date[NTP_TIME_SEC];	
}

//lookup table
PROGMEM char NTP_CLIENT_MONTH_TO_DAYS[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//convert unix timestamp to date/time.
//returns: year,month,day,hour,min,sec (year ist realyear-1970)
//
//inspired by code posted on mikrocontroller.net by peter dannegger:
// see http://www.mikrocontroller.net/forum/read-4-140631.html#new
void ntp_client_timestamp_to_time(unsigned long timestamp, unsigned char *time){
	unsigned char tmp;
	unsigned char tmp_year;
	unsigned int tmp_day;
	unsigned char leap400;

	//calculate seconds:
	time[NTP_TIME_SEC] = timestamp % 60;
	timestamp = timestamp / 60;

	//calculate minutes:
	time[NTP_TIME_MIN] = timestamp % 60;
	timestamp = timestamp / 60;

	//calculate hours:
	time[NTP_TIME_HOUR] = timestamp % 24;
	timestamp = timestamp / 24;

	tmp_year = 0;
  leap400 = 4;

	//find day of year:
	while(1){
		tmp_day = 365;

		//leap year ?! (only check 3yr exception)
		//FIXME: add 400yr exception handling !
		if((tmp_year & 0x03)==0){
			tmp_day = 365 + 1; //yes!
			
			//100/400 yr exception:
			if(tmp_year == 0 || tmp_year == 100 || tmp_year == 200) //100yr exc
				if (--leap400)
					tmp_day = 365;
		}

		//is the rest smaller than the next whole year ? 
		if( timestamp < tmp_day )
			break; //yes -> exit

		//substract daycount of current year from timestamp
		timestamp -= tmp_day;
		
		//increase year:
		tmp_year++;
	}
	
	//tmp_year is now (Ynow-1900)
	time[NTP_TIME_YEAR] = tmp_year;

	//timestamp is now how many days of the next year have
	//passed. -> calc month & day
	if ((tmp_day == 365) && timestamp > (30+28))
		timestamp++; //no leap year ! -> skip 29th feb

	//31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	//calculate month:
	PGM_P mtd_ptr = NTP_CLIENT_MONTH_TO_DAYS;
	for(tmp=1; tmp<12; tmp++){
		timestamp -= pgm_read_byte(mtd_ptr++);
		if (timestamp < pgm_read_byte(mtd_ptr))
			break;
	}
	
	//days are now in timestamp
	time[NTP_TIME_DAY] = timestamp;

	//current month is in tmp
	time[NTP_TIME_MONTH] = tmp+1;

	//finished
}
