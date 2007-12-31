# 2.4 Ghz R/C link + Wii Nunchuk

Uploaded by asgrewal on 2007-12-31 01:51:00 (rating 0 out of 5)

## Summary

A hobby project..did it to learn about AVR MCUs and Nordic 2.4 ghz transreceiver..


HARDWARE


Transmitter


The transmitter unit consists of Wii Nunchuk interfaced with ATMEGA168 microcontroller board (Bare Bones board, an Arduino look alike, <http://moderndevice.com/>) through I2C bus. MCU board is further interfaced with Nordic Semiconductor's nRF24L01 2.4Ghz transreceiver through SPI.


Receiver


The receiver unit consists of ATMEGA168 microcontroller board (Really Bare Bones board, an Arduino look alike, <http://moderndevice.com/RBBB.shtml>) interfaced with Nordic Semiconductor's nRF24L01 2.4Ghz transreceiver through SPI. MCU board receives transmitted signals and generates PWM signals for throttle and steering channels driving ESC and standard servo respectively.


SOFTWARE


Complete code attached


OPERATION


[http://www.youtube.com/watch?v=U...](http://www.youtube.com/watch?v=Uwv5_Ubad3A)


USEFUL ENHANCEMENTS


Currently uses single RF channel. A frequency agility protocol will increase robustness.


LINKS


<http://moderndevice.com>  

<http://www.windmeadow.com/node/42>  

[http://www.nordicsemi.com/index....](http://www.nordicsemi.com/index.cfm?obj=product&act=display&pro=89)

## Compilers

- AVR32 GNU Toolchain

## Tags

- Complete code with HW
- AVR32 GNU Toolchain
