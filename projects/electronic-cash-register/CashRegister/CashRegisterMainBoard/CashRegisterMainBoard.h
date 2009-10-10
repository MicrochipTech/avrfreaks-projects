//----- Include Files ---------------------------------------------------------
#include <avr/io.h>		// include I/O definitions (port names, pin names, etc)
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/eeprom.h> //internal eeprom of AVR
#include "i2ceeprom.h"	//External I2C eeprom (24c256)
#include <avr/interrupt.h>	// include interrupt support
#include "debug.h"		// include debug functions

#define LOCAL_ADDR   	0x01    // Master's address 
#define LCD_DISP_ADDR	0x05
#define PRINTER_1_ADDR	0x06
#define PRINTER_2_ADDR	0x07
#define EXT_EPROM_ADDR  0xA0
#define CLOCK_ADD   	0x09
#define Col1_OFF        PORTA |= (1<<4)
#define Col1_ON         PORTA &= ~(1<<4) 
#define Col2_OFF        PORTA |= (1<<5)
#define Col2_ON         PORTA &= ~(1<<5) 
#define Col3_OFF        PORTA |= (1<<6)
#define Col3_ON         PORTA &= ~(1<<6) 
#define Col4_OFF        PORTA |= (1<<7)
#define Col4_ON         PORTA &= ~(1<<7) 
#define NoLed           0
#define Led1            3
#define Led2            4
#define Led3            5
#define WRITE 0
#define READ 1
#define TIME 1
#define DATE 2
#define functionLeitourgia 0
#define functionX 1
#define functionZ 2
#define functionProg 3
#define eeWP 2 // external eeprom Write Protect pin.
#define sleep()  asm volatile ("sleep" ::)

// local data buffer
unsigned char localBuffer[8];
unsigned char printerBuffer[100];
unsigned char rtcBuffer[12] = {0x50, 0x12, 0x08, 0x10, 0x01, 0x01, 0x07}; 
 // Real Time Clock (seconds, Hours, Minutes, Day, Month, Year).
unsigned char rtcBuffer2[12] = {0x50, 0x12, 0x08, 0x10, 0x01, 0x01, 0x07}; 
 // Δ:Λ:Ω:Η:Μ:Η(1-7):Χ
unsigned char eepromBuffer[32] = {0xAA, 0xFF};
 // πίνακας με τα δεδομένα που θα γραφτούν στην εξωτερική eeprom (24c256).

const char message8[] EEMEM = {0,1,0,0,0,0,0,0,0,0};

const char message1[] PROGMEM = " SYNOLO -------!"; 
const char message2[] PROGMEM = " METRHTA ------!";
const char message3[] PROGMEM = " SYNOLO XWRIS  !";
const char message4[] PROGMEM = " FPA ----------!";
const char message5[] PROGMEM = " FPA  9% ------!";
const char message6[] PROGMEM = " FPA 19% ------!";
const char message7[] PROGMEM = " RESTA --------!";
const char message9[] PROGMEM = "NOMIMH APODEIJH!";

const char percent[] PROGMEM = "            19%!";
const char scrollTape[] PROGMEM = "nn             !";
const char emptyLine[] PROGMEM = "               !";
const char Logo1[] PROGMEM = "==============="
                             "  EKPAIDEYTIKH " 
                             " TAMIAKH MHXANH"
                             "===============!";

const char Time1[] PROGMEM = "05/01/07 16:41 "; 
                             
unsigned char localBufferLength = 0;
unsigned char NoOfPrintNumbers =0;
unsigned char NoOfPrintNumbers2 =0;
unsigned char r = 0;

// Καμία εντολή δέν δώθηκε ακόμα (Μετρητά, Πληρωμή, Τμήμα1, Τμήμα2).
// 0= κανένα πλήκτρο
// 1= πλήκτρο <Μετρητά>
// 2= πλήκτρο <Πληρωμή>
unsigned char command = 0; 

// 1= στην απόδειξη που θα ακολουθήσει να εκτυπωθεί το λογότυπο
// 0= στην απόδειξη που θα ακολουθήσει να μην εκτυπωθεί το λογότυπο.
unsigned char newReceiption = 1; 

unsigned int Zcounter = 0; // Δώσε σε αυτόν τον καταχωρητη τιμή 0

unsigned char i;
unsigned long Price = 0;  // Γενικός καταχωρητής τιμών
unsigned long PriceA = 0; // Καταχωρητής για τιμές με ΦΠΑ 9%
unsigned long PriceB = 0; // Καταχωρητής για τιμές με ΦΠΑ 19%
unsigned long TotalPrice=0;
unsigned long backupPrice = 0;
signed char   No_of_Pressed_key = 0;
unsigned long fpa = 0;
unsigned long temp = 0;
unsigned char functionSelect;
unsigned int  extMemAddress;


unsigned char btnState1;
unsigned char btnState2;
unsigned char btnState3;
unsigned char btnState4;
unsigned char sleepRegister = 0;

void i2cTest(void);
void i2cSlaveReceiveService(u08 receiveDataLength, u08* receiveData);
u08 i2cSlaveTransmitService(u08 transmitDataLengthMax, u08* transmitData);
void i2cMasterSendDiag(u08 deviceAddr, u08 length, u08* data);
void testI2cMemory(void);
void showByte(u08 byte);
void Send_disp_number(int a);
void Convert_Num2ASCII(unsigned long TempPrice);
long Convert_ASCII2Num();
void CheckForPressedButton (void);
void timerPause(u32 count);
void Apoforologisi ();
void CheckKeys (void);
void CheckKeys2 (void);
void PrintLogo (void);
void PrintTime (void);
void multiply (void);
void setTime (unsigned char dateTime);
void PrintfProgStr_P(const char *data);
void addDot (unsigned char percent);
void convertRTCbyte2ASCII (void);
void checkForPower (void);
void StorePricesInEEPROM (void);
