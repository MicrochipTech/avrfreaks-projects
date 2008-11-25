// **************************************************************
// 					***  LCD DRIVER ******** 		
// **************************************************************


// LCD data bus, 8 bit mode 
extern volatile unsigned char *LCD_DIR_PORT;		
extern volatile unsigned char *LCD_IP_PORT;			
extern volatile unsigned char *LCD_OP_PORT;			

// LCD Enable Port
extern volatile unsigned char *LCD_EN_PORT;			 
extern char LCD_EN_BIT;				

// LCD Register Select Port
extern volatile unsigned char *LCD_RS_PORT;			
extern char LCD_RS_BIT;		
	
// LCD Read Write Port
extern volatile unsigned char *LCD_RW_PORT;			
extern char LCD_RW_BIT;

// *** LCD Functions *** //
void LCD_init(void);
void LCD_Cursor(char row, char column);
void LCD_DisplayString_F(char row, char column, const unsigned char *string);
void LCD_DisplayString (char row, char column , unsigned char *string);
void delay_ms(int miliSec);
void LCD_WriteCommand (unsigned char CMD);
void LCD_WriteData (unsigned char Data);
void LCD_CheckBusy ( void );

// DEFINING BITS
#define BIT7 0x80
#define BIT6 0x40
#define BIT5 0x20
#define BIT4 0x10
#define BIT3 0x08
#define BIT2 0x04
#define BIT1 0x02
#define BIT0 0x01