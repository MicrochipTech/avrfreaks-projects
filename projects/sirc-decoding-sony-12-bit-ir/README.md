# SIRC Decoding - Sony 12-bit IR

Uploaded by boseji on 2008-07-01 04:06:00 (rating 0 out of 5)

## Summary

29-Jun-08:  

----------  

I have made a new faster and lower resource consuming algorithm.  

This is call the SIRC\_FAST Algorithm. Have a look at the Flow Chart and the Theory Description in the C Code.  

========================================================================  

31-Dec-07:


HAPPY NEW YEAR to ALL Avr-Freaks


Just added a description of the SIRC and some microcontroller related calculations. I have the excel sheet of the same, let me know if any one requires it.


11-Nov-2007:  

This project aims to be the basic foundation for developing code for decoding the IR remote protocol used for the Sony Remotes.  

The protocol details are available at the following site:  

[http://www.sbprojects.com/knowle...](http://www.sbprojects.com/knowledge/ir/sirc.htm)


I am using a remote which has 12 bit coding. I have enclosed an analysis of the protocol in the archives.  

The Description of my HW used is as follows:


1. ATmega16L - This is the controller that I have targeted but you can use any other small controller with ICP capability.


2. XTAL - 7.3MHz This is the clock that I use. It is very common for AVR project and I am sure that if I am able to find it in INDIA you can find it too. However you can use a 8MHz XTAL also bit you require to adjust the Serial BAUD frequency.


3. IR sensor TSOP1738 from Vishay.


4. Sony Remote - VCD RMT-M23A/RM-969 (Specific since I used this)  

The Schematics is enclosed in the Archive.


The circuit will send out the Time stamps that  

are taken when the key is getting decoded.  

The serial port runs at 115.2Kbps.  

Also the decoding for the Protocol can be done as follows:  

For the Key POWER we get the following codes:  

120C090D090D0809090D0D0908  

120D080D090D0908090D0D0908  

110C090D090D0808090D0D0809


Here the code is repeated 3 times with a single key press.  

12/11 -> This is the Start Bit time  

08-0A -> This is to indicate the Logic 0 Duration  

0B-0E -> This is to indicate the Logic 1 Duration  

Thus the above Becomes  

101010001100  

101010001100  

101010001100


Out of which the last 5 bits are the address -> 01100 ->0x0C  

And the rest part is Data for the code of POWER Key  

 -> 1010100 => 0x54


This is how the o/p will be.  

If there is any noise also it will sent to o/p but the ICP loops wont loose sync.


Here is all that I have located in this Post:  

[https://www.avrfreaks.net/index.p...](https://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=294159)


AVR Rulez !!!!


Please do let me know about your comments at [boseji@yahoo.com](mailto:boseji@yahoo.com)

## Links

- [Decription of the SIRC protocol](http://prog.ic.googlepages.com/IRAnalysis.pdf)
- [SIRC-FAST Code](http://prog.ic.googlepages.com/SIRC_Fast.c)
- [SIRC-FAST Logic Flow Chart for ISR](http://prog.ic.googlepages.com/SIRC_Fast_Logic.jpg)

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code with HW
- WinAVR (GNU GCC)
