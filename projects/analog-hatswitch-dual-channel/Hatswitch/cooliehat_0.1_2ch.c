// (c) Andreas K. Fischer
//
// Cooliehat twin channel digital potentiometer for MC3030, volatile centered positions (no eeprom usage)
// Code developed by: Andreas Fischer
// Code version: cooliehat_0.1 initiated 
// Code started: 12.7.2010 (as cooliehat_0.1.c) with 3 channels
// Last change:  25.3.2011 (Downgrade to two channels, cooliehat_0.1_2ch.c)
//
// Things done
// - Key and Pin Assignment
// - Port init
// - Initial centering
// - Continuous DAC
// - Keyboard readout
// - Progressive deflection speed
//
// Things to do:
// - Build some hardware! And seriously think about impedance and capacity.
// - A lot of testing
//  
// Things that might be done or not:
// - Maybe move the DAC business into ISR for much better style?
// - Maybe introduce nonvolatile position storage? Maybe not...
// - Status LED or noisemaker
//
// Hardware configuration
// 
// target device: 	Atmel Attiny26 AVR 
// target speed:    	1000000 internal RC Oscillator
// target voltage:	5V
// target compiler: 	avr-gcc (WINAVR Rev. 20050214 or higher)
// 
// Fuse Bits:
// BODLEVEL=0		Brownout detection level 4.0V 
// BODEN=0		Enable brownout detection
// CKSEL=0001; SUT=10	Internal 1MHZ RC Osc, 6cycles+64ms Startup (no need to hurry)
//
// Lock Bits:
// Mode 1		No locks (at least for now)
//
// Pin usage:
// PA0: ADC0 Analog feedback for DAC, input from channel 0, Pin 20
// PA1: ADC1 Analog feedback for DAC, input from channel 1, Pin 19

// PA3: ADC external reference
// PA4: switched Output DAC CH0, PIN14
// PA5: switched Output DAC CH1, PIN13

// PA7: Status LED or speaker (not used yet)
// PB0: Key Input CH0 UP, PIN1
// PB1: Key Input CH0 DOWN, PIN2
// PB2: Key Input CH1 UP, PIN3
// PB3: Key Input CH1 DOWN, PIN4
// PB6: Key Input Center, Pin 9
// PB7: MCU Reset, Pin 10

#ifndef F_CPU
#define F_CPU 1000000   				// RC-OSC running at 1Mhz
#endif
#include <stdint.h>   					// Standard variable type definitions
#include <avr/io.h>     				// input/output definitions
#include <util/delay.h>  				// delay routines for slowing down
// #include <stdlib.h>     				// Standard Library, contains ABS
// #include <math.h>					// Math library for arithmetic operations,
							// Just in case we do something weird to calibration curves later
														
#define sbi(ADDRESS,BIT) ((ADDRESS) |= (1<<(BIT))) 	// compatibility with old WINAVR versions
#define cbi(ADDRESS,BIT) ((ADDRESS) &= ~(1<<(BIT)))	// handy macros for "expired" old winavr code
#define outp(VAL,ADRESS) ((ADRESS) = (VAL))
#define inp(VAL) (VAL)

											
#define MINDEFLECT (5)   				// minimum deflection range
#define MAXDEFLECT (1023-MINDEFLECT)   			// maximum deflection range
#define CENTERDEFLECT (512)				// center position
#define TOLERANCE (4)				// allowed tolerance for DAC
#define CH0UPKEY (0)					// Definition of various keys
#define CH0DOWNKEY (1)										
#define CH1UPKEY (2)
#define CH1DOWNKEY (3)
#define CENTERKEY (6)
#define CH0ADC (0)					// Definition of ADC backreading Channels
#define CH1ADC (1)	
#define CH0DAC (4)					// Definition of DAC output pins
#define CH1DAC (5)
#define LEDPIN (7)

#define KEYSPEED (25)   				// initial number of DAC cycles between keyboard processing - determines initial key speed
#define KEYLIMIT (5)					// Final number of DAC cycles between keyboard processing - determines max key speed
#define KEYSTEPS (3)				// number of consecutive key press events before key sampling speed is increased - controls key acceleration

#define DEBOUNCETIME (75)								// debounce timing for keys in milliseconds
#define STARTUPDELAY (250)								// Delaytime after startup for ADC init in milliseconds

// Note: After initialization of ADC sample, CPU idles to wait for ADC complete interrupt.
// This CPU time is in principle available for at least 32 clock cycles worth of code.



uint16_t ADCstep(uint8_t channel)					// Read ADC Channel
{ 									// Return value: sample value 
  uint16_t sample;

  ADCSR = (1<<ADEN) |  (1<<ADPS2);    					// preset frequency prescaler to 16 and activate ADC on Port A
  ADMUX = channel;							// Select ADC channel, Voltage reference external AVCC
  ADCSR |= (1<<ADSC);              					// Initiate sample
  while ( ADCSR & (1<<ADSC));						// wait for sampling to be completed
  sample= ADCW;
  return(sample);
}	



int main(void)
 {
  
   uint16_t Ch0target=CENTERDEFLECT;				// target voltage output values 10 bit for ch0,1,2
   uint16_t Ch1target=CENTERDEFLECT;

   uint16_t Ch0fact; 
   uint16_t Ch1fact;

   uint8_t i=3;							// Loop index
   uint8_t keyread=127;						// Keyboard vector
   uint8_t keyread_old=0;					// backup byeboard vector for debouncing
   uint8_t keybrake=KEYSPEED;				// ADC cycles between key readouts - determines key rate
   uint8_t accelerate=0;					// counter for acceleration delay

  _delay_ms(STARTUPDELAY);                      		// Wait for supply voltage to stabilize
								// Initialize ports
   DDRA  = (1<<CH0DAC)| (1<<CH1DAC)| (1<<LEDPIN);  // configure output Pins DACs and LED
   DDRB  = 0;							// Port B is completely Input
   PORTB = 127; 						// activate internal pullups for all Keys in Port B except MCU reset
  

   

   
   while(PINB<127);         					// Wait for all Keys to be released
   _delay_ms(DEBOUNCETIME);                       		// Debounce keys
   while(PINB<127);						// Scan keys again
   
//   while(i>0)
//   {								// establish center position on all channels
//    i=0;							// i used as error counter here. Reset it first.
//    Ch0fact=ADCstep(0);							// Sample ch1
//    if ((Ch0target-Ch0fact)>TOLERANCE) i++;				// compare and log discrepancies
//    if ((Ch0fact-Ch0target)>TOLERANCE) i++;
//    if (Ch0fact<Ch0target) sbi(PORTA,CH0DAC); else cbi(PORTA, CH0DAC);	// DAC toggle

//    Ch1fact=ADCstep(1);
//    if ((Ch1target-Ch1fact)>TOLERANCE) i++;				// the same for ch1
//    if ((Ch1fact-Ch1target)>TOLERANCE) i++;
//    if (Ch1fact<Ch1target) sbi(PORTA,CH1DAC); else cbi(PORTA, CH1DAC);

  
//   }									// loop until all center positions reached

//-------------------------------------------------------------------------------------------------------------------
   while(1) 					// infinite main loop - repeat until weihnachten
   {
    for(i=0; i<keybrake; i++)			// Do the DAC loop
     {
      Ch0fact=ADCstep(0);							
      if (Ch0fact<Ch0target) sbi(PORTA,CH0DAC); else cbi(PORTA, CH0DAC);	

      Ch1fact=ADCstep(1);
      if (Ch1fact<Ch1target) sbi(PORTA,CH1DAC); else cbi(PORTA, CH1DAC);

    
     }						// End of DAC loop
						// Do the keyboard business
      keyread=PINB;				// read keys	
      if((keyread==keyread_old)&&(keyread<127)) // Key pressed and held for 2 main loops
      
	  {
       if(bit_is_clear(keyread, CH0UPKEY)&&(Ch0target<MAXDEFLECT)) Ch0target++;
       if(bit_is_clear(keyread, CH1UPKEY)&&(Ch1target<MAXDEFLECT)) Ch1target++;
    
     
       if(bit_is_clear(keyread, CH0DOWNKEY)&&(Ch0target>MINDEFLECT)) Ch0target--;
       if(bit_is_clear(keyread, CH1DOWNKEY)&&(Ch1target>MINDEFLECT)) Ch1target--;
     
         
       if(bit_is_clear(keyread, CENTERKEY))
       {     
        Ch0target=CENTERDEFLECT;
        Ch1target=CENTERDEFLECT;
   
       }
       
	   if (accelerate<KEYSTEPS) accelerate++;
	   
	   
	   
       if ((keybrake>KEYLIMIT)&&bit_is_set(keyread,CENTERKEY)&&(accelerate>=KEYSTEPS)) 
	   { 
	     keybrake--; //speed up repetition rate if key pressed   
		 accelerate=0;
       }
      }
       else 
		{ 
		 keybrake=KEYSPEED;			// No keys pressed: set to initial key repetition rate
         accelerate=0;				// reset acceleration delay 
			
		}
    
   
      keyread_old=keyread; 


   }						// End of infinite loop
}						// End of program
  



