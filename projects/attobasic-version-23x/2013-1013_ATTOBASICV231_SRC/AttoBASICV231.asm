;************************************************************************
;	AttoBASIC V2.31
;************************************************************************
;Original code Copyright 2003 Richard Cappels projects@cappels.org  
;	http://projects.cappels.org
;------------------------------------------------------------------------
; All Versions of AttoBASIC from V2.00 forward, including Mega88/168/328, 
;	Mega32U4, AT90USB1286 & Mega2560 ports by 
;		K. Scott Vitale, Florida, uSA 
;		email: ksv_prj@gmx.com
;------------------------------------------------------------------------
;You may use this for personal use only. Commercial License is available.
;You may republish this file provided this notice is kept intact.
;Plese let me know of any bugs or (especially) improvements. Thank you.
;************************************************************************
; NOTE: USB is supported on Mega32U4, USB1286 and other AVR USB variants.
;		Mega88 support is inherant but some routines must be disabled
;		in order to fit into the 8K Program Space.  
;		See the file "Include\Defs_Constants.inc" for details.
;
;			USB Serial Code Copyright (c) 2008,2010 PJRC.COM, LLC
;			http://www.pjrc.com/teensy/
;
;			LUFA BootLoader for M32U4 and USB1286 written by Dean Camera
;			dean@fourwalledcubicle.com - http://www.lufa-lib.org
;
;************************************************************************
; Internal Timer usage:
;	M88/168/328:
;		RTC uses TMR0 (8-bit).
;		ICP uses TMR1 (16-bit)
;		PWM uses TMR1 (16-bit)
;		DHT uses TMR1 (temporarily as sample clock).
;		DDS uses TMR2 (8-bit) as sample clock.
;	M32U4:
;		RTC uses TMR0 (8-bit).
;		PWM uses TMR1 (16-bit)
;		ICP uses TMR1 (16-bit)
;		DHT uses TMR3 (temporarily as sample clock).
;		DDS uses TMR4 (8-bit) as sample clock.
;	USB1286:
;		RTC uses TMR0 (8-bit).
;		PWM uses TMR1 (16-bit)
;		DDS uses TMR2 (8-bit) as sample clock.
;		ICP uses TMR3 (16-bit)
;		DHT uses TMR3 (temporarily as sample clock).
;	Mega2560:
;		RTC uses TMR0 (8-bit).
;		PWM uses TMR1 (16-bit).
;		DDS uses TMR2 (8-bit) as sample clock.
;		ICP uses TMR3 (16-bit)
;		DHT uses TMR3 (temporarily as sample clock).
;		    uses TMR4 (16-bit)
;		    uses TMR5 (16-bit)

;************************************************************************
;REVISION HISTORY:
; All revisions listed below by K. Scott Vitale, Florida, uSA 
;	email: ksv_prj@gmx.com
;========================================================================
; TO-DO:
;
;------------------------------------------------------------------------	
; Version 2.31
;	2013-1013	Compacted code wherein recurring calls to retrieve two
;				  paremeters from the stack are now processed as a 
;				  separate call to "GetTwoParms".
;				Disabled USB IRQ's in the 1-Wire support routines.
;				Added 1mS delay to OWI routine to allow port pin to 
;				  settle upon 1st time bus initialization.
;				Simplified THEN command by simply popping return addresses
;				  of the stack.
;				Corrected THEN commands conditional assembly to pop the 3rd
;				  byte of the return address from the stack for parts with 
;				  more than 64KW or FLASH.  Originally, the test was for 
;				  32KW of FLASH, which was incorrect and specifically 
;				  affected the AT90USB1286 only.
;	2013-1007	Added support for 1-Wire devices.  See command reference 
;				  for details. 
;				Added the PRIACT flag to XH run-time flags so that some
;				  routines have a method to know if they are following 
;				  a PRINT, PRX or PRB command.
;	2013-1003	Modified stk500v2.c code to push 2 or 3 0x00's onto the 
;				  stack so that a "RET" can be used instead of an "EICALL"
;				  when transferring control to the application program.
;				Corrected stk500v2.c code to leave all flags within the
;				  MCUSR register to remain untouched so the application 
;				  can determine a course of action based upon them.
;				Updated OptiBoot Bootloader to version 5.0a.
;				Corrected optiboot.c code to leave all flags within the
;				  MCUSR register to remain untouched so the application 
;				  can determine a course of action based upon them.
;	2013-0928	Added new "MkRndSeed" routine, which scans the entire RAM
;				  upon startup to create a new polynomial seed for the 
;				  RND command.  MkRndSeed is part of the initialization.
;	2013-0924	Modified VPG routine to use a LUT to determine the page
;				  addresses of the various constants.  This method was
;				  needed because MCU builds of differing RAM sizes caused
;				  page cross-over issues.
;				Added WTF command, which 42 is the answer to.
;	2013-0923	Added support for the nRF24L01 series of 2.4GHz trans-
;				  ceivers.  See Command List for details.
;				Modified the default size of the data stack so that when 
;				  the nRF24L01 support is enabled, the data stack increases
;				  to a depth of 32 bytes.
;				Modified RESTore routines to return the size of the data 
;				  held in the DATA buffer.  This makes it easier to 
;				  determine a payload size when using the nRF24L01
;				  routines since each payload received could be between
;				  1 and 32 bytes in length.
;	2013-0922	Updated host development system, which now uses GCC 4.5.3.  
;				  GCC 4.5.3 introduces certain changes that caused the
;				  "optiboot" loader so expand byond 512 bytes.  Added
;				  conditional compiler-time option of "OS_main" instead 
;				  of "naked" for main's attributes.
;	2013-0921	Added register "cmdparms", which is updated by the "pushU"
;				  and "popU" routines.  It is meant to be used as a rel-
;				  ative counter to determine if new data was added to the
;				  data stack after a call to the "InterpretLine" or
;				  "InterpretLineV" routines.
;------------------------------------------------------------------------	
; Version 2.30
;	2013-0811	Whew!!  Lots of stuff!! Enjoy and please report the bugs!
;	2013-0809	Bugfix: TWI, TWR routine improperly interpreted a slave's
;				  DACK to the last byte as a bus error.  Fix: testing for
;				  proper status code depending on last byte or "Nth" byte.
;				Added code to insure register r1 is always "0" when 
;				  the usb_serial_io routines are enabled and called.
;				Simplified RAM allocation for each routine if enabled by 
;				  adding specific EQUATES for each routine's size.
;	2013-0808	Disabled DDS interrupts in ICP capture routines as the 
;				  DDS interrupts caused severe inaccuracies in the timing
;				  routines.
;				Changed the manner in which variables are internally 
;				  addressed; variables are now addressed as offset pointers
;				  rather than as the low-byte of a pointer.  Also corrected
;				  all routines that process variables.
;				Added BIN (') routine; same as HEX ($) routine but takes
;				  an 8-char binary number and converts to a single 8-bit
;				  number. Ex: '01010101 is $55.
;	2013-0806	Corrected line editing, program load/save, "new program"
;				  and LIST routines to use and/or check for the correct
;				  starting or ending location of program memory.  Previous
;				  versions inadvertently checked for top of PROGMEM + 2.
;				  Normally this wasn't a problem.as the data stack was
;				  positioned just above PROGMEM and it is unlikely there 
;				  will ever be 11 bytes of data on the data stack. However,
;				  this version moves the variable space right above 
;				  PROGMEM and thus variable "A" gets clobbered by the 1st
;				  line of a program.  Or if variable "A" is assigned a 
;				  value then the 1st character of the 1st line gets
;				  clobbered and the RUN command aborts with an unknow
;				  command.
;				Corrected bug in "SPI_Init" routine where SPI_SS line was
;				  not being set high after the SPI port was initialized.
;				  This caused issues with SPI devices wherein the device
;				  was already selected when it should not have been.
;	2013-0803	Corrected THEN routine to compensate for 24-bit call/ret
;				  pointers on MCU's with more than 64KB of FLASH.
;	2013-0802	Added port pin inits for DSI routines; if enabled, set pins
;				  to inputs.
;				Added WDT timeout in DSI routines while monitoring 
;				  pin-change events to prevent software lock-up in the 
;				  event that the DS slave stops responding or isn't
;				  available.
;				Tested and verified proper operation of the DS routines.
;	2013-0730	On USB builds, added a timeout to the INIT routine, where
;				  if there is no host to initialize the USB interface 
;				  within the timeout period the the USB interface is left 
;				  uninitialized and AttoBASIC continues in a stand-alone
;				  mode just like its USART counterparts.  This is mainly 
;				  so that the self-start feature may be used without 
;				  needing a host USB to talk to.  To further support the
;				  stand-alone USB mode, also modified all serial I/O 
;				  routines to check for the USB intereface being enabled
;				  and available before attempting to fetch or send 
;				  characters through a potentially non-existent USB 
;				  interface. 
;	2013-0728	Cleaned up ADC and ADR funkyness.  ADR now enables the 
;				  ADC and initiates the 1st conversion.  ADC now handles
;				  channels > 31 on MCU's that support it. Results of the
;				  ADC command can now be assigned to a variable.
;	2013-0727	Added command "IDUMP" to dump the contents of the MCU's
;				  I/O.
;				Bugfix: The STK500V2 bootloader does not handle watchdog
;				  timer resets and thus gets stuck in the bootloader.
;				  A newer, non-functional version has a fix for this.
;				  Modified the STK500V2 source to recognize the WDT as a
;				  valid reset source and jump to the application code.
;				  This issue affects the RST command on the ARDUINO 
;				  Mega 2560 boards.
;				Added nesting of FOR-NEXT function.  Now supports 
;				  nesting to 4 levels deep.  More generates a machine
;				  stack error.
;				Modified the "ADRcommand" routine to enable the ADC, 
;				  which will set the reference and start the 1st con-
;				  version.
;	2013-0726	Renmamed "DStackPointer" to "DStkPtr".
;				Modified usage of "DStkPtr" so that it is an index that
;				  indicates the number of elements on the data stack
;				  instead of the low-byte address of the actual element
;				  in RAM.  Fixes page-crossing issues and reduced code
;				  size.
;				Modified usage of "GuSuStkPtr" so that it is an index that
;				  indicates the number of elements on the GOSUB stack
;				  instead of the low-byte address of the actual element
;				  in RAM.  Fixes page-crossing issues and reduced code
;				  size.
;				Moved "LNBFNH", "LN1TOSS" and "LNNONNC" flags out of
;				  XH registeras they are only used during line editing.
;				  "Code_Editing.inc" now use "local flags" held in the
;				  "outchar" register temporarily.
;				Moved "GSJAMR" from XH to XL as FNLNXT flag in XL is not
;				  used.
;				Moved "FNLEQ" flag from XL to XH.
;				Added "VPTRSTK" flag in XL to flag whether or not there
;				  is a variable with a pointer on the dstack or not.
;				  Set/cleared by the "ProcessVariable" routine.
;	2013-0722	Added nesting of GOSUB-RETURN function.  Now supports 
;				  nesting to 4 levels deep.  More generates a machine
;				  stack error.
;	2013-0719	Bugfix: on Mega2560, which has more than 128KB of FLASH,
;				  all "(r)call" and "irq" address pointers are three (3) 
;				  bytes instead of two (2).  Thus more stack space is 
;				  needed.
;				Bugfix: in "OC0A_int", the frequency of updating the 
;				  DHT busy timer was dependent on the value of the RTI 
;				  command.  Fix: check the DHT busy timer every OC0A_int
;				  independent of RTI's setting.
;				Added DATA, READ and RESTore commands to support storing
;				  and retrieving data in a program.  There is no check
;				  for the statement being used in a running program so
;				  these statements can be used in interactive mode.
;				Changed register definitions for H, T, U and outchar so
;				 That T and U are aligned for 16-bit register operations.
;				Modified "getterminalline", "Countlinehchars", 
;				  "storeline", "insertinmiddle" routines to use immediate
;				  pointer comparisons when working within the line buffer.
;				  This also removed the need to use registers "bufferlimL"
;				  and "bufferlimH", freeing them up for future use.
;	2013-0718	Bugfix: In some cases, after using the watchdog as an 
;				  interrupt source, the WDTIRQ flag in GPIOR0 was left 
;				  set causing the SLP command to prematurely end.  Fix:
;				  Cleared all interrupt source flags in GPIOR0 before 
;				  entering sleep mode in the "SLPcommand" routine.  
;				Added 8 second watchdog timeout in "Init" routine for
;				  USB builds.
;				Added the "VPG" command, which returns the page in RAM
;				  where the internal variables are stored.  It is 
;				  intended to be used with as the page number when 
;				  accessing the internal variables with the PEEK and 
;				  POKE statements.
;				Added "@" command.  The "FETCHcommand" routine is used
;				  to access system internal variables.  Technically, it
;				  is a dummy command that simply passes control to the 
;				  interpreter, which processes the next command in the
;				  buffer/program, which should be a variable name.
;				Added DHI, DHD, DTI and DTD commands, which return the
;				  low byte of the address of that internal variable in 
;				  RAM.  They are intended to be used with the "VPG" 
;				  command for accessing the internal variables with the
;				  PEEK and POKE commands.
;				Added the "RTC" command, which takes as its argument a
;				  number from 0 to 3, designating which of the four 
;				  Real-time Counter bytes one wishes to access.  It is
;				  intended to be used with the "VPG" command for 
;				  accessing the internal variables with the PEEK and 
;				  POKE commands.
;				Added the "DFA" command, which takes as its argument a
;				  number from 0 to 1, designating which of the two  
;				  DataFile sequential address counter bytes one wishes
;				  to access.  It is intended to be used with the "VPG" 
;				  command for accessing the internal variables with 
;				  the PEEK and POKE commands.
;				Added "FILL" command as part of the DEBUG routines.
;				  FILL is for filling RAM with a preset value. 
;	2013-0716	Modified SPW, SPR and SPS commands to check that the SPI
;				  hardware was enabled before continuing, error if not.
;				  This prevents software hang when using those commands
;				  before issuing the SPM command.
;				Moved operator commands, LSR, LSL, AND, OR, XOR, COM and
;				  NEG commands into "Code_Operators.inc" to make it easier 
;				  to find them. 
;				Modified LSR and LSL operators to accept a 2nd parameter,
;				  which is the number of bits to shift.
;				Added SWAP command to swap the nibbles of the parameter.
;				Added the NBH and NBL commands to return the high and 
;				  low nibbles of a paramter.
;				Compacted code by adding the "Get1stParm" routine since
;				  most commands always call the line interpreter and
;				  then pop the value off the datastack.  This routine
;				  only saves code space if the commands that use it are
;				  within the 2047 byte address range of it.
;	2013-0715	Modified sign-on message to include flavor-specific
;				  information, which cuts down on the amount of FLASH
;				  used.
;				Modified SStst (in "Code_Init.inc") to load program "0"
;				  from EFS, if support is enabled.
;				Bugfix: Disabled RXCIE in USART during "DHTread" routine
;				  as the CP2102 UART to USB converter would spew nulls
;				  into the receiver, which caused a flurry of IRQ's.
;				  The "DHTread" routine is only expecting pin-change 
;				  IRQ's during reading of the sensor.
;	2013-0713	Added support for the ATmega2560 MCU, which includes 
;				  support for ARDUINO MEGA2560 product (using USART0).
;				Bugfix: DHT routine will abort program execution upon a
;				  checksum error.  Changed "DHTRead" routine to zero both
;				  temperature and humidity readings as a sign that there 
;				  was a checksum error detected from the DHT sensor.
;				Bugfix: "deletekey" routine did not properly handle a
;				  backspace character.
;				Modified RUN command to accept a file number as a parameter.
;				  If no parameter, run the current program.  If a
;				  parameter is supplied and it is a valid file number
;				  then the program is loaded then run.
;	2013-0712	Bugfix: in "Checkkeybd" routine, clearing the HALT flag
;				  then looking for ^C to be pressed twice was nearly 
;				  impossible when using USB serial since the RUN commnand
;				  and the "emitchar" routine both called "Checkkeydb".
;				  This resulted in infinite loop programs being unable 
;				  to be broken out of.  Fix: in "Checkkeybd" routine, 
;				  removed clearing the HALT flag before checking for a
;				  key in case a prior call to "Checkkeybd" had already
;				  set the HALT flag before the RUN command could break
;				  program execution.  Also, ^C only needs to be pressed
;				  once to BREAK, not twice. 
;				Added a second channel of PWM for both 8-bit and 10-bit
;				  resolutions.  This is valid for M32u4 and USB1286 parts.
;				  Only valid on M88/168/328 if the SPI and Data File 
;				  rotines have been disabled.  Ex: PWM [x] [c].
;				Removed support for PWM on the OC0A pin.
;				
;	2013-0711	Implemented limited file system to be able to SAVe and 
;				  LOAd programs to/from EEPROM by program number.  The 
;				  number of files available is determined by the EEPROM 
;				  size of the device to a maximum of 8.
;				In supportof file system: added routine during INIT to 
;				  look for blank EEP and set file handles and size to 
;				  zero.
;				Renamed "Code_EEP.in" to "Code_EFS.inc".
;				Corrected compile-time issue with string generation
;				  where M88 and M168 caused the assembler to insert 0x00
;				  into the header strings thereby causing some strings
;				  to misprint.
;	2013-0709	Added the "EEW" command to write a byte to the EEPROM.
;				  If the address given is E2END+1 then then entire EEP
;				  is erased. Ex: "EEW 0 4 0" is 0x400 and will erase 
;				  the entire EEP because E2END is 0x3FF on a Mega32U4.
;				Added the "EER" command to read a byte from the EEPROM.
;				Added "SetByteRegs" routine to consolodate address pointer
;				  retrieval from datastack for the PEEK, POKE, EER and EEW
;				  routines   This reduced code size.
;	2013-0706	Bugfix: DUMp and EDUmp routines; corrected last byte of
;				  sequence problem in EDUmp by moving check for last address. 
;				Corrected printing of "0x0FFF" in DUMp and added byte
;				  index header.
;				Changed order of EDUmp and DUMp routines to dump from 
;				  start of memory to end instead of end to start.
;				Added ^S checking in the DUMp and EDUmp routines to 
;				  suspend printing the output. 
;	2013-0705	Modified SAVe routine to use SLEEP mode while waiting for
;				  completion of atomic writes.  This was done as a means 
;				  to save power as each write-byte takes about 4ms.
;				Modified INIT routine to check for the last byte of EEPROM
;				  being 0xFF, which indicates that the EEPROM is erased.
;				  If the last byte is erased, it is replaced with a 0x00,
;				  which signifies end-of-program.
;				Cleaned up coding on the DEBUG routines by consolodating
;				  printing of the value of Z register pair into one 
;				  routine and adding header to show the byte index.
;	2013-0704	Modified SAVe routine to also check PCHI/PCLO when saving
;				  programs to EEPROM.  Once PCHI/PCLO have been reached, 
;				  a null character is written too EEPROM to signify the
;				  end of the program.  This speeds execution when saving
;				  to devices with large amounts of EEPROM since only the
;				  program is saved.
;				Modified LOAd routine to check for null character in the
;				  data when loading programs from EEPROM, which signifies
;				  the end of a program.  This speeds execution when 
;				  loading from devices with large amounts of EEPROM.
;				Insured that the port pin is set to input and reduced the 
;				  code size in the ICP routines. 
;				Bugfix: The FILLCNTR in the RAMFILL macro in "NEWPROGRAM"
;				  did not fill the last byte of PROGMEM.  Fix: add one to
;				  the FILLCNTR.
;	2013-0703	Modified USB Serial I/O library to acccess PROG_DATA 
;				  residing in PROGMEM above 64K region using "morepgmspace.h"
;				  macros, which makes use of the RAMPZ register and ELPM 
;				  instruction.
;				Modified "BLDRcommand" to make use of the RAMPZ register 
;				  and ELPM instruction.
;	2013-0701	Updated USB Serial I/O library to V1.7 and modified calls 
;				  to match accordingly.  Since RAM usage changed, modified
;				  order of variables held in RAM.
;				Modified USB serial I/O init routine to await for DTR to
;				  be asserted before continuing with sign-on message.  
;				  This allows for one's terminal program to notify 
;				  AttoBASIC that the user is ready and awaiting a prompt.
;				Changed INIT routine so that the system clock pre-scaler
;				  is always set to the value of FCLK_PS (see 
;				  Defs_Constants.inc), this is partly due to Teensy++ 2.0 
;				  having the DIV8 fuse programmed, yielding a CPU clock 
;				  of 2MHz.
;				Added call to "usb_serial_shutdown" in RST command to insure
;				  proper disconnect from USB host when using the USB
;				  serial I/O builds.
;	2013-0629	Added support for TEENSY++ 2.0 HALFKAY bootloader by 
;				  relocating USB serial I/O routines and DDS data below
;				  the 512 word BOOTLOADER address of the USB1286.  Use
;				  the TEENSY definition to make this custom build.
;	2013-0627	Enabled LUFA's BootloaderDFU.c to check for PORTF4
;				  (JTAG TCK) pin shorted to GND, which will
;				  invoke the DFU bootloader.  This is for platforms
;				  running with a Mega32U4 or USB1286 where a JTAG
;				  port may available on the target PC board.
;	2013-0626	Added support for AT90USB1286 (PJRC's TEENSY++ 2.0).
;				Added "USED" bytes to "FREE" command.  Also changed
;				  message content, which also reduced code size.
;				Bugfix: May not have affected MCU's with smaller RAM
;				  sizes; RAMFILL macro's pointers in "NEWPROGRAM" 
;				  routine get destroyed due to ISR's overwriting them. 
;				  Fix: disable interrupts during RAM fill.
;------------------------------------------------------------------------	
; Version 2.22
;	2013-0619	Bugfix:  "DHTread" routine did not re-enable RTC IRQ's
;				  when a checksum or non-response is detected.
;	2013-0613	Corrected HELP command formatting error when using 
;				  Mega32U4 with USB serial.  inbyte[h:l] were being used
;				  as character and line length counters, which on a 
;				  Mega88/168/328 and Mega32U4 without USB build is 
;				  acceptable.  However, those two registers are reserved 
;				  on the Mega32U4 when USB is enabled as they are used 
;				  to pass data to the usb_serial library routines.
;	2013-0611	Corrected TWI bit-rate generation conditional statements
;				  to correctly calculate the maximum available bit clock 
;				  value based on the system clock frequency.  Added 
;				  conditional assembly to the "TWIcommand" routine to 
;				  support 400K and 100K selection if the system clock
;				  allows it, otherwise, only the maximum rate detected
;				  is allowed.
;	2013-0610	Added support for system clock prescaler.  Set this in 
;				  the DEFS_CONSTANTS.INC file. This is mainly for use in
;				  testing AttoBASIC at different clock speeds but can
;				  be used normally if so desired.
;	2013-0607	Added support for the DHT11 and DHT22 temperature and 
;				  humidity sensors.  Use DHT [x] (temperature), DHH 
;				  (humidity), DHR and DHS commands,  Rounding is performed
;				  as well as sign for the DHT22 in Celsius readings.  See 
;				  notes in the command list.
;	2013-0605	Bugfix: Moved the set flag (RTCIRQ in GPIOR0) in the 
;				  "OC0A_int" routine out of the conditional loop to the
;				  end of the routine where it should be.
;				Updated the DDS register value to provide compensation
;				  for lost cycles due to interrupt processing.  Accuracy 
;				  measured to less than 0.6% error.
;				Created "WDDisable" routine to allow global calls for 
;				  commands that make use of the watchdog timer.
; ******		Updated DDS definitions to be device-specific.  There is 
;				  no actual code change, just additional conditional 
;				  assembly statements and definitions.
;
;
;------------------------------------------------------------------------	
; Version 2.21
;	2012-0828	Corrected location of DDS routine's wave table.  The 
;				 conditional assembly moved the wave table to the end of
;				 PROGMEM if the build did not contain a boot-loader, which
;				 was fine if one was going to use ISP to prgram the HEX file.
;				 However, if one is going to use a boot-loader then the 
;				 wave table pointer is pointing to the boot-loader code,
;				 which causes eroneous waveform data.  The conditionals now
;				 always assume there is a boot-loader and the wave table is
;				 located at the beginning of the page below it.
;				Now setting DDS sample rate to FCLK/256 to accomodate for 
;				 various clock speeds.  With a constant rate, the RTC ISR
;				 would interfere with the DDS ISR routine.
;				Corrected FCLK definition for AVR Data Recorder, whereas 
;				 FCLK was always assigned a value of "8000000" reguardless
;				 of its real value.
;
;========================================================================
; Version 2.20
;	2012-0815	Added "LDDcommand", which loads the default capture
;				 and record program for the Data Recorder.
;				Bugfix: Corrected bug in "diffcommand" routine, which 
;				 is actually the "<" and ">" operators, where a half-
;				 carry produced an incorrect result.
;	2012-0808	Added "ACI" command to enable/disable analog comparator
;				 interrupts.  This is mainly for use with the "SLP 0"
;				 command.  ACI only supports inputs on AIN+ and AIN-.
;				Added "ACS" command to select source to the analog 
;				 comparator's "-" input.  Either AIN-, ADC0 or ADC1.
;				Modified "ACOcommand" routine to select AIN- source
;				 based on the value selected by the "ACS" command.
;	2012-0807	Bugfix: Corrected bug in "peekcommand" routine where 
;				 there was an extra call to "interpretlinev" at the
;				 end of the routine, which added erroneous data to the 
;				 stack.  Removed the extra call.
;				Bugfix: Corrected bug in the "DDScommand" routine that
;				 falsely detected whether the command line parameter was
;				 a "0" or a "1".
;	2012-0805	Added OSC comand to set and read the internal oscillator's
;				 calibration register.
;				Added a WORDS command to list the available commands.
;				Modified operators; "=", "!=", "<" and "<" to return a 
;				 "1" for a "true" condition or a "0" for a false condition.
;				Inverted the detection logic of the "IFcommand" routine
;				 to accomodate the modified operators.
;				Changed the "ACO" command to return the actual state of
;				 the ACO bit.  Was inverted in prior versions.
;				Combined the "lessthancommand" and the "greaterthancommand"
;				 routines to become the "diffcommand", which determines
;				 which comparision to perform then performs it.
;	2012-0803	Added "DIG" command to directly control the N-MOSFET on
;				 the AVR Data Recorder.
;	2012-0730	Shortened some error messages.
;				Added support to the "PRINT" command to print strings 
;				 embedded in quotes. Supports sending CR/LF (the "~" 
;				 character) embedded within the string.
;				Modified "getterminalline" routine to allow use of the
;				 "#" character embedded in stings.  Normally, any
;				 characters following a "#" will be stripped.
;				Modified certain operator to single-character commands;
;				 AND = "&", OR = "|", XOR = "^", and NEG to "!".  This 
;				 change reduces the amount of program memory needed for 
;				 the command while also conforming to "C conventions".
;				Changed the "<>" (not equal) operator to "!=".  Done
;				 to conform to "C conventions".
;				Modified "RUNcommand" routine to store the currently 
;				 executing line #, which is stored in RAM@CurrLine.  This
;				 aids in tracing program statement errors, which are now
;				 printed along with the error message.  
;				Added routine "GETPROGLINENO" to support the aforementioned
;				 error message. 
;				Added "RUNACT" flag for the use of the "GETPROGLINENO"
;				 routine.  The "RUN" command sets it then resets it
;				 after program ends.
;	2012-0729	Bugfix: When "BIGRAM" is disabled, the "LOADcommand"
;				 incorrrectly calculates the low-pointer based on the
;				 assembly-time constant of "dstacktop".  Cleaned up 
;				 "SAVEcommand" and "LOADcommand" routines to use
;				 16-bit macros for loading and comparing pointers.
;				Re-ordered "LOADcommand" routine; execute NEWprogram,
;				 print "loading from EEPROM" then execute "FREE" command.
;	2012-0728	Due to sampling inaccuracies with the "ADC" command, 
;				 modified the routine to average 10-bit samples then
;				 convert to an 8-bit result before saving on the data stack.
;				Added modulus operator, "%". Why?  The remainder is already
;				 available from the "DIVIDEcommand" routine, so why not?
;	2012-0726	Shortened some of the error messages to conserve string
;				 space.
;				Disabled BTL command for the Data Recorder as it is
;				 very messy to implement. :)  However, the bootloader code
;				 is still generated in specific builds and will be invoked
;				 upon a RESET if the BOOTRST is set.
;	2012-0723	Added specific commands for the "AVR Data Recorder".  If
;				 enabled, conditional assembly is performed to enable 
;				 selecting channel #'s, setting gain and the AD536's output
;				 type (TRMS or dBV). 
;	2012-0714	Bugfix: "subtract" routine issued an "arithmetic underflow"
;				 when a operation resulted in a half-carry.  The "brvs" 
;				 instruction should have been a "brcs" instruction.  This was
;				 the result of adding the "AOV" command from V2.00.
;	2012-0711	Modified "hexcommand" routine to detect 1 or 2 characters. 
;				 Therefore, when entering only 1 character no longer returns
;				 erroneous results.
;				Bugfix: Corrected Z-ptr overflow problem in the "Error" routine
;				 where adding the message offset to ZL may result in a carry 
;				 to ZH that was not accounted for.  Changed to a 16-bit add.
;				Bugfix: When the "SELFSTRT feature is activated, any console
;				 I/O would be block because global interrupts were not enabled
;				 after the serial I/O hardware was initialized.  Added "sei" 
;				 after serial I/O initialization in the "Code_Init.inc" routine.
;				Bugfix: For Mega168/328 using the Optiboot loader; modified
;				 the BASH script to correctly fill unused bytes between addresses
;				 in the HEX image.  There are two bytes required to be placed 
;				 as the last two bytes in the HEX file but were placed right
;				 after the code.
;	2012-0709	Modified DUMP command to dump all of RAM contents instead
;				 of just PROGRAM memory.
;				Added CPU Clock frequency in the sign-on and "NEW" messages.
;				 Works for integer frequencies of 1MHZ+, no fractionals.
;	2012-0629	Bugfix: Added conditional compile directives in "usb_serial.h"
;				 to properly set the PLL for using 8MHz or 16MHz CPU clock.
;				 V2.11 did not set this up properly and using the USB_Serial
;				 I/O rouines at 8MHz yielded an unrecognized USB device..
;				On Mega32u4 with USB_Serial I/O; modified variable locations
;				 in RAM so as to allow more room for programs with the 
;				 BIGRAM option set.
;	2012-0624	Added "XB[p] [n]" command to toggle bit "n" on port "p". 
;				Pre-release bugfix: "OCR0A" ISR caused an intermittant
;				 error, which resulted in halting program execution with
;				 various single errors displayed.  The fix was to save the
;				 state of SREG and restore it upon exit of the ISR. Duh!
;				Pre-release bugfix: When RTC routines are enabled, the 
;				 SLP command would randomly "hang" waiting for a WDT 
;				 interrupt to occur.  Any other interrupt would awaken 
;				 the AVR's "sleep" instruction and program execution 
;				 would continue.  This seemed to be attributed to the 
;				 SLP command routine not being keenly aware that it 
;				 should only respond to a WDT interrupt when required.  
;				 Resolved this by adding a set of flags in GPIOR0, which 
;				 are set by their correspondng routines and cleared by 
;				 the SLP routine, thus effectively ignoring the DDS and 
;				 RTC interrupts.
;	2012-0621	Added "RST" command, which invokes the watchdog timer
;				 in "System Reset" mode to cause a hardware reset 
;				 of AttoBASIC.
;				Added "Real-time Counter" interface routines.
;				Pre-release bugfix: moved "inc dstakpointer" to proper 
;				 location.  Was inadvertently moved during register 
;				 re-assignment, which caused intermittant data stack 
;				 errors.
;	2012-0531	Bugfix: The ADC command had a "default" value, which is 
;				 PUSHed onto the stack before acquiring a user-supplied 
;				 value. Variable assignment of the value returned from 
;				 the ADC command would cause a misalignment in the datastack
;				 pointer and the default value would be returned instead 
;				 of tha actual value provided by the user.  Removed the
;				 ability to use a "default" channel with the ADC command.
;				Bugfix: Corrected ADC bug whereas the Mega328 was not 
;				 actually supported in conditional assembly.
;				Added "Data File" interface routines.  Presently, the 
;				 routines support the MICROCHIP 25AAxxx series of Serial
;				 EEPROM devices using the SPI interface.  Address range is
;				 16 bits or up to 65,536 storage locations.  The intent
;				 behind this added functionality is for use as a data-
;				 recorder when used with the "SLP" instruction.  Eventually,
;				 these routines could be used with SD memory cards as they
;				 also use the SPI interface.
;				Added "#" sign, which tells the line interpreter to ignore
;				 all characters after the "#" until a CR is found.  This 
;				 aids in commenting programs when one is editing and 
;				 uploading them via a serial terminal.
;	2012-0530	Bugfix:  Line parsing routine improperly inserted and 
;				 deleted lines.  This was caused in V211 while optimizing
;				 relative branches.  Specifically, in "Code_Editing.inc",
;				 the routing named "noinsertlowerline" was a fall-through 
;				 from the "notattop3" label.  When it was moved, a jump
;				 was not inserted to compensate ("rjmp noinsertlowerline).
;				Modified DSI routines to reduce the number of instructions
;				 by implimenting the existing "Delay1mS" routine in place
;				 of the "shortdelay" routine, which is a software loop 
;				 delay dependant on the CPU clock frequency.  The "Delay1mS"
;				 routine is independant of CPU clock frequency.  Code 
;				 reduction is 16 bytes (helpful on Mega88).
;				Added "SLP N" instruction.  When executed, the CPU enters
;				 the power-down mode. The routine is exited once the 
;				 "waking event" has ocurred.
;	2012-0311	Added "BIGRAM" conditional assembly to allow larger 
;				 program storage.  Caveat: Programs larger than EEPROM
;				 cannot be saved and will be truncated!  This option is 
;				 good for when one wishes to save and load programs via
;				 a terminal emulator.
;				Changed SIZE command to print in bytes instead of pages
;				 and characters.  Easier for we humans to read. :)
;========================================================================
; Version 2.11
;	2012-0307	Bugfix: storage of programs does not exceed 1st page.  
;				 This was due to a bug introduced when relative branches
;				 in the line edting routines overflowed and reverse com-
;				 pares were used in the "noinsertlowerline" routine.  The
;				 routine was corrected and relocated to remove the branch
;				 errors.
;========================================================================
; Version 2.1
;	2012-0315	Bugfix: Corrected bug in "IBx" command.  Logic was reversed 
;				 whereas if the bit tested was high, a "0" was returned on 
;				 the data stack and versa-vise.
;	2012-0221	Added additional routines to BLDR command; the command is 
;				 always enabled on the appropriate devices that support a
;				 a boot-loader.  However, the BOOTSZ1:0 fuse bits are 
;				 used to determine the location of a boot-loader then a 
;				 check for the existence of a valid boot-loader jump table
;				 is checked before invoking the boot-loader.  If a boot-loader
;				 is not found, then an error message is displayed.
;	2012-0220	Added conditional assembly to support PEEKs and POKEs 
;				 within entire 64K of data space by passing the page # 
;				 and index into the page.  Was limited to page zero only. 
;				 Calling format is "page,index".  If assembled for page 
;				 zero only, the address byte will always point within page 
;				 zero and any additional values will be ignored. 
;	2012-0218	Changed "Init" routine to enable port pull-up, check pin 
;				 then disable port pull-up if "Self-Start" feature is 
;				 enabled.  This allows for an ARDUINO user to use AttoBASIC 
;				 without needing to be concerned about the "SS" feature
;				 loading a program from E2P when not desired.
;	2012-0213	Added conditional assembly to support USB Serial I/O and 
;				 USB LUFA DFU Boot-loader for Mega32U4 running at 8MHz
;				 and 16MHz.
;				Wrote Linux BASH shell script to support automagic creation
;				 of all processors, clock speeds and if available, USB and
;				 boot-loader code inclusion.
;	2012-0209	Added software version code to the sign-on message.
;				Changed sign-on string to reflect version with build CPU 
;				 on separate line.
;	2012-0208	Added enhanced printing support fo internal messages. 
;				 Specifically for printing centered text and repeated
;				 characters.  This decreases FLASH spaced used to hold
;				 space and repeating characters.
;				 If the first character of a string contained in PROGMEM is:
;				  - 0xF8 then the line length is calculated and centered 
;				    by prefixing spaces before the string.
;				  - 0xF9 then the next sequential number is the number of
;				    times to repeat the following character.
;				  - 0xFA emits a CR/LF combination.
;				  The constant defined as "LNLNGTH" contains the system-wide
;				  max length of system message strings.  
;				Cleaned up system message string usage conserving ~400 bytes
;				 of PROGMEM space.
;				Cleaned up use of conditional statements in "Data_Msgs,inc".
;				Corrected message order of E2PROM "loadcommand".
;				Added USB buffer overflow check.
;	2012-0207	Added new command "BLDR" to directly call the bootloader.
;				 Currently, this is only functional on the Mega32U4 (with
;				 USB serial I/O enabled) as the LUFA DFU bootloader can be 
;				 enabled via a DEFINE directive.
;				Added support for Mega328, which many ARDUINO's use.
;				Added code to disable the watchdog timer at INIT time.  If
;				 using the ATMEL FLIP DFU application to "start the app" 
;				 and the "RESET" is checked, the watchdog timer is enabled
;				 by the bootloader and a watchdog timeout is used to reset
;				 the MCU.
;				Bugfix: Regarding "PopU" and "PushU" routines, which 
;				 occured when "dstackpointer" was moved to a low register.
;				 "temp" register is now saved and restored in each of those
;				 routines.
;	2012-0205	Cleaned up code; compacted some operations and verified
;				 code functionality since committing PC and GOSUBRETL to RAM.
;	2012-0203	Added support for USB via built-in hardware controller. 
;				 The CDC protocol (virtual serial port) is used.
;				 USB library adds 3022 bytes.  Only available on M32U4.
;				Moved registers inbyteh:inbytel to R25:R24 for inter-
;				 facing to USB library routines.
;	2012-0130	Modified registers; moved dstackpointer to R12, ecode to 
;				 R13, PCHI/PCLO and GOSUBRETH/GOSUBRETL to RAM.  Adjusted 
;				 code accordingly.
;
;========================================================================
; Version 2.0
;	2011-1020	Bugfix: Corrected bug in SB[x] and CB[x] commands.  Wrong 
;				 port offset was used.
;	2011-0606	Compacted PORT I/O commands to use one routine for each
;				 command variation.  I.e. 'OPB' and 'OPD' use the same
;				 routine as the desired port is contained within the 
;				 3rd letter of the command being executed (trimed 574B).
;				Added assemble-time directive to enable/disable "PIND0
;				 test".  See DEFS_CONSTANTS.INC file.
;	2011-0525	Bugfix: Corrected ADC command to dummy read ADC before 
;				 returning the correct result.
;				Enabled support for ADC command to take up to 64 channels
;				 as Mega32U4 supports as such.
;	2011-0524	Added support for ATmega32U4.  Change the DEF file to
;				 select between Mega88/168 and Mega32U4.  USB functionality
;				 not yet provided.  Serial I/O only.  Note: the XML and DEF
;				 files included with AVRStudio 4.18 SP3 are not complete
;				 for the Mega32U4.  The TWI module is missing.
;	2011-0518	Added command separator for program lines.  The command
;				 separator is the semicolon (";").  Thus, muliple commands
;				 can be on the same program line.  This is helpful for using
;				 the TWI commands for example.
;	2011-0517	Added feature enabling support so that a user can 
;				 enable/disable certain command features if not 
;				 needed.  See DEFS_CONSTANSTS.INC file.
;				Changed UART receive handler; MPU now enters sleep 
;				 mode until a character is received.  Note: any interrupt
;				 wakes from sleep but the 'recvchar' routine checks for
;				 data in RX buffer.  If no data is ready, it goes back
;				 to sleep.  This allows for other interrupt-driven 
;				 routines (such as DDS output) to function as well and keeps
;				 the MPU running in a low-power state when waiting for
;				 user input. 
;				Added D[irect] D[igital] S[ynthesis] feature as follows:
;				'DDS [x] - Outputs a frequency on the defined port pin
;				 at the 6-BCD-digit frequency held in the X/Y/Z variables. 
;				 The DDS sample frequency is set to twice the Interrupt
;				 service routine's duration, which is 5uS.  Therefore, the
;				 output frequency range will be 0 to 25KHz in 1Hz steps.
;				 X = 0 to disable DDS and x = 1 to enable [X/Y/Z set first].
;				 Without X same as 0 [disable].
;				Added version coding to the signon prompt.
;	2011-0515	Added routines to support TWI/I2C interface as follows:
;				TWI [x] - must be rcalled first to initialize the TWI
;				 interface. X = 0 for 400Kbps and x = 1 for 100Kbps clock.
;				 Without [x] is same as x = 0. Defaults to Master 
;				 @ 400Kbps with PORT pull-ups enabled.
;				TWS - Assert a START condition on the bus.  
;				TWP - Assert a STOP condition on the bus.
;				TWA [x] - TWA sends the slave address to the bus.
;				 Returns with the bus status on the stack.
;				TWW [x] - TWW sends a byte to the bus.  Returns with
;				 the bus status on the stack.
;				TWR [x] - Receives a byte from the TWI bus and places
;				 it onto the stack.  x = 0 to signal to the slave that
;				 this is the last byte to receive, x = 1 to signal to 
;				the slave there is more data to receive. Without [x] 
;				is same as x = 1.
;				TWB - Queries the TWI status register for the last 
;				 detected condition of the bus.  [Note: the byte returned
;				 is right-shifted 3 bit positions. If a STOP condition 
;				 has been detected, $80 is returned to indicate so.
;				Restructured error code lookup table.  Saved 54 bytes for
;				 17 error messages.
;				Restructured error code routine.  Register ecode conflicts
;				 with any error codes over $12.  Implimenting some of the
;				 additional commands required additional error codes .  The 
;				 error code is now kept in RAM @ERRCode separate from 
;				 the input parsing routine.
;				Added '*' and '/' commands for 8-bit multiply and divide
;				Added underflow and overflow detection to arithmetic
;				 commands with error messages.
;				Added 'AOV [x]' command to enable arithmetic overflow
;				 and underflow detection where x = 1 enabled error and
;				 x = 0 disables detection.  Defaults to x = 1.  Note that
;				 when detection is disabled, the result from an arithmetic
;				 operation will return the 8-bit result.  Expect errors
;				 if not careful!
;				Added 'ID[p]" command to return the value of the DDR[p]
;				 register.
;	2011-0514	Fixed input line parsing routine; ^H and BACKSPACE 
;				 now actually erases the character.
;				Fixed input line parsing routine; ignores linefeed
;				 characters ($0A) when they are seen.  This allows for
;				 uploading programs using a terminal emulation program.
;	2011-0513	Added new commands to support input capture on ICP1.
;				'ICG [x]' - Initializes ICP mode and sets Input Capture 
;				 gate time to x[0..7] where x is optional (default 0):
;				  = 0 is disable ICP.
;				  = 1 is   10mS gate time
;				  = 2 is   25mS gate time
;				  = 3 is   50mS gate time
;				  = 4 is  100mS gate time
;				  = 5 is  250mS gate time
;				  = 6 is  500mS gate time
;				  = 7 is 1000mS gate time
;				'ICE [x]' - optionally sets the edge select.  Where
;				 x = 0 for falling and 1 for rising.
;				'ICP' - returns the low byte value and stores the 
;				 high byte in variable 'Z'.  'Z' is clobbered.
;				 Returns an error if there is a 16-bit overflow 
;				 (and clears 'Z').
;				Added ability to select PWM channel between OC0A and 
;				 OC1A during program assembly. If OC0A is used, the 
;				 10-bit 'PWE [x]' command is disabled.
;				 Code can be easily mod'ed to switch back and forth.
;	2011-0512	Added size check to EEP save routine. Will only save
;				 if the current program size + datastack fits into EEP.
;				 Stops saving when EEP bottom is reached because 
;				 wrap-around sux!
;				Added/modified ADC commands as follows:
;				'ADC [x]' - supports channels [0..15].
;				'ADR [x]' - optionally set INT/EXT ref.
;				 where x = 0 for INT and x = 1 for EXT.  Without [x] is
;				 same as x = 0, int. ref
;				Added RND command to generate 8-bit random #.
;	2011-0511	Added routines to support the SPI interface.  New CMD's:
;				'SPM [x]' - MUST be rcalled first.  Initializes the SPI
;				 to operate in Mode [0..3] (see data sheet).
;				 Defaults to Master, F_CLK/16, MSB first.
;				'SPO [x]' - optionally set MSB/LSB mode 
;				 where x = 0 for MSB and x = 1 for LSB.
;				'SPC [x]' - optionally set SPI_clk to [0.15].  See 
;				 Pg 168 of data sheet for actual code meanings.
;				'SPW [x]' - write byte to SPI.  Note that SPI_SS pin is
;				 set low when this cmd is executed and not restored so
;				 user must toggle pin high with the SPS cmd.
;				'SPR'	  - read byte from SPI. Note that SPI_SS pin is
;				 set low when this cmd is executed and not restored so
;				 user must toggle pin high with the SPS cmd.
;				'SPS [x]' - set SPI_SS pin to [x]. Defaults to '1'
;				Added EDUMP command to dump the contents of EEP.
;				Added VDUMP command to dump the contents of the 
;				 26 variables [A..Z].
;	2011-0510	Ported from M163 to M88/168.  
;				No PORTA on M168 so code.
;				Split out Register defs and constants to separate files
;				 to ease in reading of source code.
;				Modified greeting to include build date and Scott's
;				 copyleft info.
;				Changed DELAY routine to be F_CLK aware and thus 
;				 transparent to the end user.
;
;************************************************************************
;.nolist
.include "Include/Defs_Constants.inc"	;!!!Start Here to define your flavor
.include "Include/Defs_ADC.inc"			;ADC constants
#if USB
 #if defined(__ATmega32U4__)
  .include "Include/Defs_usb_serial-atmega32u4.inc";API defs for USB support
 #elif defined(__AT90USB1286__)
  #if TEENSY
  .include "Include/Defs_usb_serial-TEENSYPP20.inc"	;API defs for TEENSY USB support
  #else
  .include "Include/Defs_usb_serial-at90usb1286.inc";API defs for USB support
  #endif
 #endif
#endif
.include "Include/Defs_USART.inc"
.ifdef TWDR
	.if TWI
	.include "Include/Defs_TWI.inc"	;include TWI definitions
	.endif
.endif
;
; Macros
.include "Include/Macro_IO.inc"
.include "Include/Macro_Misc.inc"
.include "Include/Macro_Delays.inc"
.include "Include/Macro_AttoBASIC.inc"	;AttoBASIC specific macros
;
.list 
;
;******************************************************************************
;Define RAM/EEPROM Storage Variables
;******************************************************************************
.include	"Include/Data_RAM.inc"
.include	"Include/Data_EEP.inc"
;
;************************************************************************
;Start of code space here...
;************************************************************************
.cseg
.org     $00

;set up interupt vectors
.include "Include/Code_Vectors.inc"

;***************************************************************
RESET:
	wdr							;reset the watchdog in case it is enabled
	InitSP						;init stack pointer to RAMEND

RESET1:
	GoSub	Init				;initialize machine

#if !TEST
	GoSub	TypeGreeting		;print signon greeting
#endif
;
;***************************************************************
main:	
	sei							;insure unterrupts enabled
	GoSub	crlf				;send a CR/LF combo to the console
	INITDSTACK					;Initialize data stack pointer
	GoSub	getterminalline		;Get line from terminal (60 chars + $0D)
	GoSub	Interpretelinebuffer;Interpret the line in the buffer
	Goto	main				;loop for more

;=============================================================
; getterminalline: 	;Get characters from terminal into 
;	linebuffer. Stop accepting chars except 0x0D when end of 
;	buffer is reached.
;=============================================================
getterminalline:
#if !(USB)	;if using USART,  insure RX Interupts, enable TX and RX
	ldi		outchar,(1<<RXCIE|0<<TXCIE|0<<UDRIE| \
					1<<RXEN|1<<TXEN|0<<UCSZ2)
	STORE	UCSRB,outchar
#endif
	ldi		outchar,PROMPT		;display prompt
	GoSub	emitchar
	SetYPtr	LNBUFF+LNBUFFSZ-1	;Initialize line buffer pointer to first char 
anothertermchar:
	GoSub	recvchar			;Get char from terminal
	cpi		inchar,$0D			;is it a CR?
	brne	anothertermchar1	;branch if no CR
	CLRB	GPIOR0,IGNCHAR		;clear the IGNORE CHARACTERS flag
	CLRB	GPIOR0,STRGDET		;clear the STRING DETECTED flag
	Goto	anothertermchar3	;CR, skip ahead
anothertermchar1:
	cpi		inchar,'"'			;Is it '"' character?
	brne	anothertermchar2
	SKBC	GPIOR0,STRGDET		;skip next if STRING DETECTED flag clear
	rjmp	anothertermchar1a	;flag set, clear the STRING DETECTED flag 
	SETB	GPIOR0,STRGDET		;set the STRING DETECTED flag
	rjmp	anothertermchar2	;continue
anothertermchar1a:
	CLRB	GPIOR0,STRGDET		;set the string detected flag

anothertermchar2:
	cpi		inchar,'#'			;Is it '#' character?
	brne	anothertermchar3
	SKBC	GPIOR0,STRGDET		;skip next if STRING DETECTED flag clear
	rjmp	anothertermchar3	;flag set, don't ignore characters
	SETB	GPIOR0,IGNCHAR		;set the IGNORE CHARACTERS flag
	rjmp	anothertermchar3	;continue
;	Goto	anothertermchar		;loop for more characters

anothertermchar3:
	cpi		inchar,$08			;Is it backspace (^H) character?
	breq	deletekey
	cpi		inchar,$7F			;Is it backspace character?
	breq	deletekey
	cpi		inchar,$0A			;Is it linefeed character?
	brne	anothertermchar4	;if not, continue
	Goto	anothertermchar		;ignore linefeeds and get another character
anothertermchar4:
	SKBS	GPIOR0,IGNCHAR		;skip next if IGNORE CHARACTERS flag set
	st		Y,inchar			;Put in line buffer
	cpi		inchar,$0D			;check for CR again
	brne	anothertermchar5	;ranch if not CR
	ret							;Last char in line received
anothertermchar5:
	CPI16	YH,YL,LNBUFF		;end of buffer?
	brne	notlbuffend			;yes, don't store char but beep.
	ldi		outchar,$07			;If not CR at last position, ring the bell
	GoSub	emitchar			;send a BELL character
	Goto	anothertermchar		;loop for more characters
notlbuffend:
	SKBS	GPIOR0,IGNCHAR		;skip next if IGNORE CHARACTERS flag set
	SUBI16	YH,YL,1				;Not end of buffer and not CR, so emit and go another
echoandgo:
	mov		outchar,inchar
	GoSub	emitchar0d
	Goto	anothertermchar
;	
;========================================================================
; DeleteKey: Backup up cursor and erase character with a destructive 
;	backspace.
;	- Enter with Y pointing to the address of the character to be deleted
;		plus 1.
;========================================================================
DeleteKey:
	CPI16	YH,YL,LNBUFF+LNBUFFSZ;end of buffer?
	brne	notexceededbuftop	;no, continue
	ldi		outchar,$07			;If not CR at last position, ring the bell
	GoSub	emitchar
	Goto	anothertermchar
notexceededbuftop:
	cpi		inchar,$7F			;Is it backspace character?
	brne	notexceededbuftop1	;branch if not
	ldi		inchar,$08			;make it a ^H
notexceededbuftop1:
	mov		outchar,inchar
	GoSub	emitchar0d
	ldi		outchar,' '			;emit a space to overwrite the character
	GoSub	emitchar0d
	mov		outchar,inchar		;get ^H back in outchar
	GoSub	emitchar0d
	ADDI16	YH,YL,1				;increment buffer pointer
	ldi		inchar,$0D
	st		Y,inchar			;Put in line buffer
	Goto	anothertermchar
;
;***********************************************************************	
;Interpretelinebuffer:	This is the main entry point for the interpreter
;	executing in immediate mode.  The command line is interpreted from 
;	then line buffer starting at "LNBUFF" in RAM and groeing down.
;***********************************************************************	
Interpretelinebuffer:		;Interpret line
	SetYPtr	LNBUFF+LNBUFFSZ-1;Initialize line buffer pointer to lbufftop 

	ld		inchar,Y		;Get first char from linebuffer
	GoSub	qcast			;the the cast of the character
	cpi		temp,QC_NUM		;is it a numeral (ecode in temp)
	brne	dotheline		;First char is not a numeral so interpret line
	Goto	storeline		;If it's a number, then it's a line number -store it.
dotheline:
	GoSub	crlf
	ori		XL,(1<<VARSTK)	;Flag variable pointers to be left on stack
	rjmp	interpretline	
	
;***********************************************************************	
;InterpretLineV:	This is the main entry point for the interpreter 
;	during "RUN" mode.  This routine is usually called by the AttoBASIC 
;	commands to parse and interpret the remaining characters of the 
;	program line.
;	- It is used to place data on the data stack or execute another 
;		command to place its return data on the stack.
;	- This code is rentrant to the depth of the AVR's hardware stack and
;		the interpreter's data stack.
;***********************************************************************	
InterpretLineV:
	andi	XL,~(1<<VARSTK) 	;clear VARSTK flag
	andi	XH,~(1<<NOEXEC) 	;clear NOEXEC flag

Interpretline:
	LOAD	inbytel,spl			;get Stack-Ptr low byte
	cpi		inbytel,LOW(RAMEND-MCUSTKSZ);check for stack pointer RAM exceeeded
	brsh	stacknotexceeded	;AVR hardware stack not exceeded, continue
	STOREI	ERRCode,$08			;stack error, inform user
	Goto	error

stacknotexceeded:
	GoSub	formword			;get the command word.
	cpi		U,$0D				;CR?
	brne	stacknotexceeded1	;not, branch
	sbrc	XH,RUNACT			;skip next if we're executing interactive
	GoSub	GETPROGLINENO		;fetch and store the 1st line #
	rjmp	endofline			;stop processing

stacknotexceeded1:
	cpi		U,';'				;command separator
	breq	endofline			;yes, stop processing
	mov		temp,currentcast 	;Check to see if its a number	
	cpi		temp,QC_NUM			;is it a number?
	brne	dontmakenumber		;no, don't make it a number
	rcall	makeHTUdecimal		;Its a number so make binary and push on stack
;!!added
;	sbrs	XH,NOEXEC			;skip next if NOEXEC flag set
;!!added
	GoSub	pushU
	rjmp	interpretline		;Keep going until CR is found

dontmakenumber:
	andi 	H,$5F				;Upper-case H,T,U
	andi	T,$5F
	andi	U,$5F	

	cpi		temp,QC_QUOT		;is it a quote (string)?
	breq	notavar				;if so, its not a variable
	cpi		outchar,1			;If its a type 3 (letter) and 1 char long its a variable.
	brne	notavar				;(If outchar = 1 and ecode = 3 its a variable)
	mov		temp,currentcast	;get the last cast
	cpi		temp,QC_LTR			;is it a letter?
	brne 	notavar				;if not a letter then its not a variable
	GoSub	processvariable		;
	rjmp	interpretline

notavar:
	GoSub	getexecword			;lookup the KEYWORD and execute it
;	ld		U,Z					;fetch the next character in the program 
	cpi		U,CR				;is last character a CR
	breq	endofline			;if yes, its the end of the line
	ret							;not the end of the line
					
endofline:
	ori		XL,(1<<VARSTK)	;Flag variable pointers to be left on stack next line
	ret
;	
;********************************************************************************
GetExecWord:	;Find the address of the instruction that matches the three characters in H,T,and U.
				;Execute associated instruction then return.
	SETZPTR (2*commandlist)	;Load high part of byte address into ZH
tryanother:
	GoSub	fetchrombyte	;See if the string matches contents of H and T
	cp		romreg,H
	brne	nomatchH
	GoSub	fetchrombyte
	cp		romreg,T
	brne	nomatchT
	GoSub	fetchrombyte
	cp		romreg,U
	brne	nomatchU		;OK, A match has been found, get the address of the routine.
	adiw	ZH:ZL,1			;Increment Z registers -skip unused 4th instruction name byte
	lpm		temp,Z+			;Get address low into temp
	lpm		ZH,Z			;Get address high into ZH
	mov		ZL,temp			;move address low in ZL
;!!added
;	sbrs	XH,NOEXEC		;skip next if NOEXEC flag set
;!!added
    icall					;Do an indirect jump
    ret						;Then return when function is complete

nomatchH:					;Increment Z registers -skip T
 	adiw	ZH:ZL,1
nomatchT:					;Increment Z registers -skip U
	adiw	ZH:ZL,1	
nomatchU:					;Increment Z registers -skip address (3 bytes)
	adiw	ZH:ZL,(commandlist2-commandlist1-1)
 	Goto	tryanother
;
;********************************************************************************
fetchrombyte:	;Read byte from rom, advance pointer. Enter with ZH, ZL 
				; pointing to PROGROM list.
	lpm		r0,Z+		;Load byte from program memory into r0 and incr Z
 	tst		romreg		;Check if we've reached the end of the list of commands
 	breq	endoflist	;If so,return
	ret
;
endoflist:
	STOREI	ERRCode,$00
	Goto	error
;
;**************************************************************************************************	
formword:	;Put up to three consecutive non-delimiters in registers H,T,and U, right-justified
			;Enter with YH, YL pointing to first char in buffer to be parsed
			;Output: non-delimiters in registers T,H,and U, right-justified, count of 
			;chars in outchar. If three chars are collected, this routine advances
			;YL until the cast changes. Leaves with YH,YL pointing to next char to be parsed.
			;Used also: temp and inbyteh.
		
	ldi	H,$F0	 				;U is always loaded, no need to ininitalize
	ldi	T,$F0					;Pre-load the others
	
	ldi		outchar,$01
	ld		inchar,Y			;Get char from linebuffer into inchar
	GoSub	Qcast				;See what type of char it is.
	cpi		temp,QC_DELIM		;is it a delimiter? (ecode in temp)
	brne	nottype1cast		; **** advancelbpointer only once, just after Qcast
	GoSub	advanclbpointer	
	Goto	formword	

nottype1cast:
	mov		U,inchar 			;Store char and advance pointer
	GoSub	advanclbpointer
	cpi		inchar,$0D			;If its a carriage return, stop and return.
	breq	exitfromword0D

	mov		currentcast,ecode	;Save cast type for reference

Getanotherhcar:
	ld		inchar,Y			;Get char from linebuffer
	cpi		inchar,$0D			;If carriage return, then return
	breq	exitfromword0D
	GoSub	Qcast				;get the cast of the character
	cp		currentcast,ecode 	;If not same as cast of first char, then return.
	brne	exitfromword
	mov		H,T
	mov		T,U
	mov		U,inchar  			;Store char and advance pointer
	GoSub	advanclbpointer
	inc		outchar
	cpi		outchar,$03			;If third character, exit
	brne	Getanotherhcar		;Otherwise, get another

;********************************************************************************
; The code below is to make sure pointer is not left pointing to the end of 
;	a word that is longer than three chars. It is not entered when the pointer 
;	is pointing to the carriage return at the end of the buffer.
;********************************************************************************
ExitFromWord:
	ld		inchar,Y			;Get char from linebuffer
	GoSub	Qcast				;get the cast of the character
	cp		currentcast,ecode	;is it the same?
	breq	notclearedgroup		;yes, continue scanning
exitfromword0D:
	ret							;return to caller
;	
notclearedgroup:
	GoSub	advanclbpointer
	Goto	exitfromword

;********************************************************************************
advanclbpointer:
	;Decrements YL, Jumps to error routine if fetch is requester after 
	;pointer is moved past the end of the buffer.
	CPI16	YH,YL,LNBUFF-1		;end of buffer?
	brne	nohitend

	STOREI	ERRCode,$04
	Goto	error
	ret

nohitend:
	SUBI16	YH,YL,1				;decrement buffer pointer
	ret
;
;********************************************************************************
;///////////////INSERT LINE ROUTINES//////////////////////////////
#if TEST
showhtu:
	ldi		outchar,$20
	GoSub	emitchar
	ldi		outchar,$20
	GoSub	emitchar
	ldi		outchar,$20
	GoSub	emitchar
	ldi		outchar,$20
	GoSub	emitchar
	mov		inbytel,H	;**** only for test 
	GoSub	sendbyte
	ldi		outchar,$20
	GoSub	emitchar
	mov		inbytel,T	;**** only for test 
	GoSub	sendbyte
	ldi		outchar,$20
	GoSub	emitchar
	mov		inbytel,U	;**** only for test 
	GoSub	sendbyte
	ldi		outchar,$20
	GoSub	emitchar
	ret

breadcrumb:			; **** test only
	ldi		outchar,$20
	GoSub	emitchar
	ldi		outchar,$20
	GoSub	emitchar
	mov		inbytel,ZL		;**** only for test 
	GoSub	sendbyte
	ldi		outchar,$20
	GoSub	emitchar
	ret
#endif
;	
;*************************************************************************
; Qcast:	Determine whether a character is a letter, numeral, delimiter, 
;	or other (operator).
;	- Enter with character in inchar.
;	- Returns with cast code in ecode.
;	- Uses: inchar
;	Value in ecode = cast
;		0 = Operator (not one of the other casts
;		1 = Delimiter -space ($20) or comma ($2C)
;		2 = Numeral 0..9 ($30 through $39)
;		3 = Letter -A..Z uppercase ($41 throught $5A)
;		4 = Carriage return
;		5 = quote character (string support) or "@" for a constant
;	Tests are made after anding the input byte with $5F
;*************************************************************************
Qcast:		
	push	inchar		;save original character for later
	clr		temp		;Default is type 0
	
Qcast1:
	cpi		inchar,$0D	;Is it a carriage return? -check this before uppercasing
	brne	not0D
	Goto	makecast4
not0D:
	andi	inchar,$5F	;Make upper-case
	cpi		inchar,(' ' & $5F)	;Is it a space?
	brne	notaspacecode
	Goto	makecast1
notaspacecode:
	cpi		inchar,(',' & $5F)	;Is it a comma?
	brne	notacommacode
makecast1:
	ldi		temp,QC_DELIM
	rjmp	Qcast_out
notacommacode:
	cpi		inchar,('"' & $5F)	;Is it a quote?
	brne	notaquotecode
	ldi		temp,QC_QUOT		;its a quote
	rjmp	Qcast_out
notaquotecode:
	cpi		inchar,('@' & $5F)	;Is it an fetch?
	brne	notafetchcode
	ldi		temp,QC_QUOT		;its a "@", consider a string
	rjmp	Qcast_out
notafetchcode:
	cpi		inchar,$10
	brpl	notunder10	
	rjmp	Qcast_out
notunder10:
	cpi		inchar,$5B
	brmi	notover5B	
	rjmp	Qcast_out
notover5B:
	cpi		inchar,$40
	brmi	notover40
	ldi		temp,QC_LTR			;its a letter
	rjmp	Qcast_out
notover40:	
	cpi		inchar,$1A	
	brpl	notunder1A
	brpl	Qcast_out
	ldi		temp,QC_NUM			;its a number
notunder1A:
	rjmp	Qcast_out
;
makecast4:
	ldi		temp,QC_CR			;its a CR
;
Qcast_out:
	mov		ecode,temp
	pop		inchar
	ret
;	
;=============================================================
sendbyte:      ;Send byte contained in inbytel to terminal
	GoSub	byte_to_asciihex	
	push	inbytel				;save low digit 'cause USB kills inbyteh:l
	mov     outchar,inbyteh
	rcall	emitchar
	pop		outchar				;restore low digit
	rcall  	emitchar
	ret	
;
;=============================================================
emitchar:	;called with character to emit in 'outchar' register
#if ( USB && ( defined(__ATmega32U4__) || defined(__AT90USB1286__) ) )
	LOAD	inbytel,usb_configuration;get status of USB
	tst		inbytel				;is it 0?
	brne	PC+2				;if not, USB is config'd
	ret
	;TX a character via USB 
	GoSub	Checkkeybd 			;see if control-c or control-s is pressed
	sbrs	XH,SUSP				;if XH:SUSP (suspend) is clear, don't wait for key
	Goto	waitdone

waitforkey:	
	push	r18					;save registers clobbered by USB
	push	r1					;save r1
	clr		r1					;insure r1 = 0
	call	usb_serial_available	;check for character(s) available
	pop		r1					;restore r1
	pop		r18
	tst		inbytel				;test for zero characters received
	breq	waitforkey			;zero characters received, loop
	andi	XH,~(1<<SUSP)		;clear the suspend flag

waitdone:
	mov		inbytel,outchar		;copy character to USB register
	push	r1					;save r1
	clr		r1					;insure r1 = 0
	push	r20					;USB vlobbers r20
	call	usb_serial_putchar	;put char in USB buffer
;	call	usb_serial_putchar_nowait;put char in USB buffer
	pop		r20					;restore r20
	pop		r1					;restore r1
	tst		inbytel				;success = 0
	brne	waitdone			;loop till character is buffered
	ret							;return to caller
;
#else
	SKBS	UCSRA,UDRE,romreg	;skip next if Data Register Empty
	Goto	emitchar			;loop till Data Register Empty
	GoSub	Checkkeybd			;see if control-c or control-s is pressed

	sbrs	XH,SUSP				;if XH:SUSP (suspend) is clear, don't wait for key
	Goto	waitdone

waitforkey:	
	SKBS	UCSRA,RXC,romreg	;skip next if RX char waiting
	rjmp 	waitforkey			;wait for a key before continuing
	andi	XH,~(1<<SUSP)		;clear the suspend flag

waitdone:
	STORE	UDR,outchar			;send the byte
	ret							;return to caller
#endif
	
;=============================================================
crlf:   
	ldi		outchar,$0D	;Send carriage return and line feed to terminal
	GoSub	emitchar
	ldi		outchar,$0A
	GoSub	emitchar
	ret

;=============================================================
emitchar0D:			;Send outchar to terminal. Add line feed to carriage return.
	cpi		outchar,$0D			;Its a carraiage return, send a linefeed also
	brne	notareturn
	GoSub	crlf
	ret
notareturn:   
	Goto	emitchar

;=============================================================
recvchar:     	;Receive a byte from the terminal         
	SETB	SMCR,SE,temp		;insure SE bit is set
	sei							;insure global interrupts allowed
	sleep						;go to to sleep
	nop
#if ( USB && ( defined(__ATmega32U4__) || defined(__AT90USB1286__) ) )
	LOAD	inbytel,usb_configuration;get status of USB
	tst		inbytel				;is it 0?
	breq	recvchar			;USB not config'd, loop

	;TX a character via USB 
	push	r18					;save registers clobbered by USB
	push	r19
	push	r1					;save r1
	clr		r1					;insure r1 = 0
	call	usb_serial_getchar	;get the character
	pop		r1					;restore r1
	pop		r19					;restore registers
	pop		r18
	tst		inbyteh				;zero is good character
	brne	recvchar			;not RX, loop
	mov		inchar,inbytel		;mov byte to inchar register
#else
	SKBS	UCSRA,RXC,temp		;skip next if RX ready with character
	Goto	recvchar			;not RX, loop
	LOAD	inchar,UDR			;Read byte
#endif
	ret			
;
;***********************************************************************
;read the 6-digit packed BCD in registers H, T and U
;and convert to 6,4 or 2 ASCII characters sending to the serial 
;link (char stored in 'outchar')
;***********************************************************************
;
SEND_6:
	mov		outchar,H			;get digits 6 & 5
	GoSub	SEND_2BCD			;send them
SEND_4:
	mov		outchar,T			;get digits 4 & 3
	GoSub	SEND_2BCD			;send them
SEND_2:
	mov		outchar,U			;get digits 2 & 1
	GoSub	SEND_2BCD			;send them
	ret
;
;***********************************************************************
;convert "digits" stored in 'outchar' to two ASCII BCD digits and send 
;  them out the serial link
;***********************************************************************
SEND_2BCD:
	push	outchar				;save a copy
	andi	outchar,0xf0		;strip off low-nibble
	swap	outchar				;put into low-nibble
	subi	outchar,-(0x30)		;make it ASCII
	GoSub	emitchar			;send 10's digit
	pop		outchar				;retrieve the copy
	andi	outchar,0x0f		;strip off low-nibble
	subi	outchar,-(0x30)		;make it ASCII
	GoSub	emitchar			;send 1's digit
	ret
;
;=============================================================
; Send a string terminated in cariage return and line feed
; Called with location of start of string in Z               
;=============================================================
sendlromline:	
	GoSub	sendromstring
	GoSub	crlf
	ret
;
;=============================================================
;sendromstring: parses and prints a string from PROGMEM.
;	Called with location of string in Z
;	Parses the escape tokens for "repeat" and "center"
;=============================================================
sendromstring:
#if ( USB && ( defined(__ATmega32U4__) || defined(__AT90USB1286__) ) )
	;TX a character via USB 
;	LOAD	temp,usb_configuration ; get USB config state
;	tst		temp				;zero? (USB not configured)
;	breq	sendromstring		;loop till not zero
#endif

sendromstring1:
	lpm		outchar,Z+			;get first character
	tst		outchar				;test for NULL, is end of message
	breq	finishsendstering	;exit if NULL
	cpi		outchar,TOK_CTR		;is it a "center" token?
	brne	sendromstring2		;branch if not
	GoSub	calcromstrlen		;get length of string
	Goto	sendromstring1		;finish processing string

sendromstring2:
	cpi		outchar,TOK_REP		;is it a repeat token?
	brne	sendromstring3		;branch if not
	lpm		r1,Z+				;get character counter into r1	
	lpm		outchar,Z+			;get character to repeat
	GoSub	repeatchar			;print it
	Goto	sendromstring1

sendromstring3:
	cpi		outchar,TOK_CRLF	;is it a CR/LF token?
	brne	sendromstring4		;branch if not
	GoSub	crlf				;send CR/LF
	Goto	sendromstring1

sendromstring4:	;this section of code calls itself
	cpi		outchar,TOK_ERR		;is it a "error" string token?
	brne	sendromstring5		;branch if not
	PUSHZ						;save Z-ptr
	SETZPTR (2*emessage)		;Load Z-ptr w/ new message address
	GoSub	sendromstring		;send the string
	POPZ						;restore previous Z-ptr
	Goto	sendromstring1

sendromstring5:
	GoSub	emitchar			;send the character
	Goto	sendromstring1		;loop for next character

finishsendstering:
	ret							;return to caller
;
;=============================================================================
; Calculates the number of printable characters in the string pointed to by Z.  
;  Exits with Z pointing to the 1st printable character.  
;
calcromstrlen:
	PUSHZ						;save Z-Ptr
	clr		r1					;zero character counter
calcromstrlen1:
	lpm		outchar,Z+			;get next character
	cpi		outchar,LF			;is it a LF?
	breq	calcromstrlen5		;is a LF, branch
	cpi		outchar,CR			;is it a CR?
	breq	calcromstrlen5		;is a CR, branch
	cpi		outchar,TOK_CRLF	;is it a CR/LF token?
	breq	calcromstrlen5		;is a TOK_CRLF token, branch
	cpi		outchar,TOK_REP		;is it a repeat token?
	breq	calcromstrlen5		;is a TOK_REP token, branch
	inc		r1					;increment character counter
	Goto	calcromstrlen1		;loop for more

calcromstrlen5:	;r1 contains string length
	dec		r1					;decr character counter by 1
	ldi		outchar,LNLEN		;get max line length
	sub		r1,outchar			;subtract for difference
	neg		r1					;adjust for negative offset
	lsr		r1					;divide by two for offset

	ldi		outchar,' '			;load "space" character
	GoSub	repeatchar			;print the characters

	POPZ
	ret
;
;=============================================================================
; Repeatedly prints the character held in the "outchar" register by the number
;	of times held in the r1 register
;
repeatchar:
	GoSub	emitchar			;send the character
	dec		r1					;decrement counter
	brne	repeatchar			;loop till zero
	ret
;
;***********************************************************************	
; ProcessVariable:	;Make variable in U into a pointer and
;	- if XL bit 0 is 1, put pointer on stack (assignment to variable), or
;	- if XL bit 0 is 0, put the value of variable on stack.
;***********************************************************************	
ProcessVariable:
	subi	U,'A'				;subtract ASCII 'A' to make into an offset		
	cpi		U,(VARSTORSZ)		;Test for range
	brmi	variableok
	STOREI	ERRCode,$05			;not a good variable, inform user
	Goto	error
variableok:	;at this point, the variable is an offset into the variable space
	sbrs	XL,VARSTK			;skip next if we're to leave the value
	brne	notcodezero			;
	GoSub	pushU				;Put pointer offsett onn stack (ecode = $00)
	ori		XL,(1<<VPTRSTK) 	;pointer pushed the stack, set VPTRSTK flag
	ret
notcodezero:		
	SetZPtr	VARSTOR				;Z = pointer to variable space
	clr		T					;clear high register
	ADD16	ZH,ZL,T,U			;add offset to variable address
	ld		U,Z	
	GoSub	pushU
	andi	XL,~(1<<VPTRSTK) 	;value on stack, clear VPTRSTK flag
	ret
;
;**********************************************************
pushU:	;Push contents of U on datastack, increment 
		; offset pointer
		; Preserves Z
	push	temp				;save register
	ldi		temp,DATASTKSZ		;stack depth
	cp		temp,DStkPtr		;stack overflow?
	brne	nopusherror			;no, continue
	STOREI	ERRCode,$06			;stack overflow
	Goto	error

nopusherror:
	push	r1					;save r1
	clr		r1					;zero register
	SetZPtr	(DATASTK+DATASTKSZ)-1;Z = TOS - 1
	SUB16	ZH,ZL,r1,DStkPtr	;subtract offset
	st		Z,U					;save U on data stack
	inc		DStkPtr				;increment offset pointer
	inc		cmdparms			;increment relative pointer
	pop		r1					;restore r1
	pop		temp				;restore register
	ret
;	
;**********************************************************
popU:	;Pop contents of U from datastack, decrement 
		; offset pointer
		; Preserves Z
	tst		DStkPtr				;is DStkPtr = 0 ?
	brne	nopoperror			;no, continue
	STOREI	ERRCode,$07
	Goto	error

nopoperror:
	push	r1					;save r1
	clr		r1					;zero register
	SetZPtr	(DATASTK+DATASTKSZ)	;Z = TOS
	SUB16	ZH,ZL,r1,DStkPtr	;subtract offset
	ld		U,Z					;save U on data stack
	dec		DStkPtr				;deccrement offset pointer
	dec		cmdparms			;decrement relative pointer
	pop		r1					;restore r1
	ret
;	
;**********************************************************
; pushGS: Push the address held in YH:YL onto the 
;	GOSUB-RET stack, decrement pointer
;	- Next available address is left in GoSubStkPtr
;	- Preserves Z
;**********************************************************
pushGS:
	PUSHZ						;save Z
	ldi		temp,GOSUDEPTH		;stack depth
	cp		temp,GoSubStkPtr	;stack overflow?
	brne	pushGS1				;no, continue
	STOREI	ERRCode,$08			;stack overflow!
	Goto	error

pushGS1:
	ldi		inchar,GOSUCHUNK	;set GOSUCHUNK size
	mov		temp,GoSubStkPtr	;copy DStkPtr in temp
	mul		temp,inchar			;multiply for pointer
	SetZPtr	(GOSUSTK+GOSUSTKSZ);Z = TOS
	SUB16	ZH,ZL,r1,r0			;subtract offset
	st		-Z,YH				;save it
	st		-Z,YL
	inc		GoSubStkPtr			;increment offset pointer
	POPZ						;restore Z
	ret
;	
;***********************************************************
; popGS: Pop contents of GOSUB-RET address from the Gstack
;	and return its address in YH:YL.  Increment the pointer.
;	- "GoSubStkPtr" register holds the address of the high
;		byte of the next return address.
;	- Preserves Z
;***********************************************************
popGS:
	PUSHZ						;save Z
	tst		GoSubStkPtr			;GoSubStkPtr = 0?
	brne	popGS1				;no, continue
	STOREI	ERRCode,$07			;stack under flow!
	Goto	error

popGS1:
	ldi		inchar,GOSUCHUNK	;set GOSUCHUNK size
	mov		temp,GoSubStkPtr	;copy DStkPtr in temp
	mul		temp,inchar			;multiply for pointer
	SetZPtr	(GOSUSTK+GOSUSTKSZ)	;Z = TOS
	SUB16	ZH,ZL,r1,r0			;subtract offset
	ld		YL,Z+				;load it
	ld		YH,Z
	dec		GoSubStkPtr			;decrement offset pointer
	POPZ						;restore Z
	ret
;	
;**********************************************************
; pushLP: Push the address held in YH:YL onto the 
;	LOOP stack as well as the contents of the "currlooptgt" 
;	and "currloopvar" registers.  Decrement pointer.
;	- "LOOPStkPtr" register holds the address of the high
;		byte of the return address.
;	- Preserves Z
;**********************************************************
pushLP:
	PUSHZ						;save Z
	ldi		temp,LOOPDEPTH		;stack depth
	cp		temp,LOOPStkPtr		;stack overflow?
	brne	pushLP1				;no, continue
	STOREI	ERRCode,$08
	Goto	error

pushLP1:
	ldi		inchar,LOOPCHUNK	;set LOOPCHUNK
	mov		temp,LOOPStkPtr		;copy LOOPStkPtr in temp
	mul		temp,inchar			;multiply for pointer
	SetZPtr	(LOOPSTK+LOOPSTKSZ);Z = TOS
	SUB16	ZH,ZL,r1,r0			;subtract offset
	st		-Z,loopreturnL		;save return address
	st		-Z,loopreturnH
	st		-Z,currlooptgt		;store loop target value
	st		-Z,currloopvar		;store loop variable
	inc		LOOPStkPtr			;increment LOOP index pointer
	POPZ						;restore Z
	ret
;	
;**********************************************************
; popLP: Pop contents of LOOP address from the LOOP stack
;	and return its address in YH:YL.  Restore contents of
;	the "currlooptgt" and "currloopvar" registers and 
;	increment the pointer.
;	- "LOOPStkPtr" register holds the address of the high
;		byte of the next return address.
;	- Preserves Z
;**********************************************************
popLP:
	PUSHZ						;save Z
	tst		LOOPStkPtr			;LOOPStkPtr = 0?
	brne	popLP1				;no, continue
	STOREI	ERRCode,$07
	Goto	error

popLP1:
	ldi		inchar,LOOPCHUNK	;set LOOPCHUNK
	mov		temp,LOOPStkPtr		;copy LOOPStkPtr in temp
	dec		temp				;LOOPStkPtr less 1
	mul		temp,inchar			;multiply for pointer
	SetZPtr	(LOOPSTK+LOOPSTKSZ)	;Z = TOS
	SUB16	ZH,ZL,r1,r0			;subtract offset
	ld		loopreturnL,-Z		;fetch high address
	ld		loopreturnH,-Z		;fetch low address
	ld		currlooptgt,-Z		;fetch loop target
	ld		currloopvar,-Z		;fetch loop variable
;	dec		LOOPStkPtr			;decrement offset pointer
	POPZ						;restore Z
	ret
;	
;/////////////START DATA FORMAT CONVERSION ROTINES//////////////////

bitpositiontoormask:	;Convert bit position (0..7) to OR mask value
			;Enter with bit position on top of dstack, exit with
			;mask on top of stack
			
	GoSub	popU		;Get bit position
	ldi		temp,7
	cp		temp,U
	brmi	toomanybits			
	ldi		temp,1		;Put 0000 0001 pattern into temp for shifting
rotatesomemore:
	tst		U			;is it a zero?
	breq	finishedshifting
	lsl		temp
	dec		U
	Goto	rotatesomemore
finishedshifting:
	mov		U,temp
	GoSub	pushU
	ret

toomanybits:
	STOREI	ERRCode,$0A
	Goto	error
;
;****************************************************************
;SendHTUasdecimal:	Send decimal number in H,T,U 
;	registers (hundreds,tens,units)
;****************************************************************
SendHTUasdecimal:
			;As numerals.
	cpi		H,0
	breq	dontsendh
	subi	H,-48
	mov		outchar,H
	GoSub	emitchar
	cpi		T,0
	brne	dontsendh
	ldi		outchar,$30	;If U=0 then don't emit this zero
	GoSub	emitchar
dontsendh:	
	cpi		T,0
	breq	dontsendt
	subi	T,-48
	mov		outchar,T
	GoSub	emitchar
dontsendt:
	subi	U,-48	
	mov		outchar,U
	GoSub	emitchar
	ret
;
;*************************************
TypeGreeting:				;Type greeting
	SETZPTR	(2*hellomessage)	;Load high part of byte address into ZH
	GoSub	sendlromline		;sent it.
	ret
;
;*************************************
LISTcommand:	;lists program memory buffer to screen..
	GoSub	crlf
	SetZPtr	(PROGMEM+PROGMEMSZ)-1;Initialize PC position inidcator

	LOAD16	YH,YL,PCLO		;get PC into Y

anotherchar:
	CP16	ZH,ZL,YH,YL
	breq	endofpmem
skip1:
	ld		outchar,Z	;Put contents of line buffer into outchar;	
	GoSub	emitchar0D
	sbiw	ZH:ZL,1
	Goto	anotherchar
endofpmem:
	GoSub	crlf
	;(listcommand flows into sizecommand)

;*************************************
FREEcommand:	;Find remaining program memory space. Uses U.
	GoSub	crlf				;send CR/LF

;print USED bytes first
	LOADi16	inbyteh,inbytel,PROGMEM+PROGMEMSZ-1;Initialize PC position inidcator
	LOAD16	T,H,PCLO			;get PC
	SUB16	inbyteh,inbytel,T,H	;get used program memory

	GoSub	BIN2BCD16			;convert to 6-digit BCD
	GoSub	SEND_6				;send the digits

	SETZPTR (2*usedmem)			;Load ROMSTRING to Z-pointer 
	GoSub	sendromstring		;sent it.

;print bytes free next
	LOAD16	inbyteh,inbytel,PCLO;get PC
	SUBI16	inbyteh,inbytel,PROGMEM;calculate free bytes

	GoSub	BIN2BCD16			;convert to 6-digit BCD
	GoSub	SEND_6				;send the digits

	SETZPTR (2*freemem)			;Load ROMSTRING to Z-pointer 
	GoSub	sendromstring		;sent it.

	ret
;
;*************************************
PRINTcommand:			;Print TOS after rcall, to screen
	ori		XH,(1<<PRIACT)		;set the "print active" flag
	GoSub	interpretlinev
	ldi		temp,5				;cast code for a quote character
	cp		currentcast,temp	;was the last character a quote?
	breq	PRINTcommand9		;yes, don't proccess any numbers
	GoSub	popU
	GoSub	binarytodecimal
	GoSub	sendHTUasdecimal	
	GoSub	crlf
PRINTcommand9:
	andi	XH,~(1<<PRIACT)		;clear the "print active" flag
	ret
;
;******************************************************
printhexcommand:			;Print TOS in hex, after rcall, to screen
	ori		XH,(1<<PRIACT)		;set the "print active" flag
	GoSub	interpretlinev
	ldi		outchar,$24
	GoSub	emitchar
	GoSub	popU
	mov		inbytel,U
	GoSub	sendbyte	
	GoSub	crlf
	andi	XH,~(1<<PRIACT)		;clear the "print active" flag
	ret
;	
;***********************************************   
PRBcommand:	;print in binary format
	ori		XH,(1<<PRIACT)		;set the "print active" flag
	GoSub	Get1stParm			;get the last parameter
	ldi		temp,8				;load bit counter
stillsendingbinary:	
	ldi		outchar,'0'			;bit is a "0"
	rol		U					;rotate bit in CARRY flag
	brcc	dontsendone			;if CARRY clear, keep the "0"
	ldi		outchar,'1'			;bit is a "1"
dontsendone:
	rcall  	emitchar			;send the character
	dec 	temp				;decrement bit counter
	brne	stillsendingbinary 	;loop till 0
	GoSub	crlf   				;send a CR/LF combo
	andi	XH,~(1<<PRIACT)		;clear the "print active" flag
	ret							;return to caller
;
;******************************************************
STRINGcommand:			;Print sting enclosed in quotes
	ld		outchar,Y			;get character
	GoSub	advanclbpointer		;advance past character
	cpi		outchar,'"'			;is it the end quote mark?
	breq	STRINGcommand9		;yes, exit
	cpi		outchar,CRLFCHAR	;is it the CR/LF indicator?
	breq	STRINGcommand1		;yes, print CR/LF combo
	GoSub	emitchar			;print the character
	rjmp	STRINGcommand

STRINGcommand1:
	GoSub	crlf				;send a CR?LF combo
	rjmp	STRINGcommand		;loop for more

STRINGcommand9:
	ret							;return to caller
;
;******************************************************
setequalscommand:		;Get value at TOS after rcall to variable on stack when 
				;equals command is rcalled.
	GoSub	GetTwoParm			;get two parameters from dstack
	SetZPtr	VARSTOR				;Z = start of variable space
	clr		T					;clear high register
	ADD16	ZH,ZL,T,U			;add offset to pointer
	st		Z,temp
	ret
;
;******************************************************
evaluatecommand:;Equal sign	;If XL,2 = 0, perform EQUAL command. 
	sbrs	XH,FNLEQ		;Skip next if flag set up FOR-NEXT loop
	Goto	EQUALcommand	;Its functionally identical to subtract.
				;Set up FOR-NEXT loop

	GoSub	Get1stParm			;get the last parameter
	mov		currlooptgt,U		;Store loop target
	
	GoSub	popU				;Pop initial counter value
	mov		temp,U				;Put in temp for now

	GoSub	popU				;Pop pointer to counter
	
	mov		currloopvar,U		;Copy variable pointer into currloopvar register

	clr		T					;clear high register
	SetZPtr	VARSTOR				;point to variable sapce
	ADD16	ZH,ZL,T,U			;add to obtain variable's position
	st		Z,temp				;Store initial counter value into counter
	
	ori		XL,(1<<LPCAPA)		;Set flag asking runcommand to capture loop address
	andi	XL,~(1<<VARSTK) 	;Clear the VARSTK flag
	andi	XH,~(1<<FNLEQ)		;Clear the FNLEQ flag
	ret
;
;*************************************	
IFcommand:	;Set flag to skip next line if number on TOS returned from rest 
			; of line is not zero
	GoSub	Get1stParm			;get the last parameter
	tst		U
	brne	itsnotzero
	ori		XL,(1<<SKPNXT)		;set skip next line # flag
itsnotzero:
	ret
	
;**************************************************************	
;THENcommand: Return to previous rcalling routine using by 
;	popping the previous commands return address off the stack.	
;**************************************************************	
THENcommand:
;	GoSub	interpretlinev
;test ^^^^^
;.if USBSUPP	;USB interrupts will screw us up!
;	GoSub	D_USBINT			;disable USB ints
;.endif
	pop		temp				;pop prior 16-bit return address off stack
	pop		temp
.if (FLASHEND > 0xFFFF)			;for 64KW+ parts
	pop		temp				;pop 3rd byte of return address 
.endif
;.if USBSUPP
;	GoSub	E_USBINT			;re-enable USB ints
;.endif
	ret							;Pop up to previous rcall. Don't try this at home.
;
;*************************************	
FORcommand:						;Initial statement in FOR-NEXT structure
								;Sets flag to switch "=" routine to set up the
								;FOR-NEXT loop
	ori		XH,(1<<FNLEQ)		;Set XL:FNLEQ flag to process variable
	ret
;
;***********************************************************
;NEXTcommand: This is the distant end of the FOR-NEXT loop.
;	Tests the loop variable to see its equal target.
;	- If its equal to target, go to next line
;	- If not equal target, increment variable and 
;		Signal RUNCOMMAND to jump to loop return
;***********************************************************
NEXTcommand:
	SetZPtr	VARSTOR				;Fetch the the loop variable 
	mov		U,currloopvar		;from variable space
	clr		T					;clear high register
	ADD16	ZH,ZL,T,U			;add for offset into variable space
;	ldi		ZH,HIGH(VARSTOR)	;This is fetching the the loop 
;	mov		ZL,currloopvar		; variable from variable space
	ld		temp,Z				;Get loop variable value into temp
	cp		temp,currlooptgt
	breq	NEXTcommand1		;If they are equal, the loop's done
	inc		temp
	st		Z,temp				;Increment loop variable and 
								;  put it back into the register			
	ori		XL,1<<LPJAMN		;Set XL:LPJAMN to flag run command 
								; to jump to return address next time
	ret

NEXTcommand1:	;finished loop, check for more levels on stack
	dec		LOOPStkPtr			;decrement offset pointer
	breq	NEXTcommand3		;no more loops to process		;
	rcall	popLP				;restore prior LOOP info

NEXTcommand3:
	ret
;
;************************************************************************
; FETCHcommand:   This is a dummy command for human ease of identifying
;	an internal variable name.  Once called, control is passed again to
;	then line interpreter, which will invoke the variable name, another 
;	AttoBASIC command, to return the symbol's low-byte addess.
;************************************************************************
.if	INTREG
  FETCHcommand:					;fall through to the "TO" command
.endif
;
;************************************************************************
; TOcommand: To in FOR-NEXT structure. This is a dummy command
;************************************************************************
TOcommand:
	GoSub	interpretlinev
	ret
;
;*************************************	
ENDcommand:			;Stop execution by setting flag
	ori		XH,(1<<HALT)		;Set flag telling runcommand to stop.
	ret
;	
;*************************************	
HEXcommand:	;Nasty and dangerous - get two following chars as hex put on stack.
	ld		inbyteh,Y			;get 1st character
	GoSub	advanclbpointer		;advance past 1st character
	ld		inbytel,Y			;get next character
	mov		inchar,inbytel		;copy for qcast
	GoSub	qcast				;get cast
	cpi		temp,QC_DELIM		;is it a delimiter?
	breq	hexcommand1			;yes, only 1 character
	cpi		temp,QC_CR			;is it a CR?
	breq	hexcommand1			;yes, only 1 character
	GoSub	advanclbpointer		;advance past 2nd character
	Goto	hexcommand2			;convert and push on stack
hexcommand1:
	mov		inbytel,inbyteh		;only one char, set up for conversion
	clr		inbyteh
hexcommand2:
	GoSub	asciihex_to_byte
	mov		U,inbytel
	GoSub	pushU
	GoSub	interpretlinev
	ret 	
;	
;*******************************************************************	
; BINcommand:	Returns interprets the 8 character strings as a 
;	binary value and places it on the data stack.
;	- Hack: Because the command parser interprets the '`' and '@'
;		characters as the same, we need to determine which one 
;		caused a call to this routine and branch of necessary.
;*******************************************************************	
BINcommand:
.if INTREG	;only if internal registers enabled
	ldd		temp,Y+1			;get the character @Y+1 in temp
	cpi		temp,$27			;is it the BIN command?
	breq	BINcommand1			;yes, continue
	Goto	VPGcommand			;transfer control to the VPG command
.endif

BINcommand1:
	ldi		temp,8				;load bit counter
	mov		bytecntr,temp

BINcommand2:
	ld		inbytel,Y			;get 1st character
	GoSub	advanclbpointer		;advance past character
	mov		inchar,inbytel		;copy for qcast
	GoSub	qcast				;get cast
	mov		currentcast,temp	;copy to currentcast
	cpi		temp,QC_NUM			;is it a number?
	brne	BINcommand9			;no, error

	clc							;clear carry bit
	andi	inchar,$0F			;convert to a number
	tst		inchar				;is it a zero?
	breq	BINcommand2a		;yes, branch
	sec							;set carry bit 

BINcommand2a:
	rol		U					;rotate bit into U
	dec		bytecntr			;decrement bit counter
	brne	BINcommand2			;loop till zero

	GoSub	pushU
	GoSub	interpretlinev
	ret 	

BINcommand9:
	STOREI	ERRCode,$1C			;not a 0/1 error, inform user
	Goto	error
;	
;****************************************************************************   
;delaycommand:  Delays "N" x 10mS (10 to 2550 mS) based on the value passsed.
;	If the RTC routines are enabled, sleep mode is entered and the RTC 
;	  is used as the interrupt source.
;	If the RTC routines are not enabled, a simple software driven loop
;	  is used instead.
;	USES: U, temp and Y-ptr.
;****************************************************************************
DELAYcommand:			;Delay "N" x 10mS
	PUSHY						;save Y reg
	GoSub	Get1stParm			;get the last parameter

delaycommand1:
.if RTC
	ldi		temp,10/(1000/RTC_IPS);"n" x RTC interrupts
delaycommand2:
	sleep						;sleep for now
	nop							;for benefit of SLEEP instr.
	dec		temp
	brne	delaycommand2		;loop till 10 interrupts passed
.else
	_WAIT_mS	f_clk,9			;wait 9mS
	_WAIT_10us	f_clk,97		;wait 970uS
.endif
	dec		U					;decrement variable
	brne	delaycommand1		;loop till zero
	POPY						;restore Y reg
	ret
;
;***********************************************   
OSCcommand:	;Set/read AVR's oscillator calibration byte
	LOAD	U,OSCCAL			;get the current value of OSCCAL
	GoSub	pushU				;save on stack
	GoSub	Get1stParm			;get the last parameter
	STORE	OSCCAL,U			;save the value to OSCCAL
	LOAD	U,OSCCAL			;get the current value of OSCCAL
	GoSub	pushU				;save on stack
	ret
;
;***********************************************   
AOVcommand:
	clr		U					;disabled without a parameter
	GoSub	pushU				;save on stack
	GoSub	Get1stParm			;get the last parameter
	ldi		temp,$1				;only values of [0..1] allowed
	cp		temp,U
	brmi	AOVcommand9			;branch if allowable exceeded 
	CLRB	GPIOR1,AOVErr,temp	;enable error detection
	sbrs	U,0					;skip next if bit 0 set
	ret							;return to caller
	SETB	GPIOR1,AOVErr,temp	;disble error detection
AOVcommand1:
	ret							;return to caller

AOVcommand9:
	STOREI	ERRCode,$0C		;error code "C"
	Goto	error
;
;*********************************************************************
;RSTcommand: causes a system reset using the watchdog timer
;	set to "System Reset" mode.  The time delay defaults to 32mS.
;*********************************************************************
RSTcommand:
	CLRB	MCUSR,WDRF,temp		;clear WDRF flag

.if USBSUPP
	GoSub	usb_shutdown		;insure USB interface has been terminated
								; before restart.
	GoSub	Delay1S				;wait 1 Second before restart
.endif

	;set the WD to "System Reset Mode" (1<<WDE | 0<<WDIE)
	STOREI	WDTCSR,(1<<WDCE|1<<WDE),temp	;enable access
	STOREI	WDTCSR,(0<<WDIE|1<<WDE|6<<WDP0),temp	;set it to 1.0S
	cli							;disable global IRQ's

RSTcommand1:
	sleep						;let it happen
	nop
	ret
;
;******************************************************************   
; makeHTUdecimal: Make decimal digits in H,T,and U into a binary 
;	number in U.
;******************************************************************   
makeHTUdecimal:	
	andi	H,$0F
	andi	T,$0F
	andi	U,$0F
	GoSub	decimaltobinary	
	ret
;
;***********************************************   
getlineno:	;Search buffer for line number, set up for runcommand to jump to it.
			;Can use Y because will will be modified by this routine
			;before runcommand uses it again anyway.
	GoSub	Get1stParm			;get the last parameter
	mov		outchar,U		;temporarily put target line number into outchar

	SetZPtr	(PROGMEM+PROGMEMSZ)-1;Initialize PC position inidcator

	LOAD16	inbyteh,inbytel,PCLO	;save current PC in inbyteh:inbytel
	Goto	pufistln		;Jump in the middle so first line no. can be found
	
searchCR:
	cp		ZH,inbyteh
	brne	skip2
	cp		inbytel,ZL		;Error if past end of buffer
	brsh	gotonotfound
skip2:
	ld		temp,Z
	cpi		temp,$0D
	breq	checkforlinumb	;Take the branch if CR is found
	sbiw	ZH:ZL,1
	Goto	searchCR	;Else continue to look for CR

checkforlinumb:			;Found a CR now make line number
	sbiw	ZH:ZL,1		;Move pointer past CR
pufistln:
	clr	U
	clr	T
	clr	H
		
fetchanothernum:
	cp		ZH,inbyteh
	brne	skip3
	cp		inbytel,ZL		;Error if past end of buffer
;	brpl	gotonotfound
	brsh	gotonotfound
skip3:
	ld		inchar,Z	;put char pointed to by Z into inchar
	GoSub	qcast		;Find out what kind of char this is
	cpi		temp,QC_NUM	;is it a numeral? (ecode in temp)
	brne	notanumeral
	mov		H,T		;Ita a numeral so shift it in
	mov		T,U
	mov		U,inchar
	sbiw	ZH:ZL,1	
	Goto	fetchanothernum
notanumeral:	;The pointer is not pointing to a numeral, so test the number.
	
	rcall	makeHTUdecimal		;Its a number so make binary and push on stack
;	GoSub	decimaltobinary		;Change from 3 BCD bytes to one binary vaue	
	cp		outchar,U
	brne	searchCR			;No the current line number is in binary form
	ret
	
;***********************************************   
gotonotfound:
	STOREI	ERRCode,$09	;Claim error - goto line not found
	Goto	error

;***********************************************   
gotocommand:
	GoSub	getlineno
	COPY16	YH,YL,ZH,ZL	;copy to Y
;	mov		YH,ZH
;	mov		YL,ZL
	ret

;***********************************************   
gosubcommand:			;Do a gosub
	GoSub	getlineno	;Get destination address in ZH and ZL	
	ori		XL,(1<<GSJAMD | \
				1<<GSCAPA)	;Set flags to capture return address 
							; and jam destination address
	ret
;
;***********************************************   
RETurncommand:			;Return from gosub
	sbrs	XH,GSACT	;skip next if GOSUB active
	Goto	RETurnnotactive
	ori		XL,(1<<GSJAMR);Set flag rcalling for jaming of return address	
	ret

RETurnnotactive:
	ori		XH,(1<<HALT)	;Set flag rcalling for program halt
	ret	
	
.if EFS
;*********************************************************************
;REMcommand: REM is a dummy command used to support catalogging
;*********************************************************************
REMcommand:	
	ld		outchar,Y			;get character @PC[HI:LO]
	GoSub	advanclbpointer		;advance past 1st character
	cpi		outchar,CR			;is it the CR character?
	brne	REMcommand			;no, continue looping 
	ret
.endif
;
;***********************************************   
emitcommand:
	GoSub	Get1stParm			;get the last parameter
	mov		outchar,U
	GoSub	emitchar
	ret
;
;***********************************************   
keycommand:		;Receive a byte from the terminal     
	sleep						;go to to sleep
	nop
#if ( USB && ( defined(__ATmega32U4__) || defined(__AT90USB1286__) ) )
	LOAD	inbytel,usb_configuration;get status of USB
	tst		inbytel				;is it 0?
	breq	keycommand1			;USB is not config'd so inbytel=0
	;TX a character via USB 
	push	r18					;save registers clobbered by USB
	push	r1					;save r1
	clr		r1					;insure r1 = 0
	call	usb_serial_available	;any characters available?
	pop		r1					;restore r1
	pop		r18
	tst		inbytel				;test for zero characters received
	breq	keycommand			;zero characters received, loop

	push	r18					;save register clobbered by USB
	push	r1					;save r1
	clr		r1					;insure r1 = 0
	call	usb_serial_getchar	;get the character
	pop		r1					;restore r1
	pop		r18					;restore register
;	tst		inbyteh				;zero is good character
;	brne	keycommand			;not RX, loop
keycommand1:
	mov		U,inbytel			;mov byte to U register
#else
	SKBS	UCSRA,RXC,temp		;Wait for byte to be received
	rjmp 	keycommand
	LOAD	U,UDR	;Read byte
#endif
	cpi		U,CTRL_C			;Is it control-C?
	brne	notermhalt1
	ori		XH,(1<<HALT)		;set flag to halt program
notermhalt1:
	GoSub	pushU
	ret 
;
;***********************************************   
;Mark Jeronimus of Digital Mosular shares this code, which is an
;  implementation of the Galois LFSR (Linear Feedback Shift Register)
RNDcommand:	;generate an 8-bit random number
	ldi		temp,RNDPoly		;get our polynomial
	LOAD	U,RNDSeed			;get the random # from RAM
	ror		U					;rotate seed through carry
	brcc	RNDcommand1			;branch if carry clear
	eor		U,temp				;xor value and seed
RNDcommand1:	
	STORE	RNDSeed,U			;save our new seed
	GoSub	pushU				;put new random # on stack
	GoSub	interpretlinev		;execute any further commands
	ret
;
;*************************************************************	
NEWprogram:		;Ready program space and interpreter for new program
	SetZPtr	(PROGMEM+PROGMEMSZ)-1;Initialize program memory pointer to top of program memory
	STORE16	PCLO,ZH,ZL		;Initialize PC position indicator

	INITDSTACK					;Initialize data stack pointer

	clr		XL					;Clear interpreter mode flags
	clr		XH

	PushStat					;save SREG
	cli							;disable interrupts during
								; memory fill	
	GoSub	ClrProgMem			;clear PROGMEM

	GoSub	ClrVarMem			;clear variable

	PopStat						;restore SREG

endofpmem1:
	GoSub	crlf
	SETZPTR (2*signonmessage)	;Load high part of byte address into ZH
	GoSub	sendlromline		;sent it.
	ret
;
#if DREC	; only for the AVR Data Recorder
;******************************************************************************   
; LDDcommand: Load default capture program into program RAM.
;	This command is only for the the AVR Data Recorder
;******************************************************************************   
LDDcommand:
	rcall	NEWprogram			;clear program memory

	SetYPtr	(PROGMEM+PROGMEMSZ)	;set Y pointer to program memory in RAM
	SetZPtr	(2*Default_Prog)	;point Z to program text in PROGMEM

LDDcommand1:
	lpm		temp,Z+				;get character 
	tst		temp				;is it a null character (EOS)?
	breq	LDDcommand2			;exit if EOS
	st		-Y,temp				;store character in RAM
	rjmp	LDDcommand1			;loop

LDDcommand2:
	STORE16	PCLO,YH,YL			;Save PC position indicator
	ret							;return to caller
;
#endif
;
;****************************************************************************
; WTFcommand: Random goofiness
;****************************************************************************
WTFcommand:
#if ( defined(__ATmega328__) || \
	  defined(__ATmega328PA__) || \
	  defined(__AT90USB1286__)  || \
	  defined(__ATmega2560__) || \
	  defined(__ATmega32U4__) )

	sbrc	XH,PRIACT			;PRIACT flag clear, print to console
	rjmp	WTFcommand1			;just return a value on dstack
	sbrc	XL,VPTRSTK			;VPTRSTK flag clear, print to console
	rjmp	WTFcommand1			;just return a value on dstack

	rcall	RNDcommand			;fetch a random number
	GoSub	popU				;fetch in U
	tst		U					;test and set some flags
	brmi	WTFcommand2			;branch if N flag set

	SETZPTR (2*WTFmsg2)			;Z = string address
	rjmp	WTFcommand3			;send and exit
WTFcommand2:
	SETZPTR (2*WTFmsg1)			;Z = string address

WTFcommand3:
	GoSub	sendromstring		;sent it.
#endif
WTFcommand1:
	ldi		U,42				;"the answer"
	GoSub	pushU				;save it on the dstack
	ret
;
.if HELP
;***********************************************   
HELPcommand:
	PushY						;save Y pointer
	GoSub	Dump_Perf			;print beginning "perforation"
	ldi		outchar,' '			;toss in space for beginning of new line
	rcall	emitchar			;send the character

	SetZPtr	(2*commandlist)		;Set Z-Ptr to beginning of command list

HELPcommand1:
	ldi		YH,LNLEN-4			;line length counter

HELPcommand2:
	ldi		YL,2*(commandlist1-commandlist);character counter

HELPcommand3:
	lpm		H,Z+				;get 1st character into H
	tst		H					;end of list?
	breq	HELPcommand9		;yes, exit

	lpm		T,Z+				;get 2nd character into T
	lpm		U,Z+				;get 3rd character into U

;check for "TO" command
	cpi		T,'T'				;test for "TO" command
	brne	HELPcommand3a		;not a "T"
	cpi		U,'O'
	brne	HELPcommand3c		;not a "O"
	ldi		H,' '				;1st character is a space
	rjmp	HELPcommand4		;continue to print

HELPcommand3a:	;check for "IF" command
	cpi		T,'I'				;test for "IF" command
	brne	HELPcommand3b		;not a "I"
	cpi		U,'F'
	brne	HELPcommand3c		;not a "O"
	ldi		H,' '				;1st character is a space
	rjmp	HELPcommand4		;continue to print

HELPcommand3b:	
;check for "OR" command
	cpi		T,'O'				;test for "OR" command
	brne	HELPcommand3c		;not a "O"
	cpi		U,'R'
	brne	HELPcommand3c		;not a "R"
;check for "-OR" command
	cpi		H,'W'
	breq	HELPcommand4		;is a "W" for "WORD" command
	cpi		H,'X'
	breq	HELPcommand4		;is a "X" for "XOR" command
	cpi		H,'F'
	breq	HELPcommand4		;is a "F" for "FOR" command
	ldi		H,' '				;1st character is a space
	rjmp	HELPcommand4		;continue to print

HELPcommand3c:
;check for "  @" command
	cpi		U,'@'				;test for '@' or higher
	breq	HELPcommand3e		;yes, modify H and T registers

;check for "  ^" command
	cpi		U,('^' & $5F)		;test for '^' or higher
	brge	HELPcommand3e		;branch if same or higher

;check for "  |" command
	cpi		U,('|' & $5F)		;test for '|' or higher
	brge	HELPcommand3d		;branch if same or higher

	cpi		U,$30				;test for an operator command
	brge	HELPcommand4		;branch if higher

HELPcommand3d:
	subi	U,(-$20)			;add to character

HELPcommand3e:	;determine if it is a plain text command or not
	ldi		H,' '				;1st character is a space
	cpi		T,$30				;test for an operator command
	brmi	HELPcommand3f		;branch if higher
	clr		T

HELPcommand3f:
	subi	T,(-$20)			;add to character

HELPcommand4:
	mov		outchar,H			;send the string
	rcall	emitchar			;send the character
	mov		outchar,T			;send the string
	rcall	emitchar			;send the character
	mov		outchar,U			;send the string
	rcall	emitchar			;send the character
	ldi		outchar,' '			;toss in space for separator
	rcall	emitchar			;send the character

	adiw	ZH:ZL,(commandlist2-commandlist1+2);skip address to command

	subi	YH,2*(commandlist1-commandlist);decrement line length counter
	brpl	HELPcommand2		;loop till negative

	GoSub	crlf				;end line with a CR/LF

#if ( USB && ( defined(__ATmega32U4__) || defined(__AT90USB1286__) ) )
;This code is not really neccessary but is prudent to implement
; so the USB buffers do not overflow.
	push	r1					;save r1
	clr		r1					;insure r1 = 0
	GoSub	usb_serial_flush_output	;flush the line buffer
	pop		r1					;restore r1
	GoSub	Delay10mS			;give USB host time to retrieve
HELPcommand5:
#endif

	ldi		outchar,' '			;toss in space for beginning of new line
	rcall	emitchar			;send the character

	rjmp	HELPcommand1		;continue looping till end if list

HELPcommand9:
	GoSub	crlf				;end line with a CR/LF
	GoSub	Dump_Perf			;print beginning "perforation"
	PopY						;restore Y pointer
	ret							;return to caller
.endif
;
;*******************************************************************************   
; RUNcommand: runs the program currently in program memory.  If a value is on 
;	the datastack when the RUN command is executed, it is assumed to be a 
;	file number, which is loaded before being executed.
;*******************************************************************************   
RUNcommand:			
.if EFS	;only if EFS is enabled
	Gosub	interpretlinev
	tst		DStkPtr
	breq	RUNcommand2			;nothing on the data stack
	GoSub	popU				;fetch the data on dstack

	ldi		temp,HANDLES_MAX-1	;only valid file handles allowed
	cp		temp,U				;check for valid file handle
	brge	RUNcommand1a		;branch if good file handle

	STOREI	ERRCode,$16			;error code "16"
	Goto	Error	

RUNcommand1a:
;	sbr		XH,(1<<RUNACT)		;set flag to indicate we are executing a command
	ori		XH,(1<<RUNACT)		;set flag to indicate we are executing a command
	GoSub	ClrProgMem			;clear PROGMEM before loading
	GoSub	LOADcommand			;load the file number	
.endif
;
RUNcommand2:
	INITGSTACK					;Initialize GOS-RET stack pointer
	INITLPTACK					;Initialize FOR-NEX loop stack pointer

;	sbr		XH,(1<<RUNACT)		;set flag to indicate we are executing a command
	ori		XH,(1<<RUNACT)		;set flag to indicate we are executing a command
	SetYPtr	(PROGMEM+PROGMEMSZ)-1;Initialize PC position inidcator

	GoSub	GETPROGLINENO		;fetch and store the 1st line #

checkandrunline:	;This is the main RUN loop, executed once per program line
	INITDSTACK					;Initialize data stack pointer

	ori		XL,(1<<VARSTK)		;Flag variable pointers to be left on stack
	GoSub	Checkkeybd			;See if Control-C key was received
;*********************************************************
noaddressjam:				;GOSUB-RETURN management
	sbrs	XL,GSCAPA			;skip next if XL:GSCAPA flag set
	rjmp	nocapgosubret
	rcall	pushGS				;push return address
	andi	XL,~(1<<GSCAPA)		;clear flag

nocapgosubret:				;GOSUB-RETURN management
	sbrs	XL,GSJAMD			;skip next if XL:GSJAMD flag set
	rjmp	nodestaddjam
	mov		YH,ZH 				;jamb destination address
	mov		YL,ZL
	ori		XH,(1<<GSACT)		;Set flag indicating that gosub is active
	andi	XL,~(1<<GSJAMD)		;Flip GOSUB jam address flag off

nodestaddjam:				;GOSUB-RETURN management
	sbrs	XL,GSJAMR			;skip next if XL:GSJAMR flag set
	rjmp	loopflagcheck
	rcall	popGS				;restore GOSUB-RETURN address
	tst		GoSubStkPtr			;GoSubStkPtr = 0?
	brne	nodestaddjam1		;no, more GOSUBS active
	andi	XH,~(1<<GSACT)		;clear GSACT flag
nodestaddjam1:
	andi	XL,~(1<<GSJAMR)		;clear GSJAMR flag

loopflagcheck:				;FOR-NEXT management. 
	sbrs	XL,LPCAPA			;FOR-NEXT management. Capture F-N return adddress
	rjmp	nocaploopad
	mov		loopreturnL,YL		;Copy return address (in interpreter p. space) to registers
	mov		loopreturnH,YH
	rcall	pushLP				;push FOR_NEXT address on stack
	andi	XL,~(1<<LPCAPA)		;Turn off flag -address captured.

nocaploopad:				;FOR-NEXT management. 
	sbrs	XL,LPJAMN			;;skip next if XL:LPJAMN flag is set
	rjmp	noretjam			; LPJAMN flag not set, continue
	mov		YH,loopreturnH 		; Jam in new address
	mov		YL,loopreturnL
	andi	XL,~(1<<LPJAMN)		;clear jaming flag

;noaddressjam:				;GOSUB-RETURN management
;	sbrs	XL,GSCAPA			;skip next if XL:GSCAPA flag set
;	rjmp	nocapgosubret
;	rcall	pushGS				;push return address
;	andi	XL,~(1<<GSCAPA)		;clear flag

;nocapgosubret:				;GOSUB-RETURN management
;	sbrs	XL,GSJAMD			;skip next if XL:GSJAMD flag set
;	rjmp	nodestaddjam
;	mov		YH,ZH 				;jamb destination address
;	mov		YL,ZL
;	ori		XH,(1<<GSACT)		;Set flag indicating that gosub is active
;	andi	XL,~(1<<GSJAMD)		;Flip GOSUB jam address flag off

;nodestaddjam:				;GOSUB-RETURN management
;	sbrs	XL,GSJAMR			;skip next if XL:GSJAMR flag set
;	rjmp	noretjam
;	rcall	popGS				;restore GOSUB-RETURN address
;	tst		GoSubStkPtr			;GoSubStkPtr = 0?
;	brne	nodestaddjam1		;no, more GOSUBS active
;	andi	XH,~(1<<GSACT)		;clear GSACT flag
;nodestaddjam1:
;	andi	XL,~(1<<GSJAMR)		;clear GSJAMR flag

noretjam:					;HALT via "END" or control-C
	sbrs	XH,HALT				;Skip next if XH,HALT flag is set
	rjmp	nostop
	GoSub	breakmessage		;HALT, so send break message to user
	LOAD16	YH,YL,PCLO			;get PC into Y
	andi	XH,~(1<<HALT)		;Clear the halt flag

nostop:
	LOAD16	inbyteh,inbytel,PCLO;get end of program counter
	SUB16	inbyteh,inbytel,YH,YL;less than PCHI/LO?
	brsh	endofprogram
	;not end of program so keep processing program lines
	ld		inchar,Y			;Get char from buffer into temp
	GoSub	Qcast				;See what type of char it is.
	sbrs	XL,SKPNXT			;skip this line if XL:SKPNXT is clear
	rjmp	noskipline			;to next carriage return
	cpi		temp,QC_CR			;is it a CR? (ecode in temp)
	breq	noskipline
	SUBI16	YH,YL,1				;If numeral, move pointer past it
	rjmp	checkandrunline		;loop and continue processing lines
		
noskipline:
	andi	XL,~(1<<SKPNXT)		;Make sure that skip line flag is cleared for next time around
	; **** does clearing this flag now actuall do anyting?
;	Code below uses Qcast rcall several lines above.
	cpi		temp,QC_NUM			;is it a number? (ecode in temp)
	brne	notonlinenumber		;no, not a the begginning of a program line	
	SUBI16	YH,YL,1				;If numeral, move pointer past it
	rjmp 	checkandrunline		;loop for next program line

notonlinenumber:
	GoSub	interpretline		;interpret program line, Y pointer is at the 
								;  1st char of program line
	rjmp 	checkandrunline		;loop for next program line

endofprogram:	;we hit an END or ^C caused a BREAK condition
	STOREI	CurrLine,0			;line "0" as default
	andi	XH,~(1<<RUNACT)		;clear flag to indicate we are executing a command
	ret
;
;*********************************************************
;GETPROGLINENO:	fetches the line number of the next line to
;	execute.
;	- Enter with current program pointer in Y
;*********************************************************
GETPROGLINENO:
	LOAD	temp,PCHI			;check for end of program
	cp		YH,temp
	brne	GETPROGLINENO1
	LOAD	temp,PCLO
	cp		YL,temp
	brne	GETPROGLINENO1
	ret	

GETPROGLINENO1:
	PUSHY						;save Y-pointer
	push	ecode				;save registers
	push	inchar

	clr		T					;clear TU registers
	clr		U

GETPROGLINENO2:
	ld		inchar,Y			;Get char from buffer, increment pointer
	GoSub	Qcast				;See what type of char it is.
	cpi		temp,QC_NUM			;is it a number? (check for change of cast)
	breq	GETPROGLINENO3		;yes, continue
	GoSub	decimaltobinary		;convert HTU to binary (contains current line#)	
	STORE	CurrLine,U			;save current line # for later		
	rjmp	GETPROGLINENO9		;exit

GETPROGLINENO3:
	mov		H,T					;shift number into HTU
	mov		T,U
	mov		U,inchar			;move last digit into U
	andi	U,$0F				;convert to BCD
	sbiw	YH:YL,1				;If numeral, move pointer past it
	rjmp	GETPROGLINENO2
;
GETPROGLINENO9:
	pop		inchar				;restore registers
	pop		ecode
	POPY						;restore Y-pointer
	ret							;return to caller
;
;*******************************************************************
breakmessage:
#if ( USB && ( defined(__ATmega32U4__) || defined(__AT90USB1286__) ) )
	push	r1					;save r1
	clr		r1					;insure r1 = 0
	GoSub	usb_serial_flush_input 	;flush any junk input
	pop		r1					;restore r1
#endif
	SETZPTR (2*breakmsgtxt)	;Load high part of byte address into ZH
	GoSub	sendromstring		;sent it.
	ret

;********************************************************************
;Checkkeybd:	See if there is any reason to stop output or 
;		program execution.  The interpreter is responsible
;		for handling ^C, while the "EMITCHAR" routine handles ^S.
;	If ^S is found in keyboard buffer, set XH.6 and return.
;	If ^C is found in keyboard buffer, set XH.1 and return.
;********************************************************************
Checkkeybd:	
;	andi	XH,~((1<<HALT)|(1<<SUSP));insure flags cleared

#if ( USB && ( defined(__ATmega32U4__) || defined(__AT90USB1286__) ) )
	LOAD	inbytel,usb_configuration;get status of USB
	tst		inbytel				;is it 0?
	breq	nokeydown			;USB is not config'd so inbytel=0
	;RX a character via USB 
	push	r18					;save registers clobbered by USB
	push	r1					;save r1
	clr		r1					;insure r1 = 0
	call	usb_serial_available;any characters available?
	pop		r1					;restore r1
	pop		r18
	tst		inbytel				;test for zero characters received
	breq	nokeydown			;zero characters received, exit

	push	r18					;save register clobbered by USB
	push	r1					;save r1
	clr		r1					;insure r1 = 0
	call	usb_serial_getchar	;get the character
	pop		r1					;restore r1
	pop		r18					;restore register
;	tst		inbyteh				;zero is good character
;	brne	Checkkeybd			;not RX, loop
	mov		temp,inbytel		;mov byte to temp register
#else
	SKBS	UCSRA,RXC,romreg	;if byte received, from terminal, get it
	Goto	nokeydown
	LOAD	temp,UDR			;Read byte
#endif
	cpi		temp,CTRL_S			;Is it control-S?
	brne	notermstop
	ori		XH,(1<<SUSP)		;Set the SUSPEND flag as suspend was requested
	ret							;return to caller
		
notermstop:
	cpi		temp,CTRL_C			;Is it control-C?
	brne	notermhalt
	ori		XH,(1<<HALT)		;set HALT flag to halt program.

notermhalt:
nokeydown:
	ret							;return to caller
;
;/////////////////END DATA FORMAT CONVERSION ROUTINES/////////////////
;****************************************************************************
; MkRndSeed: scan RAM and generate a random seed from the unitialized RAM
;	at powerup.  Saves the seed in RAM for later use with the RND command.
;****************************************************************************
MkRndSeed:
	ldi		U,RNDPoly			;get poly-seed into U
	SetZPtr	SRAM_START			;Z = start of RAM

MkRndSeed1:
	ld		temp,Z+				;fetch a byte
	eor		U,temp				;xor it
	CPI16	ZH,ZL,RAMEND		;end of RAM?
	brne	MkRndSeed1			;loop till end of RAM
	STORE	RNDSeed,U			;save it in RAM
	ret
;
;****************************************************************************
; DELAY1US: Delays 1uS.  
;****************************************************************************
Delay1uS:
	push		r29					;save register
	_WAIT_uS	f_clk,1				;delay 1uS
	pop			r29					;restore register
	ret

;****************************************************************************
; DELAY10US: Delays 10uS.  
;****************************************************************************
Delay10uS:
	push		r29					;save register
	_WAIT_uS	f_clk,10			;delay 10uS
	pop			r29					;restore register
	ret
;
;****************************************************************************
; DELAY1MS: Delays 1mS.  
;****************************************************************************
Delay1mS:
	push		r28					;save register
	push		r29
	_WAIT_10uS	f_clk,99			;delay 990uS (fudge)
	pop			r29					;restore register
	pop			r28
	ret
;
;****************************************************************************
; DELAY10MS: Delays 10mS.  
;****************************************************************************
Delay10mS:
	push		r16					;save register
	ldi			r16,10				;load loop counter
Delay10mSa:
	rcall		Delay1mS			;call 1ms delay
	dec			r16
	brne		Delay10mSa
	pop			r16
	ret
;;
;****************************************************************************
; DELAY100MS: Delays 100mS.  
;****************************************************************************
Delay100mS:
	push		r16					;save register
	ldi			r16,10				;load loop counter
Delay100mSa:
	rcall		Delay10mS			;call 10ms delay
	dec			r16
	brne		Delay100mSa
	pop			r16
	ret
;
#if USB
;****************************************************************************
; DELAY1S: Delays 1 Second.  
;****************************************************************************
Delay1S:
	push	r16					;save registers
	push	r28	
	push	r29
	ldi		r16,100				;load loop count
Delay1Sa:
	rcall	Delay10mS			;delay 10mS
;	_WAIT_mS	f_clk,10
	dec		r16					;decrement
	brne	Delay1Sa			;loop till zero
	pop		r29					;restore register
	pop		r28
	pop		r16
	ret
#endif
;
;****************************************************************************
; PrnLnNo: Print program line number held in YH:YL
;****************************************************************************
PrnLnNo:
	SetZPtr	(2*errlinenum)		;point to string table
	GoSub	sendromstring		;sent it.
	clr		YH					;copy line # to YH:YL
	LOAD	YL,CurrLine
	GoSub	D3ASC				;print the line #
	ret							;return to caller
;
;*************************************************************
; ClrProgMem:	clear PROGMEM in RAM.  Sets up registers then
;	Calls "FillMem" routine
;*************************************************************
ClrProgMem:	;clear PROGMEM in RAM
	SetZPtr	PROGMEM				;start of PROGMEM in RAM
	LOADI16	inbyteh,inbytel,PROGMEMSZ	;length
	clr		temp				;set fill value
	rjmp	FillMem
;
;*************************************************************
; ClrVarMem: clear variable space in RAM.  Sets up registers then
;	Calls "FillMem" routine
;*************************************************************
ClrVarMem:	;clear PROGMEM in RAM
	SetZPtr	VARSTOR				;start of variable storage
	LOADI16	inbyteh,inbytel,VARSTORSZ	;length
	clr		temp				;set fill value
	rjmp	FillMem
;
;*************************************************************
; FillMem: Clear contents of RAM
;	- Enter with start pointer in Z
;	- Enter with number of bytes in inbyteh:inbytel
;	- Enter with fill value in temp
;	- Uses U, Z, inbyteh:inbytel
;*************************************************************
FillMem:
	st		Z+,temp				;out in SRAM and decrement X-pointer
	SUBI16	inbyteh,inbytel,1	;decrement byte counter
	brne	FillMem				;loop till zero
	ret							;return to caller
;
;*************************************************************
; Dump_Perf:	Print a perferation to screen
;*************************************************************
Dump_Perf:
	SETZPTR	(2*perferate)		;print beginning "perforation"
	GoSub	sendromstring		;sent it.
	ret
;
;****************************************************************************
; ChkCmdParms: Zeros cmdparm register, calls line interpreter and tests
;	for zero parameters passed.
;****************************************************************************
ChkCmdParms:
	clr		cmdparms			;clear relative counter
	GoSub	interpretlinev		;process line parameters
	tst		cmdparms			;any new data on stack?
	ret	
;
;*************************************************************
; URXC1_int:	USART RX Complete ISR
;*************************************************************
;.if !USBSUPP
;URXC_int:
;URXC1_int:					; USART1 Rx Complete
;	push	temp
;	CLRB	UCSRA,RXC,temp	;clear USART RX complete flag
;	pop		temp
;	reti
;.endif
;
#if ( (defined(__ATmega32U4__) || defined(__AT90USB1286__) ) && USB )
;*************************************************************
; D_USBINT:	Disable USB hardware interrupts, save state to RAM
;*************************************************************
D_USBINT:
	cli							;disable IRQs
	LOAD	temp,UDIEN			;fetch USB IRQ register
	tst		temp				;is register cleared?
	breq	D_USBINT1			;yes, already saved USB IRQ
	STORE	usb_irqstate,temp	;save in RAM
	STOREI	UDIEN,0				;disable USB interrupts
D_USBINT1:
	sei							;restore IRQs
	ret
;
;*************************************************************
; E_USBINT:	Restore USB hardware interrupts state from RAM
;*************************************************************
E_USBINT:
	cli							;disable IRQs
	LOAD	temp,usb_irqstate	;fetch from RAM
	STORE	UDIEN,temp			;fetch USB IRQ register
	sei							;restore IRQs
	ret							;return to caller
;
#endif
;
.if LPMD || DHT		; if Low-Power Mode OR DHT routines enabled
;****************************************************************************
; WDT_int: Watchdog Time-out interrupt used in conjuction with the
;	low-power sleep mode.  Disables watchdog before exit.
;****************************************************************************
WDT_int:		
	STOREI	WDTCSR,(1<<WDCE|1<<WDE),temp
	STOREI	WDTCSR,0,temp			;disable watchdog
	SETB	GPIOR0,WDTIRQ			;set WDTIRQ flag
	reti							;return to caller
;
.endif
;
;****************************************************************************
; Include code snippets
;****************************************************************************
.include "Include/Code_Editing.inc"		;Line editing routines
.include "Include/Code_EFS.inc"			;EEPROM/File System routines
.include "include/Code_Math.inc"		;include 16 and 32 bit math routines
.include "Include/Code_ERRPROC.inc"		;Error processing routine
.if TWI
  .include "Include/Code_TWI.inc"		;TWI routines
.endif
.if SPI
  .include "Include/Code_SPI.inc"		;SPI routines
.endif
.include "Include/Code_PORTIO.inc"		;PORT I/O routines
.if PWM
  .include "Include/Code_PWM.inc"		;PWM routines
.endif
.include "Include/Code_Operators.inc"	;test operators
.include "Include/Code_Analog.inc"		;ADC and AC routines
.include "Include/Code_DataBytes.inc"	;RAM and EEP routines
.if ICP
  .include "Include/Code_ICP.inc"		;ICP routines
.endif
.if DSI
  .include "Include/Code_DS.inc"		;DS interface routines
.endif
.if DATA
  .include "Include/Code_DataFile.inc"	;Data File routines
.endif
.if LPMD
  .include "Include/Code_LOPWR.inc"		;Low-power routines
.endif
.if DDS
  .include "Include/Code_DDS.inc"		;DDS routines
.endif
.include "Include/Code_Init.inc"		;Machine initialization
.if RTC
  .include "Include/Code_RTC.inc"		;Real-time Counter routines
.endif
.if DHT
  .include "Include/Code_DHT.inc"		;DHTxx sensor routines
.endif
.if NRF
  .include "Include/Code_NRF.inc"		;nRF24L01(+) routines
.endif
.if OWR
  .include "Include/Code_OWR.inc"		;One-Wire routines
.endif
;.include "Include/Code_DeCompress.inc"
.if DEBUG
  .include "Include/Code_Debug.inc"		;Debug routines
.endif
.if RENUMBR
  .include "Include/Code_Renumber.inc"	;renumber routines
.endif
#if defined(__ATmega32U4__) || \
	defined(__AT90USB1286__) || \
	defined(__ATmega2560__) || \
	defined(__ATmega168__) || \
	defined(__ATmega168PA__) || \
	defined(__ATmega328__) || \
	defined(__ATmega328PA__)
  .include "Include/Code_BLDR.inc"		;BootLoader access
#endif
;
;****************************************************************************
; INTERUPT VECTORS
;****************************************************************************
;
#if defined(__ATmega88__) || defined(__ATmega168__) || defined(__ATmega328__)
	INT0_int:			; External Interrupt Request 0
	INT1_int:			; External Interrupt Request 1
	PCI0_int:			; Pin Change Interrupt Request 0
	PCI1_int:			; Pin Change Interrupt Request 1
	PCI2_int:			; Pin Change Interrupt Request 2
.if !LPMD && !DHT	;if LP Mode and DHT routines disabled
	WDT_int:			; Watchdog Time-out Interrupt
.endif 
.if !DDS			;if DDS disabled
	DDS_int:			; Timer/Counter2 Compare Match A
.endif 
	OC2B_int:			; Timer/Counter2 Compare Match B
	OVF2_int:			; Timer/Counter2 Overflow
	ICP1_int:			; Timer/Counter1 Capture Event
	OC1A_int:			; Timer/Counter1 Compare Match A
	OC1B_int:			; Timer/Counter1 Compare Match B
	OVF1_int:			; Timer/Counter1 Overflow
.if !RTC			;if RTC disabled
	OC0A_int:			; Timer/Counter0 Compare Match A
.endif
	OC0B_int:			; Timer/Counter0 Compare Match B
	OVF0_int:			; Timer/Counter0 Overflow
	SPI_int:			; SPI Serial Transfer Complete
	URXC_int:			; USART1 Rx Complete
	UDRE_int:			; USART, Data Register Empty
	UTXC_int:			; USART Tx Complete
	ADCC_int:			; ADC Conversion Complete
	ERDY_int:			; EEPROM Ready
;	ACI_int:			; Analog Comparator
	TWI_int:			; Two-wire Serial Interface
	SPMR_int:			; out Program Memory Read

#elif defined(__ATmega32U4__)
	INT0_int:			; External Interrupt Request 0
	INT1_int:			; External Interrupt Request 1
	INT2_int:			; External Interrupt Request 2
	INT3_int:			; External Interrupt Request 3
	Res1addr_int:		; Reserved1
	Res2addr_int:		; Reserved2
	INT6_int:			; External Interrupt Request 6
	Res3addr_int:		; Reserved3
	PCI0_int:			; Pin Change Interrupt Request 0
#if !USB
	USB_GEN_int:		; USB General Interrupt Request
	USB_COM_int:		; USB Endpoint/Pipe Interrupt Communication Request
#endif
.if !LPMD && !DHT	;if LP Mode and DHT routines disabled
	WDT_int:			; Watchdog Time-out Interrupt
.endif 
	Res4addr_int:		; Reserved4
	Res5addr_int:		; Reserved5
	Res6addr_int:		; Reserved6
	ICP1_int:			; Timer/Counter1 Capture Event
	OC1A_int:			; Timer/Counter1 Compare Match A
	OC1B_int:			; Timer/Counter1 Compare Match B
	OC1C_int:			; Timer/Counter1 Compare Match C
	OVF1_int:			; Timer/Counter1 Overflow
.if !RTC			;if RTC disabled
	OC0A_int:			; TimerCounter0 Compare Match A
.endif
	OC0B_int:			; TimerCounter0 Compare Match B
	OVF0_int:			; Timer/Couner0 Overflow
	SPI_int:			; SPI Serial Transfer Complete
	URXC1_int:			; USART1 Rx Complete
	UDRE1_int:			; USART1, Data Register Empty
	UTXC1_int:			; USART1 Tx Complete
;	ACI_int:			; Analog Comparator
	ADCC_int:			; ADC Conversion Complete
	ERDY_int:			; EEPROM Ready
	ICP3_int:			; Timer/Counter3 Capture Event
	OC3A_int:			; Timer/Counter3 Compare Match A
	OC3B_int:			; Timer/Counter3 Compare Match B
	OC3C_int:			; Timer/Counter3 Compare Match C
	OVF3_int:			; Timer/Couner3 Overflow
	TWI_int:			; Two-wire Serial Interface
	SPMR_int:			; out Program Memory Read
.if !DDS			;if DDS disabled
	DDS_int:			; Timer/Counter4 Compare Match A
;	OC4A_int:			; Timer/Counter4 Compare Match A
.endif
	OC4B_int:			; Timer/Counter4 Compare Match B
	OC4D_int:			; Timer/Counter4 Compare Match C
	OVF4_int:			; Timer/Couner4 Overflow
	FPF4_int:			; Timer/Counter4 Fault Protection Interrupt
#elif defined(__AT90USB1286__)
	INT0_int:			; External Interrupt Request 0
	INT1_int:			; External Interrupt Request 1
	INT2_int:			; External Interrupt Request 2
	INT3_int:			; External Interrupt Request 3
	INT4_int:			; External Interrupt Request 4
	INT5_int:			; External Interrupt Request 5
	INT6_int:			; External Interrupt Request 6
	INT7_int:			; External Interrupt Request 7
	PCI0_int:			; Pin Change Interrupt Request 0
#if !USB
	USB_GEN_int:		; USB General Interrupt Request
	USB_COM_int:		; USB Endpoint/Pipe Interrupt Communication Request
#endif
.if !LPMD && !DHT	;if LP Mode and DHT routines disabled
	WDT_int:			; Watchdog Time-out Interrupt
.endif 
.if !DDS			;if DDS disabled
	DDS_int:			; Timer/Counter2 Compare Match A
;	OC2A_int:			; Timer/Counter2 Compare Match A
.endif
	OC2B_int:			; Timer/Counter2 Compare Match B
	OVF2_int:			; Timer/Counter2 Overflow
	ICP1_int:			; Timer/Counter1 Capture Event
	OC1A_int:			; Timer/Counter1 Compare Match A
	OC1B_int:			; Timer/Counter1 Compare Match B
	OC1C_int:			; Timer/Counter1 Compare Match C
	OVF1_int:			; Timer/Counter1 Overflow
.if !RTC			;if RTC disabled
	OC0A_int:			; TimerCounter0 Compare Match A
.endif
	OC0B_int:			; TimerCounter0 Compare Match B
	OVF0_int:			; Timer/Couner0 Overflow
	SPI_int:			; SPI Serial Transfer Complete
	URXC1_int:			; USART1 Rx Complete
	UDRE1_int:			; USART1, Data Register Empty
	UTXC1_int:			; USART1 Tx Complete
;	ACI_int:			; Analog Comparator
	ADCC_int:			; ADC Conversion Complete
	ERDY_int:			; EEPROM Ready
	ICP3_int:			; Timer/Counter3 Capture Event
	OC3A_int:			; Timer/Counter3 Compare Match A
	OC3B_int:			; Timer/Counter3 Compare Match B
	OC3C_int:			; Timer/Counter3 Compare Match C
	OVF3_int:			; Timer/Couner3 Overflow
	TWI_int:			; Two-wire Serial Interface
	SPMR_int:			; out Program Memory Read
#elif defined(__ATmega2560__)
	INT0_int:			; External Interrupt Request 0
	INT1_int:			; External Interrupt Request 1
	INT2_int:			; External Interrupt Request 2
	INT3_int:			; External Interrupt Request 3
	INT4_int:			; External Interrupt Request 4
	INT5_int:			; External Interrupt Request 5
	INT6_int:			; External Interrupt Request 6
	INT7_int:			; External Interrupt Request 7
	PCI0_int:			; Pin Change Interrupt Request 0
	PCI1_int:			; Pin Change Interrupt Request 1
	PCI2_int:			; Pin Change Interrupt Request 2
.if !LPMD && !DHT	;if LP Mode and DHT routines disabled
	WDT_int:			; Watchdog Time-out Interrupt
.endif 
.if !DDS			;if DDS disabled
	DDS_int:			; Timer/Counter2 Compare Match A
;	OC2A_int:			; Timer/Counter2 Compare Match A
.endif
	OC2B_int:			; Timer/Counter2 Compare Match B
	OVF2_int:			; Timer/Counter2 Overflow
	ICP1_int:			; Timer/Counter1 Capture Event
	OC1A_int:			; Timer/Counter1 Compare Match A
	OC1B_int:			; Timer/Counter1 Compare Match B
	OC1C_int:			; Timer/Counter1 Compare Match C
	OVF1_int:			; Timer/Counter1 Overflow
.if !RTC			;if RTC disabled
	OC0A_int:			; TimerCounter0 Compare Match A
.endif
	OC0B_int:			; TimerCounter0 Compare Match B
	OVF0_int:			; Timer/Couner0 Overflow
	SPI_int:			; SPI Serial Transfer Complete
	URXC0_int:			; USART0 Rx Complete
	UDRE0_int:			; USART0, Data Register Empty
	UTXC0_int:			; USART0 Tx Complete
;	ACI_int:			; Analog Comparator
	ADCC_int:			; ADC Conversion Complete
	ERDY_int:			; EEPROM Ready
	ICP3_int:			; Timer/Counter3 Capture Event
	OC3A_int:			; Timer/Counter3 Compare Match A
	OC3B_int:			; Timer/Counter3 Compare Match B
	OC3C_int:			; Timer/Counter3 Compare Match C
	OVF3_int:			; Timer/Couner3 Overflow
	URXC1_int:			; USART1 Rx Complete
	UDRE1_int:			; USART1, Data Register Empty
	UTXC1_int:			; USART1 Tx Complete
	TWI_int:			; Two-wire Serial Interface
	SPMR_int:			; out Program Memory Read
	ICP4_int:			; Timer/Counter4 Capture Event
	OC4A_int:			; Timer/Counter4 Compare Match A
	OC4B_int:			; Timer/Counter4 Compare Match B
	OC4C_int:			; Timer/Counter4 Compare Match C
	OVF4_int:			; Timer/Couner4 Overflow
	ICP5_int:			; Timer/Counter5 Capture Event
	OC5A_int:			; Timer/Counter5 Compare Match A
	OC5B_int:			; Timer/Counter5 Compare Match B
	OC5C_int:			; Timer/Counter5 Compare Match C
	OVF5_int:			; Timer/Couner5 Overflow
	URXC2_int:			; USART2 Rx Complete
	UDRE2_int:			; USART2, Data Register Empty
	UTXC2_int:			; USART2 Tx Complete
	URXC3_int:			; USART3 Rx Complete
	UDRE3_int:			; USART3, Data Register Empty
	UTXC3_int:			; USART3 Tx Complete
#endif
	NO_VEC:	
	reti
;
;******************************************************************************
;	include data 
.include "Include/Data_Cmds.inc"		;Command Table
.include "Include/Data_Msgs.inc"	;User messages
#if DREC	;only for Data Recorder	
.include "Include/Data_DR_Prog.inc"	;default capture program
#endif
;
;******************************************************************************
;USB Support
;******************************************************************************
#if USB
 .cseg
 ;.org		USBSERIAL
 .org		__ctors_start		;start of ROM code + data
;
 #if defined(__ATmega32U4__)
  #if FCLK == 8000000
	#message "Notice: USB support for ATmega32U4 @ 8MHz enabled"
	.include	"Include/Code_usb_serial_atmega32u4-8M.inc"
  #elif FCLK == 16000000
	#message "Notice: USB support for ATmega32U4 @ 16MHz enabled"
	.include	"Include/Code_usb_serial_atmega32u4-16M.inc"
  #else
	#message "Warning: Invalid clock specified for USB on Mega32U4!"
  #endif
#elif defined(__AT90USB1286__)
  #if FCLK == 8000000
    #message "Notice: USB support for AT90USB1286 @ 8MHz enabled"
    .include	"Include/Code_usb_serial_at90usb1286-8M.inc"
  #elif FCLK == 16000000
	#if TEENSY
	  #message "Notice: USB support for TEENSY++ 2.0 enabled"
	  .include	"Include/Code_usb_serial_TEENSYPP20.inc"
	#else
	  #message "Notice: USB support for AT90USB1286 @ 16MHz enabled"
	  .include	"Include/Code_usb_serial_at90usb1286-16M.inc"
	#endif
  #else
	#message "Warning: Invalid clock specified for USB on AT90USB1286!"
  #endif
#elif
	#message "Warning: USB support is only for Mega32U4 and AT90USB1286... Ignored!"
 #endif
#endif
;
;******************************************************************************
.if DDS
.include "Include/Data_SqrWav.inc"	;DDS waveform (8x compressed)
.endif
;
;******************************************************************************
; BootLoader add-in support
;******************************************************************************
.include "Include/Code_Bootloader.inc"	;Bootloader determinations
;
;******************************************************************************
;end of file	
;******************************************************************************
;
