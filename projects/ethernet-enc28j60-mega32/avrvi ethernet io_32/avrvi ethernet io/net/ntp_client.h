#ifndef _NTP_CLIENT_H_
#define _NTP_CLIENT_H_

#include "../config.h"
#include "mynic.h"
#include "../io/clock.h"
#include <stdint.h>
#include <avr/pgmspace.h>

#define NTP_DATALENGTH 48

#define NTP_POS_FLAGS          (UDP_POS_DATA + 0)
#define NTP_POS_STRATUM        (UDP_POS_DATA + 1)
#define NTP_POS_POLL_INTERVAL  (UDP_POS_DATA + 2)
#define NTP_POS_PRECISION      (UDP_POS_DATA + 3)
#define NTP_POS_ROOT_DELAY     (UDP_POS_DATA + 4)
#define NTP_POS_CLOCK_DISP     (UDP_POS_DATA + 8)
//...
#define NTP_POS_TX_TIMESTAMP   (UDP_POS_DATA + 40)
#define NTP_POS_LAST_DATA      (UDP_POS_DATA + NTP_DATALENGTH)

#define NTP_FLAG_NTP4_SERVER   0x24

#define NTP_TIME_YEAR  0 
#define NTP_TIME_MONTH 1
#define NTP_TIME_DAY   2
#define NTP_TIME_HOUR  3
#define NTP_TIME_MIN   4
#define NTP_TIME_SEC   5

extern PROGMEM char NTP_MONTH_TO_DAYS[12];
extern uint32_t ntp_server_ip;
extern char ntp_client_call_again;
//ntp client init 
void ntp_client_init(void);

//send NTP request
void ntp_client_send_request(unsigned char *buffer);

//decode an ntp packet & extract time data
void ntp_client_decode_packet(unsigned char *buffer, unsigned int len);

//convert unix timestamp to date/time.
void ntp_client_timestamp_to_time(unsigned long timestamp, unsigned char *time);

#endif
