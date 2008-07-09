AVR Parallel EEPROM programmer User guide and notes
=====================================================

  I found a pile of scrap older PC motherboards recently and most had the BIOS chips in sockets.  
I wrote this program in order to possibly reuse the parallel EEPROM BIOS chips.  They were all 128Kx8
and mostly the 29EE010 family that can be erased and reprogrammed with only +5 volts.

  In order to read and write a parallel EEPROM, you need a microcontroller (uC) with an external data bus 
like the 1st gen 8-bit microprocessors.  These uCs can access 64K of address space with only two I/O ports
and an Address Latch Enable (ALE) strobe.  They use a TTL 8-bit latch like the 74HC573 to store the
lower address bits when reading/writing to memory or peripheral chips located in the 64K of memory space.
The AVR family has three devices with a ALE bus.  The 90S8515 has timing problems that were fixed in
the Mega161 and the newer Mega162.  I wrote the code for a Mega161, but a Mega162 should work fine.
For a Mega162, the interrupt vector table has to be changed. Simply uncomment it in the source code.

  The advantage of using the ALE bus is that it runs very fast.  The Mega161/162 adds extra cycles
when accessing what the data sheet calls external static RAM space.  But it is still much faster than using
extra latches to create the address/data lines needed to interface to the EEPROM.

  I wrote a simple Visual Basic program to send a file from the PC to the AVR EE programmer.  But
I found that by using the terminal program "RealTerm", I didn't need it.  Download the latest version from
the web.  I used version 2.0.0.57.  Set up a shortcut with the following string in the properties section:
"C:\Realterm\realterm.exe baud=9600 port=1 flow=2 capfile=g:\AVR\src\mega161\capture.txt RTS=1 Display=2 rows=34"
Adjust the serial port number and capture file (capfile=) to match your PC and folders.  The 'flow=2' is 
important for RTS/CTS handshaking.  "Display=2" shows data in hex with a space after each pair of digits.

  Power the EE programmer and connect the serial cable to the PC.  Run RealTerm.  Click the mouse in
the data display area.  Now the following one-letter commands (case insensitive) are active:  
   
   F  - Write a file from the PC.  Waits until data starts coming from the PC.  After receiving 128
        bytes it writes them to the EEPROM, starting at address 0x00000.  While writing the 128 byte sector,
        the CTS line is de-asserted.  The terminal program will pause until the CTS line is reasserted again.
        
   A  - Read all 128K bytes of the EEPROM.  All this data should go into a capture file that is set up before 
        pressing this letter.  At 9600 baud, this takes several minutes.  Press reset to stop.
        
   E  - Erase the entire EEPROM by sending a six byte sequence.
   
   R  - Read test. Reads the first eight sectors from EEPROM.                                           
                                                                                                           
   W  - Write test. Writes sequencial numbers (0x00 to 0x7f) into the first eight sectors of the EEPROM.
      
      
   RealTerm can capture the entire 128K bytes of data that is read from an EEPROM into its 'capture' file.  
And it can transfer an entire 128K file from the PC (with pauses after every 128 byte sector) by using the 
Send function.  Included is a 128K test file that has the sector number in the first two bytes, followed
by 126 bytes of sequencial numbers.

