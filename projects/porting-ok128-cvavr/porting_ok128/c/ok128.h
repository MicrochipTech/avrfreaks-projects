/****************************************************************************************
*	DISCLAIMER:
*	
*	This library was ported from original code written by Duck-Yong Yoon in 2005.
*	The original platform was for AvrEdit and ported to CVAVR.
*	The purpose of this is for educational only, I am not
*	responsible for any damages caused by this library.
*	
*	These files ok128.lib and ok128.h are actually was ported (also added) and written
*	for specific use for OK128 ATMEGA128 toolbox kit from www.ohm.co.kr, and was used in
*	Mechatronics and Experiments in our lab robot.gsnu.ac.kr
*	
*	OK128, CVAVR and AVREDIT are both belong to their respective owner.
*	
*	Bondhan Novandy
*	http://bono02.wordpress.com/ 
*	
*	NOTE: - Copy ok128.lib into drive:\>cvavr\lib, and ok128.h  into drive:\>cvavr\inc
*	      - Don't forget to set the external memory to 61184 bytes in project settings
*	
****************************************************************************************/

#ifndef _OK128_H
#define _OK128_H

#define LCD_DATABUS	(*(volatile unsigned char *)0x2000) // LCD/GLCD data
							    // 7-segment LED pattern
#define LCD_CONTROL	(*(volatile unsigned char *)0x2100) // LCD/GLCD control signal
#define DIG_SELECT	(*(volatile unsigned char *)0x2200) // 7-segment LED digit select
							    // 8-bit parallel data

#define RTC_SECOND	(*(volatile unsigned char *)0x2300) // DS12C887 RTC
#define RTC_SEC_ALM	(*(volatile unsigned char *)0x2301)
#define RTC_MINUTE	(*(volatile unsigned char *)0x2302)
#define RTC_MIN_ALM	(*(volatile unsigned char *)0x2303)
#define RTC_HOUR	(*(volatile unsigned char *)0x2304)
#define RTC_HOUR_ALM	(*(volatile unsigned char *)0x2305)
#define RTC_WEEKDAY	(*(volatile unsigned char *)0x2306)
#define RTC_DATE	(*(volatile unsigned char *)0x2307)
#define RTC_MONTH	(*(volatile unsigned char *)0x2308)
#define RTC_YEAR	(*(volatile unsigned char *)0x2309)
#define RTC_A 		(*(volatile unsigned char *)0x230A)
#define RTC_B		(*(volatile unsigned char *)0x230B)
#define RTC_C		(*(volatile unsigned char *)0x230C)
#define RTC_D		(*(volatile unsigned char *)0x230D)
#define RTC_FLAG	(*(volatile unsigned char *)0x230F) 

//For input key PINF
#define SW1	0xE0
#define SW2	0xD0
#define SW3	0xB0
#define SW4	0x70

#define _BV(bit) (1 << (bit))

#pragma used+
void MCU_initialize(void);
void Beep(void);
void LCD_command(unsigned char command);
void LCD_initialize(void); 
int LCD_clear(void);
void LCD_data(unsigned char data);                         
int LCD_putchar(char c);		                /* print a character to LCD */
int LCD_putanychar(char c);		                /* print a character to LCD */
void LCD_string(unsigned char command, char flash *string);
void LCD_putsf(char flash *string); /* display a string on LCD */
void LCD_puts(char *string); /* display a string on LCD */
unsigned char key_input(void);			/* input key SW1 - SW4 */
void LCD_8bin(unsigned char number);             /* display 8-bit binary number */
void LCD16_gotxy_ok128(int x, int y);
void LCD_SetFont(void);

//additional functions
void LCD_2d(unsigned char number);
void LCD_3d(unsigned int number);
void LCD_4d(unsigned int number);
void LCD_2hex(unsigned char number);
void LCD_4hex(unsigned int number);
void LCD_1d1(float number);	//haven't been tested
void LCD_2d2(float number);   //haven't been tested
void LCD_5d(unsigned int number); //haven't been tested
unsigned char Bin2LED(unsigned char number);     /* convert binary to LED pattern */
void LCD_2d1(float number);
void LCD_s1d2(float number);
void Dsp_voltage(unsigned char number); 
void temperature(void);
void dimmer(void);
void running_7segmen(void);
void ambulance(void);
void kedip(void);
#pragma used-

#pragma library ok128.lib
#endif