//*****************************************************************************
//
//  File........: LCD_Driver.h
//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  Description.: Defines and prototypes for LCD_Driver.c
//
//  Revisions...: 1.0
//
//  YYYYMMDD - VER. - COMMENT                                       - SIGN.
//
//  20020606 - 0.10 - File created                                  - RM
//  20021010 - 1.0  - Clean up                                      - JLL
//  20031009          port to avr-gcc/avr-libc                      - M.Thomas
//  20070129          LCD_CONTRAST_LEVEL from Atmel's REV07-code    - mt
//
//*****************************************************************************


/************************************************************************/
// Definitions
/************************************************************************/
#define LCD_INITIAL_CONTRAST    0x0F
#define LCD_TIMER_SEED		    6
#define LCD_FLASH_SEED          10
#define LCD_REGISTER_COUNT      20
#define TEXTBUFFER_SIZE         60

#define SCROLLMODE_ONCE         0x01
#define SCROLLMODE_LOOP         0x02
#define SCROLLMODE_WAVE         0x03

/************************************************************************/
//MACROS
/************************************************************************/
//active = [TRUE;FALSE]
#define LCD_SET_COLON(active) LCD_Data[8] = active

// DEVICE SPECIFIC!!! (ATmega169)
#define pLCDREG ((unsigned char *)(0xEC))

// DEVICE SPECIFIC!!! (ATmega169) First LCD segment register
#define LCD_CONTRAST_LEVEL(level) LCDCCR=((LCDCCR&0xF0)|(0x0F & level))


/************************************************************************/
// Global variables
/************************************************************************/
// mt: volatiles added (based on jw patch)
extern volatile char gLCD_Update_Required;
// mt: extern char LCD_Data[LCD_REGISTER_COUNT];
extern volatile char gTextBuffer[TEXTBUFFER_SIZE];
extern volatile char gScrollMode;
extern volatile char gFlashTimer;
extern char gColon;
extern volatile signed char gScroll;
extern volatile char gAutoPressJoystick;

/************************************************************************/
// Global functions
/************************************************************************/
void LCD_Init (void);
void LCD_WriteDigit(char input, char digit);
void LCD_AllSegments(char show);
