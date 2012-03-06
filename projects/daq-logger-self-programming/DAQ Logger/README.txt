


		DAQ Logger using AVR with self-programming   
		----------------------------------------------

///////////////////////////////
Compiled using AVR-GCC (WinAVR)
//////////////////////////////

	
	Data aquisition and logging of a number of analog sources can be done using this project. Here
two quantities are logged: Light intensity and temperature. LDR is used for light measurement, LM35 
centigrade temperature sensor is used for temperature measurement.

	Logging is done at interval of 1 minute (can be changed). ADC of the AVR device take samples at 5 second
intervals and average of the 12 samples is stored to a temporary buffer(=Page size of AVR) in SRAM.
When the buffer is full, it is written to a flash page using self programming function.
  
	Time keeping is done by asynchronous RTC timer of AVR with a 32768Hz watch crystal. This helps
to reduce power consumption since the AVR can be set to sleep mode when there is no CPU activity.
AvR is woken up at every second (Timer interrupt) and performs necessary operations (time keeping, ADC, or
page write).


USAGE
-----

 	At reset, the date and time should be set using two push buttons with LED feedback. 
	Month,date,hour and minute has to be set in the order. 
	
	For setting:
	
	Press SW1 desired number of times(LED2 will blink each time)
	Press SW2 to set (LED2 will blink that many times, to verify the entry)

	1) set month: Reset value = 01(Jan) (01-12)
	2) set date: Reset value = 01 (00 - 31)
	3) set hour: Reset value = 00 (00 - 23)
	4) set minute: Reset value = 00 (00 - 59)
	
	After the minute has been set, the device starts taking reading. LED2 gives indication at every
	minute. The device can be placed at a remote location and be kept there for days (until the flash
	memory runs out)
	
	Flash memory read into PC can be converted to a text report file and can be plotted as graph
	using the program provided in directory 'eVeryDay' (EVERYDAY.exe)
	
	
Hardware and components
-----------------------

	The entire board can be built into a single line PCB.( see pictues )
	I have used 3 AA-size batteries (4.5v) to provide the power. It can last a long time and 
	remember we use power down sleep mode!
	
	Components: (See the diagram provided)
		
	AVR Atmega16
	Crystal: 	8 MHz, 32.768 kHz
	LED	:	LED1,LED2
	Push button: 	SW1,SW2,Reset button
	LDR
	LM35 cetigrade temperature sensor
	On-off switch
	capacitors:	22pF x 2
			0.1uF
	Resistors:	1k x 2
			10k
			100k
	Battery  : 	4.5v minimum
	
	
Improvements
------------

- Time is lost every time the device resets.
 
