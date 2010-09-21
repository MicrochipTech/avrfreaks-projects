// ad_7seg.h
//
// header file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//

#include <iom16v.h>
#include <macros.h>
#include <stdio.h>

#define OK_7SEG

#define SEG7_ON  1
#define SEG7_OFF 0
// these are the leading zero fill values
#define FILL1    0
#define FILL2    1
#define FILL3    2
#define FILL4    3
// these are the decimal point flags
#define DP_1     0x04
#define DP_10    0x08
#define DP_100   0x10
#define DP_1000  0x20

// ----------- global variables
extern char seg7_stat;          // flag for 7 segment display on
extern char seg7_dig[];         // segment map for digits
extern char seg7_s[];           // segment outputs
extern char s7cnt;              // interrupt counter

// ----------- function prototypes
void init_7seg(void);           // initialization for 7 segment display
void seg7(char);                // turn 7 segment display on/off
void seg7_hex4(unsigned int);   // display 4 hex digits on 7 seg display
void seg7_hex2(char, char);     // display 2 hex digits on 7 seg display
void seg7_hex1(char, char);     // display 1 hex digit on 7 seg display
void seg7_disp(char *);         // display segments on 7 segment display
void seg7_num(int, char);       // display number on 7 segment display
