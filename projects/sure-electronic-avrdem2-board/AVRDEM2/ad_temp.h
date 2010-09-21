// ad_temp.h
//
// header file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//

#include <iom16v.h>
#include <macros.h>
#include <stdio.h>
#include "ad_twi.h"

#define OK_TEMP

// ----------- function prototypes
void init_temp(void);           // initialize the temperature device
char read_temp(void);           // run the read-temp script
int celcius(char);              // convert raw data to celcius

// ----------- variables
extern volatile char lm75[];    // raw temperature read from lm75
