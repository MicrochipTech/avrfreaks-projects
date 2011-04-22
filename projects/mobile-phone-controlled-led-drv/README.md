# Mobile Phone Controlled Led Drv

Uploaded by drmpftemp on 2011-04-22 07:11:00 (rating 5 out of 5)

## Summary

This project has been superceded by my POD devices tutorial, [http://www.forward.com.au/PODdev...](http://www.forward.com.au/PODdevices/PhoneControlledPODDevices.html) and on AVR Freaks, which provides a mobile phone java application that allows your mobile to control any simple micro-device that implements the POD prototcol (a PODdevice)


POD or Protocol for Operations Discovery, allows the same mobile phone application to connect to different PODdevices with different capabilities. The PODdevice then determines the navigation and menus presented to the user and what commands the user can send to the PODdevice.


---------------------  

This tutorial extends the Bluetooth Controlled Led Driver to a Mobile Phone Controlled Led Driver. 


(For a general purpose extension to this project see my POD device tutorial )


There are three parts to the Mobile Phone Controlled Led Driver tutorial,


Connecting your Mobile Phone to Bluetooth Controlled Led Driver


Replacing the SparkFun BlueSMiRF\_Gold Bluetooth Module with a less expensive one (this step is optional) (Not done yet)


Enhancing the code to provide a more user friendly interface. 


Outline of the Tutorial


Introduction - Why add Mobile Phone Control  

Choosing a Java enabled phone  

Checking your existing phone  

Loading Java Programs to your Phone  

Running the BTTEST Java Program on your Phone  

Connecting your Mobile Phone to the Bluetooth Controlled Led Driver  

Finding the MAC address of your Bluetooth Module  

Starting Bttest  

Sending Commands to the Led Driver  

Exiting the Terminal Program


The User Interface  

Note on NewLine format  

RemoteLedDrive Code Features  

User Help Message  

LOAD\_CHARS\_FROM Macro  

Compiler Generated Macro  

Level and Percent Light Display  

A Binary to ASCII formatter (CONVERT\_13BITS)  

Logarithmic Levels  

LOAD\_SETPOINT\_FROM\_LEVEL  

I Am Alive Message


The first and third parts of this tutorial is available from  

[http://www.forward.com.au/Mobile...](http://www.forward.com.au/MobilePhoneControlledLedDriver/MobilePhoneControlledLedDriver.html)

## Compilers

- AVR Studio 4

## Tags

- Complete code with HW
- AVR Studio 4
