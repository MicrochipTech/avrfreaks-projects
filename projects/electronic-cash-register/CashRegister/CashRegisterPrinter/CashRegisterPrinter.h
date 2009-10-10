//*****************************************************
// File Name  : cashRegisterPrinter.c
// Title      : Εκπαιδευτική ταμιακή μηχανή (Printer)
// Target MCU : Atmel ATmega8 MCU at 4MHz XTAL
//*****************************************************
/*
                         HEADER FILES 
-----------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "i2c.h"		// include i2c support

//------------- Defines --------------------
#define dot1        PD0
#define dot2        PD1
#define dot3        PD2
#define dot4        PD3
#define dot5        PD4
#define dot6        PD5
#define dot7        PD6
#define dot8        PD7
#define journal     PC0  // Ρελέ Χατροταινίας εφορίας.
#define reciept     PC1  // Ρελέ Χαρτοταινίας απόδειξης.
#define motorEnable PC2  // Μοτέρ κίνησης κεφαλής.
#define PotSwitch   PB0  // Διακόπτης αρχικής θέσης κεφαλής.
#define optoLetter  PB1  // Οπτοζεύκτης ολοκλήρωσης γράμματος ή αριθμού ή συμβόλου.
#define optoMove    PB2  // Οπτοζεύκτης ένδειξης ελάχιστης μετακίνησης κεφαλής.
#define LOCAL_ADDR	0x06 // Η διεύθυνση αυτού του I2C μικροελεγκτή σε λειτουργία Slave (printer)
#define TARGET_ADDR	0x01 // Η διεύθυνση του κεντρικού μικροελεγκτή (master)


const char num0[] PROGMEM = {0b00111110,0b01010001,0b01001001,0b01000101,0b00111110,255}; // (0)
const char num1[] PROGMEM = {0b00000000,0b01000010,0b01111111,0b01000000,0b00000000,255}; // (1)
const char num2[] PROGMEM = {0b01110010,0b01001001,0b01001001,0b01001001,0b01000110,255}; // (2)
const char num3[] PROGMEM = {0b00100010,0b01000001,0b01001001,0b01001001,0b00110110,255}; // (3)
const char num4[] PROGMEM = {0b00011000,0b00010100,0b00010010,0b01111111,0b00010000,255}; // (4)
const char num5[] PROGMEM = {0b00100111,0b01000101,0b01000101,0b01000101,0b00111001,255}; // (5)	
const char num6[] PROGMEM = {0b00111100,0b01001010,0b01001001,0b01001001,0b00110000,255}; // (6)	
const char num7[] PROGMEM = {0b00000001,0b01110001,0b00001001,0b00000101,0b00000011,255}; // (7)
const char num8[] PROGMEM = {0b00110110,0b01001001,0b01001001,0b01001001,0b00110110,255}; // (8)
const char num9[] PROGMEM = {0b00000110,0b01001001,0b01001001,0b00101001,0b00011110,255}; // (9)

const char letterS[] PROGMEM = {0b01000001,0b01100011,0b01010101,0b01001001,0b01000001,255}; // (Σ)
const char letterY[] PROGMEM = {0b00000111,0b00001000,0b01110000,0b00001000,0b00000111,255}; // (Υ)
const char letterN[] PROGMEM = {0b01111111,0b00000010,0b00000100,0b00001000,0b01111111,255}; // (Ν)
const char letterO[] PROGMEM = {0b00111110,0b01000001,0b01000001,0b01000001,0b00111110,255}; // (Ο)
const char letterL[] PROGMEM = {0b01111100,0b00000010,0b00000001,0b00000010,0b01111100,255}; // (Λ)
const char letterM[] PROGMEM = {0b01111111,0b00000010,0b00001100,0b00000010,0b01111111,255}; // (Μ)
const char letterE[] PROGMEM = {0b01111111,0b01001001,0b01001001,0b01000001,0b01000001,255}; // (Ε)
const char letterT[] PROGMEM = {0b00000001,0b00000001,0b01111111,0b00000001,0b00000001,255}; // (Τ)
const char letterR[] PROGMEM = {0b01111111,0b00001001,0b00001001,0b00001001,0b00000110,255}; // (Ρ)
const char letterH[] PROGMEM = {0b01111111,0b00001000,0b00001000,0b00001000,0b01111111,255}; // (Η)
const char letterA[] PROGMEM = {0b01111100,0b00010010,0b00010001,0b00010010,0b01111100,255}; // (Α)
const char letterK[] PROGMEM = {0b01111111,0b00001000,0b00010100,0b00100010,0b01000001,255}; // (Κ)
const char letterP[] PROGMEM = {0b01111111,0b00000001,0b00000001,0b00000001,0b01111111,255}; // (Π)
const char letterD[] PROGMEM = {0b01111100,0b01000010,0b01000001,0b01000010,0b01111100,255}; // (Δ)
const char letterX[] PROGMEM = {0b01100011,0b00010100,0b00001000,0b00010100,0b01100011,255}; // (Ι)
const char letterI[] PROGMEM = {0b00000000,0b01000001,0b01111111,0b01000001,0b00000000,255}; // (Ι)
const char letterW[] PROGMEM = {0b01001110,0b01010001,0b01010001,0b01010001,0b01001110,255}; // (Ω)
const char letterF[] PROGMEM = {0b00001110,0b00010001,0b01111111,0b00010001,0b00001110,255}; // (Φ)
const char letterJ[] PROGMEM = {0b01000001,0b01001001,0b01001001,0b01001001,0b01000001,255}; // (Ξ)
const char letterSLA[] PROGMEM = {0b01100000,0b00010000,0b00001000,0b00000100,0b00000011,255}; // (/)
const char letterSUM[] PROGMEM = {0b00010100,0b00010100,0b00010100,0b00010100,0b00010100,255}; // (=)
const char letterPER[] PROGMEM = {0b01100011,0b00010011,0b00001000,0b01100100,0b01100011,255}; // (%)
const char letterDIV[] PROGMEM = {0b00000000,0b00100100,0b00100100,0b00000000,0b00000000,255}; // (:)
const char letterSUB[] PROGMEM = {0b00001000,0b00001000,0b00001000,0b00001000,0b00001000,255}; // (-)
const char letterDOT[] PROGMEM = {0b00000000,0b00000000,0b01010000,0b00110000,0b00000000,255}; // (,)
const char letterSPA[] PROGMEM = {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,255}; // ( )


// 
unsigned char c = 0;
unsigned int e = 0;

unsigned int bigBufferLength = 0;
unsigned char localBuffer[255];
unsigned char localBufferLength;
unsigned char bigBuffer[];//   "================"
//                                 "  EKPAIDEYTIKH  ";
/*                                 " TAMIAKH MHXANH "
                                 "================"
                                 "                "
                                 "       10,25 19%"
                                 "       12,24 19%"
                                 "SYNOLO ---------"
                                 "       22,49    "
                                 "                "
                                 "METRHTA --------"
                                 "       50,00    "
                                 "                "
                                 "RESTA ----------"
                                 "       27,51    "
                                 "                "
                                 " 11/02/07 18:20 "
                                 "*";
*/


//=================================================
//                  FUNCTIONS 
//=================================================
void delay_ms(unsigned long count);
void UART_PrintfProgStr_P(const char *data);
void changeLine(void);
void buffer2printer(void);
void i2cSlaveReceiveService(unsigned char receiveDataLength, unsigned char *receiveData);
unsigned char i2cSlaveTransmitService(unsigned char transmitDataLengthMax, unsigned char *transmitData);
void findStart(void);
void scrollTape(void);

