#include <ioavr.h>
#include <stdbool.h>
#include <stdlib.h>
#include <intrinsics.h>
#include "common.h"
#include "queue.h"
#include "mem_settings.h"
#include "bootloader.h"
#include "usart.h"
#include "leds.h"
#include "bl_can.h"
#include "move_iv.h"
#include "intel_hex.h"
#include "protocol.h"

/**
 * TODO:
 * Stöd för programmering av EEPROM
   * Vid ny rad ska en incoming köas och en ny page påbörjas
   * En ACK saknas när initCodeTransfer körs
 * Dokumentera/kommentera
 * Optimera intel_hex-parsningen för att kunna öka baud rate
 * Optimera köerna så de använder pekare istället
   * ex. tail_ptr == &(buf[SIZE-1])
 * Funktionspekare för att minska koden, eeprom_func och code_func...
 */

#ifdef DEBUG_FLASH_WRITE
unsigned int debug_page[SPM_PAGESIZE];
static inline void debug_flash_write_page(unsigned int data, unsigned int page_addr) {
  debug_page[page_addr%SPM_PAGESIZE] = data;
}
#endif

volatile unsigned long app_page_addr;
volatile bl_state_t state;
volatile unsigned long rx_counter;
bool transfer_init = true;
//volatile unsigned int crc16_sum;
//void (*application_pointer)(void) = 0x0000;
bool (*mem_write_ptr)(void);

void reset() {
  WDTCR |= 1<<WDE;
  while(1);
}

/*
 * Clears all registers which might have interrupts enabled.
 * This prevents interrupts set by the previous app to interfere with the
 * bootloader.
 */
void clear() {
  EIMSK = 0;
  TIMSK0 = 0;
  TIMSK1 = 0;
  TIMSK2 = 0;
  TCCR0A = 0;
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1C = 0;
  TCCR2A = 0;
  TCCR3A = 0;
  TCCR3B = 0;
  TCCR3C = 0;
  SPCR = 0;
  UCSR0B = 0;
  UCSR1B = 0;
  TWCR = 0;
  ACSR = 0;
  ADCSRA = 0;
}

// Erases the application part of the flash memory
static inline void erase_app_flash() {
#if APP_END > 0
  unsigned long nb_page;

  nb_page=0;
  do {
    _SPM_24_ERASE(nb_page);
    nb_page += SPM_PAGESIZE;
  } while (nb_page<APP_END);
  _ENABLE_RWW_SECTION();
#endif
}

// Writes a page of data to the specified address of the flash memory
static inline void write_page_app_flash(page_data_t *page, unsigned int page_addr) {
  unsigned int data = 0;
  for (page->page_ptr=0; page->page_ptr < SPM_PAGESIZE; page->page_ptr+=2) {
    data = page->data[page->page_ptr];
    data |= page->data[page->page_ptr+1]<<8;
    _SPM_24_FILLTEMP(page_addr + page->page_ptr, data);
#ifdef DEBUG_FLASH_WRITE
    debug_flash_write_page(data, page_addr+page->page_ptr);
#endif
  }
  _SPM_24_PAGEWRITE(page_addr);
  _WAIT_FOR_SPM();
  _ENABLE_RWW_SECTION();
}

// Adds a byte to the crc16 sum
/*void add_to_crc16(unsigned char byte) {
  int i;
  for (i = 0; i < 8; ++i) {
      unsigned long osum = crc16_sum;
      crc16_sum <<= 1;
      if (byte & 0x80)
        crc16_sum |= 1 ;
      if (osum & 0x8000)
        crc16_sum ^= 0x1021;
      byte <<= 1;
  }
}*/

// Initializes the leds
static inline void leds_init() {
  bit_set(LED_DDR1, LED1);
  bit_set(LED_DDR2, LED2);
  bit_set(LED_DDR3, LED3);
  bit_set(LED_PORT1, LED1);
  bit_set(LED_PORT2, LED2);
  bit_set(LED_PORT3, LED3);
}

// Writes one page from the page buffer to the flash memory
bool write_flash_page() {
  if (flash_page_written)
    return false;
#ifdef ENABLE_FLASH_WRITE
  write_page_app_flash(&flash_complete_page, app_page_addr);
#endif
  flash_page_written = true;
  app_page_addr += SPM_PAGESIZE;
  return true;
}

bool write_eeprom_page() {
  page_eeprom_t page;
  unsigned int i;
  if (eeprom_queue_is_empty(&eeprom_page_buf))
    return false;
  eeprom_queue_dequeue(&eeprom_page_buf, &page);
#ifdef ENABLE_EEPROM_WRITE
  for (i=0; i < page.page_ptr; i++) {
    __EEPUT(page.address + i, page.data[i]);
  }
#endif
  return true;
}

// Main function
int main() {
  app_page_addr = 0;
  state = NORMAL;
  rx_counter = 0;
  incoming_tmp = 0;
//  crc16_sum = 0;
  app_no = 1;
  unsigned int flip_counter = 0;
  clear();
  init_ihex();
#ifdef USART
  //usart0_init(103); //9600 bps
  usart0_init(51); // 19200 bps
  //usart0_init(8); // 115.2 kbps
#endif
  init_can();
  set_rx_mbs();
  leds_init();
//  static_queue_init(&page_data_buf);
  eeprom_queue_init(&eeprom_page_buf);
  can_queue_init(&can_rx_queue);
  can_queue_init(&can_tx_queue);
#ifdef USART
  __delay_cycles(8000000); // delay 0.5 s to give the other MCUs a chance to setup CAN
  broadcast_jump_bootloader();
  byte_queue_init(&usart_rx_queue);
#endif
  // Move interrupt vectors to boot section
  move_iv_to_bls();
  __enable_interrupt();
  while (true) {
    if (flip_counter++ == 0xffff)
      bit_flip(LED_PORT1, LED1);
    process_can_queue();
#ifdef USART
    process_usart_queue();
#endif
    switch (state) {
#ifdef USART
    case WAIT_APP_INFO:
      if (app_no == no_of_mcus_found) {
        __disable_interrupt();
        usart0_tx_app_info();
        __enable_interrupt();
        app_no = 1;
        state = NORMAL;
      }
      break;
#endif
      
    case TRANSFER_CODE_STATE:
    case TRANSFER_EEPROM_STATE:
      if (transfer_init) {
#ifdef ENABLE_FLASH_ERASE
        if (state == TRANSFER_CODE_STATE)
          erase_app_flash();
#endif
#ifdef USART
        usart0_tx_byte(ACK);
#elif defined CAN
        ident_out_high = FWD_MBOX_HIGH;
        ident_out_low = FWD_MBOX_LOW;
        can_tx_byte(ACK);
#endif
        transfer_init = false;
      }
      //if (!static_queue_is_empty(&page_data_buf)) // process page and write to app flash
      mem_write_ptr();
      break;

    case RUN_APP:
//      if (!static_queue_is_empty(&page_data_buf))
      if (!mem_write_ptr()) {
        store_app_info(&incoming_app_info);
        //usart0_tx_int(crc16_sum);
        // Move interrupt vectors to application section
        move_iv_to_app();
        // Transfer complete and application loaded, run application
        //application_pointer();
        reset();
        while(1);
      }
    }
  }
}


// Stores an app_info_t in eeprom at address APP_INFO_ADDR
void store_app_info(app_info_t *app_info) {
  unsigned char i;
  unsigned long date;
  __EEPUT(APP_INFO_ADDR, (unsigned char)app_info->address);
  __EEPUT(APP_INFO_ADDR+1, (unsigned char)(app_info->address>>8));
  __EEPUT(APP_INFO_ADDR+2, (unsigned char)app_info->version);
  __EEPUT(APP_INFO_ADDR+3, (unsigned char)(app_info->version>>8));
  for (i=0; i < 8; i++) {
    __EEPUT(APP_INFO_ADDR+4+i, (unsigned char)app_info->name[i]);
  }
  date = app_info->date;
  for (i=0; i < 4; i++) {
    __EEPUT(APP_INFO_ADDR+12+i, (unsigned char)date);
    date >>= 8;
  }
}

// Reads an app_info_t from address APP_INFO_ADDR in eeprom
void read_app_info(app_info_t *app_info) {
  unsigned char temp, i;
  __EEGET(temp, APP_INFO_ADDR);
  app_info->address = temp;
  __EEGET(temp, APP_INFO_ADDR+1);
  app_info->address |= (((unsigned int)temp)<<8);
  
  __EEGET(temp, APP_INFO_ADDR+2);
  app_info->version = temp;
  __EEGET(temp, APP_INFO_ADDR+3);
  app_info->version |= (((unsigned int)temp)<<8);
  
  for (i=0; i < 8; i++) {
    __EEGET(temp, APP_INFO_ADDR+4+i);
    app_info->name[i] = temp;
  }
  app_info->date = 0;
  for (i=0; i < 4; i++) {
    __EEGET(temp, APP_INFO_ADDR+12+i);
    app_info->date |= (((unsigned long)temp)<<(i*8));
  }
}
