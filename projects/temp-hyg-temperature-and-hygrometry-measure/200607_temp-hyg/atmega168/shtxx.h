#ifndef SHTXX_H
#define SHTXX_H 1

#define SHTXX_TEMPERATURE 0x03
#define SHTXX_TEMPERATURE_DBG 0x13
#define SHTXX_HUMIDITY 0x05
#define SHTXX_HUMIDITY_DBG 0x15
#define SHTXX_RESET 0x1E

int16_t shtxxGetLastMeasure(uint8_t); 
void shtxxInit(void); 

#endif
