# AVR Data Recorder

Uploaded by ksvitale on 2013-06-17 12:00:00 (rating 0 out of 5)

## Summary

The AVR Data Recorder is a versatile programmable device that can be used to capture and store analog data for retrieval at a later date. 


The programmability of this device is due to the AttoBASIC V2.20 interpreter with specific language extensions for use with the AVR Data Recorder. The AttoBASIC source code and build-specific HEX files are available from a separate source ([https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?module=Freaks%20Academy&func=viewItem&item_type=project&item_id=3621)). 


The interactive ability of AttoBASIC makes it easy to set up gain/attenutation levels and print measured ADC values when writing and debugging data capture and record programs. 


Features:  

â€¢ Processing core is an ATMEL ATmega328p running at 8MHz.  

â€¢ Versatile; can be used as a stand-alone data capture and recording device or connected to a USB serial console for real-time display and capture of data.  

â€¢ Portable; Internal Li-Ion rechargeable battery runs up to 14 days on a single charge.  

â€¢ Two (2) channels of analog input;  

> 8-bit analog resolution (9.8 mV / bit @ 1x gain, post-attenuator).  

> Each channel is bipolar (+/-) with respect to ground, reading is positive only.  

> Software selectable input attenuator for a range of 2.5volts, 25 volts and 250 volts.  

> Software selectable input amplifier extends sensitivity to 250 millivolts per channel.  

â€¢ One (1) channel of True-RMS reading, software selectable between either analog channel (post-attenuator).  

â€¢ Current sensing input with a range of 0 to 250ma, 0.5Î© series resistance, externally extensible.  

â€¢ Analog comparator input;  

> Comparator source is software selectable between dedicated input with fixed 0 to 25 volt input range or either analog channel (post-attenuator).  

> 8-bit programmable reference.  

â€¢ Digital output for control of external devices;  

> Open-drain "low-side" N-MOSFET solid-state switch with 175 mÎ© "on" resistance can switch up to 60 volt loads with 1.5 amps of continuous current.  

> DDS (direct digital synthesis) for toggling at a specific frequency (1 Hz resolution, 1 Hz minimum frequency).  

â€¢ 64K bytes of internal data storage capability in non-volatile memory.  

â€¢ Preprogrammed "out of the box" to record a 24-bit time stamp and both analog channels every 60 seconds for data recording up to 218 hours.  

â€¢ "Data" LED indicates data file access.  

â€¢ Programmable;  

> Uses AttoBASIC programming language, a Tiny-BASIC with an enhanced command set.  

> Program can be saved in internal non-volatile memory for execution.  

> Switch-selectable "self-start" feature enables immediate user program execution upon power-up.  

> Programmability creates a stand-alone intelligent capture device with decision-making and data manipulation capabilities.  

â€¢ AttoBASIC features;  

> User programmable using BASIC-like command syntax.  

> 8-bit data handling with 26 variables.  

> Arithmetic, logical and relational operators allow manipulation of data before storage.  

> 32-bit Real-time counter for time-stamping of data in resolutions of 1 ms, 10 ms and 100 mS.  

> Extensions to language for direct control of the internal hardware, including the on-chip analog-to-digital converter, gain settings for each analog channel, analog comparator, analog comparator's DAC reference generator, MOSFET digital output and the internal non-volatile "data file" storage memory.  

> Direct data file access allows data dump in ASCII text CSV format for importation into EXCELâ„¢ and other data-processing applications. Number of data fields per record is user programmable from 1 to 255 fields.  

â€¢ USB interface via Virtual Communications Port supported by all major operating systems (WINDOWSâ„¢, MAC OS-Xâ„¢, and Linux).  

â€¢ Internal boot-loader allows AttoBASIC updates or user-custom application software to be uploaded to the AVR's internal FLASH memory.


The AttoBASIC source code and pre-assembled HEX files can be found on the AttoBASIC home page at [http://cappels.org/dproj/AttoBas...](http://cappels.org/dproj/AttoBasic_Home/AttoBasic_Home.html) or the AttoBASIC project page at [https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?module=Freaks%20Academy&func=viewItem&item_type=project&item_id=3621).


I used components from my "internal stock", which I have been acquiring over the last few decades. Due to the numerous components involved in this project, both SMT and thru-hole, I designed the PCB for a single plastic case (I had only one) with front and back panels suitable for connections. I ordered five (5) PCB's from PCBfabexpress.com. The attached photos are the finished unit installed in the case.


If anyone is interested, I have three (3) bare PCB's available for US$20 (+shipping).


Peace,  

Scott

## Compilers

- AVR Assembler

## Tags

- Complete code with HW
- AVR Assembler
