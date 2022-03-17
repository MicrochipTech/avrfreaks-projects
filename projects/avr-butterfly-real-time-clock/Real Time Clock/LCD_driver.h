//
//  Author(s)...: ATMEL Norway
//
//  Target(s)...: ATmega169
//
//  mt - used for debugging only - may not work

/************************************************************************/
// Definitions
/************************************************************************/
#define LCD_INITIAL_CONTRAST    0x0F
#define LCD_TIMER_SEED		    3
#define LCD_FLASH_SEED          10
#define LCD_REGISTER_COUNT      20
#define TEXTBUFFER_SIZE         30

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
#define LCD_CONTRAST_LEVEL(level) LCDCCR=(0x0F & level)


/************************************************************************/
// Global variables
/************************************************************************/
extern volatile char gLCD_Update_Required;
extern char LCD_Data[LCD_REGISTER_COUNT];
extern char gTextBuffer[TEXTBUFFER_SIZE];
extern volatile char gScrollMode;
extern char gFlashTimer;
extern char gColon;
extern volatile signed char gScroll;


/************************************************************************/
// Global functions
/************************************************************************/
void LCD_Init (void);
void LCD_WriteDigit(char input, char digit);
void LCD_AllSegments(char show);
