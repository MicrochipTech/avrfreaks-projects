#define F_CPU 1000000UL
#include <avr/eeprom.h>
#include <avr/interrupt.h> //Equips the interrupt handlers for doing softPWM
#include <inttypes.h>
#include <avr/io.h>
#include<compat/deprecated.h>		//HEADER FILE FOR FUNCTIONS LIKE SBI AND CBI
#include<util/delay.h>				//HEADER FILE FOR DELAY
#include <stdlib.h>
#include <stdio.h>
#include "lcd.h"


#define USART_BAUDRATE 4800
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
void read_gps(void);
void usart_init();
unsigned char usart_getch();
void usart_putch(unsigned char send);
void usart_putstr(unsigned char *s);
void init_gsm(void);

 
unsigned char value,i,j,*lati_value,lati_dir[15], longi_value[15], longi_dir[15], alti[15] ;

void usart_init()
{
   
	UCSR0B |= (1<<RXCIE0) | (1 << RXEN0) | (1 << TXEN0);   // Turn on the transmission and reception circuitry
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);// | (0<<UMSEL01) | (1<<UMSEL00); // Use 8-bit character sizes
 
	UBRR0L = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
	UBRR0H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
}
void usart1_init()
{
   
	UCSR1B |= (1<<RXCIE1) | (1 << RXEN0) | (1 << TXEN1);   // Turn on the transmission and reception circuitry
	UCSR1C |= (1 << UCSZ10) | (1 << UCSZ11);// | (0<<UMSEL01) | (1<<UMSEL00); // Use 8-bit character sizes
 
	UBRR1L = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register
	UBRR1H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
}
void usart_putch(unsigned char send)
{
 
while ((UCSR0A & (1 << UDRE0)) == 0); // Do nothing until UDR is ready..
// for more data to be written to it
UDR0 = send; // Send the byte 
}
void usart1_putch(unsigned char send)
{
 
while ((UCSR1A & (1 << UDRE1)) == 0); // Do nothing until UDR is ready..
// for more data to be written to it
UDR1 = send; // Send the byte 
}
void usart_putstr(unsigned char  *s) 
{ 
  while(*s) 
  { 
    usart_putch(*s++); 
  } 
}
void usart1_putstr(unsigned char  *s) 
{ 
  while(*s) 
  { 
    usart1_putch(*s++); 
  } 
}
unsigned char usart_getch()
{
 
	while ((UCSR0A & (1 << RXC0)) == 0); // Do nothing until data have been recieved and is ready to be read from UDR
	return(UDR0); // return the byte
}



int main(void)
{
    unsigned char buffer[]={"ok"}, lcdvalue[]={"Ready Solar Time"};
  	DDRB |= 0x01;
	sbi(PORTB,0);

	usart_init();		// initialization of USART
	usart1_init();
	//Initialize the LCD Module
	LCDInit(LS_BLINK);
	LCDClear();
	LCDWriteString(lcdvalue);
//	LCDWriteStringXY(0,1,buffer);

	usart_putstr(buffer);
	i=0;
	
	while(1)
	{
		
		//buffer++;
//		buffer[i]=usart_getch();
		
//		_delay_ms(100);
//		LCDWriteIntXY(0,1,value,2);
//		LCDWriteStringXY(0,1,buffer);
		read_gps();
		init_gsm();		

	}


return(0);
}


void read_gps(void)
{int x=0,y=0;
	value=usart_getch();
		if(value=='$')
		{
			value=usart_getch();
			if(value=='G')
			{
				value=usart_getch();
				if(value=='P')
				{
					value=usart_getch();
					if(value=='G')
					{
						value=usart_getch();
						if(value=='G')
						{
							value=usart_getch();
							if(value=='A')
							{
								value=usart_getch();
								if(value==',')
								{
									value=usart_getch();
									while(value!=',')
									{
										value=usart_getch();
									}
									lati_value[x]=usart_getch();
									x++;
									value=lati_value[0];
									for(i=1;value!=',';i++)
									{
										lati_value[i]=usart_getch();
										value=lati_value[i];
									}
									lati_dir[y]=usart_getch();
									y++;
									value=usart_getch();
									while(value!=',')
									{
										value=usart_getch();
									}
									longi_value[0]=usart_getch();
									value=longi_value[0];
									for(i=1;value!=',';i++)
									{
										longi_value[i]=usart_getch();
										value=longi_value[i];
									}
									longi_dir[0]=usart_getch();
									
									LCDWriteStringXY(0,0,lati_value);
									
									LCDWriteStringXY(9,0,lati_dir);
									
									LCDWriteStringXY(0,1,longi_value);
									
									LCDWriteStringXY(9,1,longi_dir);
									_delay_ms(1000);
															
								}
							}
						}
					}
				}
			}
     	  }
        
}
void delay1(int n)
{
   int i;
            for(i=0;i<n;i++);
}
 
void delay2(int n)
{
   int i;
            for(i=0;i<n;i++)
                        delay1(1000);
}

void init_gsm(void)
{
	static unsigned char msg01[] = {"AT+CMGF=1"};
	static unsigned char msg02[] = {"AT+CMGS=+918608206695"};
	static unsigned char msg03[] = {"HI---GSM TESTING"};
	static unsigned char msg04[] = {"\r"};
	static unsigned char msg05[] = {"\r"};

	usart1_putstr(msg01);
	usart1_putstr(msg04);
//	printf("AT+CMGF=1%c",13);
	delay2(20);  //Text Mode            | hex value of 13 is 0x0D (CR )
	usart1_putstr(msg02);
	usart1_putstr(msg04);
//    printf("AT+CMGS=\"+918608206695\"%c",13); 
	delay2(20);           //Type your mobile number Eg : "9884467058"
    
    delay1(20);

//    printf("Hi :-) GSM Modem Test");
	usart1_putstr(msg03);
	usart1_putstr(msg04);

	delay2(20);  //Type text as u want
	usart1_putstr(msg05);
//    printf("%c",0x1A); 
	delay2(20);  //line feed command
}
