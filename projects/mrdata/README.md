# Mr.Data

Uploaded by dersimon on 2007-05-04 10:27:00 (rating 0 out of 5)

## Summary

Analog data recorder on SD card with full FAT16 support.  

Runs on ATMEGA88/168 @ 8MHz (internal RC clock or precision xtal).


Features:  

- records the 6 or 8 analog inputs of mega88 with 10 bits.  

- configuration of number of channels and recording interval is done with a file "settings.txt" on the SD card.  

- a LED shows operation.  

- a button to start/stop.


Circuit (text description):  

- 1 ATmega88 @ 8MHz (internal or Xtal, set fuses accordingly)  

- 3 100nF (for Vcc and Aref, also the SD card)  

- 3.3V power supply (fits for card and mcu)  

- 1 SD card slot (connected to SPI lines MISO, MOSI, SCK and SS)  

- 1 LED with resistor 220 ohms (on PORTB.0)  

- 1 button (on PIND.3, internal pull-up is used)  

For the 6 or 8 analog inputs (PORTC) and Vref input filters (RC - i.e. 1k and 100nF) as you desire.


MrData writes true text files on a FAT16 file system.


Operation:  

- format SD card with FAT16  

- create "settings.txt" (Obey shown format, letter case, spaces, everything, except the decimal numbers!):  

--file start--  

ch=6  

overflow=7812  

prescaler=13  

--file end--  

ch=number of recording channels  

overflow is the value of OCR1A register and prescaler is for TCCR1B. The adc prescaler is set to 64.  

- insert card in MrData and switch on. MrData should flash the LED  

twice. Permanent flashing means error!  

- connect analog inputs  

- press the button to start the measurement. The LED switches on.  

- press again to stop. MrData creates files using the name format MMxxx.TXT, where xxx stands for a decimal number, counting up from 1.  

- you can remove the card now. Insert into PC and open the log file (i.e. with Excel). MrData writes a tab-formatted text table. The first column is the counting measurement index, followed by the number of channels columns with the decimal values ranged from 0 to 1023 (where 0=0V and 1023=Vref).


Notes:  

- MrData will never overwrite existing files. The number in the name will start from the highest number found on the card.  

- the maximum data rate for the text file should be around 40kByte/sec.  

- the maximum time between measurements is limited by timer1 and is around 7,8 seconds.  

- removing the card or power loss while recording will result in the loss of the current file (since MrData writes the FAT only once, when recording is done).  

- MrData doesn't wear out SD cards. Sectors and FAT are only written once.  

- card full condition is not checked. Is on todo -> should stop measurement.

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
