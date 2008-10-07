# LM34DZ Temp Sensor on AVR

Uploaded by kdiggity08 on 2008-10-07 01:46:00 (rating 0 out of 5)

## Summary

In this project I have interfaced a LM34DZ temp sensor to the A/D Converter on the AVR (Atmega8). I use channel 0 on the ADC and display the temperature on an 2x16 Character LCD in the format of XX.X'F. I used general public licensed LCD code.


The LM34DZ Temperature Sensor has three pins, Supply Voltage (+5V), Ground, and Output Voltage (10mV = 1'F). I have not used any external gain hardware (op amps) in conjunction with the temp sensor. The temperature range is between 5'F and 300'F in basic mode (Reference data sheet) with a voltage conversion equal to 10mV per 1'F, then this gives a voltage range of 50mV to 3V on the ADC.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
