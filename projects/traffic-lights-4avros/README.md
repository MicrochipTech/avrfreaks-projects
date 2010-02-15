# Traffic Lights with 4AvrOS

Uploaded by AndyPalm on 2010-02-14 23:20:00 (rating 0 out of 5)

## Summary

This is a traffic light intersection simulation using the ECROS Technology Traffic Lights Board or a protoboard with at least six LEDs and three push buttons and an application RS-232 port.


It is my first attempt to use an operating system, namely, 4AvrOS. This is a minimal RTOS that is relatively easy to learn.


There are several traffic light modes that assume that the N/S direction  

is a major road while the E/W direction is a minor road.


 - Daytime with heavy E/W traffic. A standard Green/Yellow/Red sequence  

 for both directions.


 - Daytime with light E/W traffic. The N/S light remains green unless  

 the E/W vehicle sensor (push button) is tripped.


 - Nighttime. N/S flashes yellow and E/W flashes red.


 - Mixed. Daytime mode during day, nighttime mode at night. Controlled  

 by two alarm clock settings.


 - Four-way Stop. All lights flash red.


 - Pedestrians crossing. All lights red. Tripped by pedestrian push  

 button. Overrides all other modes and can't be switched off at  

 control console.


The system is controlled by a dumb terminal (e.g., PC running Tera Term  

Pro) connected through the RS-232 port. When not in command mode,  

the terminal receives status messages on the traffic light mode,  

vehicle sensors, and pedestrian push button with time stamps. The  

commands are described at any time by the Help command h.  

The commands available are:  

 h = Help  

 c = Enter command mode  

 f = Start four-way stop light mode  

 d = Start daylight (normal) light mode  

 n = Start night flashing light mode  

 m = Start mixed (day/night) light mode  

 H = Set day mode for heavy E/W traffic  

 L = Set day mode for light E/W traffic  

 t = Set system clock time  

 g = Set green light on time interval  

 y = Set yellow light on time interval  

 1 = Set mixed mode day-to-night time (Alarm 1)  

 2 = Set mixed mode night-to-day time (Alarm 2)  

 s = Show system status  

 q = Quit command mode


 Hardware:  

 AVRmega168 with 8.000 MHz crystal on STK-500 or similar protoboard.


 RS-232 driver and port connected to PC running Tera Term Pro. 


 ECROS Technology Traffic Lights Board or protoboard. See  

[http://www.ecrostech.com/General...](http://www.ecrostech.com/General/TrafficLights/index.htm)  

 for a description of the ERCOS Technology Traffic Lights board.


 In order to work with the low current source capability on some AVR  

 pins, the push button pullup and current limiting resistors on the  

 ECROS board were changed as follows:  

 R7, R11, R20, and R21 changed from 3.3K to 10K ohms  

 R12, R13, R22, and R23 changed from 2K to 150 ohms


 Alternately, a protoboard such as the STK-500 can be used for the  

 LED lights and pushbuttons. In this case, the output to the LEDs may  

 have to be complemented. The button and LED circuits are the  

 standard ones, so no schematic is provided.


AVR pin assignments:  

 PORTD0:1 - USART RX and TX connected to RS-232 driver  

 PORTD4:7 - Connection to ERCOS Technology Traffic Lights  

 project board, as follows (0 = pushed):  

 PD4 - E/W vehicle sensor push button  

 PD5 - N/S vehicle sensor push button  

 PD6 - Pedestrian push button  

 PORTB0:5 - Connection to ERCOS Technology Traffic Lights  

 project board, as follows (1 = on):  

 PB0 - E/W green light driver  

 PB1 - E/W yellow light driver  

 PB2 - E/W red light driver  

 PB3 - N/S green light driver  

 PB4 - N/S yellow light driver  

 PB5 - N/S red light driver  

 PORTB6:7 - Crystal at 8.000 MHz


Notes:  

 This version uses the 4AvrOS cooperative scheduler. There are ten  

 tasks and five semaphores, as follows:


 Day\_Lights\_Task. Operates normal GYR light sequences. Priority 4,  

 starts OFF.


 Night\_Lights\_Task. Operates night mode flashing lights. Priority  

 4, starts OFF.


 Mixed\_Lights\_Task. Operates switch between day and night light  

 modes using two clock alarm settings. Priority 3, starts OFF.


 Four\_Way\_Stop\_Task. Operates four-way stop light mode. Priority  

 4, starts ON. (If power cycles, this is the mode lights go to.)


 Ped\_Cross\_Task. Operates pedestrian crossing light mode. Priority  

 3, starts OFF.


 System\_Clock\_Task. Operates system clock (separate from 4AvrOS  

 system tick). Priority 0, starts ON. Linked to ISR.


 Send\_Status\_Msg\_Task. Sends status messages with time stamp when  

 not in command mode. Priority 7, starts ON.


 Command\_Task. Manages terminal interface for command inputs and  

 command messages. Priority 5, starts OFF. Linked to ISR.


 EW\_Veh\_Sensor\_Task. Debounces E/W vehicle sensor on both down  

 and up stroke, sends status messages on vehicles. Priority 2,  

 starts OFF. Linked to ISR and Day\_Lights\_Task.


 NS\_Veh\_Sensor\_Task. As above, but for N/S direction, except not  

 linked to Day\_Lights\_Task.


 Ped\_Button\_Task. As above, but for pedestrian push button.  

 Triggers Ped\_Cross\_Task.


 semaSMAQueue - Protects shared status message address queue.  

 semaTime - Protects shared clock time variables.  

 semaTX - Protects shared USART transmitter.  

 semaAlarmTime - Protects shared alarm time variables.  

 semaModeSwitch - Controls when a light mode task can be disabled  

 and is also used by Ped\_Cross\_Task to suspend  

 and restore operation of current light task.


 Andrew Palm  

 2010.02.12

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
