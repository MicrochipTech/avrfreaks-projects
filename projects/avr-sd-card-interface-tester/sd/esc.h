/**
 * \mainpage

This is the AVR SD Interface tester v0.2. It provides a serial terminal interface
to execute a set of operations on the SD card. This uses avr-libc and is somewhat
bloated. The intent is to create a somewhat verbose user interface that is helpful
for debugging and learning.

<b>Overview</b><br>
The applicaion provides a simple terminal-like interface to the user with a small set
of commands. An example session is shown below:

\code
SD Card Tester v0.2.0
> help
Available commands:
  get
  read
  write
  erase
  scan
  sdinit
  help
> sdinit
Initializing SD Card...
Card found
CID: 02 54 4D 53 44 35 31 32 28 81 60 D2 8F 00 72 55 
CSD: 00 2D 00 32 1B 59 83 D0 FE FB 7F 80 16 40 00 AD 
> read 0 0x20
addr: 00000000, size: 00000020
0x00000000:   68 69 20 74 68 65 72 65 FF FF FF FF FF FF FF FF    hi there........
0x00000010:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
> write 0 Hello World!
addr: 00000000, size: 0000000C
> read 0 0x20
addr: 00000000, size: 00000020
0x00000000:   48 65 6C 6C 6F 20 57 6F 72 6C 64 21 FF FF FF FF    Hello World!....
0x00000010:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
> scan 0 0x1000
0x00000000:   48 65 6C 6C 6F 20 57 6F 72 6C 64 21 FF FF FF FF    Hello World!....
0x00000010:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................

0x00000800:   53 6F 6D 65 20 68 69 64 64 65 6E 20 64 61 74 61    Some hidden data
0x00000810:   2E 2E 2E FF FF FF FF FF FF FF FF FF FF FF FF FF    ................

> erase 0 0x100
Erasing addr: 00000000, size: 00000100
Type "yes" to confirm > yes
> read 0 0x100
addr: 00000000, size: 00000100
0x00000000:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
0x00000010:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
0x00000020:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
0x00000030:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
0x00000040:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
0x00000050:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
0x00000060:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
0x00000070:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
0x00000080:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
0x00000090:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
0x000000A0:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
0x000000B0:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
0x000000C0:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
0x000000D0:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
0x000000E0:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
0x000000F0:   FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF    ................
> 
\endcode

<b>Notes about the SD card</b><br>
This application has only been tested on a 512MB SD card. It does not support SDHC, as
the addressing changes from byte-wise to block-wise. It assumes that you can do byte-wise
reads from the card as well.

<b>Hardware</b><br>
The SD card must be attached to the SPI line, with the slave select (SS) line
tied to the card's chip select (CS). No external pullups are required. I recommend
using SparkFun's MicroSD breakout board (http://www.sparkfun.com/commerce/product_info.php?products_id=544).

This has only been tested on the ATmega16, but should work fine on the ATmega32. The project
is expecting a 16MHz clock.

<b>Author</b><br>
Stefan Filipek

<b>Date</b><br>
March 2009

*/
