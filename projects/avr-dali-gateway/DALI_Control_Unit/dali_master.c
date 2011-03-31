//_____ I N C L U D E S ____________________________________________________

#include "config.h"
#include "dali_master.h"

//_____ M A C R O S ________________________________________________________

#define DALI_TX(x)		(x==0 ? (DALI_TX_PORT |= (1<<DALI_TX_PIN)) : (DALI_TX_PORT &= ~(1<<DALI_TX_PIN)))

//_____ D E F I N I T I O N S ______________________________________________

volatile Bool tick;

//_____ D E C L A R A T I O N S ____________________________________________

#pragma vector = TIMER0_COMPA_vect
__interrupt void dali_tick(void);

__interrupt void dali_tick(void)
{
  tick = 1;
}

Bool dali_send_frame(U8 byte1, U8 byte2, Bool repeat, Bool answer)
{
  U32 txBuffer = 0x00000000; //contains a manchester coded frame
  U32 mask = 0x80000000;
  U8 i;
  U8 daliBitN = 0;  //index of txBuffer
  Bool bitValue;
  
  //filling txBuffer
  for (i=0; i<8; i++){
    if (((byte1)&(1<<i)) != 0){
      txBuffer |= 1<<(2*i);
    }
    else
      txBuffer |= 1<<(2*i+1);
  }
  for (i=0; i<8; i++){
    if (((byte2)&(1<<i)) != 0){
      txBuffer |= 1<<(2*i+16);
    }
    else
      txBuffer |= 1<<(2*i+17);
  }  
  
  dali_setup_TX_timer();
  
  //send FW frame
  for (daliBitN=0; daliBitN<38; daliBitN++){
    
    if (daliBitN == 0){  //start bit, 1st half
      bitValue = 0;
    }else{
      if (daliBitN == 1){  //start bit, 2nd half
        bitValue = 1;
      }else{
        if (daliBitN >= 34){ //stop bits (34-38)
          bitValue = 1;
        }else{
          if ((txBuffer & (mask)) != 0){
            bitValue = 1;
          }else{
            bitValue = 0;  
          }
          mask = mask>>1;
        }
      }
    } 
    DALI_TX(bitValue);
        
    while(tick == 0); //loop waiting for timer tick
    tick = 0;
  }
  
  return 1;
}

void dali_setup_TX_timer(void)
{
	// ****** Inits timer 0 to generate an interrupt every 416µs (1/2 dali bit=Te), used for FW Frames
	
	//reset prescaler GTCCR
		//0 : a 1 stops the prescaler to synchronise timers
		//0 : input capture pin selection (not used)
		//00000 : reserved
		//1 : resets the prescaler
	
	GTCCR = 0x00;
	
	//init	TCCR0A : 
		//00 : no connection with output compare pin
		//00 : no connection with output compare pin
		//00 : reserved
		//10 :	clear timer on compare match
	
	TCCR0A = 0x02;
	
	//init	TCCR0B :
		//00 : no force output compare
		//00 : reserved
		//0 : clear timer on compare match
		//010 : internal clock, Prescaler : PREDIV macro
	
	TCCR0B = 0x00 + PREDIV;
  
	//TCCR0B = 0x05;
	//set compare value OCR0A
		//OCR0A = 0x34 (52d --> 416µs period)	COMP_VALUE macro
		//OCR0B = 0xFF (not used)
	
	OCR0A = COMP_VALUE;
	OCR0B = 0xFF;
	
  //clear interrupt flags (OCF0B, OCF0A, TOV0)
  
 	TIFR0 |= 0x02;
  
	//activate interrupts TIMSK0
		//00000 : reserved
		//010 : interrupt on match with OCR0A

	TIMSK0 |= 0x02;
  return;
}	

