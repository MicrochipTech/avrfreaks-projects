/**********************************************************************************
 * program:     Driver for DS1602 real time clock (2x32 bits counter) - header    *
 * autor:       Martin Sotnik, soto@wanadoo.sk                                    *
 * compiler:    AVR GNU C, http://www.avrfreaks.net                               *
 * uProcesor:   ATMEL AVR, http://www.atmel.com                                   *
 * commands:    Read Continuous Counter         10*****1b (address 81h)           *
 *              Write Continuous Counter        10*****0b (address 80h)           *
 *              Clear Continuous Counter        00***1**b (command 04h)           *
 *              Read Vcc Active Counter         01*****1b (address 41h)           *
 *              Write Vcc Active Counter        01*****0b (address 40h)           *
 *              Clear Vcc Active Counter        00****1*b (command 02h)           *
 *              Set Oscilator Trim Bits         11XXX**0b XXX set value 0-7       *
 *                                                                                *
 * for more information about DS1602 see page at http://www.dalsemi.com           *
 **********************************************************************************/

#include <io.h>
#include <stdarg.h>

#define DS1602_PORT                PORTD  /* port where is DS1602 connected */
#define DS1602_DDR                 DDRD   /* DDR register */
#define DS1602_PIN                 PIND   /* PIN register */

#define DS1602_CLOCK               6      /* CLOCK pin */
#define DS1602_CLOCK_MASK          0x40   /* mask of CLOCK pin */
#define DS1602_DATA                5      /* DATA pin */
#define DS1602_DATA_MASK           0x20   /* mask of DATA pin */
#define DS1602_RST                 4      /* RST pin */
#define DS1602_RST_MASK            0x10   /* mask of RST pin */

/* definicia funkcii a makier */
#define ds1602_rst_high()           sbi(DS1602_PORT,DS1602_RST)
#define ds1602_rst_low()            cbi(DS1602_PORT,DS1602_RST)
#define ds1602_clock_high()         sbi(DS1602_PORT,DS1602_CLOCK)
#define ds1602_clock_low()          cbi(DS1602_PORT,DS1602_CLOCK)

extern void ds1602_send_command(uint8_t command);
extern uint32_t ds1602_read_data(uint8_t address);
extern void ds1602_write_data(uint8_t address, uint32_t data);
