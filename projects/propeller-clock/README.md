# Propeller clock

Uploaded by limapapy on 2011-05-30 12:30:00 (rating 0 out of 5)

## Summary

The project is composed by two components:  

-Hardware  

-Software


The hardware is composed by :  

- 32 RGB Leds.  

- 4 Led drivers (TLC5947)  

- 1 RS232 driver (TRS3221)  

- 1 Atmega64  

- 1 Enconder (AEDB-9140)  

- 1 Dc motor  

- LDO voltage regulator  

- 1 RTC (ds1307)


The main objective is to do a clock with the 32 leds, an optional objective is two show images. The leds are in a strait line, while they spin at high speed if we turn on and turn of them they create certain padrons, if we control the padrons we can show everything we want(such as images). We use the RTC to give us the time to show on propeller. The TLC5947 fed the leds, they work with a protocol that seams to spi.  

The encoder give the angle that the leds line are at certain time that allow us to control the leds.


The software is composed by serial USART interface to send command from pc and to send images, it has interace with SPI to control the led driver, and has I2C to control the RTC.

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
