HOW TO BUILD THE BOOTLOADER

1. Use the PC tools to create the files 'aeskeys.h' and 'bootldr.h'.

2. Open the project in Avr Studio. 
   Under project configuration set the following 4 parameters:
  * Device
  * Frequency
  * Baud rate
  * Boot start address 
  
3. Choose 'Rebuild All'.

4. Program the device with the resulting HEX file.

