//_____  I N C L U D E S ___________________________________________________
#include "control_gear_main.h"
#include "dali_lib/dali_lib.c"

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N S ______________________________________________


//_____ D E C L A R A T I O N S ____________________________________________

  /*** This interrupt routine is called every 1 ms ***/

#pragma vector = TIMER1_COMPA_vect
__interrupt void tick_1ms(void);

__interrupt void tick_1ms(void){
  dali_tick();
  __delay_cycles(7750); //simulates CPU load
  return;
}

void main(void)
{
  U8 output_level = 0;
  U8 lamp_status = 1;
  

  usr_init_ports();
  usr_init_timer_1();
  Enable_interrupt();  
  //enable global interrupts
  
  dali_init();

  while(1)
  {        
    output_level = dali_control_gear(lamp_status);
    usr_update_output_level(output_level);
    lamp_status = usr_check_lamp();
  }
}

void usr_init_ports(void)
{
  //PD3 : DALI Tx PIN (output)
  //PD4 : DALI Rx PIN (input)
  //PD0 : Lamp Disconnected when SW0 pressed (input)
  //PB0..7 : LED bargraph (output)
  
  //writing PORTx when the port is configured as inputs enables pull-ups.
  
  DDRB  = 0xff; PORTB = 0xff; 
  DDRC  = 0xff; PORTC = 0x00; 
  DDRD  = 0x08; PORTD = 0xff; 
  DDRE  = 0xff; PORTE = 0x00;
}

U8 usr_check_lamp(void)
{
  U8 lamp_failure = 0;
  
  if ((PIND & 0x01) == 0)
  {
    lamp_failure = 1;
    usr_update_output_level(0);
  }
  else 
    lamp_failure = 0;
  
  return lamp_failure;
}

void usr_update_output_level(U8 outputLevel)
{
  BAR_POWER(outputLevel);
}

  /*** configure timer 1 to interrupt every 1 ms ***/

void usr_init_timer_1(void){
  
  Dali_set_tccr1a();
  Dali_set_tccr1b();
  Dali_set_tccr1c();
  Dali_set_t1_period();
  Dali_set_tifr1();
  Dali_set_timsk1();
} 
