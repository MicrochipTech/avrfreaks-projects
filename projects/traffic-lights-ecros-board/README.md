# Traffic Lights with ECROS Board

Uploaded by AndyPalm on 2010-02-03 20:23:00 (rating 0 out of 5)

## Summary

 asmTrafficLights3.asm - Traffic Light Control Exercise 3


 Hardware:  

 ATtiny2313V with 8.000 MHz crystal (PA0:1) on STK-500 or similar.


 RS-232 driver to connect with PC running Tera Term Pro. This  

 program was developed using the STK-500 and its built-in  

 spare RS-232 port. Later the connection was modified to use two  

 FriendCom FT-221/AP wireless RF modules (no details given here).


 ECROS Technology Traffic Lights project board, modified.  

 See [http://www.ecrostech.com/General...](http://www.ecrostech.com/General/TrafficLights/index.htm) for  

 a description of the ERCOS Technology Traffic Lights board.


 In order to work with PB7 on the ATtiny2313V, the push button  

 pullup and current limiting resistors on the ECROS board were  

 changed as follows:  

 R7, R11, R20, and R21 changed from 3.3K to 10K ohms  

 R12, R13, R22, and R23 changed from 2K to 150 ohms


 AVR pin assignments:  

 PORTD0:1 - USART RX and TX connected to RS-232 driver  

 PORTB0:7 - Connection to ERCOS Technology Traffic Lights  

 project board, as follows:  

 PB0 - E/W green light driver (1 = on)  

 PB1 - E/W yellow light driver  

 PB2 - E/W red light driver  

 PB3 - E/W vehicle sensor push button (0 = pushed)  

 PB4 - N/S green light driver  

 PB5 - N/S yellow light driver  

 PB6 - N/S red light driver  

 PB7 - N/S vehicle sensor push button


 Notes: This version controls a normal light sequence and reports  

 light status and passing vehicles to the serial port with  

 a time stamp. System tick increased to 200 ms (5 ticks per  

 second) to allow time for serial link writes within a single  

 tick. Time is accurate to about 10 seconds per day with  

 the crystal used.


 This program has a simple round-robin structure with the  

 main loop executed once each system tick. The system tick  

 is set to 200 ms to allow time for messages to be sent via  

 the USART. This is not a flexible design, but its simplicity  

 made it fairly easy to write. Increaseing the functionality  

 would probably require the use of ISRs.


 The main loop has two sections, one for functions executed  

 once per tick (vehicle detectors), and the other for  

 functions executed once per second (stop light sequence and  

 clock). 


 The light cycle function is essentially a state machine that  

 uses the cycle\_sec counter to determine light status. The  

 cycle timing can be changed by modifying the appropriate  

 equate statements.


 The vehicle sensor input is debounced using the system tick.  

 A detection flag is set if the sensor is still down after  

 the wait period and the pass report is sent after the sensor  

 is subsequently released. This sensor detection sequence is  

 handled by a state machine with 3 states.


 The text messages to the PC are stored in program memory.  

 The time stamp text and values are stored in sram. There  

 separate functions to send these message to the USART.


 Andy Palm  

 2010.01.30

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
