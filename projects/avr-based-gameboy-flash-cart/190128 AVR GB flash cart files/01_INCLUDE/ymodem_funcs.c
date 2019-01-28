#include "ymodem_funcs.h"
#include "../01_INCLUDE/usart_funcs.h"
#include <util/delay.h>
#include <util/crc16.h>

//
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
uint16_t  ymodemSentPackets;
uint8_t   ymodemBuffer[BYTES_PER_PACKET_1K]; // Use the largest size, don't worry about RX/TX, it's half duplex

#if _YMODEM_DEBUG_ == 1
uint16_t crcExternal;
uint8_t crcbytes[2];
#endif

uint8_t ymodem_receive(void(*buffer_func)(unsigned char*, uint16_t), uint32_t *fileSize);
static uint16_t ymodem_calcrc(unsigned char *ptr, uint16_t count);
uint8_t ymodem_send(uint8_t(*buffer_func)(unsigned char*, uint16_t), char * filename, uint32_t fileSize);

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
// ******************************************************************************************************
uint8_t ymodem_send(uint8_t(*buffer_func)(unsigned char*, uint16_t), char * filename, uint32_t fileSize) 
{
  uint8_t packetNum = 1;
  ymodemSentPackets = 0;
  uint16_t crc;
  uint8_t c = 0;
  uint16_t bufferPos = 3;
  uint32_t bytesToSend = fileSize;

  // Wait for the starting 'C' character
  while(c != 'C') {
    c = usart_receive();
  }

  // First packet is all about filename and filesize
  for(uint8_t i = 0; filename[i] != '\0'; i++, bufferPos++) {
    ymodemBuffer[bufferPos] = filename[i];
  }
  ymodemBuffer[bufferPos++] = '\0';
  
  // Need to add the file size in decimal format
  char fileSizeStr[11];
  uint16_t i = 0;
  fileSizeStr[0] = '0';
  if(bytesToSend == 0) {
    i = 1; // Need to push at least one value out
  }
  while(bytesToSend > 0) {
    fileSizeStr[i] = bytesToSend % 10 + '0';
    bytesToSend /= 10;
    i++;
  }
  bytesToSend = fileSize;
  // Now we've converted it in reverse, pump it back out correctly
  for(; i > 0; i--, bufferPos++) {
    ymodemBuffer[bufferPos] = fileSizeStr[i - 1];
  }
  ymodemBuffer[bufferPos++] = '\0';

  for(; bufferPos < BYTES_PER_PACKET; bufferPos++) {
    // Make the remaining characters nulls
    ymodemBuffer[bufferPos] = 0;
  }

  crc = ymodem_calcrc((&ymodemBuffer[3]), 128);
  ymodemBuffer[0] = YMODEM_SOH_128;
  ymodemBuffer[1] = 0;
  ymodemBuffer[2] = 0xFF;
  ymodemBuffer[131] = (uint8_t)(crc >> 8);
  ymodemBuffer[132] = (uint8_t) crc;
  
  // Send the header packet out until we get an ACK
  do {
    for(i = 0; i<BYTES_PER_PACKET; i++) {
      usart_transmit(ymodemBuffer[i]);
    }

    c = usart_receive();

  } while(c != ACK);

  // Wait for the starting 'C' character
  while(c != 'C') {
    c = usart_receive();
  }

  // Okay, now onto the data 
  while(bytesToSend > 0) {
    
    if(bytesToSend > 1024) {

      (buffer_func)(&ymodemBuffer[3], 1024);
      crc = ymodem_calcrc(&ymodemBuffer[3], 1024);
      ymodemBuffer[1027] = (uint8_t) (crc >> 8);
      ymodemBuffer[1028] = (uint8_t) crc;
      bytesToSend -= 1024;
      ymodemBuffer[0] = YMODEM_STX_1K;
      ymodemBuffer[1] = packetNum;
      ymodemBuffer[2] = ~packetNum;

      do {
        for(i = 0; i<BYTES_PER_PACKET_1K; i++) {
          usart_transmit(ymodemBuffer[i]);
        }

        c = usart_receive();

      } while(c != ACK);

    } else {
    
      if (bytesToSend > 128) {
        (buffer_func)(&ymodemBuffer[3], 128);
        
        bytesToSend -= 128;

  
      } else {
        // Leftovers (<128)
        (buffer_func)(&ymodemBuffer[3], bytesToSend);
        for(i = bytesToSend; i < BYTES_PER_PACKET;i++) {
          ymodemBuffer[3+i] = CTRLZ;
        }
        
        bytesToSend = 0;

      } 
      
      ymodemBuffer[0] = YMODEM_SOH_128;
      ymodemBuffer[1] = packetNum;
      ymodemBuffer[2] = ~packetNum;
      crc = ymodem_calcrc((&ymodemBuffer[3]), 128);
      ymodemBuffer[131] = (uint8_t) (crc >> 8);
      ymodemBuffer[132] = (uint8_t) crc;

      do {
        for(i = 0; i<BYTES_PER_PACKET; i++) {
          usart_transmit(ymodemBuffer[i]);

        }

        c = usart_receive();

      } while(c != ACK);
    }
    
    ymodemSentPackets++;
    packetNum++;
  }

  do {
    // Send an EOT
    usart_transmit(EOT);
    
    // Wait for ACK
    c = usart_receive();
  } while (c != ACK);

  // Wait for a 'C' character
  do {
    c = usart_receive();
  } while (c != 'C');

  // Send a null filename file
  for(i = 0; i < BYTES_PER_PACKET; i++) {
    ymodemBuffer[i] = 0;
  }
  crc = ymodem_calcrc((&ymodemBuffer[3]), 128);
  
  ymodemBuffer[0] = YMODEM_SOH_128;
  ymodemBuffer[1] = 0;
  ymodemBuffer[2] = 0xFF;
  ymodemBuffer[131] = (uint8_t) (crc >> 8);
  ymodemBuffer[132] = (uint8_t) crc;

  do {
    for(i = 0; i<BYTES_PER_PACKET; i++) {
      usart_transmit(ymodemBuffer[i]);
    }

    c = usart_receive();
  } while(c != ACK);

  return 0;

}
