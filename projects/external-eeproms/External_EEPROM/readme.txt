External EEPROMs AVR project file guide
__________________________________________

a2.bat	- AVR assembler batch file   call from command line without file extensions:   a2 MySource <enter>

EE.asm	- AVR assembler source code for the Parallel EEPROM programmer

EEPROM_M161_schematic.gif  - schematic file for the Parallel EEPROM programmer

I2Cm8.asm  - AVR assembler source code for the Serial EEPROM programmer

m161def.inc  - definitions for AVR Mega161

m8def.inc   - definitions for AVR Mega8

Mega8 I2C schematic.gif  - schematic file for the Serial EEPROM programmer

Parallel EEPROM readme.txt - user guide for the Parallel EEPROM programmer

Serial EEPROM readme.txt  - user guide for the Serial  EEPROM programmer

PEEtest.bin - test file for the parallel programmer.  1024 sectors of 128 bytes each.
The source code successfully writes and reads back this file from an external EEPROM.

SEEtest.bin - test file for the serial EEPROM programmer.  1024 pages of 64bytes each.
The source code successfully writes and reads back this file from an external EEPROM.