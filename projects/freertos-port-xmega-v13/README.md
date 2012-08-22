# FreeRTOS port for xmega v1.3

Uploaded by goergev on 2012-08-22 05:32:00 (rating 0 out of 5)

## Summary

==== Version 1.3 == 22-Aug-2012 ====


- based on FreeRTOS 7.2.0  

- project upgraded to AVR Studio 5.1 


no code changes in demo or port specific code


==== Version 1.2 == 24-Dez-2011 ====


The demo code of this version completely runs in the AVR Studio 5 Simulator. Just load the project and press F5. 


The main difference in the demo code (compared to version 1.1) are the run time checks to early detect stack or other memory problems. You may study those very carefully and try to understand the memory layout that FreeRTOS uses. This avoids a lot of the "stack overflow" frustration that commonly occurs when starting with FreeRTOS (and the memory is close to full).


The project settings are for the 32A4. If you want to run it on an 128A1 then just change the device (alt-F7 / device), goto FreeRTOSConfig.h and change config24BITADDRESSING to 1 and recompile.


Changes:  

- based on FreeRTOS 7.0.2  

- project moved to AVR Studio 5.  

- more runtime memory checks  

    - to avoid stack overflows (both global stack and task stacks).  

    - to ensure that there is enough memory to start the idle task.  

    - to ensure that configTOTAL\_HEAP\_SIZE is not too large.  

- the simulated usarts use a datagram with checksum. checked on transmission.  

- better clock initialization to overwrite bootloader settings  

- disabling of all unused hardware modules.  

- disabling of all unused clock sources.  

- pull ups enabled on all unused i/o pins.  

- software reset in case of unexpected errors.  

- uarts disabled in simulation mode to make sure that they don't disturb stability tests.


==== 20-Mar-2011 ====


While testing my application and this port, I just noticed that those ISRs  

calling the api don't nest (as promised). They are executed one after the  

other. Just to warn you. The port is incomplete in this aspect.


======== Version 1.1.0 === 13-Feb-2011 ===


- Runs now with xMega 32A4 and 128A1 (config24BITADDRESSING).  

- Full nesting: The low-,medium- and high-level ISRs may call the light weigth api (and make a context switch, see <http://www.freertos.org> -> API-Reference/Configuration/Customization -> section: configKERNEL\_INTERRUPT\_PRIORITY and configMAX\_SYSCALL\_INTERRUPT\_PRIORITY).


- Simpler, more elegant context switch macros.  

- Use simple context-switch routines if only low-level is used; less overhead.  

- Context switch doesn't store r2 - r17 anymore (configPRESERVE\_R2R17).  

- Based on FreeRTOS 6.1.1  

- Demo code demonstrates low-, medium- and high-level interrupts nesting  

 each other.  

- More stress in the demo code to get more testing depth.  

- Tested on atxmega32A4 (on real hardware) and 128A1 (in the simulator).


=== v1.0 17-Nov-2010 =============


This is a port specifically for the xmega 32A4 CPU. It may run on other xmega cpus, but I did not test this.


When I started with xmega and FreeRTOS, I used the other port that you can find in the project section ("fitting freeRTOS to ATxmega") by hy6r0t3d. I changed this significantly step by step with the aim to never disable the global interrupt flag in the status register.


So this port switches only the low-level interrupt in the pmic on and off and never disables all interrupts. This port also makes sure that the reti instruction is always executed at the end of an interrupt which is absolutely necessary for the xmega cpu (and what is a major difference to the older avr cpus). 


Interrupt service routines that use the low-level interrupt may call the "\_fromISR" type of api calls of FreeRTOS. ISRs using med-level and high-level ISRs are not allowed to do this.


To make this all happen, ISRs have to follow certain conventions. An ISR basically looks like this (see demo code that is included in the download):


ISR(USARTC0\_DRE\_vect, ISR\_NAKED)  

{  

 portSTART\_ISR();


 portEND\_SWITCHING\_ISR( USART\_DataRegEmpty(&USARTC0\_data) );  

}


Mid-level and high-level interrupts do not need to follow the convention and don't need the naked attribute:


ISR(xy\_vect)  

{  

 ... do something  

 Do NOT call FreeRTOS api functions !  

}


Be aware (!) that this port has been used only in a single application up to now. Not all aspects and functions of FreeRTOS are used and tested. Nested interrupts should work, but are not tested up to now. The "official" demo code is not ported and tested. Nevertheless, I will continue using this port in my own project and make improvements if necessary. I hope others will find this port useful and provide feedback.


A few words to the demo code. The code runs completely in the simulator. To test this on real hardware, the "#define SERIAL\_SIMULATION" in all.h has to be removed.  

The structure is as follows:  

- one ISR receives bytes from a serial port and puts them into queue1.  

 In the simulation case this is done by the timer1 ISR.  

- the first task receives the bytes from queue1 and puts them into  

 queue2. As soon as 0x0d is received, a certain number is  

 written into queue3.  

- the second task is waked up by this write , reads the bytes from queue2  

 and sends them to queue4.  

- another ISR takes the bytes from queue4 and sends them out via  

 a serial port.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
