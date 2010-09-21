// ad_EEPROM.h
//
// header file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//

#include <iom16v.h>
#include <macros.h>
#include <stdio.h>
#include "ad_twi.h"

#define OK_EEPROM

#define EE_W_ADDR 0xa0         // write command
#define EE_R_ADDR 0xa1         // read command


// ----------- function prototypes
extern void init_eeprom(void);                             // initialize TWI
extern char ee_rdy(void);                                  // see if EEPROM ready
extern char eewr_page(unsigned int, char *, char);         // write paged bytes
extern char eewr_byte(unsigned int, char *);               // write one byte
extern char eerd_seq(unsigned int, char *, unsigned int);  // read sequential bytes
extern char eerd_byte(unsigned int, char *);               // read one byte
extern char ee_addr(unsigned int);                         // set EEPROM address
extern char eerd_next(char *, unsigned int);               // read next bytes



// extern int eewr_seq(unsigned int, char *, unsigned int);  // write sequential bytes

