# UART based Data-logger with time & temperature log

Uploaded by DHARMANI on 2018-07-04 17:38:00 (rating 0 out of 5)

## Summary

     Here is one more data-logger circuit, based on ATmega32. providing 8-channels with 0 to 5V voltage level measurement. The circuit also has on-board RTC (based on DS1307) to log the data with time-stamp. Data is logged via UART. The circuit has 3-pin UART connector, which can be used to connect to PC/laptop using UART(TTL) to USB serial converter.  

      Also, there is a thermometer IC, DS1621, which is connected via i2c (along with the RTC IC) and provides temperature measurement from -55 to +125 Deg. C. The temperature and the voltages are sent to the UART for logging in an ASCII string format, when a pushbutton provided for logging is pressed. Pressing the same button again stops the logging.


      The circuit includes a 16x2 LCD (4-bit interface) for displaying the current date/time from RTC and the temperature, with logging status. The RTC date/time can be changed using three push-buttons provided on-board.


     The detailed circuit description and operation with relevant pics/ schematic are shared on the link given below. Also, the source code (Atmel Studio 7 project folder), the simulation files and the schematic, layout and gerber files are available for download.


 


Thanks!

## Links

- [UART based Data-logger page](https://www.dharmanitech.com/2018/07/uart-based-8-channel-data-logger-with.html)

## Compilers

- Atmel Studio 7

## Tags

- Complete code with HW
- Atmel Studio 7
- ATmega32
- DS1307
- DS1621
- datalogger
- adc
- RTC
- temperature sensor
