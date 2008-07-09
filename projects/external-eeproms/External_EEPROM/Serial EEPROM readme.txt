   AVR Serial EEPROM programmer User guide and notes
=========================================================

    Serial EEPROMs are small, inexpensive ICs that store a lot of data that doesn't fit into the 
memory of the microcontroller.  This is not program code in the EEPROM, but tables and strings. 
Since the Serial EEPROM can be rewritten over million times, it is excellent for storing data from 
sensors that will be sent to central servers once a day or week.  The serial EEPROMs of the 
24xx family can be read, erased, and reprogrammed with only +2.5 volts.

  Serial EEPROMs come in two formats. One is a SPI (Serial Peripheral Interface) with seperate input
and output lines and an individual enable line for each device.  The other format is the Philips I2C
two-line serial bus.  This format allows many devices on the same two lines, but is more difficult to
program and interface correctly.  

  The Atmel AVR microcontroller family has several devices with built-in I2C called a two-wire
interface (TWI).  These devices also have USARTs for easy interfacing to PCs using RS-232 serial ports.
This program source code (i2cm8.asm) is specifically for the AVR Mega8, but can be easily adapted to 
the other devices with TWI and USARTs.  My source code is rough and limited, but it does work and it is
fairly well documented.  It is written in assembler modulues.  These modules can be cut-and-pasted into
other assembler programs to add serial EEPROM functionality to programs that need to read and save 
amounts of data that are too large to fit into the memories of the AVR device.  Feel free to email me
with any questions or suggestions:  alan_probandt@yahoo.com

 Interfacing between PC and the AVR SEE programmer is done with the terminal program "RealTerm.exe"
Download the latest version from the web.  I used version 2.0.0.57.  Set up a shortcut with the following 
string in the properties section:

"C:\Realterm\realterm.exe baud=9600 port=1 flow=2 capfile=g:\AVR\src\m8\capture.txt RTS=1" 

Adjust the serial port number and capture file (capfile=) to match your PC and folders.  The 'flow=2' is 
needed for RTS/CTS handshaking.  

  Power the SEE programmer and connect the serial cable to the PC.  Run RealTerm.  Click the mouse in
the data display area.  Press the reset button on the SEE. Now the following one-letter commands 
(case insensitive) are active:  (most are used for development and testing)
   
   R  - Read a single test page from the serial EEPROM and display the 64 bytes on the terminal. 
        Prompts for the page number to be displayed.  
        
   W  - Write a single test page from the serial EEPROM.   The first two bytes of the test page
        hold the physical address of the beginning of the page in the EEPROM.  The remaining 62
        bytes are sequencial values 0x02 to 0x3f.
        
   E  - Erase page(s). Prompts for beginning page number and the number of pages to fill with value 0xff.
   
   D  - Dump pages.  Prompts for beginning page number and number of pages to send from EEPROM to PC.                                           
                                                                                                           
   P  - Program multiple pages with data received from the PC.  The PC will send data from a file in blocks
        of 64 bytes.  After receiving 64 bytes, the SEE will deassert the CTS line and the PC will stop sending
        data.  SEE waits about 8 milliseconds for the page to be written to the EEPROM. Then it re-asserts the 
        CTS line.  The PC will restart sending data when the CTS line is active.  Use the SendFile function
        of RealTerm.EXE to transmit the data file from the PC.  When the RealTerm character count stops 
        incrementing, press the Reset button on the SEE.  The EEPROM is now programmed.  However, the 
        last page of data received from the PC will not be written to the EEPROM if it has less than 64
        bytes of data.  This will be fixed in later versions of the i2cm8.asm program, insh'Allah.
    
   T  - MultiPage write test. Receives two pages from the PC. Writes data into the first two pages of the EEPROM.  
      
      
      
   RealTerm can log the entire 64K bytes of data that is read from an EEPROM into its 'capture' file.  
And it can transfer an entire 64K file from the PC (with pauses after every 64-byte page) by using the 
Send function.  Included is a 64K test file (SEEtest.bin) comprised of 1024 pages.  Each page has the page number
in the first two bytes, followed by 62 bytes of sequencial numbers.  With a hex editor, it looks like this:

0000 0000:  00 00 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f ; page number 0000 in bytes 00;01
0000 0010:  10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
0000 0020:  20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f
0000 0030:  30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f 
0000 0040:  00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f ; page number 0001 in bytes 00;01
0000 0050:  10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
0000 0060:  20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f
0000 0070:  30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f
.....
0000 FFC0:  03 FF 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f ; page number 1023 in bytes 00;01
0000 FFD0:  10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
0000 FFE0:  20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f
0000 FFF0:  30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f