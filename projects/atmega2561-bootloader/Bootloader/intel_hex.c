#include "intel_hex.h"
#include "bootloader.h"
#include "usart.h"
#include "queue.h"
#include "protocol.h"
#include "bl_can.h"

unsigned char hex_index;
unsigned char hex_data_size;
int hex_checksum;
unsigned char hex_in_byte;
unsigned char hex_odd;
unsigned char hex_type;
unsigned int hex_address_ext;
unsigned int hex_address;

void (*enqueue_page)(void);
void (*data_record)(void);

page_data_t incoming_page;
page_data_t flash_complete_page;
bool flash_page_written=true;
page_eeprom_t incoming_eeprom_page;

#ifdef USART

static inline void reply_byte(unsigned char byte) {
  usart0_tx_byte(byte);
}

#elif defined CAN

static inline void reply_byte(unsigned char byte) {
  ident_out_high = FWD_MBOX_HIGH;
  ident_out_low = FWD_MBOX_LOW;
  can_tx_byte(byte);
}

static inline void reply_bytes(unsigned char bytes[], unsigned char len) {
  ident_out_high = FWD_MBOX_HIGH;
  ident_out_low = FWD_MBOX_LOW;
  can_forward_bytes(bytes, len);
}

#endif

void flash_enqueue_page() {
  /*if (!static_queue_enqueue(&page_data_buf, &incoming_page)) { // Queue full
    reply_byte(ERROR);
    reply_byte(BUF_FULL);
  }*/
  unsigned int i;
  if (!flash_page_written) {
    reply_byte(ERROR);
    reply_byte(BUF_FULL);
  }
  flash_complete_page.page_ptr = incoming_page.page_ptr;
  for (i=0; i < SPM_PAGESIZE; i++)
    flash_complete_page.data[i] = incoming_page.data[i];
  flash_page_written = false;
}

void flash_fill_end() {
  for (;incoming_page.page_ptr < SPM_PAGESIZE; incoming_page.page_ptr++) // if last page, fill the rest with 0xFF
    incoming_page.data[incoming_page.page_ptr] = 0xFF; 
}

void flash_data_record() {
  incoming_page.data[incoming_page.page_ptr++] = hex_in_byte;
  if (incoming_page.page_ptr == SPM_PAGESIZE || hex_type == EOF_RECORD) { // page done
    if (hex_type == EOF_RECORD) {
      flash_fill_end();
    }
    enqueue_page();
    incoming_page.page_ptr = 0;
  }
}

void eeprom_enqueue_page() {
  incoming_eeprom_page.address = hex_address;
  if (!eeprom_queue_enqueue(&eeprom_page_buf, &incoming_eeprom_page)) { // Queue full
    reply_byte(ERROR);
    reply_byte(BUF_FULL);
  }
}

void eeprom_data_record() {
  incoming_eeprom_page.data[incoming_eeprom_page.page_ptr++] = hex_in_byte;
  if (incoming_eeprom_page.page_ptr == EEPROM_PAGE_SIZE || hex_type == EOF_RECORD) {
    enqueue_page();
    incoming_eeprom_page.page_ptr = 0;
  }
}

// Initializes the globals used for parsing the intel-hex stream.
void init_ihex() {
  hex_index = 0;
  hex_data_size = 0;
  hex_checksum = 0;
  hex_in_byte = 0;
  hex_odd = 0;
  hex_type = 0;
}

// Parses an incoming byte as a part of an intel-hex stream.
void parse_ihex(unsigned char in_byte) {
  static bool transfer_complete = false;
  if (in_byte == ':') {
    hex_index = 0;
    hex_data_size = 0;
    hex_checksum = 0;
    hex_in_byte = 0;
    hex_odd = 0;
    hex_type = 0;
    hex_address = 0;
    hex_address_ext = 0;
    return;
  }
  else if (in_byte == '\0') {
    reply_byte(ACK);
    if (transfer_complete) {
      if (state == TRANSFER_CODE_STATE)
        state = RUN_APP;
      else
        state = NORMAL;
#ifdef CAN
      can_tx_forward_complete();
#endif
      transfer_complete = false;
    }
    return;
  }
  in_byte = ascii_to_bin(in_byte);
  if (hex_odd == 0) {
    hex_in_byte = in_byte<<4;
    hex_odd++;
    return;
  }
  else {
    hex_in_byte |= in_byte;
    hex_odd = 0;
  }
  
  switch (hex_index) {
    case IHEX_DATA_SIZE_INDEX: // data size
      hex_data_size = hex_in_byte;
      break;
      
    case IHEX_DATA_ADDR_INDEX:
      hex_address = ((unsigned int)hex_in_byte)<<8;
      break;
      
    case IHEX_DATA_ADDR_INDEX+1:
      hex_address |= hex_in_byte;
      break;
      
    case IHEX_DATA_TYPE_INDEX: // data type
      hex_type = hex_in_byte;
      break;
      
    default:
      if (hex_index == (hex_data_size+4)) { // Checksum
        if (((char)(hex_checksum + hex_in_byte)) != 0) { // Checksum error
          if (hex_type == EOF_RECORD) { // Not a checksum error, just EOF
            reply_byte(ACK);
            return;
          }
#ifdef CAN
          unsigned char reply[4] = {ERROR, CHECKSUM, hex_checksum, hex_in_byte};
          reply_bytes(reply, 4);
#elif defined USART
          reply_byte(ERROR);
          reply_byte(CHECKSUM);
          reply_byte(hex_checksum);
          reply_byte(hex_in_byte);
#endif
        }
        else { // Checksum correct
          if (hex_type == EOF_RECORD) {
            if (state == TRANSFER_CODE_STATE) // EEPROM page doesn't need to be full
              flash_fill_end();
            enqueue_page();
            transfer_complete = true;  // EOF found, now the complete app has been downloaded
          }
          hex_checksum=0;
          reply_byte(ACK);
        }
        return;
      }
      else if (hex_index >= IHEX_DATA_START_INDEX) { // Data
        switch (hex_type) {
          case DATA_RECORD:
          case EOF_RECORD:
            data_record();
          break;
          case EXT_SEG_ADDR_REC:
            {
              unsigned char data_index = hex_index - IHEX_DATA_START_INDEX;
              hex_address_ext |= (((unsigned int)hex_in_byte)<<((2-data_index)*8));
            }
          break;
          case START_SEG_ADDR_REC:
          break;
        }
      }
  }
  hex_index++;
  hex_checksum += (char)hex_in_byte;
}

// Converts ascii data to binary data
unsigned char ascii_to_bin(unsigned char data) {
    if( data < 'A' ) {
	return( data - '0' );
    }
    else {
	return( data - 55 );
    }
}
