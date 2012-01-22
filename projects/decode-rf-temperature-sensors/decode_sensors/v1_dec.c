/********************************************************************************************
 Oregon Scientific v 1.0 decoder routine for THR138 temperature sensor (and others)
 
 S. James Remington sjames_remington at yahoo.com

 Began with spaghetti interrupt code (the timing constants were handy)
 by Alexander Yerezeyev 2007-2009
 URL: http://alyer.frihost.net

useful documentation: wmrx00.sourceforge.net/Arduino/OregonScientific-RF-Protocols.pdf

  MCU: ATtiny26 @ 1MHz
********************************************************************************************/
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define F_CPU 1000000UL
#include <util/delay.h>

const char   StartStr[]   PROGMEM = "OS_V1 decoder";
char       TextBuffer[21];  //string, 20 chars max

unsigned char RXdata[4]; //OS_V1 message buffer

void Delay10ms(int count)  //long delays
{
   int i;
   for (i=0; i<count; i++) _delay_ms(10);
}

// include software uart routines

#include "suart.c"


#define RF_IN   (1<<PB6)            //RF data in  PB0, ACTIVE LOW
#define LED      (1<<PB3)

// The following defines are for a receiver with RF ON = active low output
// switch them if RF ON = active high
// also, RF_OFF definition must match input bit of port connected to receiver
      
#define RF_ON  0
#define RF_OFF (1<<PB6)

/*** io_init *********************************************************************************************
 *   Initialize timers, pins and software serial port
 ********************************************************************************************************/

void io_init(void) 
{
   DDRA=0;               //initially all inputs
   DDRB=0;

   TX_init(2400);         //baud rate for LCD output on PA0

   PORTA = 0xFE;         //pullups on, TX idle   

   DDRB |= LED;         //led output   
   PORTB = 0xFF;         //pullups, led on

    TCCR0 = (1<<CS01)|(1<<CS00);    //Timer0 counts at system clock/64 = 64 usec ticks
    TCCR1A = 0;

   // Timer1 frequency = 1e6/16/(OCR1C+1) = 341.53 Hz (2928 usec, ~ bit clock period).
                                          //Timer1 counts at system clock/16 = 16 usec tick
    TCCR1B = (1<<CTC1)|(1<<CS12)|(1<<CS10); //clear on compare match with OCR1C
    OCR1C = 0xB6;                             //182 

      
}

void SetBit(unsigned char BitNum, unsigned char BitVal)
{
   unsigned char ByteNumber;

   ByteNumber=(BitNum>>3);
   
   if (BitVal == 0) RXdata[ByteNumber] &= ~(1<<(BitNum & 7)); //cbr ((RXdata[ByteNumber]), (BitNum&0x07));
    
    else RXdata[ByteNumber] |= (1<<(BitNum & 7)); //sbr ((RXdata[ByteNumber]), (BitNum&0x07));
    
}


   int main (void)
{  
    unsigned char TimerValue=0;     // time between edges
    unsigned char ErrCode;
    unsigned char BitValue=0;
   unsigned char ByteNum;
    unsigned char BitCnt;       // received bits counter
    unsigned char PreambleCnt=0;   // counts preamble pulses
   unsigned int crc;
   unsigned char Channel,Th1,Th2,Th3,LowBatt,Sign, HH;


//******************** Oregon Scientific Protocol V 1.0 decoder begins ****************************    

// the following define interval timing and allowed uncertainty
#define deltaT 16
#define InRange(TVAL, TSETUP, dT) ((TVAL<=(TSETUP+dT))&&(TVAL>=(TSETUP-dT)))

#define TminON 0x1B         // (1750us) Tmin High Level for v1 protocol
#define TminOFF 0x13       // (1190us) Tmin Low Level for v1 protocol
#define TmaxON  0x31;      // (1500us) Tmax High Level for v1 protocol
#define TmaxOFF 0x2B;      // (1500us) Tmax Low Level for v1 protocol

#define TstartBitON 0x59         //High sync pulse for v1 protocol
#define TstartBitOFF 0x42         //First Low sync pulse for v1 protocol
#define TstartBitOFF2short 0x56    //Second Short Low sync pulse for v1 protocol
#define TstartBitOFF2long 0x69       //Second Long Low sync pulse for v1 protocol

   io_init();

   cls();   //clear LCD screen

   TX_puts_f(StartStr);

   while (1)
   {
      
      PORTB &= ~LED;
      ErrCode=0;
      PreambleCnt=0;

// wait for RF on

   while ((PINB & RF_IN) == RF_OFF);      

// look for V1.0 preamble
      
      while(PreambleCnt<12)
      {
         while ((PINB & RF_IN) == RF_ON); //RF is on, wait for RF off

         TCNT0=0;             //Start T0

         while ((PINB & RF_IN) == RF_OFF); //wait for RF on

         TimerValue=TCNT0;     // Read timer value

         if (!InRange(TimerValue, TminOFF, 10)) break; // continue if T ~ Tmin |_| (Preamble off period is detected)

         PreambleCnt++;
      }
      
     if (PreambleCnt<10) ErrCode |= 1;  //probably not OS V1.0 protocol

     if (PreambleCnt>9) PORTB |= LED; //LED on, preamble detected

// RF is now on. Check for valid 1st RF off sync (from last timed interval)

      if (!InRange(TimerValue, TstartBitOFF, deltaT)) ErrCode |= 2; // T == Tstartbit Low |__| (Sync is Detected)

      while ((PINB & RF_IN) == RF_ON); //wait for RF off
         
       TCNT0 = 0; //restart timer
      
        while ((PINB & RF_IN) == RF_OFF); //wait for RF on

        TimerValue=TCNT0;

// RF is now on. Check for valid 2nd RF off sync (two possibilities)

         TCCR1B |= (1<<PSR1);          //clear Timer 1 prescaler
        TCNT1 = 0;                      //reset and start Timer1 in CTC mode (bit timer)

        if InRange(TimerValue, TstartBitOFF2long, deltaT)// T == TstartBitOFFow2Long |____| ( Long Startbit2 is Detected)
        {                       
            BitValue=0;                   //long sync, the first message bit is a zero
        }
        else if InRange(TimerValue, TstartBitOFF2short, deltaT)// T == TstartBitOFFow2short |__| ( Short Startbit2 is Detected)
        {                                              
            BitValue=1;                   //short sync, first message bit is a one

            while ((PINB & RF_IN) == RF_ON); //wait for falling edge to synchronize bit clock

            TCCR1B |= (1<<PSR1);          //clear Timer 1 prescaler
           TCNT1 = 0;                      //reset Timer1 in CTC mode as bit interval timer
         }
                           
         else ErrCode |= 4;  // Not a valid sync bit

// if OK so far, get rest of message

         if(ErrCode == 0) 
       {

// Timer 1 bit clock is now synchronized to bit transition edges, assuming 342 Hz data rate
// store first bit of message

         BitCnt=0;
       ByteNum = (BitCnt>>3);
   
        if (BitValue == 0)   RXdata[ByteNum] &= ~(1<<(BitCnt & 7));
        else             RXdata[ByteNum] |=  (1<<(BitCnt & 7));
 
// loop through next 31 bits

         while (BitCnt<32)
            {  
             if ((TCNT1 & 0x80) == 0x80)       //wait until about 2/3 through bit period
               { 
     
 // sample RF before next bit transition (sample multiple times?)
      
                 if ((PINB & RF_IN) == RF_ON) BitValue = 1;
                 else BitValue = 0;
    
                 BitCnt++; 
             ByteNum = (BitCnt>>3);
   
             if (BitValue == 0)    RXdata[ByteNum] &= ~(1<<(BitCnt & 7));
              else             RXdata[ByteNum] |=  (1<<(BitCnt & 7));
    
              while ((TCNT1 & 0x80) == 0x80); //wait for Timer1 rollover
            
            }

             } //end while (BitCnt<32)

         PORTB &= ~LED;  //led off

//  example valid message
//   RXdata[0]=0x23;
//   RXdata[1]=0x70;
//   RXdata[2]=0x01;
//   RXdata[3]=0x94;

         // message integrity check

         crc = RXdata[0]+RXdata[1]+RXdata[2];
         crc = (crc & 0xFF)+(crc>>8);       //sum in overflow
         if (crc != RXdata[3]) ErrCode |=8;  //checksum not OK

         Channel = RXdata[0] & 0xc0;     //0, 40h, 80h = channels 1,2,3

         Th3 = (RXdata[1] & 0x0f);     //BCD fraction digit of temp
         Th2 = (RXdata[1] & 0xf0)>>4;  //BCD digit 2 of temp
         Th1 = (RXdata[2] & 0x0f);     //BCD digit 1 of temp

         Sign    = (RXdata[2] & 0x20);  //nonzero = negative temp
         HH       = (RXdata[2] & 0x40);  //nonzero = HH.H overflow?
         LowBatt   = (RXdata[2] & 0x80);  //nonzero = low battery

// display message on LCD, hex except temperature in C

         cls();

         TX_puts(itoa(RXdata[0],TextBuffer,16));
         TX_puts(itoa(RXdata[1],TextBuffer,16));
         TX_puts(itoa(RXdata[2],TextBuffer,16));
         TX_puts(itoa(RXdata[3],TextBuffer,16));
         TX_putc(' '); TX_puts(itoa(ErrCode,TextBuffer,16));

         crlf();

         if(Sign) TX_putc('-');
         TX_putc(Th1+'0');
         TX_putc(Th2+'0');
         TX_putc('.');
         TX_putc(Th3+'0');

         TX_putc(' ');
         TX_puts(itoa(Channel,TextBuffer,16));
         TX_putc(' ');
         TX_puts(itoa(Sign,TextBuffer,16));
         TX_putc(' ');
         TX_puts(itoa(LowBatt,TextBuffer,16));
         Delay10ms(500);

      } //end if (ErrCode == 0)
    } //end while(1)
 }

