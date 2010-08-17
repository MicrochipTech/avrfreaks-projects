/*-------------------------------------------------------------------
  ktms1201.h   by Jack Botner

  Collection of C functions for using the KTM-S1201 12 digit
  serial LCD module.
-------------------------------------------------------------------*/

#ifndef _KTMS1201_H_
#define _KTMS1201_H_ 1

#include <util/delay_basic.h>

// Define a delay that is 1/4 the desired LCD bit rate. This depends
// on the clock speed. _delay_loop_1 takes 3 clock cycles per iteration
// and _delay_loop_2 takes 4 clock cycles. In this example the clock
// runs at 8 mHz (125ns) and we choose a data rate of 1000 b/s, so the
// write period is 1ms. The delay we need is 1/4 of 1ms or 250us.
#define LCD_Delay()  _delay_loop_2(500)

// Define the port and pins for each LCD signal. Make sure all
// pins are initialized as output except for BUSY, which is input
#define LCD_SCK_PORT	PORTD
#define LCD_SCK_PIN		0
#define LCD_SI_PORT		PORTD
#define LCD_SI_PIN		1
#define LCD_CD_PORT		PORTD
#define LCD_CD_PIN		2
#define LCD_RST_PORT	PORTD
#define LCD_RST_PIN		3
#define LCD_BUSY_PORT	PIND
#define LCD_BUSY_PIN	4
#define LCD_CS_PORT		PORTD
#define LCD_CS_PIN		5

// Function parameters
#define BLINK_OFF		0
#define BLINK_ON_SLOW	1
#define BLINK_ON_FAST	2

// Functions

void LCD_blink( char bBlink );
void LCD_display_number( int32_t lNumber, int8_t cPosition );
void LCD_init(void);	// initialize the KTM-S1201
void LCD_insert_decimal( int8_t cPosition );
void LCD_puts( char *pcBuffer, int8_t cPosition );
void LCD_puts2( char *pcBuffer, int8_t cPosition );

#endif

