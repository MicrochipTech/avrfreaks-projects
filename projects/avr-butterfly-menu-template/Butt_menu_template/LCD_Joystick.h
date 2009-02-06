/*
               BUTTLCD -- Butterfly LCD Driver

               Copyright (C) Dean Camera, 2008

            dean [at] fourwalledcubicle [dot] com
                  www.fourwalledcubicle.com

				  stuff added for flashing digits, joystick driver and countdown timers for joystick
				  debounce and autorepeat, also countdown for buzzer tone duration.
				  S. James Remington sjames_remington@yahoo.com  1/2008 to 2/2009
*/

#ifndef LCDDRIVER_H
#define LCDDRIVER_H

   // INCLUDES:
   #include <avr/io.h>
   #include <avr/pgmspace.h>
   #include <avr/interrupt.h>
   #include <stdbool.h>
   
   // EXTERNAL VARIABLES:
   extern volatile uint8_t ScrollFlags;
   
   // DEFINES:
   #define LCD_LCDREGS_START          ((uint8_t*)&LCDDR0)
   #define LCD_SPACE_OR_INVALID_CHAR  0xFF
   
   #define LCD_CONTRAST_LEVEL(level)  do{ LCDCCR = (0x0F & level); }while(0)
   #define LCD_WAIT_FOR_SCROLL_DONE() do{ while (!(ScrollFlags & LCD_FLAG_SCROLL_DONE)) {} }while(0)
   
   #define LCD_SCROLLCOUNT_DEFAULT    6
   #define LCD_DELAYCOUNT_DEFAULT     20
   #define LCD_TEXTBUFFER_SIZE        20
   #define LCD_SEGBUFFER_SIZE         19
   #define LCD_DISPLAY_SIZE           6

   #define LCD_FLAG_SCROLL            (1 << 0)
   #define LCD_FLAG_SCROLL_DONE       (1 << 1)   

   #define FLASH_TIMER_SEED 16	//period for blinking characters, 64 Hz frame rate

// Virtual key definitions and function prototypes for Joystick routines

	#define KEY_NULL    0
	#define KEY_ENTER   1
	#define KEY_NEXT    2
	#define KEY_PREV    3
	#define KEY_PLUS    4
	#define KEY_MINUS   5

// PROTOTYPES:

	void LCD_puts_f(const char *FlashData);
	void LCD_puts(const char *Data);
	void LCD_Init(void);
	void LCD_ShowColons(const uint8_t ColonsOn);

	void Joystick_Init(void);
	char ReadButtons(void);
	char getkey(void);
	void PinChangeInterrupt(void);
   
   #if defined(INC_FROM_DRIVER)
     static inline void LCD_WriteChar(const uint8_t Byte, const uint8_t Digit);
   #endif

#endif
