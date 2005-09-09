***************************************************

ATMEL AVR UART/Intel HEX Bootloader for AVR-GCC

Author: Preston Manwaring, Brigham Young University
        Provo, Utah, USA
	pmanwaring@gmail.com

	http://www.ee.byu.edu/~pkm2/

***************************************************

----------------- Description ---------------------

This project implements a simple bootloader on a 
stack-based Atmel AVR MCU. It has been tested on an
ATmega8 and ATmega162. It should be compatible
with all stack-based AVR MCU's (not ATtiny).

Excerpt from the header file of boot.c

This is a bootloader designed for the AVR processor 
family which allows new firmware to be uploaded via 
the UART in Intel Hex format. An interface for 
uploading such as Hyperterm is envisioned. This
code correctly parses the Intel Hex strings and 
upon reception of a complete page (processor 
specific) of data, writes the data to flash. Minimal
error handling is implemented. Errors during program 
upload are

ERR 1 - File upload synchronization is off (i.e. the 
        ':' was not found in the HEX file)
ERR 2 - the checksum failed.

Typical Intel hex format is supported in unix mode 
(LF not LF,CR). So be sure to strip the extra CR 
from the hex file or have this program strip the CR.

The user menu is as follows:
U - upload the hex file. A 'READY' will return when 
    the MCU is ready to receive the file. As the file 
    uploads, a '*' prints for every page written to 
    flash. When complete, 'OK' is transmitted. If an 
    error occurs, 'ERR' followed by a number is printed.

X - exit this program and start the user application.

NOTE: This software is not optimized. It fits as it 
currently stands in a BLS of 1024. Optimizations 
could be easily made, but the code is much easier 
to  understand with the way it currently stands. I spent 
a lot of time reading manuals and looking at other's 
code to figure out all I wanted to know. So I hope 
this is easily understood. Due to the lack of 
optimization, the most common error you will get 
using this code will be ERR 1 as synchronization 
fails. This failure will occur because your BAUD 
rate setting is too high and the program cannot 
keep up the incoming data. Slowing down the data 
transfer rate should fix this or you can optimize 
the code.

A special thanks needs to go to Martin Thomas for 
writing some excelent code which helped me get 
started. In fact, you will see some files and macros 
of his which I use including chipdef.h. His project
 can be found at

http://www.siwawi.arubi.uni-kl.de/avr_projects/


August 22, 2005 - Version 0.1

* implemented simple non-buffered UART
  communication
* Standard Intel HEX code file and error checking
  support enabled


------------------ Installation -------------------
by Martin Thomas and Preston Manwaring

- Change the MCU type in the makefile. If your 
  processor is not listed, modify the makefile
  according to the processor manual.

- Change the boot(loader)-size in main.c, this
  bootloader is larger than 512 words (1024 bytes), 
  so select at least _B1024!

- Change the UART constants in sercom.h to match
  your MCU speed and the listed baud rate. As this
  version is not buffered, a slower baud rate is
  required.

- Change the start-condition in main.c. Default
  is: enter bootloader if Pin A7 is connected to
  GND during reset/startup

- Edit the value MT_BOOTLOADER_ADDRESS in the 
  makefile according to the selected bootloader
  size. Keep in mind that this value has to be
  given in bytes (not words) in the linker options.

  i.e. ATMega16, boot-size 1024 words, see
  datasheet and find "Boot Reset Adress" for
  "Boot Size" 1024 words is 1C00. 1C00 is given
  in words, so set MT_BOOTLOADER_ADDRESS to
  3800 (=2*1C00)

- Please use at least avr-gcc 3.3.1/avr-libc 1.0 
  or WINAVR Sept. 2003 or later to compile and link 
  the bootloader.

- upload the hex-File to the AVR (STK500, STK200, SP12
  etc.)

- program the "Boot Flash section size" (BOOTSZ fuses)
  according to the boot-size selected in main.c
  i.e. BOOTSZ=00 for boot-size 1024 words on ATmega16

- enable the BOOT Reset Vector (BOOTTRST=0)

- Set the lock bits to protect the bootloader from
  SPM-writes (Boot Loader Protection Mode 2 in STK500-
  plugin)

- Connect the AVR UART Pins via level-shifter/inverter
  (i.e. MAX232) to you PCs Com-Port.

- Reset the AVR while fullfilling the bootloader start-
  condition. Which means connect PA7 to GND in the default 
  config during reste/power-cycle. Keep the connection 
  or hold the key down until you see the AVRPROG dialog!

- Start AVRPROG (AVRStuido/Tools or stand-alone) - 
  keep PA7 grounded!

- AVRPROG will detect the bootloader, you may release
  PA7 now

- see AVRStudio online-help for more information how
  to use AVRPROG

- make sure to EXIT from AVRPROG (button) to start
  your main-application (or toogle power/reset)

