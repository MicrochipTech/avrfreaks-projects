#include <ioavr.h>
#include "bl_can.h"
#include "bootloader.h"
#include "intel_hex.h"
#include "protocol.h"
#include "usart.h"
#include "leds.h"
#include "common.h"
#include "queue.h"

#ifdef USART
remote_app_info_t remote_apps[NO_OF_MCUS];
volatile unsigned char no_of_mcus_found = NO_OF_MCUS;

#pragma vector = TIMER1_COMPA_vect
__interrupt void mcu_timeout(void);

#endif

volatile unsigned char app_no; // app_no=0 is the master bootloader
unsigned char in_counter = 0, out_counter = 0;
unsigned char ident_in_high ;  /* CAN RX buffer */
unsigned char ident_in_low ;
unsigned char data_in_count;
unsigned char data_in[8];

can_data_t current_rx_data;

unsigned char ident_out_high ;  /* CAN TX buffer */
unsigned char ident_out_low ;
unsigned char data_out_count;
unsigned char data_out[8];

#pragma vector = CANIT_vect
__interrupt void Check_CAN_int(void);

#ifdef USART

void set_rx_mbs() {
  ident_in_high = FWD_MBOX_HIGH;
  ident_in_low = FWD_MBOX_LOW;
  can_rx_set(1); // forwarding mbox
  ident_in_high = APP_INFO_MBOX_HIGH;
  ident_in_low = APP_INFO_MBOX_LOW;
  can_rx_set(2); // app_info mbox
  ident_in_high = MSG_MBOX_HIGH;
  ident_in_low = MSG_MBOX_LOW;
  can_rx_set(3); // messages during forward
}

// Enables a timeout counter on TIMER0. The timeout is set to about 0.5 s
// on a 16 MHz clock frequency.
// On timeout all MCUs has not answered, the number of MCUs is the decreased.
static void enable_mcu_timeout() {
  OCR1AH = 0x1E;
  OCR1AL = 0x84;
  TCNT1H = 0;
  TCNT1L = 0;
  TCCR1B = 0x0D;
  TIMSK1 = 0x02;
}

void can_get_app_info() {
  // The first app_info is the overlay card
  read_app_info(&(remote_apps[0].app_info));
  remote_apps[0].app_info.address = 0;
  
  no_of_mcus_found = NO_OF_MCUS; // Try to get app_infos from all MCUs
  ident_out_high = BROADCAST_HIGH;
  ident_out_low = BROADCAST_LOW;
  can_tx_byte(GET_CARD_INFO);
  enable_mcu_timeout();
}


#elif defined CAN
void set_rx_mbs() {
  ident_in_high = MBOX_HIGH;
  ident_in_low = MBOX_LOW;
  can_rx_set(1);
  ident_in_high = BROADCAST_HIGH;
  ident_in_low = BROADCAST_LOW;
  can_rx_set(2);
}
#endif

void str8cpy(char *dest, const char *from) {
  for (unsigned char i=0; i < 8; i++)
    dest[i] = from[i];
}

#ifdef CAN
void can_tx_app_info_with_sender(app_info_t *app_info, unsigned char sender) {
  unsigned long tmp = app_info->date;
  can_data_t pkg;
  pkg.count = 8;
  pkg.id_high = ident_out_high;
  pkg.id_low = ident_out_low;
  pkg.data[0] = sender;
  pkg.data[1] = (unsigned char)MBOX_LOW;
  pkg.data[2] = (unsigned char)MBOX_HIGH;
  pkg.data[3] = (unsigned char)app_info->version;
  pkg.data[4] = (unsigned char)(app_info->version>>8);
  pkg.data[5] = (unsigned char)app_info->name[0];
  pkg.data[6] = (unsigned char)app_info->name[1];
  pkg.data[7] = (unsigned char)app_info->name[2];
  can_queue_enqueue(&can_tx_queue, &pkg);

  pkg.count = 8;
  pkg.data[0] = sender;
  pkg.data[1] = (unsigned char)app_info->name[3];
  pkg.data[2] = (unsigned char)app_info->name[4];
  pkg.data[3] = (unsigned char)app_info->name[5];
  pkg.data[4] = (unsigned char)app_info->name[6];
  pkg.data[5] = (unsigned char)app_info->name[7];
  pkg.data[6] = (unsigned char)tmp;
  tmp >>= 8;
  pkg.data[7] = (unsigned char)tmp;
  can_queue_enqueue(&can_tx_queue, &pkg);

  pkg.count = 3;
  pkg.data[0] = sender;
  tmp >>= 8;
  pkg.data[1] = (unsigned char)tmp;
  tmp >>= 8;
  pkg.data[2] = (unsigned char)tmp;
  can_queue_enqueue(&can_tx_queue, &pkg);
}
#endif

void can_tx_app_info(app_info_t *app_info) {
  unsigned long tmp = app_info->date;
  can_data_t pkg;
  pkg.count = 8;
  pkg.id_high = ident_out_high;
  pkg.id_low = ident_out_low;
#ifdef CAN
  pkg.data[0] = (unsigned char)MBOX_LOW;
  pkg.data[1] = (unsigned char)MBOX_HIGH;
#elif defined USART
  pkg.data[0] = (unsigned char)app_info->address;
  pkg.data[1] = (unsigned char)(app_info->address>>8);
#endif
  pkg.data[2] = (unsigned char)app_info->version;
  pkg.data[3] = (unsigned char)(app_info->version>>8);
  pkg.data[4] = (unsigned char)app_info->name[0];
  pkg.data[5] = (unsigned char)app_info->name[1];
  pkg.data[6] = (unsigned char)app_info->name[2];
  pkg.data[7] = (unsigned char)app_info->name[3];
  can_queue_enqueue(&can_tx_queue, &pkg);

  pkg.count = 8;
  pkg.data[0] = (unsigned char)app_info->name[4];
  pkg.data[1] = (unsigned char)app_info->name[5];
  pkg.data[2] = (unsigned char)app_info->name[6];
  pkg.data[3] = (unsigned char)app_info->name[7];
  pkg.data[4] = (unsigned char)tmp;
  tmp >>= 8;
  pkg.data[5] = (unsigned char)tmp;
  tmp >>= 8;
  pkg.data[6] = (unsigned char)tmp;
  tmp >>= 8;
  pkg.data[7] = (unsigned char)tmp;
  can_queue_enqueue(&can_tx_queue, &pkg);
}

void can_tx_byte(unsigned char byte) {
  can_data_t pkg;
  pkg.count = 1;
  pkg.id_high = ident_out_high;
  pkg.id_low = ident_out_low;
  pkg.data[0] = byte;
  can_queue_enqueue(&can_tx_queue, &pkg);
}

void can_pkg_add_byte(unsigned char byte) {
  static can_data_t pkg = {0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0}};
  pkg.id_high = ident_out_high;
  pkg.id_low = ident_out_low;
  pkg.data[pkg.count++] = byte;
  if (byte == 0 || pkg.count == 8) {
    can_queue_enqueue(&can_tx_queue, &pkg);
    pkg.count = pkg.id_high = pkg.id_low = 0;
  }
}

void can_forward_bytes(unsigned char bytes[], unsigned char len) {
  can_data_t pkg;
  pkg.count = len;
  pkg.id_high = ident_out_high;
  pkg.id_low = ident_out_low;
  for (unsigned char i=0; i < len; i++)
    pkg.data[i] = bytes[i];
  can_queue_enqueue(&can_tx_queue, &pkg);
}

void can_tx_int(unsigned int data) {
  can_data_t pkg;
  pkg.count = 2;
  pkg.id_high = ident_out_high;
  pkg.id_low = ident_out_low;
  pkg.data[0] = (unsigned char)data;
  pkg.data[1] = (unsigned char)(data>>8);
  can_queue_enqueue(&can_tx_queue, &pkg);
}

unsigned char can_rx_byte(int mbox) {
  return current_rx_data.data[in_counter++];
}

void can_rx_app_info_with_sender(unsigned char id_high) {
  unsigned char byte = can_rx_byte(2);
  unsigned char i;
  // Find the app_info to fill
  for (i=1; i < app_no; i++) {
    if ((unsigned char)(remote_apps[i].app_info.address>>8) == id_high)
      break;
  }
  // If no remote_app was found, fill the next available
  if (i == app_no)
    i = app_no++;

  if (i == NO_OF_MCUS)
    i = 1;
  if (remote_apps[i].byte_no == 0) {
    remote_apps[i].app_info.address = \
      remote_apps[i].app_info.version = remote_apps[i].app_info.date = 0;
    remote_apps[i].app_info.address = ((unsigned int)id_high)<<8;
  }
  if (remote_apps[i].byte_no <= 1) {
    remote_apps[i].app_info.address |= ((unsigned int)(byte)<<(remote_apps[i].byte_no*8));
    remote_apps[i].byte_no++;
  }
  else if (remote_apps[i].byte_no >= 2 && remote_apps[i].byte_no <= 3) {
    remote_apps[i].app_info.version |= ((unsigned int)(byte)<<((remote_apps[i].byte_no-2)*8));
    remote_apps[i].byte_no++;
  }
  else if (remote_apps[i].byte_no >= 4 && remote_apps[i].byte_no <= 11) {
    remote_apps[i].app_info.name[remote_apps[i].byte_no-4] = byte;
    remote_apps[i].byte_no++;
  }
  else if (remote_apps[i].byte_no >= 12 && remote_apps[i].byte_no <= 15) {
    remote_apps[i].app_info.date |= ((unsigned long)(byte)<<((remote_apps[i].byte_no-12)*8));
    if (remote_apps[i].byte_no == 15)
      remote_apps[i].byte_no = 0;
    else
      remote_apps[i].byte_no++;

  }
}

__monitor __interrupt void Check_CAN_int(void) {
  bit_flip(LED_PORT3, LED3);
  char mob_no = (CANHPMOB >> 4);
  int Chech_rec = can_rx(mob_no);
  can_data_t rx_data;
  unsigned char i;
  rx_data.count = data_in_count;
  rx_data.mob_no = mob_no;
  for (i=0; i < data_in_count; i++)
    rx_data.data[i] = data_in[i];
  can_queue_enqueue(&can_rx_queue, &rx_data);
}

#ifdef USART

void forward_to_usart0() {
  usart0_tx_byte(can_rx_byte(1));
}

void process_can_queue() {
  while (!can_queue_is_empty(&can_rx_queue)) {
    can_queue_dequeue(&can_rx_queue, &current_rx_data);
    // Process all data in buffer
    unsigned char id_high = 0;
    while (in_counter < current_rx_data.count) {
      switch (current_rx_data.mob_no) {
        case 1:
          forward_to_usart0();
        break;
        case 2:
          if (in_counter == 0)
            id_high = can_rx_byte(2);
          can_rx_app_info_with_sender(id_high);
        break;
        case 3: // forward complete
          state = NORMAL;
          in_counter++;
        break;
      }
    }
    in_counter = 0; 
  }
  while (!can_queue_is_empty(&can_tx_queue)) {
    can_data_t pkg;
    int i;
    can_queue_dequeue(&can_tx_queue, &pkg);
    data_out_count = pkg.count;
    ident_out_high = pkg.id_high;
    ident_out_low = pkg.id_low;
    for (i=0; i < pkg.count; i++)
      data_out[i] = pkg.data[i];
    unsigned char old_int = __save_interrupt();
    __disable_interrupt();
    can_tx();
    __restore_interrupt(old_int);
  }
}

// Timeout interrupt, at least one of the MCUs expected to be connected
// hasn't answered within the time limit - skip it.
__interrupt void mcu_timeout(void) {
  TCCR1B = 0;
  no_of_mcus_found = app_no;
}

#elif defined CAN

void can_tx_forward_complete() {
  ident_out_high = MSG_MBOX_HIGH;
  ident_out_low = MSG_MBOX_LOW;
  can_tx_byte(FWD_COMPLETE);
}

void process_can_queue() {
  while (!can_queue_is_empty(&can_rx_queue)) {
    can_queue_dequeue(&can_rx_queue, &current_rx_data);
    // Process all data in buffer
    while (in_counter < current_rx_data.count) {
      switch (current_rx_data.mob_no) {
        case 1:
        case 2:
          handle_in_byte();
        break;
      }
    }
    in_counter = 0;
  }
  while (!can_queue_is_empty(&can_tx_queue)) {
    can_data_t pkg;
    int i;
    can_queue_dequeue(&can_tx_queue, &pkg);
    data_out_count = pkg.count;
    ident_out_high = pkg.id_high;
    ident_out_low = pkg.id_low;
    for (i=0; i < pkg.count; i++)
      data_out[i] = pkg.data[i];
    unsigned char old_int = __save_interrupt();
    __disable_interrupt();
    can_tx();
    __restore_interrupt(old_int);
  }

}

#endif
