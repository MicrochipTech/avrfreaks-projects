/*
    Title:    laboratory datalogger-voltmeter based on ATmega8 
    Author:   domenico sartore
    company:  
    Date:     12/2011
    Vers:     data_logger1
    needed    Software: AVR-GCC_2010 lybraies in Winavr\avr\include\avr
    needed    Hardware: ATmega8 internal oscillator 8 MHz ( cksel0 e cksel1 a 0 ( unchecked ) cksel2 a 1 chekcked)
    Description: open acquisition system, only basic function are implemented aiming to temperature measurement, 
	             other software function can be added if necessary

	
    TODO:
   
   display auto switch off after long time
   speedup + e - button
   saving on external eeprom

  
	-modo=0:  logging: selected signal is sampled, displayed and eventually saved
    -modo=1:  set sample time
	-modo=2:  selection inputs ( 0; 1; 2; 3; 0 e 1; 2 e 3; 0,1,2 e 3; )
	-modo=3:  non active
	-modo=4:  switch 2 minus: stop saving 
	-modo=4:  switch 3 plus: restart eeprom saving 
	-modo=5:  switch 3 plus: upload eeprom: formatting and uploaing data to PC

	
	 working modes
	 modo=0:	read: acquire selected channel, display it and save if save flag is on
	 modo=1:	sample time setting mode ( sample time in second )
	 modo=2:	selecting input read
	 modo=3:	non valid ( future setting start and end eeprom address )
	 modo=4:	switch2 stop eeprom saving
				switch3 start eeprom saving
	 modo=5:	upload eeprom memory to PC via serial link
    
*/
#include <io.h>
#include <interrupt.h>

//&&#include <sig-avr.h>

typedef unsigned char  u08;
typedef          char  s08;
typedef unsigned short u16;
typedef          short s16;

// variabili globali
 
static volatile	short timer_1ms;	// counter 0..1000 ms	
static volatile	short timer_1s;		// counter sec	
static volatile	char  clock_1ms;	// 1ms cycle ended
static volatile	char  mode;			// active mode 		
static volatile	short t_sample;		// sample time sec	
static volatile   u08  tx[20];		// string buffer serial TX
static volatile   u08  tx_cnt;		// TX char counter TX  

   u08  display;			// out segm. display:PB0 a,PB1 b,PB2 c,PB3 d,PB4 e, PB5 f,PB6 g,PB7 dp, on when 0
							// pD4 digit0, pD5 digit1, pD6 digit2, pD7 digit3
   u08 	d1;					// Least significant digit
   u08 	d2;
   u08 	d3;
   u08 	d4;
   u16  stemp;				// temp short
   u08  latenza_tasto;		// switch latency in ms
   u16 	eepunta;			// eeprom address pointer
   u08  ingresso;			// active input
   u08  memflag;			// flag memory operation: 1 save, 0 don't save, 2 upload
//   u08  tempo_tasto;		// contatore tempo tasto premuto
//   u08  tasto_premuto;		// flag tasto premuto

// #define and constants

#define SWITCH_ON 		bit_is_set(PIND,3) 			// switch press	
#define SWITCH_OFF		bit_is_clear(PIND,3)
#define T_LATENZA       200							// in ms
#define MAX_EEPROM		511		// max EEPROM address
#define MIN_EEPROM		256		// min EEPROM address
#define MAX_MODE		  5		// max mode number
#define MAX_INGRESSO	  3		// max input number
#define MAX_TSAMPLE	    999		// nax sample time
// #define T_VELOCE		  5		// tempo velocizzazione tasti + e -	
// #define TSPEGNIMENTO	 20		// tempo di spegnimento del display se non in uso

// display const

#define uno     0xf9			// 1
#define due     0xa4			// 2
#define tre     0xb0			// 3
#define quattro 0x99			// 4
#define cinque  0x92			// 5
#define sex     0x82			// 6 
#define sette   0xf8			// 7
#define otto    0x80			// 8
#define nove    0x90			// 9
#define zero    0xc0			// 0
#define blank   0xff			// all seg off


//*****************************************
// main cycle interrupt timer (1ms)

SIGNAL(SIG_OVERFLOW0)       // tcnt2 overflow interrupt
{  
  TCNT0=131;          		// restore counter to have interrupt again
  if (++timer_1ms>999) 
  {
     timer_1ms=0;			// counter 0..1000ms
	 timer_1s++;			// counter secs
  }
  clock_1ms=1;				// cleared in main cycle 
}

//****************************************
// interrupt RS232 trasmission

SIGNAL(SIG_UART_TRANS)  
{
  if (tx_cnt)
  {
     UDR = tx[--tx_cnt];		// write a byte in UART data buffer 
  }
}

//*****************************************
// write eeprom byte

void EEPROM_write(unsigned int uiAddress, unsigned char ucData)
{
   while(EECR & (1<<EEWE));			// wait previous write cycle
   EEAR = uiAddress;
   EEDR = ucData;
   cli();							// interrupt disable
   EECR |= (1<<EEMWE);				// write programming sequence
   EECR |= (1<<EEWE);
   sei();							// interrupt enable
}

//*****************************************
// read eeprom byte

unsigned char EEPROM_read(unsigned int uiAddress)
{
   while(EECR & (1<<EEWE));		// wait previous read cycle
   EEAR = uiAddress;
   EECR |= (1<<EERE);
   return EEDR;
}

//*****************************************
// main program 
int main( void )
{
   // DEFINITION        
   
   u08  digit;				// digit to be write
   u08  inddigit;			// digit index to portc
   u08  pdecimale;			// decimal point position
   u08	sw_count;			// switch high counter
//adc
   u16  tensione;			// voltage value ( ADC output)
   u16  valore_ana;			// ad converter value
   u16  acc_adc;			// ADC values accumulator
   u08  conta_adc;			// ADC read counter
//
   u08  temp;				// char temp var ( input value not guaranteed )
   long  ltemp;				// long temp var

   // PORTS INITIALIZATION 
   DDRB=0xff;            		// PortB pins 7, 6,5,4,3,2,1,0,  output ( display segments ) 
   DDRC=0x00;            		// PortC pin 0,1,2,3 analog input, I2C pins initially input,6 reset  		  
   DDRD=0xf0;					// PortD pins 0,1 RX TX initially input,2 input not used, 3 input SWITCH, 4,5,6,7 output( digit select )

    // TIMER0  INITIALIZATION (clock every ms)
   TIMSK=0x01; 				// enable interrupt TCNT2 e TCNT0 overflow       
   TCNT0=131;        		// reset TCNT2 overflow after 1ms (256-131=125; 8Mhz/64/125=1000Hz) 
   TCCR0=3;        			// normal mode no prescaler clock:  cpu clock/64  

   // AD CONVERTER INITIALIZATION
   ADMUX=0x00;			// REFS1 0,REFS0 0,(rif ext),ADLAR 0,(right adjust) MUX 0,0,0,0 (in ADC0)
   ADCSR=0xc6;			// ADEN 1(gen.enable),ADSC 1(start),ADFR 0(free run),ADIF 0(int.flag),ADIE 0 (int enable) ADPS210 1,1,0 prescaling /64 clock=125000Hz

   //SERIAL OUTPUT INITIALIZATION

   UCSRB=0X48;			// enable TxD and Tx ints 
   UCSRC=0X86;			// no parity 1 stop bit 8 bit
   UBRRH=0X00;			// baud rate 9600 with 8 mhz
   UBRRL=0X33;			// baud rate 9600 with 8 mhz


   // OUTPUT AND VAR INITIALIZATION 
//adc
   valore_ana=0;
   acc_adc=0;
   conta_adc=0;
   tensione=0; 			// initially zero

   display=zero;		// 
   PORTB=display;
   digit=0;
   temp=0;
   inddigit=0;
   sw_count=0; 
   pdecimale=1;			// decimal point position
   mode=0;
   t_sample=05;			// prova: da mettere a 1
   memflag=0;			// non-save mode
   ingresso=0;
   eepunta=MIN_EEPROM;	  

   sei();				// interrupt enable 
   
   for (;;) 			// repeat mein cycle every 100us
   { 
//	ADC
      if ((timer_1ms%61==6)&&(timer_1s%t_sample==0))	// 16 time in a second, the sampling one
      {  
         valore_ana=ADCL;					// LSB (read first)        
         valore_ana+=(ADCH&0x03)<<8;		// MSB x 256
         if ( ++conta_adc<=16) 				// sum 16 values
         {
	        acc_adc+=valore_ana;
         }
	     else
	     {
	        conta_adc=0;
		    tensione=(acc_adc>>3);			// average value *2
			acc_adc=0;
	     }
//		 ADMUX=0x00;			// REFS1 0,REFS0 0,(ext ref),ADLAR 0,(right adjust) MUX 0,0,0,0 (in ADC0)
         ADMUX=ingresso&0x0f,	// REFS1 0,REFS0 0,(ext ref),ADLAR 0,(right adjust) MUX: read selected input
         ADCSR=0xc6;			// ADEN 1(gen.enable),ADSC 1(start),ADFR 0(free run),ADIF 0(int.flag),ADIE 0 (int enable) ADPS210 1,1,0 prescaling /64 clock=125000Hz
  
      } //end if (timer_1ms%20==6
// END ADC  

// DISPLAY WRITE

     if (timer_1ms%32==0)				// update display every 32ms
     { 
        if (mode==0)					// write voltage
		{
          ltemp=tensione;
          d1=ltemp%10;
          ltemp/=10;
          d2=ltemp%10;
          ltemp/=10;
          d3=ltemp%10;
          ltemp/=10;
          d4=ltemp%10;
//          d4=mode;
        }

        if (mode==1)					// write sample time
		{
          ltemp=t_sample;
          d1=ltemp%10;
          ltemp/=10;
          d2=ltemp%10;
          ltemp/=10;
          d3=ltemp%10;
//          ltemp/=10;
//          d4=ltemp%10;
          d4=mode;
        }

        if (mode==2)					//  write input to read
		{
          ltemp=ingresso;
          d1=ltemp%10;
          ltemp/=10;
          d2=ltemp%10;
          ltemp/=10;
          d3=ltemp%10;
//          ltemp/=10;
//          d4=ltemp%10;
          d4=mode;
        }

        if (mode==3)					// not used
		{
          d1=0;;
          d2=0;
          d3=0;
          d4=mode;
        }

        if ((mode==4)||(mode==5))		// upload: write EEPROM pointer
		{
          ltemp=eepunta-256;
          d1=ltemp%10;
          ltemp/=10;
          d2=ltemp%10;
          ltemp/=10;
          d3=ltemp%10;
          d4=mode;
        }

        if((mode!=0)&&(timer_1ms<500)) d4=255;	// flash mode digit
		if((memflag==1)&&(timer_1ms<500)) pdecimale=1; else pdecimale=0;	// decimal poit flashing in save mode

     }
   
     if (++digit>4) digit=1;			// digit activation every ms
     switch (digit)			
     {
       case 1:inddigit = 0xe0;temp=d1; break;    // A ZERO CIFRA1 load digit       
       case 2:inddigit = 0xd0;temp=d2; break;    // A ZERO CIFRA2        
       case 3:inddigit = 0xb0;temp=d3; break;    // A ZERO CIFRA3      
       case 4:inddigit = 0x70;temp=d4; break;    // A ZERO CIFRA4     
     }
     switch (temp)				// output display temp value
     {
       case 0:  	display=zero;    break;        
       case 1:  	display=uno;     break;      
       case 2:  	display=due;     break;  
       case 3:  	display=tre;     break;	
       case 4:  	display=quattro; break;      
       case 5:  	display=cinque;  break;	
       case 6:  	display=sex;     break;
       case 7:  	display=sette;   break;     
       case 8:  	display=otto;    break;     
       case 9:  	display=nove;    break;    
       case 255:  	display=blank;   break;        
       default: display=zero;
     } 
     if (digit==pdecimale)			// add decimal point
     {
        display&=0x7f;
     }
     PORTD&=0x0f;	 
     PORTD|=inddigit;				// output anode on high nibble
     PORTB=display;					// output display value

// EDN DISPLAY WRITE

// SWITCH READ AND DECODE
// if pressed switch is high in all digits except in the visualized, when not pressed switch is always low
// a switch is pressed if var switch is one in non-corresponding 3 digit

     if ( latenza_tasto++>=T_LATENZA )
	 {
	    latenza_tasto=T_LATENZA;
        if ((SWITCH_ON)&&(digit==1)) sw_count++;	// test if high with corresponding digit
        if ((SWITCH_ON)&&(digit==2)) sw_count+=2;
        if ((SWITCH_ON)&&(digit==3)) sw_count+=4;
        if ((SWITCH_ON)&&(digit==4)) sw_count+=8;
        if (sw_count==14) 		// switch 1 pressd ( on the right ) 
		{
		   sw_count=0; 
		   latenza_tasto=0; 
		   mode=0;
		}

        if (sw_count==13)		 // switch 2 pressd 
		{  
		   sw_count=0; 
		   latenza_tasto=0;
		   if ( mode==1)  
		   {
		     if (--t_sample>MAX_TSAMPLE) t_sample=MAX_TSAMPLE; 
			 latenza_tasto=100;
		   }

		   if ( mode==2) 
		   {
		      if ( --ingresso>MAX_INGRESSO) ingresso=0;
           }
		   if ( mode==4) 
		   {  
		      if ( memflag==1) 		// stop eeprom write
			  {
				 memflag=0; 
			  }	
		   }
		}

        if (sw_count==11)  		// switch 3 pressd	
		{
		   sw_count=0; 
		   latenza_tasto=0; 
		   if ( mode==1)  
		   {
		      if (++t_sample>MAX_TSAMPLE) t_sample=MAX_TSAMPLE; 
			  latenza_tasto=100;
		   } 
		   if ( mode==2) 
		   {
		      if ( ++ingresso>MAX_INGRESSO) ingresso=MAX_INGRESSO;
           }
		   if ( mode==4) 
		   {  
		      if ( memflag==0) 		// restart eeprom write
			  {
			     eepunta=MIN_EEPROM;
				 memflag=1; 
			  }	
           }
		   if ( mode==5) { memflag=2; eepunta=MIN_EEPROM; }	// start upload eeprom	
		}

        if (sw_count==7) 					// switch 4 pressd 
		{
		   sw_count=0; 
		   latenza_tasto=0; 
		   mode++;
		} 

	    if (sw_count>14)   sw_count=0;		// default

	    if ( mode>MAX_MODE ) mode=0;
     }

// END SWITCH READ AND DECODE

// EEPROM

  if (( timer_1ms==990 )&&(timer_1s%t_sample==0)&&(memflag==1))
  {
	 EEPROM_write(eepunta, tensione>>8);		//high byte
     if ( ++eepunta>=MAX_EEPROM) memflag=0;		//memory full
  } 
  if (( timer_1ms==999 )&&(timer_1s%t_sample==0)&&(memflag==1))
  {
	 EEPROM_write(eepunta, tensione&0xff);		//low byte
     if ( ++eepunta>=MAX_EEPROM) memflag=0;		//memory full
  }
  if ((memflag==2)&&(timer_1ms%50==11))			//memory upload
  {
     
     ltemp=EEPROM_read(eepunta);
	 eepunta++;
	 ltemp<<=8;
	 tx[4]= ',';				// this give some time between two read op
	 tx[5]= 0x0d;				// newline
	 ltemp+=EEPROM_read(eepunta);
     tx[0]='0' + ltemp%10;
	 ltemp/=10;
     tx[1]='0' + ltemp%10;
	 ltemp/=10;
	 tx[2]='0' + ltemp%10;
	 tx[3]='0' + ltemp/10;
	 tx[4]= ',';
	 tx[5]= 0x0d;				// newline
	 tx_cnt=4;
	 UDR=tx[5];					// send first byte
     if ( ++eepunta>=MAX_EEPROM) memflag=0;   
  }

// END EEPROM

// SERIAL OUTPUT

  if (( timer_1ms==300 )&&(tx_cnt==0)&&(timer_1s%t_sample==0)&&(memflag!=2))
  {
     temp=0;
     tx[0]='0' + temp%10;
	 temp/=10;
	 tx[1]='0' + temp%10;
	 tx[2]='0' + temp/10;
	 tx[3]= ',';

     temp=0;
     tx[4]='0' + temp%10;
	 temp/=10;
	 tx[5]='0' + temp%10;
	 tx[6]='0' + temp/10;
	 tx[7]= ',';

     ltemp=timer_1s;
     tx[8]='0' + ltemp%10;
	 ltemp/=10;
     tx[9]='0' + ltemp%10;
	 ltemp/=10;
     tx[10]='0' + ltemp%10;
	 ltemp/=10;
	 tx[11]='0' + ltemp%10;
	 tx[12]='0' + ltemp/10;
	 tx[13]= ',';
	 tx[14]= ',';

//     ltemp=0;
//    tx[12]='0' + ltemp%10;
//	 ltemp/=10;
//	 tx[12]='0' + ltemp%10; 
//	 tx[13]='0' + ltemp/10;
//	 tx[14]= ',';

     ltemp=tensione;
     tx[15]='0' + ltemp%10;
	 ltemp/=10;
     tx[16]='0' + ltemp%10;
	 ltemp/=10;
	 tx[17]='0' + ltemp%10;
	 tx[18]='0' + ltemp/10;
	 tx[19]= 0x0d;				// newline

	 tx_cnt=19;
	 UDR=tx[19];				// send first byte
  }

// END SERIAL OUTPUT
   
     clock_1ms=0;				// end main cycle        
     while (!clock_1ms) {};		// waiting for interrupt

  }		// end    for (;;) 
}


///////////////////////////////////////////
/*
per il voltmetro nelle tre posizioni la tensione viene divisa per 2,20,200.
poi viene amplificata per 20 per arrivare a 2V, e le tre posizioni corrispondono quindi a Vi*0.1, *1, *10
cioè a un fondoscala di 0.2V, 2V, 20V
il numero che esce è poi (Vi/Vref)*1023, per avere una lettura in volt bisogna moltiplicare per 2 ( tarando la tensione )
con un po' di correzioni escono dei valori sufficientemente precisi



*/


