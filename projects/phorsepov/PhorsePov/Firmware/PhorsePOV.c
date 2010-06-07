/**
 * PhorsePOV, a minimal handheld POV.
 * Copyright (C) 2009,2010  Julian Skidmore.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Versions
 * ********
 *
 * 0.91		2Hz sync pulses. PovMsg simplified, KeyGet can generate '#'.
 *          Improved documentation.
 * 0.9beta	Initial release. Supports 32 characters, Clr, Restore, Del.
 *
 * Contact
 * *******
 * TheOriginalSnial@Gmail.com
 *
 */
 
#include <avr/io.h>
#include <util/delay.h>	// won't need this in the end.
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#ifdef 	__PGMSPACE_H_
#define GetPgmByte(x) pgm_read_byte(&x)
#define GetPgmWord(x) pgm_read_word(&x)
#else
#define GetPgmByte(x) x
#define GetPgmWord(x) x
#endif

typedef unsigned char byte;
typedef unsigned short ushort;
typedef byte tBool;
#define kTrue 1
#define kFalse 0

/**
 * Prototypes
 **/
char E2PromRdCh(char* addr);

/**
 * LED Control.
 * Phorse POV Controls 6 LEDs from 3 output pins: B0, B1 and B2.
 * This is possible, because an LED may be placed between any 2 pins and in
 * either orientation.
 *
 * There 3 pairings possible: B0-B1, B0-B2, B1-B2 so 3*2=6 LEDs may be driven.
 * The LEDs are set up to light as follows:
 *    Name:        a,b,c,d,e,f
 *    Signal Pin:  0 1 1 2 2 0
 *    Gnd pin:     1 0 2 1 0 2
 *
 * At most, 2 LEDs can be turned on at any one time.
 * We pair the LEDs up as {d,a}, {e,b}, {f,c} so that over 3 scans we can
 * light all the LEDs in an evenly distributed pattern:
 *
 *      Scan:       0 1 2
 *      Pattern: a  *
 *               b    *
 *               c      *
 *               d  *
 *               e    *
 *               f      *
 *
 * To reduce image scatter a scan sequence should be repeated at least twice.
 * Each call to DoPovScan performs one scan on a single LED pair and after a
 * complete scan sequence repeated kRowReps times it returns kTrue so that the
 * caller may choose another row of pixels to display.
 *
 * DoPovScan expects inputs in LED pair order:
 *
 *      Bit:   5 4 3 2 1 0
 *      LED:   f c e b d a
 *
 * We light up an LED pair by setting their signal pins to 1 and the remaining
 * pin to 0. We can light up a single LED by setting its signal pin to 1 and 
 * its pair to input with no pull-up. Thus we get the following combinations:
 *
 *  Segs: fc  eb   d a
 *  Pins:210  210  210
 *  00   G00  00G  0G0 (where G=the pin always at ground, '-' = set to input)
 *  01   G-1  -1G  -G1
 *  10   G1-  1-G  1G-
 *  11   G11  11G  1G1
 *      
 **/

#define kRows 3
#define kRowReps 2
byte gRow=0,gRowReps=0;
byte gRowPortSetting[][4][2] PROGMEM = {
 	{{7,0}, {3,1}, {6,4}, {7,5}}, // {0G0, -G1, 1G-, 1G1}, Segments a,d da.
 	{{7,0}, {3,2}, {5,4}, {7,6}}, // {00G ,-1G,1-G,11G}, Segments b,e eb
 	{{7,0}, {6,2}, {5,1}, {7,3}},	//{G00,G-1,G1-,G11}  Segments c,f fc
};
 
 /**
  * DoPovScan handles a single pov scan column, returning kTrue when the 
  * scan is complete.
  * INPUT: byte scan: the scan code.
  * OUTPUT: tBool kTrue if pov scan is complete, kFalse otherwise.
  * USES gRow, gRowReps.
  **/
tBool DoPovScan(byte scan)
{
 	byte bitPair,temp;
 	byte *setting;
 	tBool doneScan=kFalse;
 	bitPair=(scan>>(gRow<<1))&3;
 	PORTB&=~5;	// set the outer ports to 0 to remove potential pull-ups.
 	setting=gRowPortSetting[gRow][bitPair];
 	temp=(DDRB&0xf8) | GetPgmByte(*setting++);
 	DDRB=temp;	// update DDRS.
 	temp= (PORTB&0xf8)|GetPgmByte(*setting);
 	PORTB=temp;
 	if(++gRow>=kRows) {
 		gRow=0;
 		if(++gRowReps>=kRowReps) {
 			gRowReps=0;
 			doneScan=kTrue;
 		}
 	}
 	return doneScan;
}

/**
 * POV display handler.
 *
 * A 64-character 6x6 character set is defined with rows in LED scan order.
 *
 * The PovMsg handler calculates the next column combined with the Sync pulse.
 **/
 
 /** 
 *         #    # #   # #    #   ##  #  #      #      #    #           #                         #  ###    #   ####  ####    ##  ##
 *                   #####   ##   # ## # ##          #      #   ###    #                      #    ##  #   #   #         # #####   
 *         #    # #  #####   ### ## #  # #     #     #      #  # # #   #                        #  #  ##  ##       #     #  # #  # 
 *         #          # #  ###   #  ##  ## #          #    #   # # #   #      #          #   #      ###   ###  ##### ####     #  ##
 *         #          # #   ##     #    #  #         #      #   ###  #####       #####         #   # # #   #    ###   ###  #  #  ##
 *                           #                                               #                                                     
 *                                                                                                                                 
 *                                                                                                                                 
 * ###   ### #####  ###   ###                 #         #     ###   ###   ###  ####   #### ###   ##### #####  #### #   #  ###    ##
 *   # #   #   #   #   #    #                #   #####   #         #     #   # #   # #     #  #  #     #     #   # #   #   #      #
 *      #        # #   # #   #               #   #####   #   #   # # # # #   # #   # #     #  #  #     #     #     #   #   #      #
 * ##   ###    #    ###   ##     #      #     #         #      #    #### #   # ####   #### ###   ##### #      #### #   #  ###   ## 
 * ##  ####     #   ###   ####   #      #   #             #    ##  # ### ##### ####  #     #   # ####  ####  # ### #####   #      #
 *                                     #                                                                                           
 *                                                                                                                                 
 *                                                                                                                                 
 * #  #  #  #    #   # #   #  ###  ####   ###   ###   ###  ##### #   # #   # #   # #   # #   # #####   ##   ###    ##    #         
 *    # #   #    #   # #  ## #   # #     #  ## # #       #   #   #   #  # #  # # #  # #    #    #      #   #  ##    #              
 *    # #   #    ## ## ##  # #   # #   # #   # #   # #       #   #   # #   # #   #  # #   # #     #    #   #  ##    #   # #        
 *    #  #  #### #   # #   #  ###  #      #### #  ##  ###    #    ###    #    # #  #   #   #   #####   ##   ###    ##              
 *    ##    #    # # # # # # #   # ####  # # # ####   ###    #   #   # #   # # # #   #     #     #     #   # ###    #              
 *                                                                                                                                 
 */ 
 byte kCharBitMap[6*64] PROGMEM = {
	 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1d, 0x0, 0x0, 0x0,
	 0x0, 0x5, 0x0, 0x5, 0x0, 0x0, 0x6, 0x1f, 0x6, 0x1f, 0x6, 0x0,
	 0x8, 0x18, 0x3f, 0x6, 0x4, 0x0, 0xd, 0x7, 0x10, 0xe, 0xb, 0x0,
	 0x6, 0x19, 0xe, 0x2, 0x18, 0x0, 0x0, 0x0, 0x5, 0x0, 0x0, 0x0,
	 0x0, 0x0, 0x16, 0x9, 0x0, 0x0, 0x0, 0x0, 0x9, 0x16, 0x0, 0x0,
	 0xc, 0x12, 0x1e, 0x12, 0xc, 0x0, 0x10, 0x10, 0x1f, 0x10, 0x10,
	 0x0, 0x0, 0x0, 0x20, 0x8, 0x0, 0x0, 0x10, 0x10, 0x10, 0x10, 0x10,
	 0x0, 0x0, 0x0, 0x8, 0x0, 0x0, 0x0, 0x8, 0x2, 0x10, 0x4, 0x1, 0x0,
	 0x16, 0xb, 0x19, 0xd, 0x16, 0x0, 0x0, 0xc, 0x1f, 0x8, 0x0, 0x0,
	 0xb, 0x19, 0x19, 0x19, 0xc, 0x0, 0x9, 0x19, 0x19, 0x19, 0x6, 0x0,
	 0x12, 0x6, 0x3, 0x1f, 0x2, 0x0, 0x1d, 0x19, 0x19, 0x19, 0x3, 0x0,
	 0x12, 0x1c, 0x19, 0x19, 0x3, 0x0, 0x1, 0x1, 0xb, 0x11, 0x5, 0x0,
	 0x6, 0x19, 0x19, 0x19, 0x6, 0x0, 0x4, 0x19, 0x19, 0x13, 0x14, 0x0,
	 0x0, 0x0, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 0x20, 0x18, 0x0, 0x0,
	 0x0, 0x10, 0x6, 0x9, 0x0, 0x0, 0x6, 0x6, 0x6, 0x6, 0x6, 0x0,
	 0x0, 0x9, 0x6, 0x10, 0x0, 0x0, 0x4, 0x1, 0x19, 0x11, 0x4, 0x0,
	 0x16, 0x9, 0x1d, 0x19, 0x1c, 0x0, 0x1e, 0x11, 0x11, 0x11, 0x1e, 0x0,
	 0x1f, 0x19, 0x19, 0x19, 0x6, 0x0, 0x16, 0x9, 0x9, 0x9, 0x9, 0x0,
	 0x1f, 0x9, 0x9, 0x6, 0x10, 0x0, 0x1f, 0x19, 0x19, 0x19, 0x9, 0x0,
	 0x1f, 0x11, 0x11, 0x11, 0x1, 0x0, 0x16, 0x9, 0x19, 0x19, 0x1b, 0x0,
	 0x1f, 0x10, 0x10, 0x10, 0x1f, 0x0, 0x0, 0x9, 0x1f, 0x9, 0x0, 0x0,
	 0x0, 0x8, 0x9, 0x17, 0x1, 0x0, 0x0, 0x1f, 0x10, 0x6, 0x9, 0x0,
	 0x0, 0x1f, 0x8, 0x8, 0x8, 0x0, 0x1f, 0x4, 0x10, 0x4, 0x1f, 0x0,
	 0x1f, 0x4, 0x10, 0x2, 0x1f, 0x0, 0x16, 0x9, 0x9, 0x9, 0x16, 0x0,
	 0x1f, 0x11, 0x11, 0x11, 0x4, 0x0, 0x16, 0x9, 0x19, 0xb, 0x1e, 0x0,
	 0x1e, 0x11, 0x13, 0x19, 0xc, 0x0, 0x4, 0x19, 0x19, 0x19, 0x2, 0x0,
	 0x1, 0x1, 0x1f, 0x1, 0x1, 0x0, 0x17, 0x8, 0x8, 0x8, 0x17, 0x0,
	 0x15, 0x2, 0x8, 0x2, 0x15, 0x0, 0x17, 0x8, 0x12, 0x8, 0x17, 0x0,
	 0x9, 0x6, 0x10, 0x6, 0x9, 0x0, 0x1, 0x4, 0x1a, 0x4, 0x1, 0x0,
	 0x9, 0xb, 0x19, 0xd, 0x9, 0x0, 0x0, 0x0, 0x1f, 0x9, 0x0, 0x0,
	 0x16, 0x9, 0x19, 0x1f, 0x16, 0x0, 0x0, 0x0, 0x9, 0x1f, 0x0, 0x0,
	 0x0, 0x0, 0x3f, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
 };
 
#define kPovBuffSize 32
extern char gPovBuff[kPovBuffSize+1] EEMEM;

#define kPovColsPerChar 6
#define kCharSetSize 64*kPovColsPerChar
#define kPovImgWidth 12	// was 6.

byte *gBitPos;
byte gPovCol=5,gPovPos=kPovBuffSize;  // 0..5, char pos in display.
byte gPovImgPos=0;
byte gPovLen;  // Length generated by Add method.
char gPovDir=1;  // forwards.

/**
 * A PovMsg is organised similarly to a video scan.
 *
 *           LHMargin         Scan Data        RHMargin
 * Forward:  |<--->|:::::::::::::::::::::::::::|<--->|
 * Retrace:  |<----Blank---------------------------->|
 *
 * The Image width is 12 characters or 48 columns and because of visual sync
 * issues it currently outputs data only on the forward pass.
 *
 * The scan data will always be a column from a character in the gPovBuff, but
 * on each pass the scan data will scroll right by kPovScroll columns. Thus,
 * at the beginning of each proper scan we start at the scroll column and
 * character, advancing 1 column each time.
 * 
 **/
#define kLHMargin 6
#define kPovImgCols kPovImgWidth*kPovColsPerChar
#define kRHMargin (kLHMargin+kPovImgCols)

// For unidirectional
#define kFullScanPeriod ((kRHMargin+kLHMargin)*2)
#define kPovSyncCol (kLHMargin+kPovImgCols/2)
#define kPovScroll 1	/* scroll forwards 1 pixel */

byte gScroll=-kPovScroll,gScrollCh=0;
byte gCharPos=0,gSyncScan;

void ResetPov(void)
{
	gScroll=-kPovScroll;
	gCharPos=gScrollCh=gPovImgPos=0;
	gPovDir=1;
	gSyncScan=0;
}

byte PovMsg(void)
{
	byte scan=0;
	if(gPovImgPos>=kFullScanPeriod) {
		gPovImgPos=0;	// reset position ready for next scan.
		gSyncScan++;
		gScroll+=kPovScroll;	// The new scroll offset.
		while(gScroll>=kPovColsPerChar) {
			gScrollCh++;
			if(gScrollCh>=gPovLen)
				gScrollCh=0;
			gScroll-=kPovColsPerChar;
		}
	}
	if(gPovImgPos==kLHMargin) {	// at exactly the margin we reset the info.
		gCharPos=gScrollCh;			// Start at this character in the msg.
		gPovCol=gScroll;			// and this column.
		if(gPovDir<0) {
			gCharPos+=kPovImgWidth-1;
			gPovCol+=kPovColsPerChar-1;	// scanning from the end.
		}
	}
	if(gPovImgPos>=kLHMargin && gPovImgPos<kRHMargin) { // displayable.
		if(gPovCol>=kPovColsPerChar) {
			gPovCol+=(gPovDir>0)? (-kPovColsPerChar):kPovColsPerChar;
			gCharPos+=gPovDir;	// increment to next char.
			if(gCharPos>=gPovLen)	// wrap to start of msg?
				gCharPos-=gPovLen;	// reset the char pos.
		}
		if(gPovLen>0) {
			scan|=GetPgmByte(kCharBitMap[kPovColsPerChar*(E2PromRdCh(&gPovBuff[
													gCharPos])-' ')+gPovCol]);
		}

		gPovCol+=gPovDir;
	}
	if(gPovImgPos==kPovSyncCol && (gSyncScan&1))
		scan|=0x20;	// segment 'f' is on bit 5 too.
	gPovImgPos++; // next scanning position.
	return scan;
}


/**
 * This is the keyboard processing code for PhorsePOV and consists of 2
 * functions:
 * PhorseButton, which debounces the button and generates Phorse key codes.
 * KeyGet, which processes phorse keycodes into characters.
 **/

#define gDdrD (*((byte *)0x2a))
#define gPortD (*((byte *)0x2b))

#define kButtonPin 8
#define kDotPeriod 2  // >=40ms.
#define kDashPeriod 11  //>=150ms.
#define kModeSwitchPeriod 110	// >=1500ms
#define kKeyNone 1
byte gButtonOnFor=0,gButtonOffFor=0;
byte gNewKey=kKeyNone;
byte gKey=kKeyNone;

#define kDispKey 0
#define kDispMsg 1
#define kDispInfo 2
byte gDispMode=kDispKey;
#define kKeyRun 1

/**
 * PhorseButton debounces the button and generates Phorse key codes in gNewKey.
 * It expects to be called every 20ms.
 *
 * The easiest *non-blocking* way to debounce a key is to simply count the
 * number of times the button is held down when the routine is called.
 *
 * A button press will look something like this to the hardware:
 *
 * Time (ms):      ....:....10...:....20...:....30...:....40...:....50...
 * Button input:   ---_--__-_-_-_________________________--_--__-----
 *
 * When a key is pressed 'properly' it will be held for over 20ms so the
 * gButtonOnFor count will be >1. Counting the button period is useful for
 * identifying '.'s and '-'s - when the button is released we can see
 * how many consecutive times the button was held down when PhorseButton
 * was called.
 *
 * So, the basic method is to count the number of periods the button is held
 * for, resetting it each time the button appears to be released.
 * We also count the number of periods the button has been released for to
 * distinguish between entering a digit or an entire character.
 * Because we count the button periods we need to prevent the counts from
 * wrapping around if they've been held too long (>5s in our case).
 *
 * How To Encode Phorse Code (and Morse Code).
 * *******************************************
 * The intuitive way to encode Morse code (and Phorse code) is by storing the
 * sequence of '.'s and '-'s as a binary sequence along with the length of
 * the sequence. That's because spaces are used as a third symbol to encode
 * variable length characters.
 *
 * However, we can observe that since there's 2 * 1-bit characters: . and -
 * + 4 * 2-bit characters: .., .-, -. and --
 * + 8 * 3-bit characters: ..., ..-, .-., .--, -.., -.-, --. , ---.
 * Then for any morse-code type system up to n bits, there will be: 2^(n+1)-2
 * characters that can be encoded, which means we should be able to store all
 * the encodings in n+1 bits.
 * The easiest way to do that is to preceed each bit pattern with an imaginary
 * '-'. Thus, . and - are encoded as 10 and 11; .., .-, -., -- as 100 101 110
 * and 111 etc. This technique will work for any variable-length morse code
 * type system.
 **/
void PhorseButton(void)
{
    if((PINB&kButtonPin)==0) {	// if the button is down,
      if(gButtonOnFor<255) {	// as long as we don't wrap around,
        gButtonOnFor++;  // increment the number of times the button is held.
      }
      if(gButtonOnFor==kModeSwitchPeriod) {		// handle display / data mode.
      	gDispMode=(gDispMode==kDispKey)? kDispMsg:kDispKey;
      	ResetPov();
      }
      gButtonOffFor=0;	// reset the release count.
    }
    else {  // button is released.
		if(gButtonOffFor<255)	// prevent wrap around.
		  gButtonOffFor++;
		if(gButtonOnFor>=kDotPeriod && gButtonOnFor<kModeSwitchPeriod) {
			gNewKey=(gNewKey<<1)|(gButtonOnFor>=kDashPeriod);  // add . or -
		}
		if(gButtonOffFor>=kDashPeriod) {  // it was off for end of char.
		  if(gKey==kKeyNone) {  // old key consumed.
			gKey=gNewKey;
			gNewKey=kKeyNone;	// was 255.
		  }
		}
        gButtonOnFor=0;	// reset the pressed count.
    }
}

#define kKeyTableSize 32
#define kKeyNumMode 6
#define kKeyAlphaMode 7
#define kKeyAlt 8
#define kKeyClr 33 /* kKeyClr = ....- */
#define kKeyDel 34 /* kKeyDel = ...-. */
#define kKeyResetText 35 /* ...-- */
#define kKeyMaxTime 49 /* = -...- */

char kAlphaTable[kKeyTableSize] PROGMEM = {
  '\0', kKeyNone, '.', ' ', 'S', 'Z', kKeyNumMode, kKeyAlphaMode,
  'T', 'W', 'A', 'D', 'G', 'J', 'M', 'P',
  'U', 'V', 'X', 'Y', 'B', 'C', 'E', 'F',
  'H', 'I', 'K', 'L', 'N', 'O', 'Q', 'R', 
};

char kNumTable[kKeyTableSize] PROGMEM = {
  '\0', kKeyNone, '.', ' ', '0', '1', '#', kKeyAlphaMode,
  '8', '9', '2', '3', '4', '5', '6', '7',
  '(', ':', ')', '\'', '\"', '!', '@', '-',
  '$', '+', '%', kKeyAlt, '*', '=', '/', ';',
};

char *gKeyTable=kAlphaTable;

/**
 * KeyGet consumes a gKey if it isn't kKeyNone.
 * It translates any gKey keycode into a character using a translation table:
 * either kAlphaTable or kNumTable.
 * The key codes: kKeyNumMode and kKeyAlphaMode cause the translation table to
 * be switched.
 * Key codes above kKeyTableSize aren't translated, this means that we can
 * use them for commands.
 *
 * 
 **/
char KeyGet(void)
{
    byte ch=gKey;
    if(ch!=kKeyNone)
      gKey=kKeyNone;  // allow another char.
	if(ch<kKeyTableSize)	// translate key if it's in range.
		ch=GetPgmByte(gKeyTable[ch]);
	if(ch==kKeyNumMode) {	// switch to num table?
		gKeyTable=kNumTable;
		ch=kKeyNone;
	}
	else if(ch==kKeyAlphaMode) {	// switch to alpha table?
		gKeyTable=kAlphaTable;
		ch=kKeyNone;
	}
    return ch;
}

/**
 * EEProm String routines.
 * We split erase and writes so that we can support string writes with
 * only 1 erase/write cycle per added character.
 * Normally a character would be added to a string by writing the character
 * and following it by '\0' to terminate the string. However, this involves
 * two writes per character. However, if we use 0xff as the string terminator
 * an erase with do the job of terminating a string. Thus clearing a string
 * is simply done by erasing the first character and adding a character is
 * simply done by writing to the erased char and then erasing the next char
 * (thus terminating that string).
 * Writing a byte will glitch the display since there will be 1.8ms between
 * The write and subsequent erase. 1.8ms is equivalent to 4 interrupts.
 **/

#define E2PromClr(addr) { 	\
	while(EECR & (1<<EEPE)) /* wait */ \
		;					\
	EECR = (0<<EEPM1)|(1<<EEPM0); /* erase only mode */ \
	EEAR = (ushort)addr; \
	cli();	/* need to disable interrupts here */ \
	EECR |= (1<<EEMPE); /* prep erase */ \
	EECR |= (1<<EEPE);  /* erase */ \
	sei();	\
	}

#define E2PromWr(addr,data) { 	\
	while(EECR & (1<<EEPE)) /* wait */ \
		;					\
	EECR = (1<<EEPM1)|(0<<EEPM0); /* write only mode */ \
	EEAR = addr; \
	EEDR = data; /* and set the data byte */ \
	cli();	/* need to disable interrupts here */ \
	EECR |= (1<<EEMPE); /* prep write */ \
	EECR |= (1<<EEPE);  /* write */ \
	sei();	\
	}

char E2PromRdCh(char* addr)
{
	while(EECR & (1<<EEPE))
		;
	EEAR=(ushort)addr;
	EECR |= (1<<EERE); /* start read */
	return (char)EEDR;
}

// POV key handling.

#define kBlah 1

void PovDisp(void)
{
   gPortD=0x5;
}

#define kPovBuffSize 32
#define kPovDefaultMsg "PHORSEPOV (C)NICHEMACHINES 2010\377"
char gPovBuff[kPovBuffSize+1] EEMEM=kPovDefaultMsg;
char gDefaultBuff[kPovBuffSize+1] EEMEM=kPovDefaultMsg;
#define kEndOfString -1
byte gPovLen=0;

void PovEndStr(void)
{
	E2PromClr((ushort)&gPovBuff[gPovLen]);
}

void PovAddCh(char ch)
{
    if(gPovLen<kPovBuffSize-1) {
    	E2PromWr((ushort)&gPovBuff[gPovLen++],ch);
    	PovEndStr();
    }
}

void PovClr(void)
{
    gPovLen=0;
	PovEndStr();
}

void PovGetLen(void)
{
	byte len=0;
	while(len<kPovBuffSize && E2PromRdCh(&gPovBuff[len])!=kEndOfString)
		len++;
	gPovLen=len;
}

void PovDelCh(void)
{
	if(gPovLen>0) {
		gPovLen--;
		PovEndStr();
	}
}

/**
 * Timer code. We run at 1MHz.
 * The main POV interrupt will be about 250us, 1/256 clk.
 * The keyboard interrupt should be 20ms. 80 POV interrupts.
 * We'll use minimal resources.
 * 250us (KHz/8)/(kPovFreqInKHz) (actually generates 248us).
 * The timer update isn't drift-free, because the EEProm write will mess
 * up the next ISR interrupt.
 **/
#define kClkKHz 1000
#define kPrescale 8
#define kPovFreq 4 // was 2.
#define kPovPeriod ((kClkKHz/kPrescale)/kPovFreq)

/**
 * The ISR is simply used to generate events at 4KHz.
 **/

ISR(TIM0_COMPA_vect)
{
	OCR0A=TCNT0+kPovPeriod;
}

void Setup()
{
  //Serial.begin(9600); 
  DDRB&=~kButtonPin;	// button is input.
  PORTB|=kButtonPin;  // use with pullup.
  cli();
  TCCR0A=0;	//no COM0A output, nor COM0B output, waveform=normal.
  OCR0A=kPovPeriod;
  TCCR0B=2;	//start clock, clk/8; gives us 2048us per overflow
  TIMSK|=(1<<OCF0A);	// enable CompA interrupt.
  sei();
  PovGetLen();
}

/**
 * Because we don't do PovMsg and keyboard scanning during the same event,
 * the gKbdCount is only incremented 5/6 times so kKbdRate=33 => 50.5Hz.
 **/
#define kKbdRate 67	// was 33
byte gKbdCount=0;

void Idle(void)
{
	set_sleep_mode(0);
	cli();
	sleep_enable();
	sei();
	sleep_cpu();
	sleep_disable();
}

byte BinaryToScan(byte val)
{
	byte ix,scanPattern=0;
	for(ix=0;ix<3;ix++) {
		scanPattern<<=2;
		if(val&4)
			scanPattern++;
		if(val&0x20)
			scanPattern+=2;
		val<<=1;
	}	// calc pattern into scanPattern.
	return scanPattern;
}

/**
 * Test code for bringing up PhorsePOV.
 **/
//#define _PhorseTests_
#ifdef _PhorseTests_

#define _PhorseTestsProc_(x) x

//#define _PhorseTest01_
//#define _PhorseTest02_
//#define _PhorseTest03_
#define _PhorseTest04_
void Blinky()
{
	byte ix;
#ifdef _PhorseTest04_
	PovClr();	// in this test, we don't use the default message.
	return ;
#endif
#ifdef _PhorseTest03_
	byte povImg=0,newKey=0,countRate=64;
#endif

	DDRB|=7;	// pins 0..2 are output.
	for(;;) {
#if defined(_PhorseTest01_) || defined(_PhorseTest02_)
		ushort i;
		for(ix=0;ix<7;ix++) {
			PORTB=ix;	// ok, output the port setting.
#ifdef _PhorseTest01_
			for(i = 0; i < 10; i++){
				_delay_ms(30); // max is 262.14 ms / F_CPU in MHz 
			}	// delay 300ms.
#endif
#ifdef _PhorseTest02_
			for(i=0;i<273;i++)
				Idle();	// 0.55ms delay. * 545 => 300ms roughly.
#endif
		}
#endif
		
#ifdef _PhorseTest03_
		Idle();	// do the 0.5ms sleep.
		if(DoPovScan(povImg)==kTrue) {	// generate a scan for the current code.
			if(--countRate==0) {
				povImg=BinaryToScan(++newKey);
				countRate=64;
			}
		}
#endif
	}
}

#else

#define Blinky()

#define _PhorseTestsProc_(x)

#endif

byte gMaxPovTime=0;


/**
 * The main code is event driven - it's an easy way of providing
 * a real time response with few resources whilst avoiding deadlocks and
 * other race hazards.
 *
 * There's 2 main tasks:
 *    The DoPovScan routine which runs at 4KHz. The subtasks: PovMsg and
 *        BinaryToScan are called every time DoPovScan completes a scan,
 *        which is every 1.5ms. All the subtasks however, must run in <250us
 *        to maintain the realtime response.
 *    The PhorseButton task which runs at 50Hz. The subtask KeyGet is called
 *        whenever a key is available.
 * 
 **/
int main()
{
	byte povImg=0;
	char ch;
	Setup();	// setup I/O and timer.
	_PhorseTestsProc_(Blinky());
	for(;;) {	// loop forever.
		Idle();	// sleep();
		if(DoPovScan(povImg)==kTrue) {
			switch(gDispMode) {
			case kDispMsg:
				{
					byte povTime=TCNT0;
					povImg=PovMsg();	// handle the next column.
					povTime=TCNT0-povTime;
					if(povTime>=gMaxPovTime) {	// we have an error.
						gMaxPovTime=povTime;
					}
				}
				break;
			case kDispKey:
				povImg=BinaryToScan(gNewKey);
				break;
			case kDispInfo:
				break;
			}
		}
		else if(++gKbdCount>=kKbdRate) {
			gKbdCount=0;
			PhorseButton();
			if((ch=KeyGet())!=kKeyNone) {
				switch(ch) {
				case kKeyClr:
					PovClr();
					break;
				case kKeyDel:
					PovDelCh();
					break;
				case kKeyResetText:
					{
					byte len;
					PovClr();
					for(len=0;len<kPovBuffSize;len++)
						PovAddCh(E2PromRdCh(&gDefaultBuff[len]));
					}
					break;
				case kKeyMaxTime:
					povImg=BinaryToScan(gMaxPovTime>>2);
					gDispMode=kDispInfo;
					break;
				default:
					PovAddCh(ch);
					break;
				}
			}
		}
	}
}
