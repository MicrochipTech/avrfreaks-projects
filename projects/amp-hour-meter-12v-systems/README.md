# Amp-hour meter for 12v systems

Uploaded by AndyPalm on 2010-03-06 01:33:00 (rating 5 out of 5)

## Summary

The original amp-hour meter project, developed by Mert Nellis W0UFO, appeared in an article in the Spring 2009 issue of â€œThe QRP Quarterlyâ€ published by QRP ARCI. I used the original circuit except for substituting an Atmel ATtiny26 MCU and a parallel LCD in 4-bit mode for the original PICAXE-08M and serial LCD. Besides cost there was an additional advantage to this change in that using an 8 MHz crystal for the MCU clock made it unnecessary to empirically calibrate the main loop timing.


The amp-hour meter displays current, voltage, and the amp-hours used. It is designed for a maximum current of 5 amps in 12 volt DC systems with voltages between 8 and 20 volts, for example, low power communication systems.


There are two sensing circuits, one for voltage and one for current. The voltage sensing circuit is a simple voltage divider adjusted to give an analog-to-digital (ADC) count of 600 (out of 1023) when the input voltage is 12.0. The current sensing circuit uses a 0.1 ohm 3 watt resistor in the ground return circuit. The voltage across this resistor is amplified by an op amp to give an ADC count of 800 for a load drawing 4 amps. The voltage ADC count divided by 5 gives the voltage in deci-volts. The current ADC count divided by 2 gives the current in centi-amps. These are displayed in units of volts and amps on the first line of the display.


To calculate amp-hours, the main software loop is set to execute once every 360 milliseconds, which is 0.0001 hours. Thus the number of amp-hours consumed during one time interval is (ADC count)/2 centi-amps x 0.0001 hrs which is  

(ADC count)/2 micro-amp-hours. This value is added to a running total in a 16-bit register. When this register exceeds 1000 a second 16-bit register representing milli-amp-hours is incremented. The current value of this second register is shown on the second line of the display in units of amp-hours.


The Atmel AVR Attiny26 MCU was programmed in assembly language as previous projects had provided sections of software that could be used in this project. A great advantage to the Atmel AVR products are that software can be easily transferred from one model to another, as they all share the same instruction set and register structure. 


A simple round-robin software structure is used. At the top of the main loop a small loop is executed until a flag is set by the TIMER0 interrupt service routine (ISR). This ISR reloads TIMER0 for a 4 ms overflow interval and counts 90 of these intervals to yield a 90 x 4 ms = 360 ms = 0.0001 hr tick. Both ADCs are run in one-shot, 10-bit mode once per tick. The display is updated once per tick. 


The 12 character by 2 line LCD used for this project, the Crystalfontz CFAH1202A-YYH-JP, was obtained surplus from All Electronics. The connections are the same for any LCD that follows the common Hitachi 44780 standard. The more common size 2x16 LCD can be used without modification (but you can easily expand the display to 16 characters). The two display lines are set up in the data SRAM area. The display digits are calculated from the corresponding binary values by the same division macro used in a previous frequency counter project.


This amp-hour meter will be used to monitor battery consumption while operating low power amateur radio equipment in the field or while using emergency battery power at my home station. 


;----------------------------------------------------------------------  

; asmAHrMeter.asm - Amp-hour meter with LCD display  

;  

; Converts a current sense circuit voltage in ADC3 and increments a  

; running total which represents amp-hours used. Converts a voltage  

; divider voltage in ADC4 to represent the voltage used. A character  

; LCD shows voltage, current, and amp-hours.  

;  

; This project is based on an article in the Spring 2009 issue of  

; "The QRP Quarterly" by Mert Nellis W0UFO. The original current and  

; voltage sensing circuits are used here. However, the original  

; published project used a PICAXE-08M and a serial LCD. Using an AVR  

; ATtiny26 and a regular LCD allows quicker updates of the display.  

;  

; Hardware:  

; - ATtiny26 with external crystal at 8 MHz. Fuse settings are  

; High: F1, Low: CC  

;  

; - Standard 2x16 or 2x12 character LCD in 4-bit data mode  

;  

; - A 0.1 ohm 3 watt current sensing resistor with CA3140 op amp  

; circuit is input for a current reading ADC (see schematic).  

;  

; - A voltage divider is input for a voltage reading ADC (see  

; schematic).  

;  

; AVR pin assignments:  

; PB0:3 - LCD data lines  

; PB4:5 - 8 MHz crystal  

; PA0 - LCD E line  

; PA1 - LCD RS line  

; PA4 - ADC3 input from current sensing circuit  

; PA5 - ADC4 input from voltage divider circuit  

;  

; Notes:  

; - A simple round-robin structure is used. At the top of the main  

; loop a small loop is executed until a flag is set by the  

; TIMER0 interrupt service routine. This ISR reloads TIMER0 for  

; a 4 ms overflow interval and counts 90 of these intervals to  

; yield a 90 x 4 ms = 360 ms = 0.0001 hr tick. Both ADCs are run  

; in one-shot, 10-bit mode once per tick. The display is updated  

; once per tick.  

;  

; - The current sensing circuit that is input to ADC3 is scaled so  

; that an ADC count of 800 corresponds to 4 amps. With a tick  

; of 0.0001 hr, a current of 4 amps means 0.0004 amp-hours = 400  

; micro-amp-hours have been used during the tick interval. Since  

; this corresponds to an ADC count of 800, dividing the ADC count  

; by 2 gives both the current in centi-amps and the number of  

; micro-amp-hours used during the tick interval (assuming a  

; constant current). These values are accumulated in two 16-bit  

; registers, one for micro-amp-hours and one for milli-amp-hours.  

; The latter is shown on the LCD along with the current in amps.  

;  

; - The voltage sensing circuit that is input to ADC4 is scaled so  

; that an ADC count of 600 corresponds to 12 volts. Thus if the  

; ADC count is divided by 5 the result is in units of decivolts.  

; This value is displayed as volts on the LCD.  

;  

; - The LCD subroutines are unsophisticated, but functional. Since  

; PB7 is used as the RESET pin on the ATtiny26 during serial  

; programming, the E and R/S lines were moved to PORTA, the same  

; port as the ADC pins. However, these digitial lines are not  

; changed during ADC conversions and so should not contribute to  

; ADC analog noise.  

;  

; Andy Palm  

; 2010.02.28  

;  

;----------------------------------------------------------------------

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
