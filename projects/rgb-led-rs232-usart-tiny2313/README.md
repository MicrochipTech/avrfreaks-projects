# RGB led rs232 USART tiny2313

Uploaded by epaloniemi on 2014-05-20 14:44:00 (rating 0 out of 5)

## Summary

This is a small project to control RGB led's with the ATMEL AVR attiny2313.


External crystal 8.000Mhz buad rate 19200(seems to work well, freq. error aprox. 1%)


MAX232 level shifter to connect to rs232 of computer(Tietomyrsky Oy EXB2313 development board)


cheap cc 5mm 3x 20mA RGB led


 


RED led connected to OCR0A = PB2 = pin no. 14


GREEN led connected to OCR0B = PD5 = pin no. 9


BLUE led connected to OCR1A = PB3 = pin on. 15


Non inverting phase correct PWM clk/8 prescaler.(timer clk=1Mhz) PWM freq. approximately 1.9Khz.


Code sweeps through colors and white while "a" is pressed on the computer key board. When "a" is released sweeping stops and color stays constant.


Terminal must be open and connected properly for this work. Terminal displays PWM timer output compare register value for each color channel.


0=off 255=full on, a value in between 0 and 255 is a partially on state. The higher the value is the brighter that color is, basic PWM.

## Links

- [Video of testing the code and LED's](https://www.youtube.com/watch?v=_om0qQCJE94)

## Compilers

- CodeVisionAVR-Standard

## Tags

- Complete code
- CodeVisionAVR-Standard
- ATtiny2313
- Lighting Control
- Microcontrollers
- avr
- Code
- RGB
- leds
- software
