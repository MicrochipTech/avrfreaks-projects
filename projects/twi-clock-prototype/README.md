# TWI Clock Prototype

Uploaded by AndyPalm on 2010-03-14 18:26:00 (rating 0 out of 5)

## Summary

This was an exercise to learn about TWI. It is a simple application without TWI error checking, but uses the compact bit-bang TWI code from Atmel's Application Note A300. Included is A300 code asm code with a small typo corrected.


;----------------------------------------------------------------------  

; asmTWIClock.asm - Prototype clock using TWI bus for communication  

; with a PCF8583 time clock and SAA1064 LED driver.  

;  

; In normal clock mode the MCU reads the real time from the PCF8583  

; and displays it on four seven segment display digits. One push  

; button allows the momentary display of the month and day. Another  

; allows the momentary display of minutes and seconds.  

;  

; When a third button is pushed the clock is put in a set time mode.  

; As this button is successively pushed the user can change the year  

; code, month, day, hours, minutes. A decimal point is illuminated  

; to show which data can be changed by the two other buttons, which  

; in this mode serve as Up and Down buttons. On the final push of  

; the Set Time button the clock is restarted with the seconds set  

; to zero. The year code is the year modulo 4, so a 0 represents a  

; leap year.  

;  

; Hardware:  

; ATtiny2313 with 1 MHz internal clock (8 MHz internal RC oscillator  

; with the divide-by-8 flag set) on STK500 board. Fuse settings are  

; Low 0x64, High 0x9F, Extended 0xFF.  

;  

; Homebrew prototyping board holding:  

; - SAA1064 LED driver with four 7 segment digit displays and two  

; 2N4401 npn transistors  

; - PCF8583 real-time clock with 32.768 clock crystal and trim cap  

; Both of these are wired as per their data sheets. The board also  

; has two 4.7K pullup resistors for the SDA and SCL lines.  

;  

; AVR pin assignments:  

; PORTD0:1 - D0 = SDA, D1 = SCL  

; PORTB0:2 - Three pushbutton inputs on STK500  

; B0 = Set time (cycles thru year code, month, day, hours, mins)  

; B1 = Show mins and secs (momentary) / DOWN in set time mode  

; B2 = Show month and day (momentary) / UP in set time mode  

;  

; Notes:  

; The Atmel A300 Application Note code is used for bit-bang TWI  

; operations with the ATtiny2313. There is a typo in the original  

; file which has been corrected. The original comments for these  

; TWI functions have been retained in this code. There is no  

; TWI error checking for this simple system.  

;  

; For a stand-alone clock, a better choice for a real time clock  

; would be a model with battery backup if the display were to be  

; turned off. Also, the SAA1064 is a 5 volt device, and one might  

; prefer a 3.3v system with a different display setup.  

;  

; Since only 5 I/O pins are used a smaller chip with 2K of program  

; flash RAM and some data SRAM could be used. Also, TWI could be  

; done using the Universal Serial Interface as per the Atmel  

; A310 Application Note.  

;  

; Andy Palm  

; 2010.03.14  

;  

;----------------------------------------------------------------------

## Compilers

- AVR Assembler

## Tags

- Complete code
- AVR Assembler
