
   YAMAHA TQ5/TX81-Z  'TwoPot' Program Operation Overview
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

   This is a bare-bones Real-Time controller for the Yamaha TQ5 4-operator FM tone-module audio synthesizer.
It also works also with the popular Yamaha TX81-Z 4-op FM tone module.

-- There are a lot of features that can be added to make this program better.  But this beta code version 
works and I want to get it in circulation now.  Please send your suggestions and improvements to me at:
   alan_probandt@yahoo.com


   The main code checks repeatedly for a keypress from the PS2 keyboard.  When a key is
pressed, the code checks if it is a keypad digit, an arrow key, or a key from the main
area.  The main keys are arranged like two halves of a piano; the ~12345 row is the black keys
(sharps and flats) of the QWERTY row of white keys.  The 'Z' key is middle C.  Pressing a
key sends Note ON to the synth, releasing the key turns the note off.  Up to eight keys
be sounded at once, but due to the PS2 internal wiring, some key combinations don't work.

   The 'up' arrow shifts the keyboard notes down one octave and the 'down' key shifts the notes
played up one octave. The left and right keys change the voice number. Changing the voice
causes all the parameter values for that voice to be loaded into the processor.  The top line
of the LCD shows the new voice name and number.  The left and right keys roll over: pressing
left key at voice #0 goes to #127, pressing right at #127 goes to #0.  Pressing two keypad
digits changes the voice number.  From voice #23, pressing keypad '5' selects voice #25.
Then pressing '8' loads voice #58.

   Pressing a function key (1 to 8) selects a new algorythm for the voice. The display on the top right
changes to show the new algorythm selection.

   The Caps-Lock key toggles the Sustain feature of the synth.  When Sustain is ON the voice is not
turned off when its key is released. The entire voice envelope plays.  After pressing eight notes, the 
first note turns off.  This feature is great for developing sound environments using voices that have 
long AttackRate times and operators set to non-harmonic frequencies.

   Every 1/10 of a second, the main code interrupts and does a scan of two variable resistors
(potentiometer knobs or sliders).  The new value for each 'pot' are compared to the value
from the last set of conversions. If they are the same, then the interrupt exits and nothing
happens.  If the user moves the Parameter slider pot, then the code puts a new text string on the LCD,
The parameter's current value gets displayed.  This is the value of the parameter that was loaded 
into the TwoPot when the voice was loaded from the synth.  

   The Parameter slider is very sensitive -  a second variable resistor of much lower value can be added 
in series to make it easier to select params that are close together.  For example, a 5K ohm pot placed
in series with a 50,000 ohm pot.  The large value pot selects the operator range and the smaller
pot selects the individual parameter.

              voicename  vx#   algorythm                    voicename  vx#   algorythm
           --------------------------                 F   --------------------------   
    LCD   :   GrandPiano  32    2    :                R  :   GrandPiano  32    2    :  
    16x2  :   AttackRt_1  31   10    :                E  :   Frq_4 2.00   8    0    :  
           --------------------------                 Q   --------------------------   
          param name  init_val  adj_val                  freq_opr  ratio Coarse Fine 

   The LCD displays the voice name on the top line left, the voice number on top center, and the algorythm
number on top right.  The current selected parameter is on second line left, the original value for this 
param in the center, and the current adjusted value that was last sent to the synth on the right.  In
frequency-param mode, the Value slider has 256 steps.  So it is more sensitive than the other modes that have
a maximum of 128 steps for the Value slider.

   The display is a little different when working with a frequency parameter.  Now the second line shows
the operator number on the second line left, the current frequency ratio value in the left center, and the 
current coarse and fine setting numbers on the right.  The ratio string and settings display changes as
the Value slider potentiometer is moved.   When you change a parameter, the displayed values go back to the
original display settings even though the synth has the adjusted values.  I should probably change this to
have the display always reflect the values last sent to the synth.

   Changing the Value pot selects a new setting for the parameter.  This pot value is adjusted 
according to the maximum value of the selected parameter.  For example, a parameter that has only 
three settings (like Amp Mod Sensitivity) needs to have the knob turned (or slider pot moved) further
than a parameter with 32 settings (like Attack Rate) to effect any change.

   All the parameters used by the Yamaha 4-op series of modules are listed.  Some parameters are 
not used by a particular module.  For example, the PEG (programmable envelope generator rate and level)
settings are used only by the DX-11.  Since the settings are in the VCED sys-ex structure, it's easier
to just leave them into the parameter list than it is to write code to nullify them.  Changes sent to 
unused parameters are ignored by the tone module.  

   The parameters are listed in the order that they are found in the sys-ex VCED structure.  After
the VCED parameters come the ACED parameters like Oscillator Waveform and Fine Frequency.  If you are
using a TQ5 tone module, the effects parameters are at the very end of the list (the top of the slider pot).

   Only the single voice settings are changed, the multi voice settings are not implemented.  Nor are
TX81-Z effects or microscale tuning tables.  All these features can be set by pressing front panel buttons.
Switching Memory-Protect to OFF is done only for the TQ5 model.  For some unknown reason, the Yamaha 
designers used different sys-ex strings for each model to turn the Memory-Protect feature off.    

   If the LCD screen shows only the version ID when powered-on, the MIDI-in and MIDI-out cables are not
both connected to the synth, or the synth is not set to send/receive on MIDI channel 1. 
  
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 
   There are three versions of the program included in the ZIP file.  They are for different AVR devices.
The Mega48 version doesn't have the display of the frequency ratio.  The Coarse and Fine frequency parameters are
handled exactly like every other parameter.  The Mega8 and Mega88 versions show the operator's frequency as a 
ratio string, such as "1.00" or "5.76".  Changing the Value slider pot for a frequency changes to the next frequency
ratio in order.  This allows precise adjustments at the expense of having the slider be very sensitive.

   The frequency ratio display feature isn't available for the Mega48 because there isn't enough memory in 
that device for all the tables needed by the code.  The TX81-Z/TQ5 doesn't store the frequencies in any order, nor 
is there any formula to get the ratio from the coarse and fine parameter values.  There has to be extensive linked-lists 
or tables in the code.  This feature is worth the extra expense for the Mega88 IC because selecting non-harmonic FM 
frequency ratios lets you get the great metal/industrial-wasteland sounds not possible on any other type of synthesizer. 
Very few of the presets on any of the Yamaha FM synths use non-harmonic FM ratios.


~~~~~~~~~~~~~~   Hardware design notes ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                                                     |  Basic parts list                             DigiKey #
 Atmel AVR Mega88  microcontroller   8K Flash        |                                               
                       _______                       |  2   DIN5 (180 degree) MIDI jacks             DIN-5500-5S
 +5V--10K----<  /reset |1  28|1  ADC5 SCL            |  1   MiniDIN6 jack for PS2 keyboard           CP-2460-ND
     MIDI in   RxD PD0 |2  27|2  ADC4 SDA            |  1   MIDI opto-isolator (4n24..PC817)         
     MIDI out  TxD PD1 |3  26|3  ADC3  Param pot     |  1   Power jack (2.1mm pin=ground)            PJ-102A
    PS2 kbd clock INT0 |4  25|4  ADC2  Value pot     |  1   LM78L05 +5 volt regulator IC             LM78L05ACZFS-ND
    PS2 kbd data   PD3 |5  24|5  ADC1                |  1   Atmel AVR Mega88 microcontroller         ATMEGA88P-20PU-ND
                   PD4 |6  23|6  ADC0                |  1   16x2 LCD display (standard HD44780 style)
          +5V <-   VCC |7  22|7  GND                 |  2   Slide potentiometers  (10K-100K ohm is OK)
                   GND |8  21|8  AREF  -> +5V        |  1   small 'tach' switch for reset                     
                   PB6 |9  20|9  AVCC  -> +5V        |  2   220 - 360 ohm resistors (1/4 watt) for MIDI 
                   PB7 |10 19|10 PB5 LCD data7       |  1   470 to 1K ohm resistor for MIDI-In opto-isolator
                   PD5 |11 18|11 PB4 LCD data6       |  1   Power adapter +9 (to +15 volts) DC ~ 50 milliAmps 
                   PD6 |12 17|12 PB3 LCD data5       |      - runs processor, LCD display, and PS2 keyboard
                   PD7 |13 16|13 PB2 LCD data4       |  
         LCD_RS    PB0 |14 15|14 PB1 LCD_E           |  see file "Two_Pot_TX81.gif" for schematic drawing
                       -------                                      

   Other AVR devices work well also.  Adjust the interrupt vector table, the .org addresses in the SRAM init section,
the peripheral (USART, timer, etc...) names, and the doADC code for your selected device.  Other AVRs should have USART, 
ADC, and at least eight I/O port pins.  The AVRs that don't have USARTs can run with software UART code.  Check the Projects
section of the the www.AVRfreaks.org website for examples (and my other MIDI projects).

   Almost any opto-isolator will work.  I've built MIDI interfaces with the 4N25, PS817, H1L11, 6N138 and other
devices.  The only requirement is that the opto-isolator be able to handle the 31250 MIDI baud rate. The resistor that 
pulls-up the output voltage to +5 should be selected to make a sharp falling edge between +5 volts and 0.5V.  
Different opto-isolators will need different values, but they are usually between 390 and 1500 ohms.  See MIDI hardware 
and opto-isolator tutorials on the web for more info.

   You can assemble this code with the free assembler available from the Atmel website: AVRASM2.exe  
From the PC command line, use:  C:\AVR\avrasm2 TP_Mega88.asm -fI -l TP_Mega88.lst -o TP_Mega88.hex

   Load the code into the AVR device using an Atmel STK500 dev system or AVR's inexpensive ISP (in-system programmer). 
There are many DIY AVR programmers available on the web.  My favorite is the free, high-quality programmer "SP12.exe" that 
is available at: www.xs4all.nl/~sbolt/e-spider_prog.html.  Here are some examples of using SP12 with the PC command-line:

    C:\AVR\SP12 -i <enter>    { identify the AVR IC and verify communication}
    C:\AVR\SP12 -wpf TP_Mega88.hex  <enter>  { load the assembled code into the AVR device}
    C:\AVR\SP12 -rpfh WhatsThis.txt <enter>  { Read the AVR IC. Put the code into a text file resembling a binary dump}
  
       
   The AVR devices come from the factory with internal fuses used to configure the device. The fuses need to be set 
differently from the factory default for this program to work correctly. AVR device programmers (such as the STK500 or 
SP12) allow you to change these fuses as needed.  The primary change is adjusting the AVR system clock. AVRs are set 
to run at a 1_MHz system clock and this needs to be changed to 8_MHz for the MIDI timing to be correct.
    
   Each device has several banks of fuses.  The fuses that need changing are in the low bank. Take care when changing 
these fuses as it is easy to put the AVR device into a mode that requires a high-voltage programmer like the STK500 or 
the AVR-Dragon to re-configure it.  For example, the fuse settings can be changed to require an external system clock 
signal instead of the AVR's internal clock.  If the device is set into external clock mode, then it needs a 500KHz 
square-wave signal applied to the XTAL pin to return it to internal-clock mode.  The SP12 programmer has some safeguards 
against setting the AVR into a mode where it can't be easily reprogrammed.
	
Mega48/88 fuses: extended: unchanged 
		 high: unchanged from factory default                                                                                                                                                                                                                             
	    	 low:	 clk/8=off, no clk out, full-swing, slow-rise power, internal_RC: 8MHz system clock

The SP12 command to set the fuses correctly for the Mega48/88 is: C:\AVR\sp12 -wF11100010 <enter>   

   ~~~~~~~~
   
The Mega8 fuses are slightly different:  
		high:  unchanged from factory default
		low:  clk/8=off, no clk out, full-swing, slow-rise power, internal_RC: 8MHz system clock
		
The SP12 command to set the fuses correctly for the Mega8 is: C:\AVR\sp12 -wF11100100 <enter> 
                                                                                                     
   The AVR microcontroller is running on its internal system clock which is generated by a tiny resistor/capacitor
combination built into the IC.  The manufacturing process can cause these R-C values to vary enough to throw off the 
precise MIDI timing.  Each AVR chip coming of the line at the factory gets calibrated.  This calibration value
gets put into the AVR's internal clock-calibration register (OSCCAL) at when the device is powered-on.  The clock speed
register can be adjusted for more precise clock speeds.  In my experience, the Mega48/88 devices don't need adjustment. 
MIDI timing using the factory calibration value has always worked OK.
 
   However, the Mega8 devices have different calibration values for the 1_MHz and the 8_MHz internal clock values.
The calibration value for the 1_MHz speed gets loaded into OSCCAL at power-up.  This value may not work for the 8_MHz
internal clock speed that this program uses.  There is a routine at the beginning of the program that reads the byte
at the highest internal AVR EEPROM location and puts this value into the OSCCAL clock speed register.  You need to put
the correct value for 8_MHz operation into the high EEPROM location to avoid MIDI timing errors with the Mega8.
Here are the SP12 instructions to read the 8_MHz calibration value and put it into the AVR EEPROM:

   C:\AVR\sp12 -rc11 <enter>       returns the factory-calibrated value for internal 8MHz operation
   C:\AVR\sp12 -wea 0x1ff:0xnn  <enter>  writes 'nn' [two digit hex value] that was returned by the previous instruction
   C:\AVR\sp12 -rea 0x1ff <enter>  returns the value found at the highest Mega8 internal EEPROM location
	
  The Memory_Programming section of the Mega48/88 and Mega8 datasheets talks about the fuses and calibration.
Questions can be posted to the AVRfreaks website general forum at www.AVRfreaks.org.  These questions come up often and
there are a lot of threads and tutorials about the details and complications of AVR configuration/programming.



~~~~~~~~~~~~~~~~~~~~~~~~~~
Files in the ZIP

Two_Pot_TX81.gif -  schematic drawing of electronics                                                                                                                                                                                            
TX_ReadMe.txt    -  this file                    

TP_Mega48.asm   -  AVR Mega48 source code in assembler
TP_Mega48.hex   -  Intel Hex file of code, ready for loading with standard AVR device programmers
               
TP_Mega88.asm   -  AVR Mega88 source code in assembler
TP_Mega88.hex   -  Intel Hex file of code, ready for loading with standard AVR device programmers
               
TP_Mega8.asm    -  AVR Mega8 source code in assembler
TP_Mega8.hex    -  Intel Hex file of code, ready for loading with standard AVR device programmers      

  If you don't want to bother with programming the AVR device or are having difficulty getting a single chip from
an electronics supplier without a large minimum order, I'll sell you a fully-tested Mega8 or Mega88 programmed 
with the latest version for $5.00 US (plus USPS shipping $2 in the USA, a little more worldwide).  Send me a message at:
  alan_probandt@yahoo.com
  
~~  Enjoy
