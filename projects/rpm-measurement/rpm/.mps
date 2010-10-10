Map File Summary V3.5.2
run at 2010-10-10 11:32:03

Project:      ..\RPM\RPM
Build Number: 92
Date:         Sun Oct 10 11:31:55 2010
Compiler:     ICCAVR
Version:      8.01.05

 Please note: If a SRAM variable or FLASH item appears larger than it should be,
 then almost certainly it is because your program contains unnamed items such as
 literal strings, e.g. from cstrcpy(s,"abc"), or static variables in system lib
 as e.g. printf local buffer.
 The compiler is always right in that cases, all space allocated is really used.

Memory summary:

flash size is 8192 bytes (8KB)
flash used for
- int vectors     = 42 (0x2A) bytes from 0x0000 to 0x0029 = 21 (0x15) instruction words from 0x0000 to 0x0014
- constants       = 6 (0x06) bytes from 0x002A to 0x002F
- code            = 336 (0x150) bytes from 0x0030 to 0x017F = 168 (0xA8) instruction words) from 0x0018 to 0x00BF
flash usage is 384 bytes or 4.6%, leaving 7808 bytes (7KB)

sram size is 512 bytes
sram used for
- variables       = 6 (0x06) bytes from 0x0060 to 0x0065
- heap, s/w stack = 486 (0x1E6) bytes from 0x0066 to 0x024B
- h/w stack       = 20 (0x14) bytes from 0x024C to 0x025F

eeprom size is 512 bytes
no eeprom area found


Areas:
(items sorted by name)

FLASH area "idata" with init values for data area objects
FLASH area "idata" = 6 (0x06) bytes from 0x002A to 0x002F
  no user objects in this area

FLASH area "text" used for code
FLASH area "text" = 336 (0x150) bytes from 0x0030 to 0x017F = 168 (0xA8) instruction words from 0x0018 to 0x00BF
  __start          =   33 (0x0021) words
  _exit            =    1 (0x0001) word
  _init_devices    =   11 (0x000B) words
  _int0_isr        =   51 (0x0033) words
  _main            =    8 (0x0008) words
  _port_init       =   11 (0x000B) words
  _timer2_comp_isr =   18 (0x0012) words
  _timer2_init     =    7 (0x0007) words
  div16u           =    1 (0x0001) word
  mod16u           =    2 (0x0002) words
  xdiv16u          =   25 (0x0019) words

SRAM area "data" with global and static variables initialised explicitly
SRAM area "data" = 6 (0x06) bytes from 0x0060 to 0x0065
  _RPM             =    2 (0x0002) bytes
  _count_1         =    2 (0x0002) bytes
  _iter            =    1 (0x0001) byte
  _j               =    1 (0x0001) byte

FLASH area "vector" used for interrupt vectors
FLASH area "vector" = 8 (0x08) bytes from 0x0000 to 0x0007 = 4 (0x04) instruction words from 0x0000 to 0x0003

<eof>
