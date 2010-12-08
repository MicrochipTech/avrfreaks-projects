/**
 * lcd.c
 *
 * 4 bit interface on HITACHI LCD
 */

#include <avr/io.h>
#include <avr/pgmspace.h>

#include "lcd.h"
#include "timing.h"
#include "common.h"
#include "utils.h"

#define FS_8BIT				0x30
#define FS_4BIT				0x20
#define FS_2LINES				0x80
#define FS_1LINE				0x00
#define FS_FNT5x11			0x40
#define FS_FNT5x8 			0x00

#define LCD_H_SHIFT			0x10
#define LCD_L_SHIFT_RIGHT	0xc0
#define LCD_L_SHIFT_LEFT	0x80
#define LCD_H_EMS				0x00
#define LCD_L_EMS				0x60
#define LCD_H_HOME			0x00
#define LCD_L_HOME			0x20
#define LCD_H_CLEAR			0x00
#define LCD_L_CLEAR			0x10
#define LCD_H_OFF				0x00
#define LCD_L_OFF				0x80
#define LCD_H_ON				0x00
#define LCD_L_ON				0xc0 	/* display on, no cursor */

#define LCD_H_CGRAM_ADDR	0x40
#define LCD_H_DDRAM_ADDR	0x80

#define MAX_BATTERY 			145

#define LCD_STATE_FIXED		0x00
#define LCD_STATE_SHIFTL	0x01
#define LCD_STATE_SHIFTR	0x02
#define LCD_STATE_BLINK		0x03
#define LCD_STATE_CENTER	0x04

extern uint32_t ticks;

static struct row_states {
	uint8_t row1;
	uint8_t row2;
} rs = { 0,0 };

static uint8_t ll1;
static uint8_t ll2;
static char line1[42];
static char line2[41];

static unsigned char c0[8] PROGMEM = {
	0b00000000,
	0b00000100,
	0b00001010,
	0b00000100,
	0b00000000,
	0b00000000,
	0b00000000,
	0b00000001
};

static unsigned char c1[8] PROGMEM = {
	0b00000000,
	0b00000001,
	0b00000111,
	0b00001111,
	0b00001101,
	0b00001111,
	0b00011111,
	0b00011110
};

static unsigned char c2[8] PROGMEM = {
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00010000,
	0b00000000,
	0b00000001
};

static unsigned char c3[8] PROGMEM = {
	0b00011000,
	0b00011100,
	0b00011110,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111
};

static unsigned char c4[8] PROGMEM = {
	0b00000001,
	0b00000011,
	0b00000011,
	0b00000110,
	0b00000100,
	0b00001000,
	0b00010000,
	0b00000000
};

static unsigned char c5[8] PROGMEM = {
	0b00011000,
	0b00010001,
	0b00000011,
	0b00000111,
	0b00001111,
	0b00001111,
	0b00011111,
	0b00011111
};

static unsigned char c6[8] PROGMEM = {
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011110,
	0b00011000,
	0b00010000,
	0b00010000
};

static unsigned char c7[8] PROGMEM = {
	0b00011111,
	0b00011111,
	0b00011111,
	0b00011101,
	0b00000010,
	0b00000100,
	0b00001000,
	0b00001000
};

extern unsigned int v_battery;
extern unsigned int v_current;
extern unsigned int v_temperature;

/* 8 bit commands */

void __lcd_write (unsigned char value)
{
	__asm__ volatile (
		"clr	r16			\r\n"
		"out  %0, r16 		\r\n"
		"ori  r16, 0x04	\r\n"		/* tas */
		"out  %0, r16 		\r\n"		/* E high */
		"or   r16, %1		\r\n"		/* ter */
		"out  %0, r16 		\r\n"		/* value out now */
		"andi r16, 0xfb	\r\n"		/* tdsw */
		"out  %0, r16 		\r\n"		/* E low, apply */
		"nop	            \r\n"		/* tah */
		"nop					\r\n"		/* tcyc */
		:: "I" _SFR_IO_ADDR(PORTD), "r" (value) : "r16" );
}

void __lcd_write_ram (unsigned char value)
{
	__asm__ volatile (
		"ldi	r16, 0x01	\r\n"
		"out  %0, r16 		\r\n"
		"ori  r16, 0x04	\r\n"		/* tas */
		"out  %0, r16 		\r\n"		/* E high */
		"or   r16, %1		\r\n"		/* ter */
		"out  %0, r16 		\r\n"		/* value out now */
		"andi r16, 0xfb	\r\n"		/* tdsw */
		"out  %0, r16 		\r\n"		/* E low, apply */
		"nop	            \r\n"		/* tah */
		"nop					\r\n"		/* tcyc */
		:: "I" _SFR_IO_ADDR(PORTD), "r" (value) : "r16" );
}

void C8FunctionSet (unsigned char interface_type)
{
	__lcd_write (interface_type);
}

/* 4 bit commands */

void C4DisplayClear (void)
{
	__lcd_write (LCD_H_CLEAR);
	__lcd_write (LCD_L_CLEAR);

	msleep(2);
}

void C4BackHome(void)
{
	__lcd_write (LCD_H_HOME);
	__lcd_write (LCD_L_HOME);

	msleep(2);
}

void C4EntryModeSet (void)
{
	__lcd_write (LCD_H_EMS);
	__lcd_write (LCD_L_EMS);

	usleep(50);
}

void C4DisplayOn (void)
{
	__lcd_write (LCD_H_ON);
	__lcd_write (LCD_L_ON);

	usleep(50);
}

void C4DisplayOff (void)
{
	__lcd_write (LCD_H_OFF);
	__lcd_write (LCD_L_OFF);

	usleep(50);
}

void C4DisplayShift(void)
{
	__lcd_write (LCD_H_SHIFT);
	__lcd_write (LCD_L_SHIFT_LEFT);

	usleep(50);
}

void C4FunctionSet (char two_lines, char font5x11 )
{
	__lcd_write (FS_4BIT);
	__lcd_write (((two_lines)?FS_2LINES:FS_1LINE) | ((font5x11)?FS_FNT5x11:FS_FNT5x8));

	usleep(50);
}

/** CGRAM Address is in bytes, 0 = char 0, 8 = char 1 ... **/
void C4SetCGRAMAddr(char pos)
{
	char value = LCD_H_CGRAM_ADDR | pos;

	__lcd_write (value&0xf0);
	__lcd_write (value<<4);

	usleep(50);
}

void C4SetDDRAMAddr(char pos)
{
	char value = LCD_H_DDRAM_ADDR | pos;

	__lcd_write (value&0xf0);
	__lcd_write (value<<4);

	usleep(50);
}

void C4WriteToRam (unsigned char value)
{
	__lcd_write_ram (value&0xf0);
	__lcd_write_ram (value<<4);

	usleep(50);
}

void WriteCustomChar (volatile unsigned char *c)
{
	int i;

	for (i=0;i<8;++i)
		C4WriteToRam(pgm_read_byte_near(&c[i]));
}

void InitLcd(void)
{
	DDRD	= GLOBAL_DIRD;

	/* start with all to low */
	PORTD	= 0x00;

	msleep(100);
	C8FunctionSet(FS_8BIT);
	msleep(10);
	C8FunctionSet(FS_8BIT);
	usleep(150);
	C8FunctionSet(FS_8BIT);
	usleep(50);

	C8FunctionSet(FS_4BIT);
	C4FunctionSet(1,1);
	C4DisplayOff();
	C4DisplayClear();
	C4EntryModeSet();
	C4DisplayOn();

	C4SetCGRAMAddr(0);
	WriteCustomChar(c0);
	WriteCustomChar(c1);
	WriteCustomChar(c2);
	WriteCustomChar(c3);
	WriteCustomChar(c4);
	WriteCustomChar(c5);
	WriteCustomChar(c6);
	WriteCustomChar(c7);

	C4SetDDRAMAddr(0);
}

void LcdClear (void)
{
	rs.row1=rs.row2=0;

	C4DisplayClear();
}

void LcdWrite (char *string)
{
	while (*string) C4WriteToRam (*string++);
}

void LcdWriteAt (char *string, uint8_t line, uint8_t pos)
{
	C4SetDDRAMAddr(0x40*line+pos);

	while (*string) C4WriteToRam (*string++);
}

void LcdWrite_P (char *string)
{
	char value;

	while ((value=pgm_read_byte_near(string))!=0)
	{
		string++;
		C4WriteToRam (value);
	}
}

void LcdWriteLine_P (char *string, uint8_t line)
{
	if (line) C4SetDDRAMAddr(0x40);

	LcdWrite_P(string);
}

void LcdWriteAnimated_P (char *string, uint8_t line)
{
	char *dst;

	uint8_t len = szlen_fmp(string);

	if (line)
	{ dst=line2; ll2=len; rs.row2=LCD_STATE_SHIFTL; }
	else
	{ dst=line1; ll1=len; rs.row1=LCD_STATE_SHIFTL; }

	dst[len]=0;

	mcopy_fpm(dst, string, len);

	LcdWrite(dst);
}

static char *cptr, *cptl, lpos, rpos, lend;

void LcdWriteToCenter_P (char *string, uint8_t line)
{
	uint8_t clen = szlen_fmp(string);

	mcopy_fpm(line1, string, clen);

	cptr = &line1[clen>>1];
	cptl = cptr-1;
	lpos = -1;
	rpos = 16;

	lend = 7;

	rs.row1=LCD_STATE_CENTER;
}

void LcdDisplaySplash (void)
{
	C4SetDDRAMAddr(0);
	C4WriteToRam(0);
	C4WriteToRam(1);
	C4WriteToRam(2);
	C4WriteToRam(3);
	C4SetDDRAMAddr(5);
	LcdWrite_P(PSTR("Welcome"));
	C4SetDDRAMAddr(0x40);
	C4WriteToRam(4);
	C4WriteToRam(5);
	C4WriteToRam(6);
	C4WriteToRam(7);
	C4SetDDRAMAddr(0x45);
	LcdWrite_P(PSTR("to Airone"));
}

static void LcdShiftString (char *line, unsigned char l)
{
	char lost=line[0];

	mcopy(&line[0],&line[1],l);
	line[l-1]=lost;

	C4SetDDRAMAddr	(0x0);
	LcdWrite			(line);
}

static void LcdTextToCenter(void)
{
	if (cptl<line1) return;

	C4SetDDRAMAddr	(++lpos);
	C4WriteToRam	(*cptl);
	C4SetDDRAMAddr	(lpos-1);
	C4WriteToRam	(' ');
	C4SetDDRAMAddr	(--rpos);
	C4WriteToRam	(*cptr);
	C4SetDDRAMAddr	(rpos+1);
	C4WriteToRam	(' ');

	if (lpos==lend)
	{
		lpos=-1;
		rpos=16;

		cptr++;
		cptl--;
		lend--;
	}
}

static uint8_t bl_pos, bl_len;
static char		input [5];

void LcdUpdateInput (char *string)
{
	bl_len=szlen(string);

	mcopy (input, string, bl_len);

	input[bl_len]=0;

	/** immediate refresh on update **/
	C4SetDDRAMAddr	(bl_pos);
	LcdWrite			(input);
}

void LcdSetInput(char *string, unsigned char line, unsigned char pos)
{
	if (line)
	{ rs.row2=LCD_STATE_BLINK; bl_pos = pos+0x40; }
	else
	{ rs.row1=LCD_STATE_BLINK; bl_pos = pos; }

	LcdUpdateInput(string);
}

void LcdUpdate()
{
	static uint8_t bon=0;

	switch (rs.row1)
	{
		default: break;
		case LCD_STATE_SHIFTL:
			LcdShiftString(line1, ll1);
		break;
		case LCD_STATE_BLINK:
			C4SetDDRAMAddr(bl_pos);
			if (ticks&8)
			{
				if (!bon) { bon=1; LcdWrite(input); }
			}
			else
			{
				if (bon)
				{
					uint8_t i=0;

					bon=0;

					for (;i<bl_len;++i) C4WriteToRam(' ');
				}
			}
		break;
		case LCD_STATE_CENTER:
			LcdTextToCenter();
		break;
	}
	// TO DO, eventually animation in line 2 in intelligent way
}

static char battery[] = {"XX.XV"};
static char current[] = {"XX.XA"};
static char temp[]    = {"XX.X*C"};

static void LcdUpdateAnalogValue (char *dst, int v)
{
	char *vb=i2azp(v,3);

	dst[3]=vb[2];
	dst[1]=vb[1];
	if (vb[0]!='0') dst[0]=vb[0]; else dst[0]=' ';

	C4SetDDRAMAddr(10);

	LcdWrite (dst);
}

static void LcdUpdateAnalogBar (int v)
{
	static uint8_t old_segments=0;
			 uint8_t i,segments;

	segments = (v<<4)/MAX_BATTERY;

	if (segments>old_segments)
	{
		C4SetDDRAMAddr(0x40+old_segments);
		for (i=old_segments;i<segments;++i) C4WriteToRam ('\x6f');
	}
	else
	{
		C4SetDDRAMAddr(0x40+segments);
		for (i=segments;i<old_segments;++i) C4WriteToRam ('\x5f');
	}
	old_segments=segments;
}

void LcdSetupAnalogBar (int v)
{
	uint8_t i,segments;

	segments = (v<<4)/MAX_BATTERY;

	C4SetDDRAMAddr(0x40);
	for (i=0;i<segments;++i)	C4WriteToRam ('\x6f');
	for (i=segments;i<16;++i)	C4WriteToRam ('\x5f');
}

void LcdShowScreenBattery (void)
{
	LcdUpdateAnalogValue (battery, v_battery);
	LcdSetupAnalogBar 	(v_battery);
}

void LcdShowScreenCurrent (void)
{
	LcdUpdateAnalogValue (current, v_current);
	LcdSetupAnalogBar 	(v_current);
}

void LcdShowScreenTemperature (void)
{
	LcdUpdateAnalogValue (temp, v_temperature);
}

void LcdUpdateBattery (void)
{
	LcdUpdateAnalogValue (battery, v_battery);
	LcdUpdateAnalogBar	(v_battery);
}

void LcdUpdateTemp (void)
{
	LcdUpdateAnalogValue (temp, v_temperature);
}

void LcdUpdateCurrent (void)
{
	LcdUpdateAnalogValue	(current, v_current);
	LcdUpdateAnalogBar	(v_current);
}

extern struct time t;

/**
 * show or updating time / date without flockering
 **/

void LcdShowDateTime (void)
{
	LcdClear();

	C4SetDDRAMAddr(3);

	LcdWrite(i2azp(t.day,2));
	C4WriteToRam('.');
	LcdWrite(i2azp(t.month,2));
	C4WriteToRam('.');
	LcdWrite(i2azp(t.year,4));

	C4SetDDRAMAddr(0x44);
	LcdWrite(i2azp(t.hour,2));
	C4WriteToRam(':');
	LcdWrite(i2azp(t.minute,2));
	C4WriteToRam(':');
	LcdWrite(i2azp(t.second,2));
}

struct time t_old = {0xff,0xff,0xff,0xff,0xff,0xffff};

void LcdUpdateDateTime (void)
{
	if (t_old.day!=t.day)
	{
		C4SetDDRAMAddr(3);
		LcdWrite(i2azp(t.day,2));
		t_old.day=t.day;
	}
	if (t_old.month!=t.month)
	{
		C4SetDDRAMAddr(6);
		LcdWrite(i2azp(t.month,2));
		t_old.month=t.month;
	}
	if (t_old.year!=t.year)
	{
		C4SetDDRAMAddr(9);
		LcdWrite(i2azp(t.year,4));
		t_old.year=t.year;
	}

	if (t_old.hour!=t.hour)
	{
		C4SetDDRAMAddr(0x44);
		LcdWrite(i2azp(t.hour,2));
		t_old.hour=t.hour;
	}
	if (t_old.minute!=t.minute)
	{
		C4SetDDRAMAddr(0x47);
		LcdWrite(i2azp(t.minute,2));
		t_old.minute=t.minute;
	}
	if (t_old.second!=t.second)
	{
		C4SetDDRAMAddr(0x4a);
		LcdWrite(i2azp(t.second,2));
		t_old.second=t.second;
	}
}
