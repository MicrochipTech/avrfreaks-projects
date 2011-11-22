#include <ioavr.h>
#include <intrinsics.h>
#include "../common.h"
#include "../bl_can.h"
#include "leds.h"
#include "../protocol.h"
#include "../move_iv.h"

#define NONE 0
#define JUMP_TO_BOOTLOADER 1

#pragma vector = TIMER0_COMP_vect
__interrupt void timer0(void);

#pragma vector = CANIT_vect
__interrupt void Check_CAN_int(void);

static inline void leds_init() {
  bit_set(LED_DDR1, LED1);
  bit_set(LED_DDR2, LED2);
  bit_set(LED_DDR3, LED3);
  bit_set(LED_PORT1, LED1);
  bit_set(LED_PORT2, LED2);
  bit_set(LED_PORT3, LED3);
}

void set_rx_mbs() {
  ident_in_high = BROADCAST_HIGH;
  ident_in_low = BROADCAST_LOW;
  can_rx_set(1); // messages during forward
}

unsigned char ident_in_high ;  /* CAN RX buffer */
unsigned char ident_in_low ;
unsigned char data_in_count;
unsigned char data_in[8];

unsigned char ident_out_high ;  /* CAN TX buffer */
unsigned char ident_out_low ;
unsigned char data_out_count;
unsigned char data_out[8];

unsigned char action=NONE;


/*
TODO:
Besluta om det är bättre att använda WDT och MCUCSR för att komma till BLS.
Problemet är att ingen app då kan använda WDT till annat.
*/

int main() {
  leds_init();
  init_can();
  set_rx_mbs();
  action = NONE;
  OCR0A = 255;
  bit_set(TIMSK0, 0x02);
  bit_set(TCCR0A, 0x05);
  __enable_interrupt();
  while(1) {
    switch (action) {
      case JUMP_TO_BOOTLOADER:
        __disable_interrupt();
        action = NONE;
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

__interrupt void Check_CAN_int(void) {
  char mob_no = (CANHPMOB >> 4);
  int Chech_rec = can_rx(mob_no);
  switch (mob_no) {
    case 1:
      if (data_in[0] == JUMP_BOOTLOADER) {
        action = JUMP_TO_BOOTLOADER;
      }
      break;
  }
}
