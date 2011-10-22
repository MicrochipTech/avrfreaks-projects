# RS232 module, tutorial, Attiny84

Uploaded by drmpftemp on 2011-10-21 22:04:00 (rating 0 out of 5)

## Summary

This is the first part of a larger tutorial, a Bluetooth controlled Led driver.  

The tutorial is available at  

[http://www.forward.com.au/Blueto...](http://www.forward.com.au/BluetoothLedDriver/BluetoothControlledLedDriver.html)


This RS232 module part of the tutorial includes 


Building the RS232 circuit  

 RS232 Circuit Description  

 RS232 Circuit Construction  

 RS232 Connection to the PC Terminal Software  

What is RS232  

 RS232 Hardware Control Signals  

Calibrating the uC clock  

 An Alternative to Calibration  

How to Code uC Interrupt Driven Programs  

 How to Enable an Interrupt  

 How to Avoid Interrupt Data Corruption  

 1) How to prevent interrupt routines from un-intentionally changing the values  

 1) a) Preserving and restoring the SREG register.  

 1) b) Saving and Restoring General Purpose Registers  

 1) c) I/O and Control registers  

 1) d) Reserving specific SRAM locations  

 2) How the main program can safely access Registers and SRAM updated by an Interrupt routine  

 2) a) Disable the interrupt inside the interrupt routine  

 2) b) Save the data to SRAM, then copy to a different location  

 2) c) Make sure your main program has enough time to process the data before the next interrupt occurs  

 2) d) Do all the processing inside the interrupt routine itself  

 2) e) Use 'guard' flags to control access to registers and SRAM locations


Coding the RS232 module  

 Limitations of this RS232 Module  

 Testing the RS232 Module  

 Loading the RS232 Module  

 Communicating with your uC

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
