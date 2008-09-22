#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <avr/pgmspace.h>

#include "config.h"
#include "serial.h"
#include "twi-eeprom.h"
#include "pcf-rtc.h"

void pcf_read_time(uint8_t addr, pcf_time_t *time_buf) {
#if USE_RTC
	// FIXME - check error code
	ee24xx_read_bytes(0, sizeof(pcf_time_t), (uint8_t *)time_buf);
#endif
}

void pcf_write_time(uint8_t addr, pcf_time_t *time_buf) {
#if USE_RTC
	// FIXME - check error code
	ee24xx_write_page(0, sizeof(pcf_time_t), (uint8_t *)time_buf);
#endif
}

#if INCLUDE_PCF_SETTER
void rtc_set_from_serial(uint8_t addr) {
	printP("Enter time:\nMMDDHHMMSS\r");

	pcf_time_t time_buf;
	pcf_read_time(addr, &time_buf);
	uint8_t *time_bytes = (uint8_t *)&time_buf;
	// FIXME - year
	for(int8_t idx=6; idx>=2; idx--) {
		char c = getchr();
		putchr(c);
		if(c < '0' || c > '9') goto error;
		time_bytes[idx] = (c-'0') << 4;
		c = getchr();
		putchr(c);
		if(c < '0' || c > '9') goto error;
		time_bytes[idx] += (c-'0');
	}
	time_bytes[1] = 0; // hundredths
	pcf_write_time(0, &time_buf);

	printP("\nClock set.\n");
	return;

	error:
		printP("\nError.\n");
		return;
}

void rtc_string_for_setter(uint8_t addr, char *str) { 
	pcf_time_t time_buf;
	pcf_read_time(addr, &time_buf);

	*(str++) = ((time_buf.month & 0x10) >> 4) + '0';
	*(str++) = ( time_buf.month & 0xf) + '0';
	*(str++) = '-';
	*(str++) = ((time_buf.date & 0x30) >> 4) + '0';
	*(str++) = ( time_buf.date & 0xf) + '0';
	*(str++) = ' ';
	*(str++) = (time_buf.hour >> 4) + '0';
	*(str++) = (time_buf.hour & 0xf) + '0';
	*(str++) = ':';
	*(str++) = (time_buf.minute >> 4) + '0';
	*(str++) = (time_buf.minute & 0xf) + '0';
	*(str++) = ':';
	*(str++) = (time_buf.second >> 4) + '0';
	*(str++) = (time_buf.second & 0xf) + '0';
	*(str++) = 0;
}

void rtc_set_from_string(uint8_t addr, char *str) { 
	pcf_time_t time_buf;
	pcf_read_time(addr, &time_buf); // read to get config bits

	time_buf.month &= 0xe0;
	time_buf.month |= (str[0] - '0') << 4;
	time_buf.month |= (str[1] - '0');

	time_buf.date  &= 0xc0;
	time_buf.date |= (str[3] - '0') << 4;
	time_buf.date |= (str[4] - '0');

	time_buf.hour  = (str[6] - '0') << 4;
	time_buf.hour |= (str[7] - '0');

	time_buf.minute  = (str[9] - '0') << 4;
	time_buf.minute |= (str[10] - '0');

	time_buf.second  = (str[12] - '0') << 4;
	time_buf.second |= (str[13] - '0');

	time_buf.hundredths = 0;

	pcf_write_time(0, &time_buf);
}
#endif

void rtc_print_pretty_date(pcf_time_t *time_buf) {
	uint8_t month = (time_buf->month & 0xf) + (time_buf->month & 0x10 ? 10 : 0);
	if(month < 1 || month > 12) {
		printP("???");
	} else {
		const char *months = PSTR(
			"Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec");
		printstr_p(months + (month-1)*4);
	}
	putchr(' ');
	print_hex8(time_buf->date & 0x3f);
}

void rtc_print_pretty_time(pcf_time_t *time_buf) {
	print_hex8(time_buf->hour);
	putchr(':');
	print_hex8(time_buf->minute);
	putchr(':');
	print_hex8(time_buf->second);
	putchr('.');
	print_hex8(time_buf->hundredths);
}

