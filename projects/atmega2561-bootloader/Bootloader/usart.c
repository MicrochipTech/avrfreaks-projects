#include <ioavr.h>
#include "usart.h"
#include "common.h"
#include "bl_can.h"
#include "bootloader.h"
#include "queue.h"
#include "leds.h"
#include "intel_hex.h"
#include "protocol.h"

#ifdef USART

#pragma vector = USART0_RX_vect
__interrupt void usart0_rx(void);

void usart0_init(unsigned char baud){
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
unsigned char usart0_rx_byte() {
  while (!bit_get(UCSR0A, 1<<RXC0));
  return UDR0;
}

// Transmits a byte to USART0
void usart0_tx_byte(unsigned char byte) {
  while(!(UCSR0A & (1<<UDRE0)));
  UDR0 = byte;
}

// Transmits all app_info_t:s to USART0
void usart0_tx_app_info() {
  unsigned char i;
  usart0_tx_byte(no_of_mcus_found);
  for (i=0; i < no_of_mcus_found; i++) {
    /*
    usart0_tx_int(remote_apps[i].app_info.address);
    usart0_tx_int(remote_apps[i].app_info.version);
    usart0_tx_str(remote_apps[i].app_info.name, 8);
    usart0_tx_long(remote_apps[i].app_info.date);
    */
    usart0_tx_block((void *)&(remote_apps[i].app_info), sizeof(app_info_t));
  }
}

// Transmits an int to USART0
void usart0_tx_int(unsigned int data) {
  usart0_tx_byte((unsigned char)data);
  data = data >> 8;
  usart0_tx_byte((unsigned char)data);  
}

// Transmits a string of length len to USART0
void usart0_tx_str(char *data, unsigned char len) {
  usart0_tx_block((void *)data, len);
}

// Transmits a long (4 bytes) to USART0
void usart0_tx_long(unsigned long data) {
  usart0_tx_byte((unsigned char)data);
  data = data >> 8;
  usart0_tx_byte((unsigned char)data);
  data = data >> 8;
  usart0_tx_byte((unsigned char)data);
  data = data >> 8;
  usart0_tx_byte((unsigned char)data);
}

void usart0_tx_block(void *block, unsigned int size) {
  unsigned char *bytes = (unsigned char *)block;
  unsigned int i;
  for (i=0; i < size; i++)
    usart0_tx_byte(bytes[i]);
}

void process_usart_queue() {
  while (!byte_queue_is_empty(&usart_rx_queue))
    handle_in_byte();
}

// Handles USART receive interrupt
__interrupt void usart0_rx(void) {
  bit_flip(PORTA, LED2);
  unsigned char rx_data;
  rx_data = usart0_rx_byte();
  byte_queue_enqueue(&usart_rx_queue, &rx_data);
}
#endif
