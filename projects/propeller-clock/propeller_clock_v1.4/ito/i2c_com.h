/*
 * i2c_com.h
 *
 * Created: 29-05-2011 00:58:50
 *  Author: limapapy
 */ 


#ifndef I2C_COM_H_
#define I2C_COM_H_
#include "globals.h"
unsigned char sec;

void start_TWI_RTC(void);
void stop_TWI_RTC(void);
void rep_start_TWI_RTC(void);
void send_addr_TWI_RTC(unsigned char RTC_ADDR);
void send_ptr_value_TWI_RTC(unsigned char ptr_value);
void send_DATA_TWI_RTC(unsigned char rtc_data);
unsigned char readACK_DATA_TWI_RTC(void);
unsigned char readNACK_DATA_TWI_RTC(void);
void set_clockRTC(unsigned int second,unsigned int minut,unsigned int hour);
void get_clock(timep_t *relogio);
#endif /* I2C_COM_H_ */