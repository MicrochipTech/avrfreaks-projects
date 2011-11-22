#include "protocol.h"
#include "bootloader.h"
#include "usart.h"
#include "bl_can.h"
#include "intel_hex.h"
#include "leds.h"
#include "common.h"
#include "queue.h"

volatile unsigned long incoming_tmp;
app_info_t incoming_app_info;

/*
  Define wrapper functions
*/
#ifdef USART

static inline void tx_byte(unsigned char byte) {
  usart0_tx_byte(byte);
}

static inline void tx_int(unsigned int data) {
  usart0_tx_int(data);
}

static inline unsigned char rx_byte() {
  //return usart0_rx_byte();
  unsigned char rx_data;
  byte_queue_dequeue(&usart_rx_queue, &rx_data);
  return rx_data;
}

void broadcast_jump_bootloader() {
  ident_out_high = BROADCAST_HIGH;
  ident_out_low = BROADCAST_LOW;
  can_tx_byte(JUMP_BOOTLOADER);
}

#elif defined CAN

static inline void tx_byte(unsigned char byte) {
  can_tx_byte(byte);
}

static inline void tx_int(unsigned int data) {
  can_tx_int(data);
}

static inline unsigned char rx_byte() {
  return can_rx_byte(1);
}

#endif

void handle_in_byte() {
  switch (state) {
    case NORMAL:
    {
      cl_cmds_t in_byte = (cl_cmds_t)rx_byte();
      switch (in_byte) {
        case GET_BL_VER:
          tx_byte(BL_VER_LOW);
          tx_byte(BL_VER_HIGH);
          tx_byte(ACK);
        break;
        case GET_APP_VER:
        {
          app_info_t app_info;
          read_app_info(&app_info);
          tx_int(app_info.version);
        }
        break;
        case GET_CARD_INFO:
#ifdef USART
          state = WAIT_APP_INFO;
          can_get_app_info();
#elif defined CAN
          {
            app_info_t app_info;
            read_app_info(&app_info);
            ident_out_high = APP_INFO_MBOX_HIGH;
            ident_out_low = APP_INFO_MBOX_LOW;
            can_tx_app_info_with_sender(&app_info, MBOX_HIGH);
          }
#endif
        break;
        
        case TRANSFER_EEPROM:
#ifdef CAN
          ident_out_high = FWD_MBOX_HIGH;
          ident_out_low = FWD_MBOX_LOW;
#endif
          transfer_init = true;
          mem_write_ptr = &write_eeprom_page;
          enqueue_page = &eeprom_enqueue_page;
          data_record = &eeprom_data_record;
          state = TRANSFER_EEPROM_HEADER;
        break;
        
        case TRANSFER_CODE:
#ifdef CAN
          ident_out_high = FWD_MBOX_HIGH;
          ident_out_low = FWD_MBOX_LOW;
#endif
          transfer_init = true;
          mem_write_ptr = &write_flash_page;
          enqueue_page = &flash_enqueue_page;
          data_record = &flash_data_record;
          state = TRANSFER_CODE_HEADER;
        break;
        
        case JUMP_BOOTLOADER:
#ifdef USART
          broadcast_jump_bootloader();
          tx_byte(ACK);
#endif
        break;
      }
    }
    break;
    
    case TRANSFER_EEPROM_HEADER:
    {
      unsigned char in_byte = rx_byte();
      rx_counter++;
      parse_header(in_byte, TRANSFER_EEPROM_STATE);
    }
    break;
    
    case TRANSFER_CODE_STATE:    
    case TRANSFER_EEPROM_STATE:
    {
      unsigned char in_byte = rx_byte();
      //add_to_crc16(in_byte);
      parse_ihex(in_byte);
      rx_counter++;
    }
    break;

    case TRANSFER_CODE_HEADER:
    {
      unsigned char in_byte = rx_byte();
      rx_counter++;
      parse_header(in_byte, TRANSFER_CODE_STATE);
    }
    break;
    
    case FORWARD:
    {
#ifdef USART
      can_pkg_add_byte(rx_byte());
#endif
    }
    break;
    
    case RUN_APP: 
    {
      unsigned char in_byte = rx_byte();
      //add_to_crc16(in_byte);
    }
    break;
  }
}

// Parses the incoming byte as a part of an incoming header
void parse_header(unsigned char in_byte, bl_state_t next_state) {
  if (rx_counter == 1) // Receive CAN destination (2 bytes)
    incoming_tmp = (unsigned long)in_byte;
  else if (rx_counter == 2) {
    incoming_tmp |= (((unsigned long)in_byte)<<8);
    incoming_app_info.address = (unsigned int)incoming_tmp;
  }
  else if (rx_counter == 3) // Receive app version (2 bytes)
    incoming_tmp = (unsigned long)in_byte;
  else if (rx_counter == 4) {
    incoming_tmp |= (((unsigned long)in_byte)<<8);
    incoming_app_info.version = (unsigned int)incoming_tmp;
  }
  else if (rx_counter >= 5 && rx_counter <= 12) { // Receive name (8 bytes)
    incoming_app_info.name[rx_counter-5] = in_byte;
  }
  else if (rx_counter == 13) { // Receive date (4 bytes)
    incoming_tmp = (unsigned long)in_byte;
  }
  else if (rx_counter == 14) {
    incoming_tmp |= (((unsigned long)in_byte)<<8);
  }
  else if (rx_counter == 15) {
    incoming_tmp |= (((unsigned long)in_byte)<<16);
  }
  else if (rx_counter == 16) {
    incoming_tmp |= (((unsigned long)in_byte)<<24);
    incoming_app_info.date = incoming_tmp;
    rx_counter = 0;
#ifdef CAN
    tx_byte(ACK);
    if (incoming_app_info.address == MBOX_IDENT)
      state = next_state;
#elif defined USART
    if (incoming_app_info.address == 0) {
      state = next_state;
      tx_byte(ACK);
    }
    else {
      ident_out_high = (unsigned char)(incoming_app_info.address>>8);
      ident_out_low = (unsigned char)(incoming_app_info.address);
      can_tx_byte(next_state);
      can_tx_app_info(&incoming_app_info);
      state = FORWARD;
    }
#endif
  } 
}

