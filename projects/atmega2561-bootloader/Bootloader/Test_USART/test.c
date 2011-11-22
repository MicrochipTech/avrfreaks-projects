#include <ioavr.h>
#include <intrinsics.h>
#include "../common.h"
#include "../usart.h"
#include "leds.h"
#include "../protocol.h"
#include "../move_iv.h"

#define NONE 0
#define JUMP_TO_BOOTLOADER 1

#pragma vector = TIMER0_COMP_vect
__interrupt void timer0(void);

#pragma vector = USART0_RX_vect
__interrupt void usart0_rx(void);

static inline void leds_init() {
  bit_set(LED_DDR1, LED1);
  bit_set(LED_DDR2, LED2);
  bit_set(LED_DDR3, LED3);
  bit_set(LED_PORT1, LED1);
  bit_set(LED_PORT2, LED2);
  bit_set(LED_PORT3, LED3);
}

unsigned char action=NONE;
__eeprom unsigned char test1@0x000 = 0x00;
__eeprom unsigned char test2@0x001 = 0xee;
__eeprom unsigned char test3@0x010 = 0xaa;

static void usart0_init(unsigned char baud){
  UBRR0H = (unsigned char)(baud>>8);
  UBRR0L = (unsigned char)baud;
  
  // enable for double speed
  //UCSRA = (1<<U2X);
  // enable receiver, transmitter and receiver interrupts
  UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);
  // set frame format 8 data bits, 1 stop bit
  UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);// | (1<<UPM01);
}

// Receives a byte from USART0
static unsigned char usart0_rx_byte() {
  while (!bit_get(UCSR0A, 1<<RXC0));
  return UDR0;
}

// Transmits a byte to USART0
void usart0_tx_byte(unsigned char byte) {
  while(!(UCSR0A & (1<<UDRE0)));
  UDR0 = byte;
}

/*
TODO:
Besluta om det är bättre att använda WDT och MCUCSR för att komma till BLS.
Problemet är att ingen app då kan använda WDT till annat.
*/

int main() {
  leds_init();
  usart0_init(51);
  action = NONE;
  OCR0A = 255;
  bit_set(TIMSK0, 0x02);
  bit_set(TCCR0A, 0x05);
  unsigned char test;
  test = test1;
  test = test2;
  test = test3;
  __enable_interrupt();
  while(1) {
    switch (action) {
      case JUMP_TO_BOOTLOADER:
        __disable_interrupt();
        action = NONE;
        usart0_tx_byte(ACK);
        __indirect_jump_to(0x1E000);
        while(1);
    }
    bit_flip(LED_PORT1, LED1);
    __delay_cycles(1600000);
    bit_flip(LED_PORT2, LED2);
    __delay_cycles(1600000);
  }
}

__interrupt void timer0(void) {
  bit_flip(LED_PORT3, LED3);
}

__interrupt void usart0_rx(void) {
  unsigned char rx_data;
  rx_data = usart0_rx_byte();
  switch (rx_data) {
    case JUMP_BOOTLOADER:
      action = JUMP_TO_BOOTLOADER;
      break;
  }
}
