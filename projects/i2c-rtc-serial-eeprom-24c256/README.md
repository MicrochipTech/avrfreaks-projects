# I2C RTC & Serial EEPROM 24C256

Uploaded by DHARMANI on 2009-04-25 17:16:00 (rating 0 out of 5)

## Summary

Hi friends,  

one more of my projects here: Interfacing the RTC DS1307 and 256Kbit i2c compatible serial EEPROM with TWI bus of ATmega128, and also the interfacing of 16x2 LCD in 4-bit mode.


The software routines include DS1307 library, 24C256 library, I2C library, UART routines, main function file and header files. 


The program uses PC hyper terminal for display purpose. Hyper Terminal used with 19200 Baud, No parity, None flow control settings.


When the circuit is powered on, a welcome message is displayed on the Hyper Terminal window and a menu with 9 options (0-8):


0: Erase EEPROM  

1: Write EEPROM  

2: Read EEPROM  

3: Write eeprom page  

4: Read eeprom page  

5: Display RTC Date  

6: Display RTC time  

7: Update RTC Date  

8: Update RTC time


The option is selected from PC keyboard. While writing to eeprom or RTC the data is entered using PC keyboard as specified by the program.


For further info & your comments, visit here:  

[http://www.dharmanitech.com/2008...](http://www.dharmanitech.com/2008/08/interfacing-rtc-serial-eeprom-using-i2c.html)


Thanks!


 **Edit1: 25-April-2009**   

-------------------------------  

Source code available for downloading in winAVR (AVR-GCC) format (earlier it was written with ICCAVR compiler)

## Links

- [Schematic, Project details and screen-shots](http://www.dharmanitech.com/2008/08/interfacing-rtc-serial-eeprom-using-i2c.html)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
