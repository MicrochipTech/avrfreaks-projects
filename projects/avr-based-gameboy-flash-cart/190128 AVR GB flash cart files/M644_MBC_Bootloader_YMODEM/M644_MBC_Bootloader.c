
#define F_CPU         25804800
#define UART_BAUD     115200

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/crc16.h>
#include <avr/boot.h>
#include <util/delay.h>

// YMODEM bootloader. This one doesn't seem to work above 57600 :( bad because I need at least 115200
// I don't know why it suddenly becomes unreliable.

// YMODEM stuff
#define YMODEM_SOH_128      0x01 // 1st byte in header when 128B packets used
#define YMODEM_STX_1K       0x02 // 1st byte in header when 1kB packets used
#define END_OF_TRANSMISSION 0x04
#define YMODEM_CAN          0x18 // Cancel TRX value
#define ACKNOWLEDGE         0x06
#define NOT_ACKNOWLEDGE     0x15
#define ASCII_C             0x43
#define CTRLZ               0x1A // May need this...
#define EOT   END_OF_TRANSMISSION
#define ACK   ACKNOWLEDGE
#define NACK  NOT_ACKNOWLEDGE

#define BYTES_PER_PACKET    133 // SOH PACK# ~PACK# DATA[128] CRC_HI CRC_LO
#define BYTES_PER_PACKET_1K 1029// STX PACK# ~PACK# DATA[1024] CRC_HI CRC_LO
#define YMODEM_MAX_RETRIES  3

uint16_t  ymodemReceivedPackets;
uint8_t ymodemBuffer[BYTES_PER_PACKET_1K]; // Use the largest size, don't worry about RX/TX, it's half duplex
uint8_t wordsInBuff; // Because YMODEM has variable sized packets, we need to count how many bytes have already been put in the buffer.
uint16_t pageAddress;

uint8_t pageBuff[256];
uint8_t bytesInBuff;

uint8_t writeErrorFlag;
uint16_t writeErrorAddress;
uint16_t writeErrorAddressLast;

// Function prototypes
void init_uart();
void usart_transmit(unsigned char data);
char usart_receive(void);
void usart_transmit_str_P(const char* data);
void usart_transmit_int32(uint32_t num);
void wait_for_continue();
uint8_t ymodem_receive(void(*buffer_func)(unsigned char*, uint16_t), uint32_t *fileSize);
static uint16_t ymodem_calcrc(unsigned char *ptr, uint16_t count);
void buff_add(unsigned char* buffer, uint16_t byteCnt);
static void do_flash_write(uint16_t address);

int main() {

  // Start off like the MBC mode. Check for startMode. If not set, JMP to 0x0000!
  
  PORTA |= (1 << PA7); // Pull up INTRAMEN ASAP, may avoid RAM corruption

  //uint8_t startMode = PINA & (1 << PA6); // Mode detect!
  uint8_t startMode = PIND & (1 << PD0); // Mode detect!

  if(startMode) {  

    // Set up the UART - We're going minimalist for the bootloader, so code is copied from other files!
    init_uart();

    // Request input from the user. Space to go back to the app, '1' to go into program mode
    
    usart_transmit_str_P(PSTR("\x1b[2JAVR CART TERMINAL BOOTLOADER\r\nPress space for menu or '1' to flash\r\n"));

    char input; 
    do {
      input = usart_receive();
    } while((input != '1') && (input != ' '));

    if(input == ' ') {
      // Run the MBC application!
    asm("jmp 0000");
    }

    usart_transmit_str_P(PSTR("YMODEM protocol used. Select file within 10 secs else reset\r\n"));

    // Put a 10 second countdown on the screen so the user can select the file to send
    for(uint8_t i=0; i<10; i++) {
      usart_transmit(i+'0');
      _delay_ms(1000);
    }

    // Assume that if the user didn't press space, they pressed '1'!
    // Now flash the chip
    uint32_t fileSize = 0;
    pageAddress = 0;
    wordsInBuff = 0;
    writeErrorFlag = 0;
    writeErrorAddress = 0;
    ymodem_receive(buff_add, &fileSize);

    // Put final buffer write here
    if(bytesInBuff) {
      do_flash_write(pageAddress);
    }

    if(writeErrorFlag) {
      usart_transmit_str_P(PSTR("\r\nFlash Wr Errors:"));
      usart_transmit_int32(writeErrorFlag);
      usart_transmit_str_P(PSTR("\r\n"));
      usart_transmit_str_P(PSTR("First error in flash write @:"));
      usart_transmit_int32(writeErrorAddress);
      usart_transmit_str_P(PSTR("\r\n"));
      usart_transmit_str_P(PSTR("Last error in flash write @: "));
      usart_transmit_int32(writeErrorAddressLast);
      usart_transmit_str_P(PSTR("\r\n"));
    }

    // Put a finished message in
    usart_transmit_str_P(PSTR("\r\nFlash complete. Press spacebar.\r\n"));
    wait_for_continue();

  }

  // Run the MBC application!
  asm("jmp 0000");

  while(1) {
    // This is a catch all. Should never get here...
  }

  return 0;
}

void init_uart() {
  // Calculate the baud value
  uint16_t num = (uint32_t)F_CPU / (16 * (uint32_t)UART_BAUD) - 1;

	// Set baud rate 
	UBRR0H = (unsigned char)(num>>8);
	UBRR0L = (unsigned char)num;
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

void usart_transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)));
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

char usart_receive(void)
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );
	/* Get and return received data from buffer */
	return UDR0;
}

void usart_transmit_str_P(const char* data)
{
  while (pgm_read_byte(data) != 0x00) {
	  usart_transmit(pgm_read_byte(data++));
  }
}

void usart_transmit_int32(uint32_t num)
{
	char buffer[11];
  uint8_t i = 0;

  buffer[0] = '0';

  if(num == 0) {
    i = 1; // Need to push at least one value out
  }

  while(num > 0) {
    buffer[i] = num % 10 + '0';
    num /= 10;
    i++;
  }

  // Now we've converted it in reverse, pump it back out correctly
  for(; i > 0; i--) {
    usart_transmit(buffer[i - 1]);
  }
  usart_transmit('\0');
}

void wait_for_continue() {

  char input; 
  do {
    input = usart_receive();
  } while(input != ' ');
}

uint8_t ymodem_receive(void(*buffer_func)(unsigned char*, uint16_t), uint32_t *fileSize)
{
  ymodemReceivedPackets = 0;
  uint32_t recdBytes = 0;
  uint16_t crc;
  uint8_t canAttempts = 0;
  uint8_t resendPacket = 0;
  uint8_t lastPacketNum = 0; // =0 is a hack so that packet num 0 is not caught by accident - actually maybe not...
  
  // Send 'C' to initiate header transfer with CRC-16 
  usart_transmit('C');

  // Receive data till end of transmission is received
  do {

    uint8_t shortPacketMode = 0;

    for(uint16_t i=0; i<BYTES_PER_PACKET_1K;i++) {

      if(shortPacketMode && (i >= BYTES_PER_PACKET)) {
        // This is a short packet! Don't read over 133 bytes
        break;
      }
      
      uint8_t c = usart_receive();

      ymodemBuffer[i] = c;

      if(i==0) {
        if(ymodemBuffer[i] == YMODEM_SOH_128) {
          shortPacketMode = 1;
          canAttempts = 0;

        } else if (ymodemBuffer[i] == YMODEM_STX_1K) {       
          shortPacketMode = 0;
          canAttempts = 0;

        } else if(ymodemBuffer[i] == EOT) {
          // Download complete
          usart_transmit(ACK);

          // To gracefully finish a Ymodem program, should confirm null filename, but I'll blindly consider it done

          usart_transmit('C');
          _delay_ms(250); // is there a better way to delay here?
          usart_transmit(ACK);
      
          // Maybe should have a timeout here if the final ACK is lost and the sender fires EOT through again
          return 0;

        } else if(!((ymodemBuffer[i] == EOT) || (ymodemBuffer[i] == YMODEM_SOH_128) || (ymodemBuffer[i] == YMODEM_STX_1K))) {
          // Don't know what that was. Increment the cancellation counter and send a CAN
          usart_transmit(NACK);
          canAttempts++;

          if(canAttempts > YMODEM_MAX_RETRIES) { 
            // Something's not working. Cancel by returning.
            usart_transmit(YMODEM_CAN);
            usart_transmit(YMODEM_CAN);

            return 1;
          }
        }
      }
    }

    // Data is in. Check the packet number and CRC
    if (shortPacketMode) {
      crc = ymodem_calcrc((&ymodemBuffer[3]), 128);
    } else {
      crc = ymodem_calcrc((&ymodemBuffer[3]), 1024);
    }

    if (!shortPacketMode && ((ymodemBuffer[1027] != (unsigned char)(crc >> 8)) || (ymodemBuffer[1028] != (unsigned char)(crc)))) {
      // bad crc - must be the first cases to catch bad data errors for 128 & 1K Txs

      resendPacket = 1;
      canAttempts++;
      usart_transmit(NACK);
    
    } else if (shortPacketMode && ((ymodemBuffer[131] != (unsigned char)(crc >> 8)) || (ymodemBuffer[132] != (unsigned char)(crc)))) {
      // bad crc - must be the first cases to catch bad data errors for 128 & 1K Txs

      resendPacket = 1;
      canAttempts++;
      usart_transmit(NACK);
    
    } else if(ymodemBuffer[1] + ymodemBuffer[2] != 0xFF) { //(ymodemReceivedPackets + 1 != ymodemBuffer[1]) ||
      // Packet number not correct, could be a bad packet no. if it passed the above test

      resendPacket = 1;
      canAttempts++;
      usart_transmit(NACK);
    
    } else if (ymodemReceivedPackets == 0 && ymodemBuffer[1] == 0) {
      // It's a name and fileSize packet

      // PARSE THE FILESIZE (and name if you like)

              /* The spec suggests that the whole data section should
               * be zeroed, but I don't think all senders do this. If
               * we have a NULL filename and the first few digits of
               * the file length are zero, we'll call it empty.
               */
      // Find the filename length
      uint16_t fileDatPos = 0;
      for (fileDatPos = 3; fileDatPos < 1026; fileDatPos++) {
        if (ymodemBuffer[fileDatPos] == '\0') {
          break;
        }
      }

      if(fileDatPos < 7) {
        // Null filename?
        // May have to be careful here. Ymodem may use this to indicate no further files are coming. This may not be an error...
        usart_transmit(YMODEM_CAN);
        usart_transmit(YMODEM_CAN);
        return 3;
      }
      
      // To tell you the truth, I really don't care what the incoming filename is
      // No parsing of the filename done here.

      fileDatPos++; // Need one more increment to get away from the last NULL

      // strip leading spaces if any
    	while(ymodemBuffer[fileDatPos] == ' ' && fileDatPos < 1026) {
        fileDatPos++;
      }

      *fileSize = 0;
      while((ymodemBuffer[fileDatPos] != ' ' && ymodemBuffer[fileDatPos] != '\0') && fileDatPos < 1026) {
        // Find the end of the file size! It's in decimal format, so convert as we go

        *fileSize *= 10;
        *fileSize += (ymodemBuffer[fileDatPos] - '0');

        fileDatPos++;
      }

      if(*fileSize == 0) {
        // 0 byte file. Hmmm...
        usart_transmit(YMODEM_CAN);
        usart_transmit(YMODEM_CAN);
        return 4;
      }

      resendPacket = 0;
      canAttempts = 0;
      usart_transmit(ACK);
      usart_transmit('C');

    } else if (lastPacketNum == ymodemBuffer[1]) {
      // It's a repeated packet. We've already received it successfully though

      resendPacket = 0;
      canAttempts++;
      usart_transmit(ACK);
    
    } else {
      // That was a good packet

      // First, see how many bytes should be passed to the buff_func, then process them
      if(shortPacketMode) {
        if(recdBytes + 128 > *fileSize) {
          // Must be a half packet, do buffer_func accordingly
          uint8_t j = *fileSize - recdBytes;
          (buffer_func)(&ymodemBuffer[3], j);
          recdBytes += j;

        } else {
          // Do a 128 byte buffer func
          (buffer_func)(&ymodemBuffer[3], 128);
          recdBytes += 128;

        }
      } else {
        if(recdBytes + 1024 > *fileSize) {
          // Must be a half packet, do buffer_func accordingly
          uint16_t j = *fileSize - recdBytes;
          (buffer_func)(&ymodemBuffer[3], j);
          recdBytes += j;

        } else {
          // Do a 1k byte buffer func
          (buffer_func)(&ymodemBuffer[3], 1024);
          recdBytes += 1024;

        }
      }

      // Update some values so we are good for the next run
      lastPacketNum = ymodemBuffer[1];
      resendPacket = 0;
      ymodemReceivedPackets++;
      canAttempts = 0;
      usart_transmit(ACK);

    }

    if(canAttempts > YMODEM_MAX_RETRIES) {
      usart_transmit(YMODEM_CAN);
      usart_transmit(YMODEM_CAN);
      return 2;
    }

  } while(1);

  /* Should wait here to make sure no additional data is sent. If it is, then
  the ACK probably got garbled and the EOT has been sent again. */
  return 0;
}

static uint16_t ymodem_calcrc(unsigned char *ptr, uint16_t count)
{
   uint16_t crc = 0;

  while (count > 0) { 
    crc = _crc_xmodem_update(crc, *ptr++);
    count--;
  }

   return (crc);
}

void buff_add(unsigned char* buffer, uint16_t byteCnt)
{
  // Okay, here's the magic for this application. Need to erase, write and check pages.

  for(uint16_t i = 0; i<byteCnt; i++, bytesInBuff++) {

    pageBuff[bytesInBuff] = buffer[i];

    if(bytesInBuff == 255) {
      // Do a write
      do_flash_write(pageAddress);
    }
  }
}

static void do_flash_write(uint16_t address) {

  // Erase the temporary page buffer - shouldn't need this, but it's a test
  SPMCSR |= (1 << RWWSRE);

  for(uint16_t i = 0; i<= bytesInBuff; i++) {
    uint16_t w = pageBuff[i++];
    w += (pageBuff[i]) << 8;

    boot_page_fill (pageAddress + i, w);
  }

  eeprom_busy_wait();
  boot_page_erase(pageAddress);
  boot_spm_busy_wait();

  boot_page_write(pageAddress); // Store buffer in flash page.
  boot_spm_busy_wait();         // Wait until the memory is written.

  // Reenable RWW-section again. We need this if we want to jump back
  // to the application after bootloading.
  boot_rww_enable();

  // Read back the data and make sure it's good
  for(uint16_t i = 0; i<bytesInBuff; i++) {
    if((pageBuff[i] != pgm_read_byte_near(pageAddress + i))) {
      // Set an error flag and save the address
      writeErrorFlag++;
      writeErrorAddressLast = pageAddress;

      if(writeErrorFlag == 1) {
        writeErrorAddress = pageAddress + i;
      }
    }
  }

  pageAddress += 256;
}
