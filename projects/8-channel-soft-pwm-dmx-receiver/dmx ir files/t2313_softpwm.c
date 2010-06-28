// 8 channel soft pwm based on avr136 for tiny2313
// with random fading function...
// now with DMX-512 !!

#include <ctype.h>
#include "t2313_softpwm.h"
#include <stdlib.h>
  #include <avr/io.h>
  #include <avr/interrupt.h>
  #include <avr/wdt.h>
#include <util/delay.h>


//! global buffers
unsigned char compare[CHMAX];
volatile unsigned short compbuff[CHMAX];
signed char direction[CHMAX] ;			// -1 or +1 for each channel
unsigned char time[CHMAX];


unsigned short top =300;
unsigned char offtime =250;




//DMX Variables, etc...
unsigned char address; 					// DMX address and random key number... 

enum {IDLE, BREAK, STARTB, STARTADR};	//DMX states

uint8_t 	 gDmxState;
uint8_t 	*gDmxPnt;
uint16_t	 DmxCount;

#define DMX_CHANNELS    (8)					//use at least 2ch

volatile uint8_t	 DmxField[DMX_CHANNELS]; //array of DMX vals (raw)
volatile uint16_t	 DmxAddress;			//start address


volatile uint8_t DMX_go_flag =0; // when flag is 1, use dmx data for pwm, else use random
volatile uint8_t DMX_time_out =0; // count up, when =255, reset DMX_go_flag to 0 (random)






// The program entry point.  Enters an eternal loop.
 __C_task main(void)
{	


	

  	Init();
	
    

	// 	_delay_ms(150); // wait for dmx intterupt to set go flag....

	for(;;)
	{
  
  	
	
	
	if (DMX_go_flag ==1)
  		{
		dmx_function(); // use data from dmx for pwm...
   		
		DMX_time_out ++; // increment the timeout...

		if (DMX_time_out == 255) {DMX_go_flag =0;} // when timeout doesnt get reset from dmx isr.... reset go flag and do random... 
		
		}
       	else 
  		{
	 	random_function(); 
		} // end of if

	}//end of forever loop

} //end of main


void dmx_function (void)
{

unsigned char i =0; 	//loop counter

for(i=0 ; i<CHMAX ; i++)      // do all channels // 0 to 7 = 8 channels
		{
		compbuff[i] = DmxField[i];
		}	
} //end of dmx function




void random_function (void)
{

unsigned char i =0; 	//loop counter
	


 _delay_us(50); // too fast add massive delay
	
		for(i=0 ; i<CHMAX ; i++)      // do all channels // 0 to 7 = 8 channels
  			{
   	 		if (compbuff[i] == 255)	{direction[i] = -1;} //switch direction to down = -1
				
			if (compbuff[i] == 0)	// for when led is off, then delay some time...
				{
				direction[i] = 0;
				if (offtime == 0)
					{	
					direction[i] = +1; 
					offtime =250;
					}
				offtime--;	
				}
		
			if (time[i] == 0) 	
				{
				compbuff[i] += direction[i] ;  // increment or decrement the PWM based on direction
 				time[i] = (rand()%top) ;//+ offset;
				}
			
				
				time[i]--;
		
			} //end of for each channel loop




}












// Init function. This function initialises the hardware
void Init(void)
{
  unsigned char i;//, pwm;

  CLKPR = (1 << CLKPCE);        // enable clock prescaler update
  CLKPR = 0;                    // set clock to maximum (= crystal)

  __watchdog_reset();           // reset watchdog timer
  MCUSR &= ~(1 << WDRF);        // clear the watchdog reset flag
  WDTCSR |= (1<<WDCE)|(1<<WDE); // start timed sequence
  WDTCSR = 0x00;                // disable watchdog timer

  DDRD = (0<<PD6)|(0<<PD2)|(0<<PD3)|(0<<PD4)|(0<<PD5);			// set port d to input
  PORTD = (1<<PD6)|(1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5); // enable pullups for inputs
  PORTA = (1<<PD2); //reset pullup
  DDRB = 0xFF;            		// set port pins to output // entire Port B // PB0 - PB7

//  pwm = PWMDEFAULT;				// set for default value... in header file...

// address for random seed and DMX
address = ((~PIND&(1<<PD6))>>2) | ((~PIND&(1<<PD5))>>5) | ((~PIND&(1<<PD4))>>3) | ((~PIND&(1<<PD3))>>1) | ((~PIND&(1<<PD2))<<1) ;

// address =
// Make DMX address from address, times 8... 8 clusters per drop...
//DmxAddress = (8 * address) -7; // bitshift left 4...
DmxAddress = (8 * address) -7; // bitshift left 4...

  srand(address);	 // use address to send random key...
 
  for(i=0 ; i<CHMAX ; i++)      // initialise all channels // 0 to 7 = 8 channels
  {
  	compare[i] = i*32;
	compbuff[i]= i*32; 
	time[i] = 0;
  }

for(i=0 ; i<4 ; i++) {direction[i] = 1;} //half starts up

for(i=4 ; i<8 ; i++) {direction[i] = -1;} //half starts down


  TIFR = (1 << TOV0);           // clear interrupt flag
  TIMSK = (1 << TOIE0);         // enable overflow interrupt
  TCCR0B = (1 << CS00);         // start timer, no prescale

// Initilize the DMX 
DDRD |= (1<<2);
PORTD &= ~(1<<2);								//enable reception
UBRRH  = 0;
UBRRL  = ((F_CPU/4000000)-1);						//250kbaud, 8N2
UCSRC |= (3<<UCSZ0)|(1<<USBS);
UCSRB |= (1<<RXEN)|(1<<RXCIE);
gDmxState= IDLE;

//uint8_t i;
for (i=0; i<DMX_CHANNELS; i++)
	{
	DmxField[i]= 0;
	}
//end of dmx init



  sei();         				// enable interrupts // sei();
}

// Interrupt Service Routine
SIGNAL (SIG_TIMER0_OVF)
{
  static unsigned char pinlevelB=0xFF; // was portmask
  static unsigned char softcount=0xFF;

  PORTB = pinlevelB;            // update outputs

  if(++softcount == 0){         // increment modulo 256 counter and update
                                // the compare values only when counter = 0.
    compare[0] = compbuff[0];   // verbose code for speed
    compare[1] = compbuff[1];
    compare[2] = compbuff[2];
    compare[3] = compbuff[3];
    compare[4] = compbuff[4];
    compare[5] = compbuff[5];
    compare[6] = compbuff[6];
    compare[7] = compbuff[7];  // last element should equal CHMAX - 1

    pinlevelB = 0xFF;     // set all port pins high

  }
  // clear port pin on compare match (executed on next interrupt)
  if(compare[0] == softcount) CH0_CLEAR;
  if(compare[1] == softcount) CH1_CLEAR;
  if(compare[2] == softcount) CH2_CLEAR;
  if(compare[3] == softcount) CH3_CLEAR;
  if(compare[4] == softcount) CH4_CLEAR;
  if(compare[5] == softcount) CH5_CLEAR;
  if(compare[6] == softcount) CH6_CLEAR;
  if(compare[7] == softcount) CH7_CLEAR;
 
} // end of ISR ...


// *************** DMX Reception ISR ****************
ISR (USART_RX_vect)
{
DMX_go_flag =1; //set the volatile flag to indicate DMX is active
DMX_time_out =0; // reset timeout counter to 0

uint8_t USARTstate= UCSRA;						//get state
uint8_t DmxByte= UDR;							//get data
uint8_t DmxState= gDmxState;					//just get once from SRAM!!!
 
if (USARTstate &(1<<FE))						//check for break
	{
	UCSRA &= ~(1<<FE);							//reset flag
	DmxCount= DmxAddress;						//reset frame counter
	gDmxState= BREAK;
	}

else if (DmxState == BREAK)
	{
	if (DmxByte == 0) 
		{
		gDmxState= STARTB;						//normal start code detected
		gDmxPnt= ((uint8_t*)DmxField +1);
		}
	else gDmxState= IDLE;
	}
	
else if (DmxState == STARTB)
	{
	if (--DmxCount == 0)						//start address reached?
		{
		gDmxState= STARTADR;
		DmxField[0]= DmxByte;
		}
	}

else if (DmxState == STARTADR)
	{
	uint8_t *DmxPnt;
	DmxPnt= gDmxPnt;
	*DmxPnt= DmxByte;
	if (++DmxPnt >= (DmxField +DMX_CHANNELS)) 	//all ch received?
		{
		gDmxState= IDLE;
		}
	else gDmxPnt= DmxPnt;
	}							
}

