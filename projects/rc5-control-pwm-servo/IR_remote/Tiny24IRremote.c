/*:               Philips RC-5 Remote Control Servo Driver
    
    Date:     July 2009
    Purpose:  Decodes RC5-commands and outputs servo PWM drive (1 channel)
    
    Device = ATtiny24  running at 8 Mhz, 

    RC5 data format:
        SB1 SB2 TB AB5 AB4 AB3 AB2 AB1 AB0 CB5 CB5 CB4 CB3 CB2 CB1 CB0
        SB = Start bit, TB = Toggle bit, AB = address bits,  CB = command bits
    
    Program description:
    
    The IR-Receiver receives and demodulates the IR-signal from the 
    remote control into an active-low signal, which is fed into port PB2 (Pin 5)
  
    RC5 data is output on Port A (Lowest 4 bits).
    
    Timer0 is used as a time base. The Timer0 overflow interrupt sets the 
    global variable timerflag to 1. 
    
	The servo driver uses Timer1 (16 bit) for the timing of the PWM output. 
	The PWM output appears on OC1A (Pin7)
    
    Only certain codes are permitted to activate the servo so checks are performed 
    on the received data prior to servo action.
    
	    
/******************************************************************************/
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <util/delay.h>

#define TMC8_STOP	0			//Timer stopped, TCCR0A = 0
#define TMC8_CK8	_BV(CS01);	//Set timer 0 prescaler to divide by 8
                

//Macros and constants

#define RC5BitHigh()    (bit_is_set(PINB,PB2))		//IR input hi
#define RC5BitLow()     (bit_is_clear(PINB,PB2))	//IR input lo
#define WAITFORTIMER()  { while ( timerflag == 0); timerflag = 0; }

#define TIMER_0_CNT 0xCA     //  111us with CLK/8 prescale
#define RC5BITREF1  6  
#define RC5BITREF2  11
#define RC5BITREF3  14

///Defs for Servo - Specific to servo actually used./////

unsigned int min = 1300;	    //min servo travel - OCR1A = 1300
unsigned char demand;   		//data from rc5 decoder - 4 bits
unsigned int span;	   			//max servo travel (min to max) = 3700 counts
unsigned int step;	    		//demand step size = span/9
unsigned char deg = 180;	    //set total mechanical travel required in degrees
unsigned int servo;	    		//servo demand - send count to OCR1A
unsigned char hyb;				//rc5data hi byte
unsigned char lob;				//rc5data lo byte

/*
** function prototypes
*/
unsigned int rc5decode( void );
 


//module global variables 

static volatile uint8_t timerflag;  //must be volatile because modified by interrupt handler


ISR(TIM0_OVF_vect)				    //ISR handler for timer 0 interrupt

{
    timerflag = 1;                  //set global variable 

    TCNT0 = TIMER_0_CNT;            //reset counter to get this interrupt again 
}


unsigned int rc5decode( void )

//decoded RC5 data is returned, or 0x0000 if RC5 data not recognized

{
    unsigned int    rc5data;
    unsigned char   timer, i;
    

// init timer/Counter0 - Used for rc5 decode
    TCCR0B = TMC8_CK8;               // use CLK/8 prescale
    TCNT0 = TIMER_0_CNT;             // set timer T/16 = 111us 
    TIMSK0 = _BV(TOIE0);             // enable TCNT0 overflow interrupt
    
// measure startbit
    timerflag = 0; 
	timer = 0; 
    while ( RC5BitLow() && (timer < RC5BITREF2) ) 
    {
        WAITFORTIMER();
        timer++;
    }
    if ( (timer > RC5BITREF1) && (timer < RC5BITREF2) ) 
    {
        // startbit ok, decode 

        // wait T/4: synchronize in the middle of first half of second bit
        while ( timer < RC5BITREF3 ) 
        {
            WAITFORTIMER();
            timer++;
        }
        
        // read the remaining bits
        rc5data = 1;
        for (i=0; i<13; i++) 
        {
            rc5data <<= 1;  
            if ( RC5BitHigh() ) 
            {
                rc5data |= 0x0001;
          // wait max T/2 for H->L transition (middle of next bit)
                timer = 0;
                while ( RC5BitHigh() && (timer < 16) ) 
                {
                    WAITFORTIMER();
                    timer++;
                }
            }
            else
                { 
                rc5data &= ~0x00001;
                // wait max T/2 for L->H transition (middle of next bit)
                timer = 0;
                while ( RC5BitLow() && (timer < 16) ) 
                    {
                    WAITFORTIMER();
                    timer++;
                    }
                } 
                if ( timer == 16 ) 
                    {
                    rc5data = 0x0000;   // error, next bit not found
                    break;
                    }
            
            // wait 3/4 T: await next bit
                        for ( timer=0; timer < 12 ; timer++) WAITFORTIMER();
                        }

                        }
                        else   
                            {
                            rc5data = 0x0000;  // error, invalid RC-5 code
                            }
                            TCCR0B = TMC8_STOP;    // stop timer0    
    
                            return (rc5data);

}	

//rc5decode and servo output control

int main(void)
{

   CLKPR = 0x80;				//Set CLKPCE (Clock prescaler enable)
   CLKPR = 0x01;				//Set CLKPS bits (Clock prescale select) 
                                //to a division of 2. System clock now 4MHz
   	
   unsigned int rc5data;

// init timer/Counter1	 Used for servo PWM output
	TCCR1A = 0b10000000;			//Set COM1A1 high (Clear OC1A on compare match)
	TCCR1B = 0b00010001;			//Set WGM13 high (Phase and Frequency correct PWM), 
                                    //set CS10 high - prescaler = 1
	ICR1 = 40000;				    //Set PWM frame rate to 50 Hz
    
//Set up input / outputs

    DDRA  = 0xff;         		// use port A for outputs
    PORTA = 0xff;				// Clear Port A - inverted output
  
    DDRB  = 0x00;         		// use all pins on port B for input 
    PORTB = 0xff;         		// enable internal pull-up resistors on inputs.
    
//Set up servo parameters

	span = (deg*20);			//Set servo travel (min to max count) for
								//degrees reqquired
	step = span/9;				//Set demand step size relative to travel

    sei();                      //Enable global interrupts


    for(;;)    					// loop forever
    {
        while ( RC5BitHigh() );  // wait until RC5 code received

        rc5data = rc5decode();	  
		
		if ( rc5data & 0x2000 ) //Check 2 start bits are present
		{
            	
			hyb = (rc5data >>8);				        //Hi byte - address and toggle bits	
			lob = rc5data;						        //Lo byte - address and data bits
			
			if ((hyb == 0x31) || (hyb == 0x39))       //check for valid address in high byte. 
                                                        //2 checks needed due to toggle bit.  				
			{										
	
					if ((lob > 0x39) && (lob < 0x4A))	//Check code function is valid
														//Commands 0 to 9 only reqd.
					{		
						demand = (rc5data&0x000f);		//Demand = lowest 4 bits only	
                    	PORTA = ~demand;	            //Show lo byte of rc5data - optional
                        servo =  (demand*step)+min;	//servo demand count.
                        OCR1A = servo;					//send servo demand to output 
														//compare register.
					}
		
		  				_delay_ms(20); 	                //Reset Port A outputs after delay - optional
						PORTA = ~0; 	                //Reset PORT bits to zero. - optional
			}
		}
	}
}
