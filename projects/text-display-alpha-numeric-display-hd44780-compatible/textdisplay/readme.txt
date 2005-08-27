Readme file for textdisplay 1.0.1 avr 050914 by Erik Häggström (xpress@xpress.mine.nu).


* WHAT'S UP *
-------------

Removed all of the sbi/cbi macros
Moved the io-defines to textdisplay.conf.h
Wrote a small demo program

* DOWNLOAD *
------------

http://xpress.mine.nu/code/


* KNOWN BUGS *
--------------

none


* FILES *
---------

demo.c - demo program
textdisplay.conf.h - your configuration
textdisplay.c
textdisplay.h
textdisplay_extras.c
textdisplay_extras.h

* DEPENDABLES *
---------------

none


* INSTALL INFO *
----------------

You need to setup this module for your needs. This is done in textdisplay.conf.h

First you need to decide which bus width you are using.
Supported values by the HD-chip are 4 and 8.

#define TEXTDISPLAY_BUS_WIDTH 4
/* #define TEXTDISPLAY_BUS_WIDTH 8 */

There are two i/o modes io_port and io_pins. io_port is faster but io_pins lets you 
specify any pin (in any port) for each of the data signals.
If your are using io_port with 4 bit bus width the four high bits in your port are used.
This means that you can use the low bits in the same port for the control signals.

/* Set I/O mode for textdisplay bus */
/* #define TEXTDISPLAY_IO_PORT 1 */
#define TEXTDISPLAY_IO_PINS 1

Next you need to define which port/pins you want textdisplay to use.
This is done by setting three defines for each of the signals.

/* Select memory signal (RS) */
#define TEXTDISPLAY_RS_DDR			DDRC
#define TEXTDISPLAY_RS_PORT			PORTC
#define TEXTDISPLAY_RS_P				PC3

If you are using port mode you do not need to define each of the signals in the data bus.
Use the following defines to specify the port.

/* Set data port */ 
#define TEXTDISPLAY_DATA_DDR	DDRB
#define TEXTDISPLAY_DATA_PORT	PORTB
#define TEXTDISPLAY_DATA_PIN	PINB


* COMPILE INFO *
----------------

Compiles with GCC

* GENERAL INFO *
----------------

Functions in textdisplay.c

Be sure to call td_init() before using any of the other functions.

void     td_init(uint8_t width,uint8_t height);

void     td_clearDisplay(void)
void     td_clearRow(uint8_t row);
void     td_cursorHome(void)
void     td_entryModeSet(uint8_t mode)
void     td_displayControl(uint8_t ctrl)
void     td_cursorDisplayShift(uint8_t options)
void     td_functionSet(uint8_t options)
void     td_setCGaddress(uint8_t address)
void     td_setDDaddress(uint8_t address)
uint8_t  td_readBusyFlagAndAddress(void)
void     td_put(uint8_t data)
uint8_t  td_get(void)
uint8_t  td_readBusyFlag(void)
void     td_waitWhileBusy(void)
uint8_t  td_readAddress(void)
void     td_putN(uint8_t data, uint8_t count);
uint8_t  td_getRowAddr(uint8_t row);
void     td_putCh(uint8_t ch, uint8_t x, uint8_t y);
void     td_putStr(const uint8_t *str, uint8_t x, uint8_t y);
uint8_t  td_getCh(uint8_t x, uint8_t y);
uint8_t *td_getStr(uint8_t *buf, uint8_t size, uint8_t x, uint8_t y);
void     td_moveCursor(uint8_t x, uint8_t y);
uint8_t  td_getWidth(void);
uint8_t  td_getHeight(void);

Function in textdisplay_extras.c

void     td_putBar(uint8_t value, uint8_t size, uint8_t x, uint8_t y);
