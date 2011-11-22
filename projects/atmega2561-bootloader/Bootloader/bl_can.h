#ifndef BL_CAN_H
#define BL_CAN_H

#include "bootloader.h"

// The server MCUs mbox idents
#define FWD_MBOX_HIGH 0x21
#define FWD_MBOX_LOW 0x00
#define APP_INFO_MBOX_HIGH 0x22
#define APP_INFO_MBOX_LOW 0x00
#define MSG_MBOX_HIGH 0x23
#define MSG_MBOX_LOW 0x00

// Mbox ident used to broadcast a message to all MCUs.
#define BROADCAST_HIGH 0x00
#define BROADCAST_LOW 0x01

// The total number of MCUs in the CAN network including the server MCU.
#define NO_OF_MCUS 4

// This mbox ident has to be set to the desired mbox ident
// of the MCU being programmed. The high byte of the mbox ident
// must be unique for this MCU for the bootloader to work.
#ifdef CAN
#define MBOX_IDENT 0x9180
#define MBOX_HIGH 0x91
#define MBOX_LOW 0x80
#endif

typedef struct can_data {
  unsigned char count;
  char mob_no;
  char id_high;
  char id_low;
  unsigned char data[8];
} can_data_t;

typedef struct remote_app_info {
  app_info_t app_info;
  unsigned char byte_no;
} remote_app_info_t;

extern remote_app_info_t remote_apps[NO_OF_MCUS];
extern volatile unsigned char app_no;
extern volatile unsigned char no_of_mcus_found;

extern unsigned char ident_in_high ;  /* CAN RX buffer */
extern unsigned char ident_in_low ;
extern unsigned char data_in_count;
extern unsigned char data_in0;
extern unsigned char data_in1;
extern unsigned char data_in2;
extern unsigned char data_in3;
extern unsigned char data_in4;
extern unsigned char data_in5;
extern unsigned char data_in6;
extern unsigned char data_in7;

extern unsigned char ident_out_high ;  /* CAN TX buffer */
extern unsigned char ident_out_low ;
extern unsigned char data_out_count;
extern unsigned char data_out0;
extern unsigned char data_out1;
extern unsigned char data_out2;
extern unsigned char data_out3;
extern unsigned char data_out4;
extern unsigned char data_out5;
extern unsigned char data_out6;
extern unsigned char data_out7;
extern unsigned char out_ready;

extern void can_get_app_info();
extern void str8cpy(char *dest, const char *from);
extern void init_can();
extern void set_rx_mbs();
extern void can_tx(void);
extern void can_rx_set(int);
extern int can_rx(int);
extern void can_tx_app_info(app_info_t *app_info);
extern void can_tx_app_info_with_sender(app_info_t *app_info, unsigned char sender);
extern void can_tx_byte(unsigned char byte);
extern void can_tx_int(unsigned int data);
extern void can_rx_app_info();
extern unsigned char can_rx_byte(int mbox);
extern void forward_to_usart0();
extern void can_tx_forward_complete();
extern void process_can_queue();
extern void can_forward_bytes(unsigned char bytes[], unsigned char len);
extern void can_pkg_add_byte(unsigned char byte);

#endif
