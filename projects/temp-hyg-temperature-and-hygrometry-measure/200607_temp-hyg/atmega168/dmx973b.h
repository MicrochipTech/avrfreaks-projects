#ifndef DMX973B_H
#define DMX973B_H 1

#define DMX973B_DISPLAY_ON 	0xAF 
#define DMX973B_DISPLAY_OFF	0xAE 
#define DMX973B_SET_BIAS	0xA3
#define DMX973B_SET_ADC		0xA0
#define DMX973B_SET_CONTRAST	0x88
#define DMX973B_SET_LINE	0xB0
#define DMX973B_SET_COL_H	0x11
#define DMX973B_SET_COL_L	0x00
#define DMX973B_SET_POWER_CTRL	0x2F

void dmx973bWriteData(uint8_t);
void dmx973bSetLocation(uint8_t, uint8_t);
void dmx973bInit(void);

#define lcdHardwareInit() dmx973bInit()
#define lcdHardwareSetLocation(column, line) dmx973bSetLocation(column, line)
#define lcdHardwareWriteData(data) dmx973bWriteData(data)

#endif
