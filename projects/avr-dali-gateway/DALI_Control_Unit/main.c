//_____  I N C L U D E S ___________________________________________________
#include "main.h"
#include "config.h"

//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

//_____ D E C L A R A T I O N S ____________________________________________

void main(void)
{
  U8 rx_byte;
  U8 power_level = 0;
  
  init_ports();
	Enable_interrupt();
  
  Dali_tx(1);
  
  do
  {
    power_level++;
	  dali_send_command(0xff,0x90,0,1, &rx_byte, 50);
    __delay_cycles(0xfffff);
    while(rx_byte == 0xff);
  }
  while(1);
}

void init_ports(void)
{
  DDRA = 0xff; PORTA = 0x08;  //(O) Red Led on A3, (O) Blue Led on A4
  DDRB = 0xff; PORTB = 0x00;  //nothing
  DDRC = 0xff; PORTC = 0x00;  //nothing
  DDRD = 0xf7; PORTD = 0x00;  //(I) RxDALI on D3, (O) TxDALI on D4
  DDRE = 0xff; PORTE = 0x00;  //nothing
  DDRF = 0x0f; PORTF = 0x00;  //F4-F7 : JTAG
  
  return;
}


  
