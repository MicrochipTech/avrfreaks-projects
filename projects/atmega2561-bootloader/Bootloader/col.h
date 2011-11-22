#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "bootloader.h"

// Errors which an occur during transfer.
typedef enum error_e { NONE, BUF_FULL, CHECKSUM } error_t;

// Commands sent by the PC client. The bootloader must implement these.
typedef enum client_cmds { GET_BL_VER, GET_APP_VER, GET_CARD_INFO, TRANSFER_CODE, JUMP_BOOTLOADER, TRANSFER_EEPROM } cl_cmds_t;

typedef enum bootloader_cmds { FWD_COMPLETE=0x01, ERROR=0xFE, ACK=0xFF } bl_cmds_t;

typedef enum mem_type { CODE, EEPROM } mem_type_t;

/*
SHORT PROTOCOL DESCRIPTION

GET_BL_VER:   Answer with the bootloader version number
GET_APP_VER:  This command is followed by a 2 bytes long CAN address of a MCU, 
              LSB first.
              Answer with that CPU's application ID and version.
GET_CARD_INFO:First answer with one byte describing the number of MCUs connected
              Answer with 16 bytes per MCU. First 2 bytes represents the CAN
              address of the MCU. The second 2 bytes represents the version of 
              the application running on the MCU. The next 8 bytes represents the
              name of the application running on the MCU.
              The last 4 bytes represents the date the application was uploaded to
              the MCU.
TRANSFER_CODE:This command is followed by a 2 bytes long CAN address of a MCU,
              LSB first. After the address 4 bytes with the program size is
              sent, LSB first. Following the address comes the application to 
              store in that MCU's flash memory.
              Answer with ACK.
ACK:          Acknowledge, tells the client that the transfer was OK.
ERROR:        Tells the client that an error occurred. Followed by an error_t
              describing the error.
*/

extern volatile unsigned long incoming_tmp;
extern app_info_t incoming_app_info;

void handle_in_byte();
void parse_header(unsigned char in_byte, bl_state_t next_state);
void broadcast_jump_bootloader();

#endif
