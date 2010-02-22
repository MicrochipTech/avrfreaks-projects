// Panel8x8 Library
// Simplifies programming the 8x8 LED Panel avaialable from ModernDevice.Com
// See Examples and ReadMe.TXT
// Kits & Schematics avaialbe at ModernDevice.Com
//
// By Dataman aka Charley Jones, 8x8Panel@CRJones.Com
// 2009-10-13 V1.9 Glitching removed 
// 2009-10-13 V1.8 SD Sample online - Wifi Sample fixed - Dance of Joy - Feature Complete 
// 2009-10-09 V1.7 Ethernet & Wifi Update Samples
// 2009-10-06 V1.6 Working Serial Library / Flicker Free Display /  Support for Panel8x8Support Package
// 2009-09-17 Initial Version

#ifndef Panel8x8Serialb_h
#define Panel8x8Serailb_h

#include <Panel8x8.h>                  // Header to access Panel8x8 Library
//#include "avr/pgmspace.h"            // Header to access ProgMem
//#include "..\Streaming\Streaming.h"  // Header to allow streaming operations String << "TEST"; Simplifies output.
//#include "..\String\WString.h"       // Header to allow string operations

//#define DEBUG8X8SERIAL
// To Use this serial debug object you will need to define a NewSoftSerial object in your main code
// And pass the object to the SerialPanel object
// In this case, we use A4/A5 for serial output (9600 baud).
// #include <NewSoftSerial.h>
// NewSoftSerial debug(18,19);
// Panel8x8Serial Panel(&debug);
#ifdef DEBUG8X8SERIAL
#include "../NewSoftSerial/NewSoftSerial.h"
#define crlf "?n"
#define cls  "?f?a"
#endif

// -------------------- DEFINES ---------------------------

// Switches, uncomment to enable and disable functionality
//#define PANEL8X8_DUMPPANELS          // Dumps panel data to serial line after each loop
//#define PANEL8X8_NO_CHDATA           // Saves 760 of progmem by omitting text from Rom space, 
//                                        no text scrolling though.

// Configuration

// Animation File Version
#define PANEL8X8SERIALMAJOR   1            // Current library version
#define PANEL8X8SERIALMINOR   9            // Current library version

#define PANEL8X8SERIALFILE    2            // Serial File Version Number

// Defines

class Panel8x8Serial:public Panel8x8 { 
public:
    Panel8x8Serial(void){
          Panel8x8::Panel8x8();
    }

#ifdef DEBUG8X8SERIAL
    Panel8x8Serial(NewSoftSerial *serial){
		  _debug = serial;
		  _debug->begin(9600);
		  (*_debug) << cls;
		  About();
          Panel8x8::Panel8x8();
    }
#endif

    void About();

	void Begin(char *Buffer, int pBufferSize, int pBufferLen, int pIsBufferProgMem) {
		Serial.begin(9600);	
		Panel8x8::Begin(Buffer,pBufferSize,pBufferLen,pIsBufferProgMem);
	}

    void Loop(){
		if (!CheckSerial()) {
			Panel8x8::Loop();
		}
	}

    int CheckSerial();
private:
#ifdef DEBUG8X8SERIAL
	NewSoftSerial *_debug;
#endif
};

#endif Panel8x8Serialb_h
