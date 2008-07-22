// library trimmed and ported to avg-gcc using SPI

//********************************************************************
// Library for display control over a serial shift register
// Author: G. Burger
// Compiler: C2C, Pavel Baranov
//********************************************************************

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "config.h"
#include "lcd_spi.h"

// from main.c: (the avr-libc delay function seems to take too much space)
void my_delay(uint8_t ms);

#define PinLCD_BKLT         0x08
#define PinLCD_SEL          0x04
#define PinLCD_WR           0x02
#define PinLCD_RS           0x01

#define clear_lcd           0x01 /* Clear Display                     */
#define return_home         0x02 /* Cursor to Home position           */
#define entry_mode          0x06 /* Normal entry mode                 */
#define entry_mode_shift    0x07 /* - with shift                      */
#define system_set_8_bit    0x38 /* 8 bit data mode 2 line (5x7 font) */
#define system_set_4_bit    0x28 /* 4 bit data mode 2 line (5x7 font) */
#define display_on          0x0c /* Switch ON Display                 */
#define display_off         0x08 /* Cursor plus blink                 */
#define set_dd_line1        0x80 /* Line 1 position 1                 */
#define set_dd_line2        0xC0 /* Line 2 position 1                 */
#define set_dd_ram          0x80 /* Line 1 position 1                 */
#define write_data          0x00 /* With rs = 1                       */
#define cursor_on           0x0E /* Switch Cursor ON                  */
#define cursor_off          0x0C /* Switch Cursor OFF                 */

uint8_t SRData = 0;
uint8_t column = 0;

static void init_port() {
	LCD_CS_DDR |= _BV(LCD_CS_BIT);
}

static void write_port(uint8_t val) {
	cli(); // disable interrupts so we can use SPI in the interrupt routines
	SPCR &= ~(_BV(CPOL) | _BV(CPHA));
	SPDR = val;
	while(!(SPSR & (1<<SPIF)));
	LCD_CS_PORT |=  _BV(LCD_CS_BIT);
	LCD_CS_PORT &= ~_BV(LCD_CS_BIT);
	sei();
}

static void set_bitLcd(uint8_t BitNr) {
	SRData = SRData | BitNr;
	write_port(SRData);
}

static void clear_bitLcd(uint8_t BitNr) {
	SRData = SRData & ~BitNr;
	write_port(SRData);
}

static void WriteNib(uint8_t nib) {
	SRData = (SRData & 0x0f) | (nib << 4);

	write_port(SRData);
	write_port(SRData | PinLCD_SEL);
	write_port(SRData);

	wdt_reset();
}

static void Write_8_Bit(uint8_t d) {
	WriteNib(d >> 4);
}

static void Write_4_Bit(uint8_t d) {
	WriteNib(d >> 4);
	WriteNib(d);
}

static void DataMode(void) {
	set_bitLcd(PinLCD_RS);
}

static void FunctionMode(void) {
	clear_bitLcd(PinLCD_RS);
}

void LcdBacklight(uint8_t state) {
	if(state) set_bitLcd(PinLCD_BKLT);
	else clear_bitLcd(PinLCD_BKLT);
}

void LcdClear(void) {
	FunctionMode();
	Write_4_Bit(clear_lcd);
	DataMode();
}

void LcdSetup(void) {
	init_port();

	SRData=0;
	write_port(SRData);

	FunctionMode();
	Write_4_Bit(system_set_8_bit);
	DataMode();

	my_delay(30);
	FunctionMode();
	Write_8_Bit(system_set_4_bit); /* This sequence resets the LCD */
	my_delay(5);
	Write_8_Bit(system_set_4_bit);
	my_delay(5);
	Write_8_Bit(system_set_4_bit);
	my_delay(5);

	Write_4_Bit(system_set_4_bit);
	my_delay(5);

	Write_4_Bit(display_off);
	my_delay(2);
	Write_4_Bit(entry_mode);
	my_delay(2);
	Write_4_Bit(display_on);
	my_delay(2);
	Write_4_Bit(set_dd_ram);
	my_delay(2);
	LcdClear();

	DataMode();

	my_delay(250);
}

void LcdSetPos(uint8_t pos) {
	FunctionMode();
	Write_4_Bit(0x80 | pos);
	DataMode();
	column = pos;
}

void LcdLine1(void) {
	FunctionMode();
	Write_4_Bit(set_dd_line1);
	DataMode();
	column = 0;
}

void LcdLine2(void) {
	FunctionMode();
	Write_4_Bit(set_dd_line2);
	DataMode();
	column = 0;
}

void LcdClearLine1(void) {
	LcdLine1();
	uint8_t i;
	for(i=0; i<20; i++) LcdWriteChar(' ');
	LcdLine1();
}

void LcdClearLine2(void) {
	LcdLine2();
	uint8_t i;
	for(i=0; i<20; i++) LcdWriteChar(' ');
	LcdLine2();
}

void LcdWriteStr(const char *s) {
	while(*s) LcdWriteChar(*(s++));
}

void LcdWriteStr_p(const char *s) {
	char c;
	while((c = pgm_read_byte(s++))) LcdWriteChar(c);
}

void LcdWriteChar(char num) {
	Write_4_Bit(num);
	column++;
}

void LcdClearToEnd() {
	while(column<20) LcdWriteChar(' ');
}
