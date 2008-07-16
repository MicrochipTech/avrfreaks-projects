#ifndef _SERVO_H_
#define _SERVO_H_
#include "../config.h"
extern unsigned char servo_pos;
void servo_init(void);
void servo_set_pos(unsigned char servo_pos);
unsigned int servo_generate_bmp(unsigned char *buffer, unsigned int datapos, unsigned int len, unsigned int streampos, unsigned char source);
#endif

