# Mouse Mover

Uploaded by rgroome on 2021-07-29 22:30:00 (rating 0 out of 5)

## Summary

MouseMover application for Pro Micro, Leonardo or Arduino Micro (or other modules using AVR **ATMega32U4**)


 


This controller moves the mouse cursor a tiny amount every few minutes, simulating the user being at the computer and using it to prevent the host PC from going to sleep or locking the screen. Unlike software only solutions that many IT infrastructure systems know how to ignore, this solution looks as though a real (second) mouse is added to the computer. The normal mouse is still used for normal operation. Usually moving the mouse cursor a few pixels every minute or so is enough to keep the computer from going to sleep or locking. Because the amount moved is small and it only is done every minute or so, this will usually not interfere with normal PC operation. To maintain some semblance of system security (that IT groups want) this added mouse controller can be programmed to only run for a limited time, after which it stops moving the mouse, until commanded to run again.


 


This controller has a number of user changeable parameters. All of the parameters for the controller can be modified by sending commands to the controller’s serial port, which is also added to the host PC when the controller is plugged in. This controller will appear to Windows as a mouse device (HID) and a serial com port. Once the controller is programmed, it will have default parameters selected for all of the changeable parameters, but these can be changed and then saved to EEPROM so that the user selected parameters are used each time the controller is connected to the PC.


 


This firmware is designed for use with a ProMicro Arduino board (that uses an ATMEGA32U4) and is connected to the PC with it's USB port. It could probably be adapted to other Arduino controllers like the Arduino Leonardo, but the Pro Micro board is tiny, powered by the USB port and low cost, making it the ideal hardware solution for this task. The only hardware required for this solution is an Arduino Pro Micro controller and a USB cable that connects this controller to the PC.


 


There are a few jumpers that can be used with this firmware. Usually the jumpers are not used, but are provided for, in case the user prefers to control this device in this way.


 


One jumper called JPE (defined in the code) can be used to disable the controller. By grounding this pin, the mouse functionality will stop (as though you unplugged the mouse) and will be restored when the jumper is removed. This jumper is on Arduino pin 6 [PD7].


 


A second jumper, JP2 (defined in the code), if grounded, will disable the software run timer and keep the mouse "running" all the time, no matter what the programmed run time is. Remove the jumper to restore normal operation. This jumper is on Arduino pin 2 [PD1].


 


A third jumper, JP3 (defined in the code), if grounded, will cause the mouse to move every 5 seconds, instead of the configured move time. Remove the jumper to restore normal operation. This jumper is on Arduino pin 5 [PC6].


 


An LED and resistor can be added between pin A3 (Arduino Digital pin 21 [PF4]) and VCC to indicate when the MouseMover is running (Pin is labeled "A3" on the Pro Micro). While not required, it is handy to have this indicator so that you know if the MouseMover is running or not (on or off). The LED can be any LED you might have. The resistor shown is a 330 ohm 1/4W part, but any resistance from about 100 ohms to 1K would be usable, depending on the LED used. There are also LED’s available with a built in resistor, such as Lumex SSL-LX3044GD-5V (Digikey 67-1062-ND) that are designed to run directly from the 5V supplied by the module. Using this type of LED simplifies connection to the module – Just solder the two leads of the LED to the two pins indicated. For any LED, be sure to observe the polarity of the LED – The cathode end goes to pin A3 and the anode goes to the resistor and VCC.


 


The controller accepts serial commands to perform various functions and report current settings. Open com port with any baud rate (other than 1200 baud) (N,8,1) using a program like Hyperterm or Teraterm (or your favorite terminal program). You can also create batch files to send commands to the com port.


 


Commands are a single letter (to read) or a single letter followed by a number (to set value) and a or as in X or X20 (can also be X=20)




| Read | Set | Description |
| --- | --- | --- |
| V |  | Return a version / copyright string. |
| *IDN? |  | Return a version / copyright string (This command is for compatibility with SCPI devices). |
| X | X4 | Amount to move mouse in x direction (0..100) (default 4) |
| Y | Y0 | Amount to move mouse in y direction (0..100) (default 0) |
|  |  | (Note: For X and Y values, the mouse will move plus this many pixels on the first move and then move minus this many pixels on the next move and then alternate between + and - moves.) |
| T | T60 | How often to move mouse (seconds) (2..3600) (default 60=1 min) |
| R | R540 | How long to run the mouse mover (minutes) (-1..64800)(45 days max) (0=Turn off, -1=run forever) (default 540=9 hrs) |
| Z |  | Return all variables in a comma separated list. X,Y,[set T],[set R], [current T],[current R], days HH:MM:SS (days HH:MM:SS is total run time) |
| E | E0 | Reinitialize current variables to default values (shown above) |
|  | E1 | Get current variables from EEPROM |
|  | E2 | Save current variables to EEPROM |
| ? |  | Return a help string of commands. |
| H |  | Return a help string of commands. |


You can create a batch file to send a command to this device as follows:



```

@echo off
rem MoverPort is comport address in format \\.\COM# (set to your comport#)
set MoverPort=\\.\COM4
rem R=<# minutes to run> (9 hours=540) (0=off)(%1=get from cmd line)
echo R=%1 >%MoverPort%

```
Then call the batch file with a parameter as in: MyBatchFile 540 (This will set Runtime to 540 minutes)


You can then create a desktop shortcut that can call this batch file with a parameter by setting the "target" of the shortcut to “MyBatchFile.bat 540" (either set the "Start in" field to the directory where the batch file is or add the path to the target batchfile name) Then when the icon is double clicked the shortcut will turn on the controller for another 9 hours. (user would do this daily)


 


The user could also create a scheduled task to call this batch file every weekday (or whatever interval is desired) or every time the user logs on.


This firmware has been released to the public domain using the license included with this file set. The only request I ask is that the version string continues to contain my name. You are free to use it as you see fit. No warrantee or fitness for any purpose is implied.


 


********************* DISCLAIMER ***********************


This firmware and it's associated device can be used to override corporate IT department directives and security mechanisms in place (by keeping the system from going back to a lock screen). This may not be allowed in a corporate or government environment and may result in mild to severe penalties. If in doubt, check with your IT department before using this device. Use at your own risk.


********************* DISCLAIMER ***********************


 


The project also includes a documentation file that provides more details on the use of this program, including how to install the device driver and setting up a task.

## Links

- [Mouse Mover](https://github.com/Rick-G1/MouseMover)

## Compilers

- Arduino IDE

## Tags

- Part-specific Lib.functions
- Arduino IDE
- Arduino
- Arduino Pro Micro
- Arduino Leonardo
- AVR
- Mouse Mover
- Office automation
- Mouse controller
