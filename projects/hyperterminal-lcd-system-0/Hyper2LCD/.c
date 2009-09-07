/*made by darsh 
code for implementation of usart. connect lcd to your microcontroller. open hyperterminal and set baud rate 
2400 and com1 and flow none. 
*/ 
#include<avr/io.h> 
#include<util/delay.h> 
#include<avr/interrupt.h> 

#include <stdlib.h>

#include <avr/pgmspace.h>
#include "lcd.h"





void InitUART( unsigned int BaudRate ); 
unsigned char ReceiveByte( void ); 
void TransmitByte( unsigned char data ); 


int main(void) 
{ 
	unsigned char a; 
	unsigned int i,j; 
	DDRD=0X0F; // txd and rxd pins are present 
	DDRC=0x01;// leds for debugging 
	InitUART( 25 ); // 2400 bps @ 1MHz hence value of UBBR=25 . refer datasheet 

	lcd_init(LCD_DISP_ON); /* initialize display, cursor off */ 

	lcd_clrscr();
        
        /* put string to display (line 1) with linefeed */
	//lcd_gotoxy(10,1);
    lcd_puts("RS232");
	PORTC=0x01;
	_delay_ms(500);
	lcd_clrscr();


	
	for (;;) /* loop forever */ 
	{ 
	
		for(i=0;i<2;i++) // 2 lines of lcd 
		{ 
			for(j=0;j<16;j++) // 16 characters in each line 
				{ 
				
					a=ReceiveByte(); // recieves data from hyperterminal 
			
					_delay_ms(10); 
			  		lcd_gotoxy(j,i); // goes to specified position 
			
					lcd_putc(a); // display the character 
			
					_delay_ms(50);// delay imp so that value can be displayed on lcd 
					PORTC=0X01; // led on after display 
					TransmitByte(a); // transit it back to hyperterminal 
					_delay_ms(200); 
					PORTC=0X00; 
				} 
		} 
	} 
} 
// -----------------INITIALIZE USART--------------------------------------- 
void InitUART( unsigned int BaudRate) 


{ 
	UBRRH=0x00; 
	UBRRL = (unsigned char)BaudRate; /* Set the baud rate */ 
	UCSRB = 0b00011000; // enable transmitter and reciever 
	UCSRC = 0b10000110; // 8 bit data, no parity bit, 1 stop bit. 
} 
//------------FUNCTIONS TO READ USART------------------------------- 
unsigned char ReceiveByte( void ) 
{
	unsigned char data; 
	//lcd_puts("function");
	while ( (UCSRA & 0x80) == 0x00 ) 
		{;} /* Wait for incomming data */ 
	data=UDR;
	
	return data;/* Return the data */ 
} 
//------------FUNCTIONS TO WRITE USART----- 
void TransmitByte( unsigned char data ) 
{ 
	while ( (UCSRA & 0x20) == 0x00) 
		{;} /* Wait for empty transmit buffer */ 
	UDR = data; /* Start transmittion */ 
} 


