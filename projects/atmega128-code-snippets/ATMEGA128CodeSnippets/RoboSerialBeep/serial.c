#include <avr/io.h>
#include <avr\interrupt.h>
#include <avr\iom128.h>

unsigned char temp = 0;

extern int _textmode;

void port_init(void)
{
 PORTA = 0xFF;
 DDRA  = 0x00;
 PORTB = 0x7F;
 DDRB  = 0x80;
 PORTC = 0xFF; //m103 output only
 DDRC  = 0x00;
 PORTD = 0xFF;
 DDRD  = 0x00;
 PORTE = 0xFF;
 DDRE  = 0x00;
 PORTF = 0xFF;
 DDRF  = 0x00;
 PORTG = 0x1F;
 DDRG  = 0x00;
}

//UART1 initialisation
// desired baud rate:9600
// actual baud rate:9600 (0.0%)
// char size: 8 bit
// parity: Disabled
void uart1_init(void)
{
 UCSR1B = 0x00; //disable while setting baud rate
 UCSR1A = 0x00;
 UCSR1C = 0x06;
 UBRR1L = 0x47; //set baud rate lo
 UBRR1H = 0x00; //set baud rate hi
 UCSR1B = 0x98;
}

SIGNAL(SIG_UART1_RECV) { // USART RX interrupt
 temp = UDR1;
 
 if(temp == 0x69)
 {
  PORTB = 0xFF; // Buzzer on
 }
 
  if(temp == 0x6A)
 {
  PORTB = 0x00; // Buzzer off
 }
}


//call this routine to initialise all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 XDIV  = 0x00; //xtal divider
 XMCRA = 0x00; //external memory
 port_init();
 uart1_init();

 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x00; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 sei(); //re-enable interrupts
 //all peripherals are now initialised
}

//
int main(void)
{
 init_devices();
 //insert your functional code here...
}
