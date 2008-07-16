#ifndef _LM75_H_
#define _LM75_H_
#include "../main.h"
extern volatile unsigned char lm75_state;
extern volatile unsigned int lm75_temp_tmp;

void lm75_init(void);
void lm75_send_start(void);
void lm75_send_stop(void);
void lm75_send_device_address(unsigned char addr);
void lm75_send_byte(unsigned char byte);
unsigned char lm75_read(void);
void lm75_write(unsigned char data);
//int lm75_read_temp(void);
unsigned char lm75_nonblocking_temp_read(unsigned char *data);
#endif
