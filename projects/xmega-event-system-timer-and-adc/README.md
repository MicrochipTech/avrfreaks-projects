# Xmega EVENT System With timer and ADC

Uploaded by TintuVayalattu on 2017-10-07 08:49:00 (rating 5 out of 5)

## Summary

Hi all,


This is my first Project post in AVR Freaks. Correct me if i'm doing something wrong.


My intention was to familiarize with Xmega EVENT system. I tried the EVENT system with Timer overflow as the trigger and ADC read as the action. Here i'm trying to describe the same for others who are interested in it.


Details:- 


* Target - Xmega128A1U xplained pro board
* Tools used - Atmel Start, Atmel Studio 7, Data Visualizer.
* Aim - Create an ADC read event using Timer overflow.

Project Steps:-


**Atmel Start Configurations**


* Create an Atmel Start project with xmega128A1U as the target MCU.
* Add EVENT System init driver
	+ Under **CHANNEL0 setting** Select CH0MUX  as "*Timer/Counter C0 Overflow*"
	+ Leave all others as it is.
* Add ADC init driver
	+ Under **BASIC CONFIGURATION**--> Select ADCA instance and Enable the module.
	+ Change REFSEL to "*External reference on PORT A*". External voltage should be connected to PA0 pin as reference.
	+ Under **ADC CHANNEL 0 CONFIGURATION** --> change INPUTMODE to "*Single-ended input, no gain*".
	+ Change MUXPOS to input pin 1. Select PA1 as ADC1 under **Signals** (Now analog input is PA1)
	+ Under **CHANNEL 0 INTERRUPT CONFIGURATION -->**Select INTMODE as "*Interrupt on conversion complete"*and select INTLVL to "*Low level*". (**We need to enable global interrupt and Low level interrupts under PMIC setting for using the interrupt**)
	+ Under **EVENT CONFIGURATION -->**EVSEL as "*Event Channel 0,1,2,3*" and EVACT as "*First event triggers channel 0*".
* Add Timer init driver
	+ Under **BASIC CONFIGURATION -->** CLKSEL as "*System Clock / 64*" and PER as "*32768*" to make ~1 sec time out with default clock setting.
* Add USART basic driver for printf support.
	+ Select USARTE0 instance because PE2 and PE3 are the virtual com port pins for EDBG interface.
	+ Select printf support.
	+ Baudrate - 9600, Stop bit - 1, Parity - None. Character size - 8 Bits.
* Export project to Atmel Studio.

**In Atmel Studio**


Now we have a code for triggering the ADC read function using Timer overflow as Event source. ADC conversion complete ISR will be invoked by this ADC read.


* In driver\_isr.c, modify ADC ISR with printf message to view the result.
* Connect PC with the EDBG USB port in the board.
* Wait for the detection.
* Select EDBG-PDI in debugger section in tools under Project-->Properties.
* Build-->continue (F5). ( It will build the project and the resulting binary will be flashed to the MCU and will start the debugging session as well )

**Setup Data Visualizer**


* Open Data Visualizer under tools.
* Open serial port under configuration-->External Connection-->Serial Port (Configuration is placed left side of the window)
* Click the Serial Port Number in Serial Port Control panel and select EDBG virtual com port.
* Set the baudrate ,stop bit ,parity etc similar to MCU setting.
* Click Connect. If everything is correct, you can see the result printing there.

 


Attachments:-


1. Atmel Start Configuration.
2. Atmel Studio Working Project.
3. Screenshots.

Regards,


Tintu Thomas.

## Compilers

- Atmel Studio 7

## Tags

- Complete code
- Atmel Studio 7
- #ATxmega128A1U
- events; event handlers; tc
