#ifndef I2C_H
#define I2C_H

#include <progmem.h>

#ifndef F_CPU
#define F_CPU                         16000000   /* CPU clock frequency for STK500 */
#endif

#define I2c_not_init -10

extern unsigned char I2C_Init(uint32_t SCL_Speed);
extern unsigned char I2C_Readdata1(uint8_t add, unsigned char *data);
extern unsigned char I2C_Readdata(uint8_t add,unsigned char *data, uint8_t length);

extern unsigned char I2C_Writedata (unsigned char add,unsigned char *data, uint8_t length);
extern unsigned char I2C_Writedata1 (unsigned char add,unsigned char data);






#endif
