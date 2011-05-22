# Addressable LED Dimmer

Uploaded by oscouro on 2011-05-22 16:42:00 (rating 0 out of 5)

## Summary

Included in the zip, you will find master and slave source-code folders, as well as two shell scripts. These compile source files and program the appropriate target chips.


Software has not been tested on WinAVR!  

This is a LINUX AVR-GCC & AVRDUDE compilation  

to program the controller, first:  

*download zip  

*build hardware!  

*get avrdude and avr-gcc  

*program by opening a terminal and type:  

$ cd /(where you put the folder)/leddim  

(connect controller unit to spi)  

$ ./masterprog.sh  

(connect receiver to spi)  

$ ./slaveprog.sh


Hardware:  

*Materials*  

controller{  

-ATmega1284p  

-16x2 character LCD display  

-50k potentiometers (6)  

-LTC485 (RS485 transceiver)  

-5V power supply  

} (you will need one controller per installation)  

fixture{  

-ATmega328p (ATTiny2313 will work with some code modification)  

-RFP30N06 N-FETs (4)  

-1k Resistor (4)  

-LTC485  

-RGBW Emitter (such as LEDEngin LZC RGBW)  

-12V @ 4A Power Supply  

-LM7805 (5V Linear regulator)  

}  

programmer{  

-AVR Pocket Programmer (Sparkfun, $15)  

}


pin-connection outline:  

controller{  

 VCC/AREF/AVCC to 5V, both GND-pins to GND  

-PORTA - Analog Input  

 PA0:6 - wipers of 50k Pots wired between 5V and GND  

-PORTB - SPI Programming  

 PB5 - MOSI  

 PB6 - MISO  

 PB7 - SCK  

 RESET - RESET  

-PORTC - LCD Control  

 PC6 - RS  

 PC5 - RW  

 PC4 - EN  

 PC3:0 - D4:7  

-PORTD - USART TXD  

 PD1 - Pin 4 on LTC485  

LTC485: pins 2,3,8 to 5V; 4 to TXD; 5 to GND; 6,7 to 6,7 on receiver;  

}  

receiver{  

-SPI connected for programming, otherwise NC  

-PORTC - 6-bit address (8-bit possible on SMD versions)  

 address set with either Pull-up/down to power/ground  

 or alternately with dip-switches  

-RXD on pin 2, White to pin 5, green to 11, red to 12, blue to 17.  

COLOR lines go to either:  

 -positive lead of LED (through resistor) for low-power LEDS.  

 -Gate of N-FET acting as low-side gate, for high-power LEDS.  

LTC485: pin 1 to RXD; 2,3,5 to GND; 6,7 to 6,7 on transmitter; 8 to 5V;  

}

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
