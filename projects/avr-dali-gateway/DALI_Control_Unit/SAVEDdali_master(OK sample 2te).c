//_____ I N C L U D E S ____________________________________________________

#include "config.h"
#include "dali_master_lib.h"

//_____ M A C R O S ________________________________________________________

#define Dali_disable_timer()		(TIMSK0 = 0x00)	

#define Dali_tx(x)		(x==0 ? ((DALI_TX_PORT |= (1<<DALI_TX_PIN)),\
                               PORTA = 0x00)\
                            : ((DALI_TX_PORT &= ~(1<<DALI_TX_PIN)),\
                               PORTA = 0x10))
#define Dali_rx				((DALI_RX_PORT & (1<<DALI_RX_PIN)) == 0 ? 1 : 0)	

#define Debug_pin(x)  ( x==0 ? (PORTA |= 0x20) : (PORTA &= 0xdf) )

#define Enable_int3()   ( (EIFR |= 0x08),\
                          (EIMSK |= 0x08) )

#define Disable_int3()  ( EIMSK &= 0xf7 )
#define Setup_int3()    ( EICRA = INT3_SNS<<ISC30 \
                                | INT2_SNS<<ISC20 \
                                | INT1_SNS<<ISC10 \
                                | INT0_SNS<<ISC00 )




//_____ D E F I N I T I O N S ______________________________________________

volatile bit tick_Te = 0;
volatile U8 delay_Te = 0;
volatile bit bw_started = 0;

#pragma vector = INT3_vect
__interrupt void start_bw(void);

__interrupt void start_bw(void)
{
  bw_started = 1;
  return;
}


#pragma vector = TIMER0_COMPA_vect
__interrupt void dali_tick(void);

__interrupt void dali_tick(void)
{
  tick_Te = 1;
  if (delay_Te != 0)
    delay_Te--;
  return;
}

/*************************/
/*** DALI SEND COMMAND ***/
/*************************/

bit dali_send_command(U8 tx_byte_1, U8 tx_byte_2, Bool repeat, Bool answer, U8 *rx_byte, U8 repeat_delay)
{
  bit tx_status = 0;
  U8 answer_byte = 0;
/***/ Debug_pin(1);
  //1st sending :
  //tx_status is set if an error occurs during transmission  
  dali_start_timer_0(TE_PERIOD);    //tick_Te = 1 every Te (416.67us)
  tx_status |= dali_send(tx_byte_1, tx_byte_2);
    
  if (repeat == 1)
  {
    //wait before 2nd frame
    dali_start_timer_0(MS_PERIOD); //tick_Te = 1 every 1 ms
    delay_Te = repeat_delay;
    while(delay_Te !=0);
    //send 2nd FW frame
    //tx_status is set if an error occurs during transmission  
    tx_status |= dali_send(tx_byte_1, tx_byte_2); 
  }

  if (answer == 1)
  {
    //wait before BW frame
    dali_start_timer_0(TE_PERIOD);    //tick_Te = 1 every Te (416.67us)    
    delay_Te = MIN_FW_BW_DELAY;
    while(delay_Te !=0);
    //receive BW frame
    answer_byte = dali_receive();
    *rx_byte = answer_byte;
  }
  
  delay_Te = END_DELAY;
  while(delay_Te !=0);
  
  dali_stop_timer_0();  
  return tx_status;
}

/********************/
/*** DALI RECEIVE ***/
/********************/

U8 dali_receive(void){
  
  U8 rx_buffer = 0;
  U8 bit_n = 0;

  Setup_int3();
  Enable_int3();
  //waiting for 1st edge of BW frame
  delay_Te = MAX_FW_BW_DELAY - MIN_FW_BW_DELAY;
  
  //while(bw_started == 0);
  while ((bw_started == 0) && (delay_Te != 0));
  
/***/Debug_pin(0);
__delay_cycles(0xf);
/***/Debug_pin(1);

  Disable_int3();
  delay_Te = 0;
  
  if (bw_started == 1){
    //BW frame started before timeout
    bw_started = 0;
    tick_Te = 0;
    //1.5 Te to sample on the middle of 2nd half of the start bit
    dali_start_timer_0(TE_PERIOD+(TE_PERIOD>>1)); 
    //wait...
    while(tick_Te == 0);
    tick_Te = 0;
    //sample 1st bit (start bit)
    if (Dali_rx == 1){
      //start bit received
      //reconfigure timer
      dali_start_timer_0(TE_PERIOD<<1);
      //sample other bits...(1st: MSB -  8th: LSB)
/***/Debug_pin(0);
__delay_cycles(0xf);
/***/Debug_pin(1);      
      for(bit_n = 0; bit_n < 8; bit_n++)
      {
        while(tick_Te == 0);
        tick_Te = 0;
/***/Debug_pin(0);
__delay_cycles(0xf);
/***/Debug_pin(1);        
        if (Dali_rx == 1){
          //if a high level is detected, set corresponding bit.
          rx_buffer |= (1<<(7-bit_n));  
        }
      }
    }//start bit not received, cancel reception
  }
    
  return rx_buffer;
}

/*****************/
/*** DALI SEND ***/
/*****************/

bit dali_send(U8 tx_byte_1, U8 tx_byte_2){

  U8 bit_n = 0;
  U16 tx_buffer = 0;
  U16 tx_mask = 0x8000;
  bit tx_status = TX_OK;
  
  tx_buffer = (tx_byte_1<<8) | (tx_byte_2);
    
  tick_Te = 0;
  
  for(bit_n = 0; bit_n < 38; bit_n++)
  {
    switch (bit_n)
    {
      case 0:{              //start bit : 1st half
        if (Dali_rx == 0)   //line level should be high
          tx_status |= TX_ERROR;
        Dali_tx(0);
        break;
      }
      case 1:{              //start bit : 2nd half
        if (Dali_rx == 1)   //line level should be low
          tx_status |= TX_ERROR;
        Dali_tx(1);
        break;
      }
      case 34:{Dali_tx(1); break;}    
      case 35:{Dali_tx(1); break;}
      case 36:{Dali_tx(1); break;}
      case 37:{Dali_tx(1); break;}
      
      default:{
        if ((bit_n & 0x01) == 0){  //odd half bits
          Dali_tx((tx_buffer & tx_mask)==0);
        }else{                      //even half bits
          Dali_tx((tx_buffer & tx_mask)!=0);
          tx_mask = tx_mask>>1;
        }
        break;
      }//end default
    }//end switch
    
    while (tick_Te == 0);   //wait for timer interrupt, every TE_PERIOD
    tick_Te = 0;
    
  }//end for
  
  //idle line state is high. 
  Dali_tx(1);
  
  if (Dali_rx == 0)   //line level should be high
    tx_status |= TX_ERROR;
  return tx_status;
}

/************************/
/*** DALI START TIMER ***/
/************************/

void dali_start_timer_0(U8 period)
{
	// ****** Inits timer 0 to generate an interrupt every 416µs (1/2 dali bit=Te), used for FW Frames
	
	//reset prescaler GTCCR
		//0 : a 1 stops the prescaler to synchronise timers
		//0 : input capture pin selection (not used)
		//00000 : reserved
		//1 : resets the prescaler
	
	GTCCR = 0x01;
	
  //Reset timer :
  
  TCNT0 = 0;
  
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
	
	OCR0A = period;
	OCR0B = 0xFF;
	
  //clear interrupt flags (OCF0B, OCF0A, TOV0)
  
 	TIFR0 |= 0x02;
  
	//activate interrupts TIMSK0
		//00000 : reserved
		//010 : interrupt on match with OCR0A

	TIMSK0 |= 0x02;
  return;
}	

/***********************/
/*** DALI STOP TIMER ***/
/***********************/

void dali_stop_timer_0(void)
{
	TIMSK0 &= 0xfd;
  return;
}	
  
