/*********************************************
* Chip type           : ATmega16
* Clock frequency     : 4,000000 MHz
Poort1 --> DCE Modem (poort bij open gedeelte)
Poort0 --> Computer  (poort bij MAX232 chip)
*********************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <inttypes.h>
#include "lcd.h"
#include "led.h"
#include "RS232.h"
#include "init.h"
#include "status.h"
#include "NMEA0183Parser.h"
#include "GSMParser.h"


typedef volatile unsigned char BYTE; //definitie type BYTE
typedef volatile unsigned int WORD; //definitie type WORD
///////////////////////////////////////////////////////////////////////////////
// Defines
///////////////////////////////////////////////////////////////////////////////
#define STOP_TIMER0    TCCR0 &= 0xF8
#define START_TIMER0   TCCR0 |= 0x05
#define STOP_TIMER1    TCCR2 &= 0xF8
#define START_TIMER1   TCCR2 |= 0x05


///////////////////////////////////////////////////////////////////////////////
// Global Varibles
///////////////////////////////////////////////////////////////////////////////
WORD Ticks256; 
WORD Ticks512;

int bufferwritepos = 0; // de schrijf positie van de buffer (GPS)
int bufferreadpos = 0; //de lees positie van de buffer (GPS)
int bufferwriteposGSM = 0; // de schrijf positie van de buffer (GSM)
int bufferreadposGSM = 0; //de lees positie van de buffer (GSM)
char lexeme[100]; //de buffer van de GPS
char GSMlexeme[60]; //de buffer van de GSM 

/**
 @brief    Laat het groene lampje maar knipperen
 @param    none                                    
 @return   none
*/
SIGNAL(SIG_OVERFLOW0)
//ISR(TIMER0_OVF_vect)
{
 // 256 ticks have gone by
 Ticks256++;
 // If you do the math you'll find that if the interrupt goes off 275 times, a minute has passed
 if (Ticks256 == 200)
 {
  led_toggle_green();
  Ticks256 = 0;
 }
}


/**
 @brief    Update de status van het display periodiekt
 @param    none                                    
 @return   none
*/
SIGNAL(SIG_OVERFLOW2)
//ISR(TIMER0_OVF_vect)
{
 // 256 ticks have gone by
 Ticks512++;


 // If you do the math you'll find that if the interrupt goes off 275 times, a minute has passed
 if (Ticks512 == 200)
 {
  showFixstate();
  led_toggle_red();
  Ticks512 = 0;
 }
}

/**
 @brief    Initialisatie timers
 @param    none                                    
 @return   none
*/
void ConfigureDevice ()
{



cli(); // disable interrupts just in case
	Ticks256 = 0; 
	Ticks512 = 0;
 
 // configure PORTB... and other settings

 TCNT0 = 0x00;   // clear Timer/Counter 0
 TCNT2 = 0x00;   // clear Timer/Counter 2
 START_TIMER0;
 START_TIMER1;
 
 // enable interrupts on timer 0 overflow
 TIMSK  |= _BV(TOIE0);
 // enable interrupts on timer 2 overflow
  TIMSK  |= _BV(TOIE2);


		
 sei(); // enable interrupts 
}


///////////////////////////////////////////POORT 0 (GSM)/////////////////////////////////////
/**
 @brief    Definitie van het receive interrupt aan de GSM-poort
 @param    none                                    
 @return   none
*/
SIGNAL (SIG_USART0_RECV) { // USART RX interrupt
   unsigned char c;
   c = UDR0;
  if (bufferwriteposGSM==60)
   {
   	bufferwriteposGSM=0;
	GSMlexeme[bufferwriteposGSM]=c;
   	bufferwriteposGSM ++;
   }
   else
   {
   		GSMlexeme[bufferwriteposGSM]=c;
   		bufferwriteposGSM++;
   }

   }
//////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////POORT 1 (GPS)/////////////////////////////////////
//
/**
 @brief    Definitie van het receive interrupt aan de GPS kant
 @param    none                                    
 @return   none
*/
SIGNAL (SIG_USART1_RECV) 
{ 
   unsigned char c;
   c = UDR1;
   if (bufferwritepos==100)
   {
   	bufferwritepos=0;
	lexeme[bufferwritepos]=c;
   	bufferwritepos++;
   }
   else
   {
   		lexeme[bufferwritepos]=c;
   		bufferwritepos++;
   }
}
//////////////////////////////////////////////////////////////////////////////////////////
//
/**
 @brief    Main function
 @param    none                                    
 @return   int
*/

int main(void) {
	//initialisatie leds
	led_init();
	
	//initialisatie timers
	ConfigureDevice ();

	
	//lexeme vullen met ~ teken om producer/consumer probleem tegen te gaan
	int i = 0;
	for (; i<99; i++)
	{//lexeme vullen met het teken ~
		lexeme[i]='~';
	}

	//GSMlexeme om dezelfde rede vullen
        int a = 0;
	for (; a<59; a++)
	{
		GSMlexeme[a]='~';
	} 
	
	//initialisatie van LCD disply
	InitLCDText();
	
	//rode led aan (init voltooid tot zover)
	led_red_on();

	//initialisatie UARTS
	InitUarts();

	//initialisatie GSM module 		
	initGSM();
	
	//sendTestSMS();
	
	//Initialisatie van NMEA0183 Parser
        initNMEA();

	//Initialisatie van GSP Parser
	InitGSMParser();
	
    	//geel ledje aan (init voltooid)
	led_yellow_on();

      while (1) {

	      //Buffer van NMEA parser voeden indien lees en schrijf positie
	      //niet gelijk zijn, en de entry nog niet is gelezen.	
      	      if (lexeme[bufferreadpos]!='~'&&bufferreadpos!=bufferwritepos)
		{
			// uart0_putc(lexeme[bufferreadpos]);
			 NMEAFeedBuffer (lexeme[bufferreadpos]);
			 lexeme[bufferreadpos]='~';
			 bufferreadpos++;
			 if (bufferreadpos==100){
			 	bufferreadpos=0;}
		}  
	      	//buffer van de GSM parser voeden indien less en schrijf positie
		//niet gelijk zijn en de entry nog niet is gelezen
		if (GSMlexeme[bufferreadposGSM]!='~'&&bufferreadposGSM!=bufferwriteposGSM)
		{
			GSMFeedBuffer (GSMlexeme[bufferreadposGSM]);
			GSMlexeme[bufferreadposGSM]='~';
			bufferreadposGSM++;
			if (bufferreadposGSM==60)
			{
				bufferreadposGSM=0;
			}
		}

  }
 return 0; 
}
