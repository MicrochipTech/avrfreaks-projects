/*
xmodem.c

For specification see:
http://www.atmel.com/dyn/resources/prod_documents/doc1472.pdf
http://www.avrfreaks.net/index.php?module=Freaks%20Academy&func=viewItem&item_id=200&item_type=project


Change history:


2007-03-15
Adapted for ATMEGA88.
/Henrik Bjorkman

2007-06-27
Made a correction suggested by Jerry Mulchin at
JWM Engineering Group, Inc.
Shall be able to load larger files now.


2012-05-14
Some comments added.
Henrik Bjorkman

*/


#include <stdio.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/boot.h>
#include <setjmp.h>
#include "avr_cfg.h"
#include "avr_uart.h"


#define XMODEM_BUFFER_SIZE 128+SPM_PAGESIZE

#define XMODEM_SOH 0x01 // Start of header
#define XMODEM_EOT 0x04 // End of transmission
#define XMODEM_ACK 0x06 // Acknowledge
#define XMODEM_NACK 0x15 // Not acknowledge
#define XMODEM_RRC 'C' // Ready to receive with CRC


#ifdef USE_XMODEM_CRC

inline int xmodem_crc(unsigned char *ptr, int count)
{
  int crc=0;
  while (--count >= 0)
  {
    crc = crc ^ (int) *ptr++ << 8;
    char i = 8;
    do
    {
      if (crc & 0x8000)
      {
        crc = crc << 1 ^ 0x1021;
      }
      else
      {
        crc = crc << 1;
      }
    } while(--i);
  }
  return (crc);
}
#endif


int xmodem(void)
{
  int timercount=0;
  unsigned char packetNumber=1;
  unsigned int programMemoryPtr=0;
  unsigned char data[XMODEM_BUFFER_SIZE];
  unsigned int dataPtr=0;

  // Wait for start of header
  while(uart_getchar()!=XMODEM_SOH)
  {
    if (timercount==0)
    {
      uart_putchar(XMODEM_RRC);
    }
    --timercount;
  }


  do
  {
    if(packetNumber==(unsigned char)uart_waitchar())
    {
      if(packetNumber==(unsigned char)(~uart_waitchar()))
      {
        int i=0;
        while(i<128)
        {
          data[dataPtr]=(unsigned char)uart_waitchar();  
          ++dataPtr;
          ++i;  
        }
        unsigned int crc=(uart_waitchar()<<8);
        crc+=uart_waitchar();
#ifdef  USE_XMODEM_CRC
        if(xmodem_crc(&data[dataPtr-128],128)!=crc)
        {
          uart_putchar(XMODEM_NACK); // crc was not ok, send nack
        }
        else
#endif
        {
          while(dataPtr>=SPM_PAGESIZE) // loop until we have written all pages to flash.
          {
            int j=0;
          #if 1
            boot_page_erase(programMemoryPtr); // erase next page in flash memory
            while(boot_rww_busy())
            {
              boot_rww_enable();
            }
            // write next page to flash memory
            for(i=0;i<SPM_PAGESIZE;i+=2)
            {
              boot_page_fill(programMemoryPtr%SPM_PAGESIZE,data[i]+(data[i+1]<<8));
              programMemoryPtr+=2;
            }  
            boot_page_write(programMemoryPtr-1);
            while(boot_rww_busy())
            {
              boot_rww_enable();
            }
          #else
            // This was used while testing, does not write to memory.
            i=SPM_PAGESIZE;
          #endif
            // move memory in buffer to make room for next packet
            while(i<dataPtr)
            {
              data[j]=data[i];
              ++i,++j;
            }
            dataPtr=j;
          }  
          uart_putchar(XMODEM_ACK); // now ready for next packet, send ack.
          ++packetNumber;
        }
      }
    }
    else
    {
      uart_putchar(XMODEM_NACK);
    }
  }while(uart_waitchar()!=XMODEM_EOT);
  uart_putchar(XMODEM_ACK);
  
  return 0;
}



