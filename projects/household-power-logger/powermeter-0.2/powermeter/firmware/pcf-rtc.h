//#define INCLUDE_PCF_SETTER 1

#include "config.h"

typedef struct {
	uint8_t ticker;
	uint8_t hundredths;
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t date;
	uint8_t month;
} pcf_time_t;

void pcf_read_time(uint8_t addr, pcf_time_t *time_buf);
void pcf_write_time(uint8_t addr, pcf_time_t *time_buf);
#if INCLUDE_PCF_SETTER
void rtc_set_from_serial(uint8_t addr);
void rtc_string_for_setter(uint8_t addr, char *str); 
void rtc_set_from_string(uint8_t addr, char *str); 
#endif
void rtc_print_pretty_date(pcf_time_t *time_buf);
void rtc_print_pretty_time(pcf_time_t *time_buf);
