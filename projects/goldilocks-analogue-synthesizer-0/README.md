# Goldilocks Analogue Synthesizer

Uploaded by feilipu on 2015-06-03 07:52:00 (rating 0 out of 5)

## Summary

For the past year, I’ve been prototyping an Arduino clone, the [Goldilocks Analogue](http://feilipu.me/2014/12/06/goldilocks-analogue-part-6/ "Goldilocks Analogue – Prototyping 3"), which incorporates advanced analogue output capabilities into the design of the [original Goldilocks](http://feilipu.me/2013/03/08/goldilocks-1284p-arduino-uno-clone/ "“Goldilocks” 1284p Arduino UNO Clone") with ATmega1284p AVR MCU and uSD card cage. Recently the design scope crept up to include [two SPI memory devices (EEPROM, SRAM, FRAM)](http://feilipu.me/2015/03/06/nasa-eefs-on-avr-atmega/ "Implementing NASA EEFS on AVR ATmega"), and [microphone audio input](http://feilipu.me/2015/04/29/xbee-walkie-talkie/ "XBee Walkie Talkie"). But, before I go through another prototype cycle, I thought it would be a good idea to build some demonstration applications, showcasing the capabilities of an arduino compatible platform with integrated analogue output and have some fun with audio.


 


![](https://www.avrfreaks.net/sites/default/files/p1010928.jpg "Goldilocks Analogue - Prototype 3")


 


It seemed appropriate to build a synthesizer using the Goldilocks Analogue as the platform, and a [Gameduino 2 shield incorporating a FDTI FT800 EVE GPU,](http://feilipu.me/2014/01/31/gameduino-2-with-goldilocks-and-eve/ "Gameduino 2 with Goldilocks and EVE") and see how close I could get to a musical outcome.


 


![](https://www.avrfreaks.net/sites/default/files/p1010913.jpg "Synthesizer GUI using Gameduino 2 FTDI Chip EVE GPU")

## Links

- [Goldilocks Analogue Synthesizer](http://feilipu.me/2015/06/02/goldilocks-analogue-synthesizer/)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
- mcp4822
- ATmega1284P
- synthesizer
- opamp
- DAC
- Waveform Generation using DAC
- SPI configuration
