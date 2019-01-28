#ifndef _YMODEM_FUNCS_
#define _YMODEM_FUNCS_

#include <avr/io.h>

#define _YMODEM_DEBUG_ 1 // 0 or 1 currently

#if _YMODEM_DEBUG_ == 1
extern uint16_t crcExternal;
extern uint8_t crcbytes[2];
#endif 

/*  
  Run this function to receive a file via the ymodem protocol
  
  The argument is a function that takes a char* array for the user to 
  manipulate each packet of data as it comes in and a buffer byte count. 
  If you don't do anything with the data in your own handler, then it will 
  be overwritten!
  Second variable is to pass the return file size of the file received

  return 0 is normal
  return 1 is CAN - error with header byte
  return 2 is CAN - error with valid packet Rx
  return 3 is CAN - null filename
  return 4 is CAN - 0B file received
*/
uint8_t ymodem_receive(void(*buffer_func)(unsigned char*, uint16_t),uint32_t *fileSize);

/*  
  Run this function to send a file via the ymodem protocol
  
  The argument is a function that takes a char* array . This function is
  called when the user requires 128 bytes for a packet.
  Second variable is the filename that will be received on the other end
  Third variable is the size of the file to be sent (don't screw it up!)
  
  A return value of 0 from this function dictates whether the buffer is
  empty, at which point end of transfer will be sent.
*/
uint8_t ymodem_send(uint8_t(*buffer_func)(unsigned char*, uint16_t), char * filename, uint32_t fileSize);

// Eh, I just made this available so a user can count ~size of a tx'd file
extern uint16_t ymodemReceivedPackets;
extern uint16_t ymodemSentPackets; // Conveniently, 2^16 * 128 is 8MB, max size for a GBROM!

/*
  TODO: 
  - Add a timeout for waiting for a file. Otherwise ymodem_receive blocks
  - Add an EOT double check, in case the last packet is resent (or clear
    the receive buffer after a small timeout delay - this means the TXer
    is still waiting on an ACK though...)
  -  

*/

#endif
