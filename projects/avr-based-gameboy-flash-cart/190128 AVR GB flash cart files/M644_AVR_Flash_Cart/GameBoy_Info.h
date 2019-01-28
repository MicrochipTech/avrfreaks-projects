#ifndef __GBINFO__
#define __GBINFO__

#include <avr/io.h>

// Gameboy Structs
typedef struct GBromData_t{
  char    name[17];
  uint8_t ROMsize;
  uint32_t ROMsizeBytes;
  uint8_t RAMsize;
  uint16_t RAMsizeBytes;
  uint8_t MBCtype;
  char MBCname[21];
  uint8_t CGBsupport;
  char CGBsupportText[9];
} GBROMDATA;

extern GBROMDATA gbRomData;

// Gameboy function defines
void get_GBROM_info(GBROMDATA * gbromData);

#define GBROM_CODE_ENTRY 0x0100 // Unlikely to be used, but it's handy to know

// GB header is 79 bytes long located at 0x0100 - 0x014F (256 - 335th byte)

// Game name - located in header at 0x0134 - 0x142/143 (308th byte - 323/324th byte) - CGB only goes to 323

// CGB flag - located in header at 0x0143 (324th byte)
#define GB_PRE_CGB                  0x00
#define GB_AND_CGB                  0x80
#define CGB_ONLY                    0xC0

// Cart types (well, accepted cart types) - located in header at 0x0147 (328th byte)
#define GBCART_ROM_ONLY             0x00
#define GBCART_MBC1                 0x01
#define GBCART_MBC1_RAM             0x02
#define GBCART_MBC1_RAM_BATT        0x03
#define GBCART_MBC2                 0x05
#define GBCART_MBC2_BATT            0x06
#define GBCART_MBC3_RTC_BATT        0x0F
#define GBCART_MBC3_RTC_RAM_BATT    0x10
#define GBCART_MBC3                 0x11
#define GBCART_MBC3_RAM             0x12
#define GBCART_MBC3_RAM_BATT        0x13
#define GBCART_MBC5                 0x19
#define GBCART_MBC5_RAM             0x1A
#define GBCART_MBC5_RAM_BATT        0x1B
#define GBCART_MBC5_RUMBLE          0x1C
#define GBCART_MBC5_RUMBLE_RAM      0x1D
#define GBCART_MBC5_RUMBLE_RAM_BATT 0x1E

/* Unknown cart types below in case more info becomes available to add support
08h  ROM+RAM                  
09h  ROM+RAM+BATTERY          
0Bh  MMM01
0Ch  MMM01+RAM
0Dh  MMM01+RAM+BATTERY
20h  MBC6
22h  MBC7+SENSOR+RUMBLE+RAM+BATTERY
FCh  POCKET CAMERA
FDh  BANDAI TAMA5
FEh  HuC3
FFh  HuC1+RAM+BATTERY
*/

// ROM size - located in header at 0x0148 (329th byte)
#define GBROM_32k  0x00 // 32KByte (no ROM banking)
#define GBROM_64k  0x01 // 64KByte (4 banks)
#define GBROM_128k 0x02 // 128KByte (8 banks)
#define GBROM_256k 0x03 // 256KByte (16 banks)
#define GBROM_512k 0x04 // 512KByte (32 banks)
#define GBROM_1M   0x05 // 1MByte (64 banks)  - only 63 banks used by MBC1
#define GBROM_2M   0x06 // 2MByte (128 banks) - only 125 banks used by MBC1
#define GBROM_4M   0x07 // 4MByte (256 banks)
#define GBROM_8M   0x08 // 8MByte (512 banks)
#define GBROM_1_1M 0x52 // 1.1MByte (72 banks)
#define GBROM_1_2M 0x53 // 1.2MByte (80 banks)
#define GBROM_1_5M 0x54 // 1.5MByte (96 banks)

// RAM size - located in header at 0x0149 (330th byte)
#define GBRAM_0k   0x00 // None
#define GBRAM_2k   0x01 // 2 KBytes
#define GBRAM_8k   0x02 // 8 Kbytes
#define GBRAM_32k  0x03 // 32 KBytes (4 banks of 8KBytes each)
#define GBRAM_128k 0x04 // 128 KBytes (16 banks of 8KBytes each)
#define GBRAM_64k  0x05 // 64 KBytes (8 banks of 8KBytes each)

// Double speed mode usage?

// MBC3 RTC register info (used when RAM/RTC bank set to the first column value)
/*
 08h  RTC S   Seconds   0-59 (0-3Bh)
 09h  RTC M   Minutes   0-59 (0-3Bh)
 0Ah  RTC H   Hours     0-23 (0-17h)
 0Bh  RTC DL  Lower 8 bits of Day Counter (0-FFh)
 0Ch  RTC DH  Upper 1 bit of Day Counter, Carry Bit, Halt Flag
       Bit 0  Most significant bit of Day Counter (Bit 8)
       Bit 6  Halt (0=Active, 1=Stop Timer)
       Bit 7  Day Counter Carry Bit (1=Counter Overflow)
       */

#endif
