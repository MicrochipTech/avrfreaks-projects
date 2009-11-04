/*
	LCD EA DOGM128-6 control
	use ATmega32



*/


#include "global.h"
#include "main.h"

typedef enum {LCD_DATA, LCD_CMD} lcd_type_t;


#include "dogm-core.h"
#include "LCD_ST7565.h"
#include "rprintf.h"
//#include "graphics.h"


 
void lcd_spi_write(unsigned char hexwrite)  //SPI Init
{
	delay_us(1);
	SPDR = hexwrite;				// Daten nach Sender und starten
	// while(!(SPSR & (1<<SPIF)));			// warte bis Bit 1: fertig
	loop_until_bit_is_set(SPSR, SPIF);		// warte bis Bit 1: fertig
	delay_us(1);
	hexwrite = SPDR;				// SPIF wider 0 (Pseudolesen)
}


void lcd_set_a0(uint8_t type)
{
	if (type) cbi(LCD_DOGM_PORT, LCD_DOGM_A0); // Command (LCD_CMD) // Reg Select: 0 => Instruction Reg (w)
	else      sbi(LCD_DOGM_PORT, LCD_DOGM_A0); // Data (LCD_DATA)	// Reg Select: 1 => Data Reg (w)
}


void lcd_set_cs(uint8_t enable)
{
	if (enable) cbi(LCD_DOGM_PORT, LCD_DOGM_CSB);	// DOGM select = LOW
	else 		sbi(LCD_DOGM_PORT, LCD_DOGM_CSB);	// DOGM deselect = HIGH
}


int main(void)
{

	RES_PORT |= (1<<RES_BIT); 
	
	LCD_DOGM_DPORT = 0xff;			// set output
	LCD_DOGM_PORT = 0xff;			// clear LEDS
	sbi(DDRC,7);	

	SPCR = (1 << SPE) | (1 << MSTR) | (1<<SPR0);	// SPI Master
	InitDelay();
	dogm_core_set_remore(&(lcd_spi_write), &(lcd_set_a0), &(lcd_set_cs), &(delay_us)); //SPI Init

	
	rprintfInit(LCD_DisplayChar5_7); // define output function for 5x7 char size

	Delay(1000); //Inicialization 100ms Default delay is 100us


	disp_init();


	LCD_DisplayString5_7("DOGM128-6", 0, 0); 	//Zeile 0     // Befehl aus LCD_ST7565 
	LCD_DisplayString8_16("   DOGM128-6   ", 6, 0); 	//Zeile 0     // Befehl aus LCD_ST7565 
	XY_Goto5_7(2,0);	
	rprintf("1234567890");                        	// Befehl aus AvrLib, siehe  rprintf.c 
	Delay(20000);
	LCD_Draw_Pixel(0,0,1);

while (1) 
	{
	}

}

void delay_us(u32 time_us)
{
	u32 i;
	if (time_us < 1 ) time_us = 1;
	for (i=0; i < time_us*2; i++) {};
}



