//********************************************************
//*** MAIN PROGRAM FOR TESTING M32_CARD FUNCTIONALITY ****
//********************************************************
//Controller:	ATmega32 (Crystal: 16Mhz)
//Compiler:		ICCAVR 
//Author:		CC Dharmani, Chennai, India
//Date:			Aug 2008
//********************************************************

#include <iom32v.h>
#include <macros.h>
#include "M32_main.h"
#include "LCD_routines.h"
#include "UART_routines.h"
#include "i2c_routines.h"
#include "RTC_routines.h"
#include "ADC_routines.h"


//port initialize
void port_init(void)
{
 DDRA  = 0xFF;
 PORTA = 0x00;
 DDRB  = 0xE1;
 PORTB = 0xff;
 DDRC  = 0x00;
 PORTC = 0x00; 
 DDRD  = 0xF0;
 PORTD = 0x00;
}


//UART0 initialize
// desired baud rate: 19200
// actual: baud rate:19231 (0.2%)with 16MHz crystal
// char size: 8 bit
// parity: Disabled
void uart0_init(void)
{
 UCSRB = 0x00; //disable while setting baud rate
 UCSRA = 0x00;
 UCSRC = BIT(URSEL) | 0x06;
 UBRRL = 0x33; //set baud rate lo
 UBRRH = 0x00; //set baud rate hi
 UCSRB = 0x98;
}

#pragma interrupt_handler uart0_rx_isr:14
void uart0_rx_isr(void)
{
 unsigned char data;
 //uart has received a character in UDR
 data = receiveByte();
 transmitByte(data);
 if(data == 0x0d)
 transmitByte(0x0a);
}

//TWI initialize
// bit rate:18
void twi_init(void)
{
 TWCR= 0x00; //disable twi
 TWBR= 0x12; //set bit rate
 TWSR= 0x01;
 TWAR= 0x00; //set slave address
 TWCR= 0x44; //enable twi
}

//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 CLI(); //disable all interrupts

 port_init();
 LCD_init();
 uart0_init();
 ADC_init();
 twi_init();
 
 MCUCR = 0x00;
 TIMSK = 0x00; //timer interrupt sources
// SEI(); //re-enable interrupts
} 

void checkKeys()
{
  unsigned char i;
  
  TX_NEWLINE;
  TX_NEWLINE;
  transmitString_F("Press any of the 4 general purpose Push-Buttons on the M32_Card");
  
  DDRB  = 0xE1;  //keyboard port
  
   for(i=0; i<4; i++)
  {
    RECHECK:
	while((PINB & 0x1e) == 0x1e);  //wait here till any key is pressed
	delay_ms(50); 
    if((PINB & 0x1e) == 0x1e) goto RECHECK;
	
	switch(PINB & 0x1e)
	{
	  case(pushButton1): TX_NEWLINE;
     			  		 TX_NEWLINE;
   				  		 transmitString_F("Push-Button 1 pressed");
						 break;
	  case(pushButton2): TX_NEWLINE;
     			  		 TX_NEWLINE;
   				  		 transmitString_F("Push-Button 2 pressed");
				  		 break;
	  case(pushButton3): TX_NEWLINE;
     			  		 TX_NEWLINE;
   				  		 transmitString_F("Push-Button 3 pressed");
				  		 break;
	  case(pushButton4): TX_NEWLINE;
     			  		 TX_NEWLINE;
   				  		 transmitString_F("Push-Button 4 pressed");
				  		 break;
	  default: 	  		 TX_NEWLINE;
     			  		 TX_NEWLINE;
   				  		 transmitString_F("Invalid Push-Button response");
				  		 break;
	 }//end of switch()
	 
	 while((PINB & 0x1e) != 0x1e);
	 delay_ms(50);
  }//end of for loop
}	

void checkLED()
{
  unsigned char i;
  
  TX_NEWLINE;
  transmitString_F("LED will blink for 5 seconds..");
  
  for(i=0; i<10; i++)
  {
   if(i%2 == 0)
     PORTD |= 0x10;	 	//LED ON
   else
     PORTD &= ~0x10;	//LED OFF
	 
   delay_ms(500);		//delay of 500ms
  }
  
  PORTD &= ~0x10;	//LED OFF before leaving this function
}   


//****************** MAIN FUNCTION *******************

 void main(void)
{
   unsigned char option;
   
   init_devices();
   
   transmitByte(0x0d);
   transmitByte(0x0a);
   transmitString_F("Welcome to M32_Card Testing!");
   transmitByte(0x0d);
   transmitByte(0x0a);
   transmitString_F("----------------------------");
   transmitByte(0x0d);
   transmitByte(0x0a);
   transmitString_F("----   By CC Dharmani   ----");
   transmitByte(0x0d);
   transmitByte(0x0a);
  
   LCD_DisplayString_F(1,1,"   WELCOME    ");
   LCD_WriteCommand(0xc0);

   while(1)
   {
   receiveByte();
   TX_NEWLINE;
   transmitString_F("> 1 : Display RTC Date");
   TX_NEWLINE;
   transmitString_F("> 2 : Display RTC Time");
   TX_NEWLINE;
   transmitString_F("> 3 : Update RTC Date");
   TX_NEWLINE;
   transmitString_F("> 4 : Update RTC Time");
   TX_NEWLINE;
   transmitString_F("> 5 : Check Push-buttons");
   TX_NEWLINE;
   transmitString_F("> 6 : Check LED");
   TX_NEWLINE;
   transmitString_F("> 7 : Read ADC Channel");
   TX_NEWLINE;
   TX_NEWLINE;
   transmitString_F("> Select Option (1-7): ");
   
   option = receiveByte();
   transmitByte(option);
   
   i2c_stop();
 
   switch (option)
   {
	case '1': RTC_displayDate();
		 	  break;
	case '2': RTC_displayTime();
		 	  break;
	case '3': RTC_updateDate();
		 	  break;
	case '4': RTC_updateTime();
		 	  break;
	case '5': checkKeys();
		 	  break;
	case '6': checkLED();
		 	  break;	
	case '7': ADC_transmitValue();
		 	  break;		  
	default:  TX_NEWLINE;
   		 	  transmitString_F(" Invalid option!");
			  TX_NEWLINE;
	}		  
   
   TX_NEWLINE;
   transmitString_F(" Finished..");
   TX_NEWLINE;
   }
   
}

