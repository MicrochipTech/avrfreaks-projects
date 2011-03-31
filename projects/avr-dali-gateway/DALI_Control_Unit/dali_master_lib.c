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
volatile bit new_edge = 0;

#pragma vector = INT3_vect
__interrupt void edge_detect(void);

__interrupt void edge_detect(void)
{
  new_edge = 1;
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

U8 dali_send_command(U8 tx_byte_1, U8 tx_byte_2, Bool repeat, Bool answer, U8 *rx_byte, U8 repeat_delay)
{
  bit tx_status = OK;
  bit rx_status = OK;

  //1st sending :
  //tx_status is set if an error occurs during transmission  
  tx_status &= dali_send(tx_byte_1, tx_byte_2);
    
  if (repeat == 1)
  {
    //wait before 2nd frame for 'repeat_delay' ms.
    dali_start_timer_0(MS_PERIOD); //tick_Te = 1 every 1 ms
    delay_Te = repeat_delay;
    while(delay_Te !=0);
    dali_stop_timer_0();
    //send 2nd FW frame
    //tx_status is set if an error occurs during transmission  
    tx_status &= dali_send(tx_byte_1, tx_byte_2); 
  }

  if (answer == 1)
  {
    //wait before BW frame
    dali_start_timer_0(TE_PERIOD);    //tick_Te = 1 every Te (416.67us)    
    delay_Te = MIN_FW_BW_DELAY;
    while(delay_Te !=0);
    dali_stop_timer_0();
    
    //receive BW frame
    rx_status &= dali_receive(rx_byte);
  }
  
  dali_start_timer_0(TE_PERIOD);    //tick_Te = 1 every Te (416.67us)    
  delay_Te = END_DELAY;
  while(delay_Te !=0);
  dali_stop_timer_0();

  return (tx_status<<1 | rx_status);
}

/********************/
/*** DALI RECEIVE ***/
/********************/

bit dali_receive(U8 *rx_byte){
  
  U8 rx_buffer = 0;
  U8 edge_counter = 0;
  bit rx_status = OK;

  Setup_int3();
  Enable_int3();
  new_edge = 0;
  
  dali_start_timer_0(TE_PERIOD);    //tick_Te = 1 every Te (416.67us) 
  delay_Te = MAX_FW_BW_DELAY - MIN_FW_BW_DELAY;
  
  while ((new_edge == 0) && (delay_Te != 0));
  
  delay_Te = 0;
  
  if (new_edge == 1){
    //an edge was detected before timeout, start reception
    new_edge = 0;
    edge_counter++;
    
    dali_start_timer_0(MS_PERIOD); //if (tick_Te == 1), 1ms elapsed.
    
    while((edge_counter < 18) && (rx_status == OK))
    {
      while ((new_edge == 0) && (tick_Te == 0));
      new_edge = 0;
      edge_counter++;

      if (TCNT0 > (TE_PERIOD + (TE_PERIOD>>1))){
        //time between 2 edges was > than 1.5Te --> transition from 0 to 1 or 1 to 0
        edge_counter++;
      }
      
      dali_start_timer_0(MS_PERIOD); //reset timer
      
      if (tick_Te == 1){
        //error, too much time between edges (>1ms)
        tick_Te = 0;
        rx_status &= KO;
      }
      
      if ((edge_counter > 2) && ((edge_counter & 0x01) == 0x00)){
        
        Debug_pin(0);
        __delay_cycles(0xf);
        Debug_pin(1);
        
        //sample on even edges
        if (Dali_rx == 1){
          //if a high level is detected, set corresponding bit.
          rx_buffer |= (1<<(9-(edge_counter>>1)));
          //useful edges are even and in range [4-18]
        }
      }  
    }//end while()
  }//end if (new_edge == 1) : no BW frame started, answer is 'NO'
  Disable_int3();
  dali_stop_timer_0();
    
  *rx_byte = rx_buffer;
  return rx_status;
}

/*****************/
/*** DALI SEND ***/
/*****************/

bit dali_send(U8 tx_byte_1, U8 tx_byte_2){

  U8 bit_n = 0;
  U16 tx_buffer = 0;
  U16 tx_mask = 0x8000;
  bit tx_status = OK;
  
  tx_buffer = (tx_byte_1<<8) | (tx_byte_2);
  
  dali_start_timer_0(TE_PERIOD);    //tick_Te = 1 every Te (416.67us)
  tick_Te = 0;
  
  for(bit_n = 0; bit_n < 38; bit_n++)
  {
    switch (bit_n)
    {
      case 0:{              //start bit : 1st half
        if (Dali_rx == 0)   //line level should be high
          tx_status &= KO;
        Dali_tx(0);
        break;
      }
      case 1:{              //start bit : 2nd half
        if (Dali_rx == 1)   //line level should be low
          tx_status &= KO;
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
    tx_status &= KO;
  
  dali_stop_timer_0();
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
  
  tick_Te = 0;
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
  
