# Temperature Data Logger

Uploaded by kdiggity08 on 2009-01-09 22:15:00 (rating 0 out of 5)

## Summary

I have built a Temperature Data Logger for my Atmega8 around the LM34DZ Temperature sensor. This is somewhat of an expansion of my Temp Sensor project. In this project I have utilized a DS1307 Real Time Clock to trigger a A/D Converter sample every 10 seconds. The 10 seconds is an arbitrary value and I just used it for convenience.


My program is basically a simple state machine with 5 states. The '0' State is the default start up state and gets the first temperature reading. The '1' State is the time triggering state and it waits here until 5 seconds have elasped. The '2' State samples the ADC for a temperature reading (I used a 10k trimpot to test functionality). The '3' State processes the temperature samples and determines the high, low, and average values in addition to the current temperature. The finally State ('4') places everything in a "pretty" form to display on my lcd.


Note. The Sensor range is 0-1000mV or 0'F - 100'F. 10mV = 1'F.


I eventually want to add EEPROM or Flash memory and USART functionality to my system. The I2C bus is already being used so EEPROM won't be a huge step up. 


I have added an optional Fan Control. Replace that main.c file with the original to have a basic Fan Control System. It compares the current temp to a threshold value (I set it to 75) and sets a bit once its reach or clears it if below. I have wired it into my circuit and tested it successfully with a 5V 0.3W Fan.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
