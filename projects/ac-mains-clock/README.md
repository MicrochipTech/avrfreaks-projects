# AC Mains Clock

Uploaded by AndyPalm on 2010-06-08 02:03:00 (rating 0 out of 5)

## Summary

; asm60HzClock.asm - Clock using AC mains frequency as a time base.  

; Uses TWI bus for communication with an SAA1064  

; LED driver for the display.  

;  

; In normal clock mode displays the time on four seven segment  

; display digits. One push button allows the momentary display of  

; the month and day. Another allows the momentary display of  

; minutes and seconds.  

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

; - ATtiny2313 with 1 MHz internal clock (8 MHz internal RC osc.  

; with the divide-by-8 flag set). Fuse settings are:  

; Low 0x64, High 0x9F, Extended 0xFF.  

; - SAA1064 LED driver with two HDSP-K511 2-digit 7 segment  

; displays and associated components wired as per its data sheet.  

; The SDA and SCL lines each have a 4.7K pullup resistor.  

; - Power supply fed from a 6 to 9v (rms) AC wall wart, including  

; bridge rectifier and 7805 voltage regulator with heat sink.  

; - A tap on the AC input feeds a single 1N914A diode rectifier and  

; voltage divider which are input to a 74LS14 Schmitt trigger  

; whose output goes to the T0 pin of the MCU as an external  

; TIMER0 clock source.  

;  

; AVR pin assignments:  

; PORTD0:1 - D0 = SDA, D1 = SCL  

; PORTD4/T0 - 60 Hz input for TIMER0 external clock  

; PORTB0:2 - Three pushbutton inputs on STK500  

; B0 = Set time (cycles thru year code, month, day, hours, mins)  

; B1 = Show mins and secs (momentary) / DOWN in set time mode  

; B2 = Show month and day (momentary) / UP in set time mode  

;  

; Notes:  

; The system tick using TIMER0 is set to 200 ms so that the clock  

; is updated every five ticks. This tick value was chosen for a  

; reasonable response time to the push buttons but with allowance  

; for a 20 ms button debounce interval.  

;  

; To adapt the program for AC mains with frequency 50 Hz, simply  

; change the value of the equate statement for AC\_MAINS\_FREQ.  

;  

; The Atmel A300 Application Note code is used for bit-bang TWI  

; operations with the ATtiny2313. There is a typo in the original  

; file which has been corrected. The original comments for these  

; TWI functions have been retained in this code. There is no  

; TWI error checking for this simple system.  

;  

; Andy Palm  

; 2010.06.07


The zip file includes the assembly code and schematics for this project plus the assembly code and schematic for a previous project using a real-time clock module and crystal time base. Both projects share a common display unit.

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
