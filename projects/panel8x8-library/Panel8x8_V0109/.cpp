// Panel8x8 Library
// Simplifies programming the 8x8 LED Panel avaialable from ModernDevice.Com
// See Examples and ReadMe.TXT
// Kits & Schematics avaialbe at ModernDevice.Com
//
// By Dataman aka Charley Jones, 8x8Panel@CRJones.Com
// 2009-10-13 V1.9 Glitching removed 
// 2009-10-13 V1.8 SD Sample online - Wifi Sample fixed - Dance of Joy - Feature Complete 
// 2009-10-09 V1.7 Ethernet & Wifi Update Samples
// 2009-10-06 V1.6 - Working Serial Library / Flicker Free Display /  Support for Panel8x8Support Package
// 2009-09-28 V1.5 - Fixup include path for unix compiler 
// 2009-09-23 V1.4 - Overrides not working from client code
// 2009-09-19 V1.3 - Initial Release
// 2009-09-18 V1.2 - Bug Fixes
// 2009-09-17 V1.1 - Initial Version
#include "Panel8x8.h"

#ifndef PANEL8X8_NO_CHDATA
// Character Data 
// the following is a complete printable ASCII character set 32 - 126
// there are 8 bytes for each character, top row to bottom,
// put character set into program memory (flash)
prog_uint8_t charSet8[] PROGMEM = {
/*   32 */ 0,0,0,0,0,0,0,0,
/* ! 33 */ 16,16,16,16,16,0,16,0,
/* " 34 */ 20,20,20,20,0,0,0,0,
/* # 35 */ 0,10,31,10,31,10,0,0,
/* $ 36 */ 4, 31, 20, 31, 5, 31, 4, 0,
/* % 37 */ 1, 26, 26, 4, 11, 11, 16, 0,
/* & 38 */ 4, 10, 10, 12, 20, 18, 13, 0,
/* ' 39 */ 12, 12, 4, 0, 0, 0, 0, 0,
/* ( 40 */ 4, 8, 8, 8, 8, 8, 4, 0,
/* ) 41 */ 4, 2, 2, 2, 2, 2, 4, 0,
/* * 42 */ 0, 21, 14, 31, 14, 21, 0, 0,
/* + 43 */ 0, 4, 4, 31, 4, 4, 0, 0,
/* , 44 */ 0,0,0,0,0,4,4,8,
/* - 45 */ 0, 0, 0, 31, 0, 0, 0, 0,
/* . 46 */ 0,0,0,0,0,24,24,0,
/* / 47 */ 0,1,2,4,8,16,0,0,
/* 0 48 */ 6, 9, 9, 9, 9, 9, 6, 0,
/* 1 49 */ 2, 6, 2, 2, 2, 2, 7, 0,
/* 2 50 */ 6, 9, 1, 6, 8, 8, 15, 0,
/* 3 51 */ 6, 9, 1, 6, 1, 9, 6, 0,
/* 4 52 */ 9, 9, 9, 15, 1, 1, 1, 0,
/* 5 53 */ 15, 8, 8, 15, 1, 9, 15, 0,
/* 6 54 */ 15, 8, 8, 15, 9, 9, 15, 0,
/* 7 55 */ 15, 9, 1, 2, 4, 4, 4, 0,
/* 8 56 */ 6, 9, 9, 6, 9, 9, 6, 0,
/* 9 57 */ 6, 9, 9, 7, 1, 1, 6, 0, 
/* : 58 */ 0, 6, 6, 0, 6, 6, 0, 0,
/* ; 59 */ 0, 6, 6, 0, 6, 6, 2, 4,
/* < 60 */ 1, 2, 4, 8, 4, 2, 1, 0,
/* = 61 */ 0, 0, 15, 0, 15, 0, 0, 0,
/* > 62 */ 8, 4, 2, 1, 2, 4, 8, 0,
/* ? 63 */ 6, 9, 1, 6, 4, 0, 4, 0,
/* @ 64 */ 14, 17, 23, 21, 23, 16, 15, 0,
/* A 65 */ 4,10,17,31,17,17,17,0,
/* B 66 */ 30,17,17,30,17,17,30,0,
/* C 67 */ 14,17,16,16,16,17,14,0,
/* D 68 */ 30,17,17,17,17,17,30,0,
/* E 69 */ 31,16,16,30,16,16,31,0,
/* F 70 */ 31,16,16,30,16,16,16,0,
/* G 71 */ 14,17,16,16,23,17,14,0,
/* H 72 */ 17,17,17,31,17,17,17,0,
/* I 73 */ 14,4,4,4,4,4,14,0,
/* J 74 */ 14,4,4,4,4,4,24,0,
/* K 75 */ 17,18,20,24,20,18,17,0,
/* L 76 */ 16,16,16,16,16,16,31,0,
/* M 77 */ 17,27,21,17,17,17,17,0,
/* N 78 */ 17,17,25,21,19,17,17,0,
/* O 79 */ 14,17,17,17,17,17,14,0,
/* P 80 */ 30,17,17,30,16,16,16,0,
/* Q 81 */ 14,17,17,17,21,18,13,0,
/* R 82 */ 30,17,17,30,20,18,17,0,
/* S 83 */ 14,17,16,14,1,17,14,0,
/* T 84 */ 31,4,4,4,4,4,4,0,
/* U 85 */ 17,17,17,17,17,17,14,0,
/* V 86 */ 17,17,17,17,17,10,4,0,
/* W 87 */ 21,21,21,21,21,21,10,0,
/* X 88 */ 17,17,10,4,10,17,17,0,
/* Y 89 */ 17,17,10,4,4,4,4,0,
/* Z 90 */ 31,1,2,4,8,16,31,0,
/* [ 91 */ 14, 8, 8, 8, 8, 8, 8, 14,
/* \ 92 */ 0, 16, 8, 4, 2, 1, 0, 0,
/* ] 93 */ 14, 2, 2, 2, 2, 2, 2, 14,
/* ^ 94 */ 4, 10, 17, 0, 0, 0, 0, 0,
/* _ 95 */ 0, 0, 0, 0, 0, 0, 0, 31,
/* ` 96 */ 24, 12, 4, 0, 0, 0, 0, 0,
/* a 97 */ 0, 0, 30, 2, 30, 18, 30, 0,
/* b 98 */ 16, 16, 30, 18, 18, 18, 30, 0,
/* c 99 */ 0, 0, 30, 16, 16, 16, 30, 0,
/* d 100 */ 2, 2, 30, 18, 18, 18, 30, 0,
/* e 101 */ 0, 0, 30, 18, 30, 16, 30, 0,
/* f 102 */ 0, 0, 30, 16, 28, 16, 16, 0,
/* g 103 */ 0, 0, 30, 18, 18, 30, 2, 30,
/* h 104 */ 16, 16, 30, 18, 18, 18, 18, 0,
/* i 105 */ 0, 4, 0, 4, 4, 4, 4, 0,
/* j 106 */ 0, 4, 0, 4, 4, 4, 20, 28,
/* k 107 */ 16, 16, 18, 20, 24, 20, 18, 0,
/* l 108 */ 4, 4, 4, 4, 4, 4, 4, 0,
/* m 109 */ 0, 0, 31, 21, 21, 21, 21, 0,
/* n 110 */ 0, 0, 30, 18, 18, 18, 18, 0,
/* o 111 */ 0, 0, 30, 18, 18, 18, 30, 0, 
/* p 112 */ 0, 0, 28, 18, 18, 18, 28, 16, 
/* q 113 */ 0, 0, 12, 18, 18, 18, 14, 2, 
/* r 114 */ 0, 0, 30, 18, 16, 16, 16, 0,
/* s 115 */ 0, 0, 31, 16, 31, 1, 31, 0,
/* t 116 */ 0, 8, 30, 8, 8, 9, 15, 0, 
/* u 117 */ 0, 0, 17, 17, 17, 17, 31, 0,
/* v 118 */ 0, 0, 17, 17, 10, 10, 4, 0,
/* w 119 */ 0, 0, 17, 21, 21, 21, 31, 0,
/* x 120 */ 0, 0, 17, 10, 4, 10, 17, 0,
/* y 121 */ 0, 0, 17, 10, 4, 4, 4, 8,
/* z 122 */ 0, 0, 31, 2, 4, 8, 31, 0,
/* { 123 */ 6, 8, 8, 24, 8, 8, 6, 0,
/* | 124 */ 4, 4, 4, 4, 4, 4, 4, 4,
/* } 125 */ 12, 2, 2, 3, 2, 2, 12, 0,
/* ~ 126 */ 0, 0, 8, 20, 5, 2, 0, 0
};
#else
 prog_uint8_t charSet8[2] PROGMEM = "";
#endif PANEL8X8_NO_CHDATA

// PANEL8x8
// Constructor - Creates a new Panel8x8 object
// Initializes private/public variables.
Panel8x8::Panel8x8() {
 pinMode(LATCHPIN, OUTPUT);
 digitalWrite(LATCHPIN, HIGH);
 pinMode(OUTPUTENABLE, OUTPUT);
 digitalWrite(OUTPUTENABLE, LOW);
 pinMode(CLOCKPIN, OUTPUT);
 pinMode(DATAPIN, OUTPUT);  
 bIsScrolling =true;
 PanelMode=1;
 frameDelay = 100;
 isBufferProgMem=false;
 iBufferLen=0;
 iBufferSize=0;
 ramBuffer=0;
 idxNext=0;
 idxScroll=0;
 idxMsg=0;
 idxLine=0;
 idxPanel=0;
 iFrames=0;
 iPanels=PANELS;
}


// ABOUT
// Displays information about this library
void Panel8x8::About() {
  Serial << "8x8 Panel Support - Version " << VERSION8X8MAJOR << "." << VERSION8X8MINOR << endl;
}

// Begin
// Stores buffers configuration data
void Panel8x8::Begin(char *Buffer, int pBufferSize, int pBufferLen, int pIsBufferProgMem) {
 isBufferProgMem = pIsBufferProgMem;
 ramBuffer = (uint8_t *)Buffer;
 iBufferSize = pBufferSize;
 iBufferLen = pBufferLen;
 // The current bit index into the display array.
 idxScroll = 0;
 // If idxScroll is ever less than idxNext add 1 new char
 idxNext = (int((iPanels*8)/6) * 6) + 6; 
 // And start processing the new message
 NewMessage();
}

// Loop
// Part of the arduino functionality
// Execute over and over by the aurduino.
void Panel8x8::Loop() {
  if (PanelMode==1) { // Text Mode
   if (iBufferLen==0) return;
   Scroll();
   if (--idxScroll == idxNext) {
    AddChar();
   }
  }
  else if (PanelMode<4) {
   // Otherwise, must be animation
   LoadAnimation();
   Scroll();
  }
  // Define PANEL8X8_DUMPPANELS to dump panel data to serial line for debugging once each loop.
  #ifdef PANEL8X8_DUMPPANELS
  DumpPanels();
  #endif
}

// NewMessage
// Called when a new text message is received
// Resets the matric to the new message
void Panel8x8::NewMessage() {
 idxScroll=0;
 idxMsg=0;
 FillOutput();
}

// ClearOutput
// Called to clear the output buffer
void Panel8x8::ClearOutput() {
 // Reset message related variables
 idxScroll = 0;                    // The current bit index into the ouput matrix
 idxMsg = 0;                       // The current index into the message
 // Clear out the ouptut matrix
 for (i=0; i<iPanels+2; i++) {     
   for (j=0;j<8;j++) {
     iScroll[i][j]=0;
   }
 }
}

// FillOutput
// Called to prefill output buffer with text
void Panel8x8::FillOutput() {
// Fill up the output matrix
 if (PanelMode==1) {
  #ifdef PANEL8X8_NO_CHDATA
   return;
  #endif
  while (idxScroll <= idxNext) {
   AddChar();
  }
 }
}

// GetByte
// Returns a byte from the buffer
uint8_t Panel8x8::GetByte(int ib) {
 if (!iBufferLen) {
	 return 0;
 }
 if (!isBufferProgMem) {
   return *(ramBuffer + ib);
 }
 else {
   return pgm_read_byte_near(ramBuffer + ib);
 }
}

// LoadAnimation
// Called to load up an animation frame
void Panel8x8::LoadAnimation() {
  if (PanelMode==2) {   
   i = (8 * iPanels) * idxScroll;
   for (j=0; j<iPanels;j++) {
     for (k=0;k<8;k++) {
       iScroll[j][k] = GetByte(i++);
     } 
   }
   if (++idxScroll>=iFrames) {
    idxScroll=0;
   }
  }
}
   
// AddChar: Adds a character to the output matrix.
// Characters are assumes 6 wide, 1 space + 5 data
void Panel8x8::AddChar() {
  #ifdef PANEL8X8_NO_CHDATA
   return;
  #endif
  prog_uint8_t iRawByte;    // The next raw byte
  // k iBitsToShift Number of bits to be shifted to fit next letter.
  // j iRawByte Index into the RawByte Pattern in Prog Memory
  // i iLtr Next letter to be displayed in message
  //
  i = getNextLetter(idxMsg++);
  if (idxMsg >= iBufferLen) {idxMsg=0;}
  idxPanel = int(idxScroll / 8);
  k = ((idxPanel + 1) * 8) - idxScroll - 6;
  for (j=0; j<8; j++) {
   tbyte = pgm_read_byte_near(charSet8 + i + j);
     idxLine = j;
     // There are only 4 possible shift patterns for 6 wide chars fitting into 8 bits, 2 left, 0, 2 right, 4 right.
     // 1111 1122 | 2222 3333 | 33 44 4444 |
     switch (k) {
     case  2: iScroll[idxPanel][idxLine] |= (tbyte << 2);
              break; 
     case  0: iScroll[idxPanel][idxLine] |= tbyte;
              break;
     case -2: iScroll[idxPanel][idxLine] |= (tbyte >> 2);
              iScroll[idxPanel+1][idxLine] |= (tbyte << 6);
              break;
     case -4: iScroll[idxPanel][idxLine] |= (tbyte >> 4);
              iScroll[idxPanel+1][idxLine] |= (tbyte << 4);
              break;
     }
  }
  idxScroll += 6;
}     

int Panel8x8::getNextLetter(int l){
 char c;
 l = l % iBufferLen;        // roll over out-of-bounds indecis
 c = GetByte(l);  
 j =  c - 32  ;             // get text, convert ASCII to array indexing ( - 32)  
 if (j < 0 || j>126){j=32;} // handle spaces and terminal nulls
 return j * 8;              // j * 8 to make up for 8 byte entries in the charSet (ascii table) array
}
     
// Scroll: Scrolls the outputput matrix to the left and outputs.
void Panel8x8::Scroll() {
  // Usage t = Temporary panel byte
  //       k = Temporary carry bit storage.
  uint8_t iCarryBit[8] = {0,0,0,0,0,0,0,0};
  uint8_t t;
  // Set Latch Low for data update.
  digitalWrite(LATCHPIN, LOW);
  // for each panel
  for (idxPanel=iPanels+1; idxPanel>=0; idxPanel--) {
   // for each line
   for (idxLine=0;idxLine<8;idxLine++) {
    // Get the row of pixels into j
    t = iScroll[idxPanel][idxLine];
    if (bIsScrolling) {
     // Check if shifting left will set a carry bit
     if ((t & HIGHBIT) > 0) {k=1;}
     else {k=0;}
     // Finally, shift the data, add carry bit from last panel.
     t = (t << 1) | iCarryBit[idxLine];
     // Store the modified data back to the matrix.
     iScroll[idxPanel][idxLine] = t;
     // Remember the carry bit for the next panel
     iCarryBit[idxLine] = k;
    }
    // If this isn't the phantom panel, output it.
    if (idxPanel < iPanels) {
      // Every even line should be LSBout, Every odd MSBout
      shiftOut(DATAPIN, CLOCKPIN, (idxLine % 2 == 0 ? LSBFIRST : MSBFIRST), t);
     }
    }
  }
  //udpate scroll index, add a new char if needed
  //return the latch pin HIGH to update LED's 
  digitalWrite(LATCHPIN, HIGH);
  //digitalWrite(OUTPUTENABLE, HIGH); // v1.9 controls glitching
  digitalWrite(OUTPUTENABLE, LOW);
  delay(frameDelay);                  // change frameDelay above to control scrolling speed
  digitalWrite(OUTPUTENABLE, HIGH);   // v1.9 controls glitching
  

}

// Converts a byte into 8 digit binary number as string
// IE: 255 in = 11111111 out
char *Panel8x8::Binary8(uint8_t ibyte) {
 static char ptr[9];
 static byte ib[8]={128,64,32,16,8,4,2,1};
 uint8_t tbyte = ibyte;
 for (i=0;i<8;i++) {
   if (tbyte>=ib[i]) {
    ptr[i] = '*';
    tbyte-=ib[i];   
   }
  else {
    ptr[i]='.';
  }  
 }
 ptr[8]=0;
 return ptr;
}

// DumpPanels
// For testing purposes.
// Dumps each panel to the serial port for debugging purposes.
void Panel8x8::DumpPanels() {
  int ip, il;
  uint8_t t;
  for(il=0;il<8;il++) {
    Serial << il << ": ";
    for(ip=0; ip<iPanels+2; ip++) {
      t=iScroll[ip][il];
      Serial << Binary8(t) << " ";
    }
    Serial << endl;
   }
   Serial << endl;
}

// SetScrolling
// Turns panel scrolling on or off
void Panel8x8::SetScrolling(short i) {
	if (!i) {bIsScrolling = false;}
	else {bIsScrolling = true;}
}

// WriteByte
// Writes an byte to the given index of the input buffer if possible,
// returns -1 if not  
int Panel8x8::WriteByte(int idx, byte iBtye) {
  if (isBufferProgMem) {return -1;}
  *(ramBuffer+idx)=iBtye;
  return 0;
}

