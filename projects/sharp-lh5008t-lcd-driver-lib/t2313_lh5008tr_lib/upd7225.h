//biblioteka do obslugi wyswietlacza LCD na sterowniku NEC uPD7225 (SHARP LH5008TR)
//Na podstawie: Emilio (Pietro Giovanni) Ficara
 
#include <avr/interrupt.h> 
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>


#ifndef UPD7225_H
#define UPD7225_H



/*---------------------------------------------------------------------------|
|CHIP pinout and I/O map (ATMEGA32 40 Pin DIP)                               |
|----------------------------------------------------------------------------|
|Pin    |   29   |   28   |   27   |   26   |   25  |   24  |   23  |    22  |
|----------------------------------------------------------------------------|
|Port   |  PC.7  |  PC.6  |  PC.5  |  PC.4  |  PC.3 |  PC.2 |  PC.1 |  PC.0  |
|Func   | [TOSC2]| [TOSC1]| [TDI]  | [TDO]  | [TMS] |  [TCK]|  [SDA]|   [SCL]|
|I/O    | cl1    |        | /busy  | c-/d   | /res  |  /sck |  si   |   /cs  |
|---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------|
|CHIP pinout and I/O map (ATTINY2313 28 Pin DIP)                             |
|----------------------------------------------------------------------------|
|Pin    |   19   |   18   |   17   |   16   |   15  |   14  |   13  |    12  |
|----------------------------------------------------------------------------|
|Port   |  PB.7  |  PB.6  |  PB.5  |  PB.4  |  PB.3 |  PB.2 |  PB.1 |  PB.0  |
|Func   |        |        |        |        |       |       |       |        |
|I/O    | cl1    |        | /busy  | c-/d   | /res  |  /sck |  si   |   /cs  |
|---------------------------------------------------------------------------*/

#define cs_pin		0	// 	/CS
#define si_pin 		1	// 	SI
#define sck_pin		2	// 	/SCK
#define res_pin		3	// 	/RESET
#define cd_pin		4	// 	/CD
#define cl1_pin		7	// 	CL1

#define busy_pin 	5	//	/BUSY

#define 	LCDPORT 	PORTB
#define 	LCDPIN		PINB
#define 	LCDDDR		DDRB

#define SET_LCD_CS	LCDPORT |=	_BV(cs_pin);
#define CLR_LCD_CS	LCDPORT &=  ~_BV(cs_pin);

#define SET_LCD_SI	LCDPORT |=	_BV(si_pin);
#define CLR_LCD_SI	LCDPORT &=  ~_BV(si_pin);

#define SET_LCD_SCK	LCDPORT |=	_BV(sck_pin);
#define CLR_LCD_SCK	LCDPORT &=  ~_BV(sck_pin);

#define SET_LCD_RES	LCDPORT |=	_BV(res_pin);
#define CLR_LCD_RES	LCDPORT &=  ~_BV(res_pin);

#define SET_LCD_CD	LCDPORT |=	_BV(cd_pin);
#define CLR_LCD_CD	LCDPORT &=  ~_BV(cd_pin);

#define SET_LCD_CL1	LCDPORT |=	_BV(cl1_pin);
#define CLR_LCD_CL1	LCDPORT &=  ~_BV(cl1_pin);
#define FLP_LCD_CL1 LCDPORT ^= 	_BV(cl1_pin);

// LCD commands definition
#define DISPLAY_OFF     0x10
#define DISPLAY_ON      0x11
#define WITHOUT_DECODER 0x14
#define WITH_DECODER    0x15
#define BLINK_OFF       0x18
#define BLINK_SLOW      0x1a
#define BLINK_FAST      0x1b
#define CLEAR_MEM       0x20
#define MODE_SET        0x40
#define WRITE_DATA      0xd0
#define WRITE_BLINKING	0xc0
#define LOAD_DP         0xe0


//PROTOTYPY
void SetDataPointer(char pos);
void WriteData(char data);
void InitLcd();
void InitPortToLCD();
void InitVariablesToLCD();
void Startx(char type);
void Stoptx();
void SIout(char byte);

void LcdCommand(char command);
void lcd_putc(uint8_t index);
void lcd_putc_p(uint8_t index, uint8_t dot);
void lcd_putc_e(uint8_t index);
void write_text(char *s);
void write_digits(char *s);


#endif 
