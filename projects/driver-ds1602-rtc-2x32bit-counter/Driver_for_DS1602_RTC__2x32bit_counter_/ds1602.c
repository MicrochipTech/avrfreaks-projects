/********************************************************************************
 * program:     Driver for DS1602 real time clock (2x32 bits counter)           *
 * autor:       Martin Sotnik, soto@wanadoo.sk                                  *
 * compiler:    AVR GNU C, http://www.avrfreaks.net                             *
 * uProcesor:   ATMEL AVR, http://www.atmel.com                                 *
 * commands:    Read Continuous Counter         10*****1b (address 81h)         *
 *              Write Continuous Counter        10*****0b (address 80h)         *
 *              Clear Continuous Counter        00***1**b (command 04h)         *
 *              Read Vcc Active Counter         01*****1b (address 41h)         *
 *              Write Vcc Active Counter        01*****0b (address 40h)         *
 *              Clear Vcc Active Counter        00****1*b (command 02h)         *
 *              Set Oscilator Trim Bits         11XXX**0b XXX set value 0-7     *
 *                                                                              *
 * for more information about DS1602 see page at http://www.dalsemi.com         *
 ********************************************************************************/

#include <io.h>
#include <inttypes.h>
#include "ds1602.h"

/* send command - write 8 control bits */
void ds1602_send_command(uint8_t command)
{
 uint16_t i;
 outp(inp(DS1602_DDR)|DS1602_DATA_MASK|DS1602_CLOCK_MASK|DS1602_RST_MASK,DS1602_DDR); /* all pins as output */
 ds1602_rst_low(); /* initialization of RST pin */
 ds1602_clock_high(); /* initialization of CLOCK pin */
 ds1602_rst_high(); /* start communication */
 for (i=0;i<8;i++)
  {
   ds1602_clock_low();
   if ((command&0x01)==0x01) sbi(DS1602_PORT,DS1602_DATA); /* send logical 1 */
    else cbi(DS1602_PORT,DS1602_DATA); /* else logical 0 */
   ds1602_clock_high();
   command=command>>1;
  }
 if (command==0x02||command==0x04||command>0xBF) ds1602_rst_low(); /* end of communication */
}

/* read 32 data bits */
uint32_t ds1602_read_data(uint8_t address)
{
 uint32_t temp=0;
 uint8_t i;
 ds1602_send_command(address); /* initialize DS1602 to send data from specific register */
 outp(~DS1602_DATA_MASK,DS1602_PORT); /* release internal PULL-UP rezistor at DATA pin (?) */
 outp(inp(DS1602_DDR)&~DS1602_DATA_MASK,DS1602_DDR); /* set DATA pin as input pin */
 for (i=0;i<32;i++)
  {
   ds1602_clock_low(); /* CLOCK to logical 0 */
   if ((inp(DS1602_PIN)&DS1602_DATA_MASK)==DS1602_DATA_MASK) temp|=0x80000000; /* set MSB(31th bit) if logical 1 is read */
   ds1602_clock_high(); /* CLOCK to logical 1 */
   temp=temp>>1;
  }
 ds1602_rst_low(); /* end of communication */
 return (temp); /* return 32bit value */
}

/* write 32 data bits */
void ds1602_write_data(uint8_t address,uint32_t data)
{
 uint8_t i;
 ds1602_send_command(address); /* initialize DS1602 for writing to specific register */
 for (i=0;i<32;i++) /* write 32 data bits */
  {
   ds1602_clock_low(); /* CLOCK to logical 0 */
   if ((data&0x01)==0x01) sbi(DS1602_PORT,DS1602_DATA);
    else cbi(DS1602_PORT,DS1602_DATA);
   ds1602_clock_high(); /* CLOCK to logical 1 */
   data=data>>1;
  }
 ds1602_rst_low(); /* end of communication */
 }
 