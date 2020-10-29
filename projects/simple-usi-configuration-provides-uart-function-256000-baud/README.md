# Simple USI configuration provides UART function up to 256000 baud

Uploaded by osbornema on 2020-10-29 16:54:00 (rating 0 out of 5)

## Summary

There are many applications in which the USI () is only required to send strings and user prompts to a PC and receive keypresses from the keyboard. The user requires:


 


A straightforward method of deriving the USI clock signal:


           No issues due to interrupt latency


          Simple calculation of the clock period


The ability to use any popular baud rate: typically 115200 and above.


Minimum hardware: No external clock source


Possibly use of the default RC clock


 


The user does not require:


       To have an underlying process that must not be halted while the USI is functioning.


       The requirement to determine and take into account any interrupt latencies.


 


USI hardware provides for all these requirements, both simple and more demanding. Any one of three clocks sources can be selected:


        Timer interrupts which enable the presence of an underlying processes.


        Soft ware interrupts which provide the basic configuration considered here.


        An external clock for the most demanding applications.

## Compilers

- Atmel Studio 7

## Tags

- Complete code with HW
- Atmel Studio 7
