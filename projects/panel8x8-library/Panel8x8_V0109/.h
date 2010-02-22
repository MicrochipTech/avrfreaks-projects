// Panel8x8 Library
// Simplifies programming the 8x8 LED Panel avaialable from ModernDevice.Com
// See Examples and ReadMe.TXT
// Kits & Schematics available at ModernDevice.Com
//
// By Dataman aka Charley Jones, 8x8Panel@CRJones.Com
// 2009-10-13 V1.9 Glitching removed 
// 2009-10-13 V1.8 SD Sample online - Wifi Sample fixed - Dance of Joy - Feature Complete 
// 2009-10-09 V1.7 Ethernet & Wifi Update Samples
// 2009-10-06 V1.6 - Working Serial Library / Flicker Free Display /  Support for Panel8x8Support Package
// 2009-09-25 V1.5 - Fixup include path for unix compiler
// 2009-09-23 V1.4 - Overrides not working from client code
// 2009-09-19 V1.3 - Initial Release
// 2009-09-18 V1.2 - Bug Fixes
// 2009-09-17 V1.1 - Initial Version

#ifndef Panel8x8b_h
#define Panel8x8b_h

#include "avr/pgmspace.h"            // Header to access ProgMem
#include "../Streaming/Streaming.h"  // Header to allow streaming operations String << "TEST"; Simplifies output.

// -------------------- DEFINES ---------------------------

// Switches, uncomment to enable and disable functionality
//#define PANEL8X8_DUMPPANELS          // Dumps panel data to serial line after each loop 
//                                        Does not init serial, must Serial.Begin(57600) in setup
//#define PANEL8X8_NO_CHDATA           // Saves 760 of progmem by omitting text from Rom space, 
//                                        no text scrolling though.

// Configuration
#define PANELS       4  // The Max Number of panels (each panel consumes 8 bytes ram buffer spaces)
#define LATCHPIN     4  // Pin connected to SPI latch
#define OUTPUTENABLE 5  // Pin connected to SPI output enable - high equals off
#define CLOCKPIN     6  // Pin connected to SPI clock - low to write new new data - high to transfer the data to the output buffers
#define DATAPIN      7  // Pin connected to SPI clock

// Animation File Version
#define VERSION8X8MAJOR   1  // Current library version MAJOR.MINOR
#define VERSION8X8MINOR   9  // Current library version

// Defines
#define LOWMASK    255  // The LOWMASK for stripping off High Bits
#define HIGHBIT    128  // In 8 bits, the highest bit, left.
#define LOWBIT       1  // In 8 bits, the lowest bit, right.


class Panel8x8 { 

 //Private Variables
 // Globals
public:
 int i, j, k, l;                    // Generic Loop counters
 int maxStringSize;                 // Size of the output message
 boolean bIsScrolling;              // Turns on and off scrolling
 byte bbuffer[16];                  // General purpose buffer
 uint16_t *ptr16;                   // General 2 byte pointer
 uint8_t *ptr8;                     // General 1 byte pointer
 uint8_t tbyte;                     // General temporary 1 byte
 prog_uint8_t iRawByte;             // General Pogram Memmory type 1 Byte;

 byte     iPanels;                  // Current number of panels
 uint16_t iFrames;                  // Number of animation frames
 int idxPanel;                      // Current Panel Number
 int idxLine;                       // The current line # 0-7
 int idxMsg;                        // Where are we within the character text.
 int idxScroll;                     // Current high water mark in Scrolling Matrix
 int idxNext;                       // Trigger point for next letter
 byte PanelMode;                    // Which mode are the panels in, 1=Text, 2=Animation, 3=Live

 //Public Buffer Stuff
 uint8_t *ramBuffer;                // Adddress if Ram memmory
 //prog_uint8_t *progBuffer;        // Address if Program (flash) memmory
 int          iBufferSize;          // Capacity of buffer including 0 byte
 int          iBufferLen;           // Length of buffer
 boolean isBufferProgMem;           // 0=Ram Buffer, 1=FLASH buffer
 int frameDelay;                    // Default delay between refreshes
 uint8_t iScroll[PANELS+2][8];      // The matrix for scrolling
 
 Panel8x8();
 
 void    About();                   // Dumps Version info to Serial                           
 void    AddChar();                 // Adds 1 text character to panel ouput
 void    Begin(char *Buffer, int pBufferSize, int pBufferLen, int pIsBufferProgMem);  // Defines buffers
 char   *Binary8(uint8_t ibyte);    // Formats input as 8 digit binary text
 void    ClearOutput();             // Clears the output buffer
 void    DumpPanels();              // Dumps the output buffer contents in panel form to serial
 void    FillOutput();              // Fill output buffer with text
 uint8_t GetByte(int ib);           // Gets given byte from the input buffer
 int     WriteByte(int idx, byte ib);   // Writes given byte to the input buffer
 int     getNextLetter(int l);      // Gets next letter in the text input buffer
 void    LoadAnimation();           // Loads next animation frame
 void    Loop();                    // The main processing pump
 void    NewMessage();              // Called whenever the text message is changed, resets buffers
 void    Scroll();                  // Called to display & optionally scroll 
 void    SetScrolling(short i);     // Called to set scrolling 0=off, 1=on
};




#endif Panel8x8b_h
