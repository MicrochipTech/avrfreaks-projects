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

#define XMODEM_TIMERCOUNT_RESTART_VALUE 3000L*(AVR_FOSC/1000000L)



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



int xmodem(void)
{
  int timercount=0;
  unsigned char packetNumber=1;
  unsigned int programMemoryPtr=0;
  unsigned char data[XMODEM_BUFFER_SIZE];
  unsigned int dataPtr=0;

  while(uart_getchar()!=XMODEM_SOH)
  {
    if (timercount==0)
    {
      uart_putchar(XMODEM_RRC);
      timercount=XMODEM_TIMERCOUNT_RESTART_VALUE;
    }
    else
    {
      timercount--;
    }
  }


  do
  {
    if(packetNumber==(unsigned char)uart_waitchar())
    {
      if(packetNumber==(unsigned char)(~uart_waitchar()))
      {
        unsigned int crc;
        int i;
        int j;
        for(i=0;i<128;i++)
        {
          data[dataPtr]=(unsigned char)uart_waitchar();  
          dataPtr++;  
        }
        crc=(uart_waitchar()<<8);
        crc+=uart_waitchar();
        if(xmodem_crc(&data[dataPtr-128],128)==crc)
        {
          while(dataPtr>=SPM_PAGESIZE)
          {
          #if 1
            boot_page_erase(programMemoryPtr);
            while(boot_rww_busy())
            {
              boot_rww_enable();
            }
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
            i=SPM_PAGESIZE;
          #endif
            for(j=0;i<dataPtr;i++,j++)
            {
              data[j]=data[i];
            }
            dataPtr=j;
          }  
          uart_putchar(XMODEM_ACK);
          packetNumber++;
        }
        else
        {
          uart_putchar(XMODEM_NACK);
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



