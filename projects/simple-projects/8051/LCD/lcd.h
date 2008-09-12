/*
 * Filename : lcd.h
 * Hardware : Controller  -> P89V51RD2
 *            XTAL        -> 18.432 MHz
 *            Mode        -> 6 Clock/MC
 * I/O      : RS          -> P2.5
 *            Enable      -> P2.4
 *            Data4567    -> P2.0,P2.1,P2.2,P2.3
 * Compiler : SDCC            
 * Author   : sci-3d@hotmail.com
 * Date		: 01/08/06
 */

 #define LCD_en P2_4
 #define LCD_rs P2_5 
 #define LCD_DELAY 1535 /* Delay for 1 ms */
 #define LCD_clear() LCD_command(0x1)	/* Clear display LCD */
 #define LCD_origin() LCD_command(0x2)	/* Set to origin LCD */
 #define LCD_row1() LCD_command(0x80)	/* Begin at Line 1 */
 #define LCD_row2() LCD_command(0xC0)   /* Begin at Line 2 */
 
/***************************************************
 * Prototype(s)                                    *
 ***************************************************/
void LCD_delay(unsigned char ms);
void LCD_enable();
void LCD_command(unsigned char command);
void LCD_putc(unsigned char ascii);
void LCD_puts(unsigned char *lcd_string);
void LCD_init();

/***************************************************
 * Sources                                         *
 ***************************************************/
void LCD_delay(unsigned char ms)
{
	unsigned char n;
	unsigned int i;
	for (n=0; n<ms; n++)
	{
		for (i=0; i<LCD_DELAY; i++); /* For 1 ms */
	}
    
}

void LCD_enable()
{
    LCD_en = 0; /* Clear bit P2.4 */
    LCD_delay(1);
    LCD_en = 1; /* Set bit P2.4 */
}

void LCD_command(unsigned char command)
{
    LCD_rs = 0; /* Clear bit P2.5 */
    P2 = (P2 & 0xF0)|((command>>4) & 0x0F);
    LCD_enable();
    P2 = (P2 & 0xF0)|(command & 0x0F);
    LCD_enable();
    LCD_delay(1);
}

void LCD_putc(unsigned char ascii)
{
    LCD_rs = 1; /* Set bit P2.5 */
    P2 = (P2 & 0xF0)|((ascii>>4) & 0x0F);
    LCD_enable();
    P2 = (P2 & 0xF0)|(ascii & 0x0F);
    LCD_enable();
    LCD_delay(1);
}

void LCD_puts(unsigned char *lcd_string)
{
	while (*lcd_string) 
	{
		LCD_putc(*lcd_string++);
	}
}

void LCD_init()
{
    LCD_en = 1; /* Set bit P2.4 */
    LCD_rs = 0; /* Clear bit P2.5 */   
    LCD_command(0x33);
    LCD_command(0x32);
    LCD_command(0x28);
    LCD_command(0x0C);
    LCD_command(0x06);
    LCD_command(0x01); /* Clear */
    LCD_delay(256);
}
