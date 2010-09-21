// ad_temp.c
//
// source file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated

#include "ad_temp.h"

volatile char lm75[2];  // raw temperature read from lm75
struct twi_struct slm;  // script definition to read temperature

volatile __flash unsigned int scr75[] = {          // the script to read the temperature
  STS_START | (TS_SNDW << 8),                        // send write cmd
  STS_SLW   | (TS_SNDF << 8),                        // send fixed data (reg select)
  STS_DATA  | (TS_RS << 8),                          // send restart to switch to read
  STS_RS    | (TS_SNDR << 8),                        // send read cmd
  STS_SLR   | (TS_RCV_A << 8),                       // start receive & advance
  STS_ACK | W_FIRST | ((MS_STORE | TS_RCV_H) << 8),  // store, receive & hold
  STS_NAK | W_LAST  | ((MS_STORE | TS_STOP_R) << 8)  // store, quit
};

// -------------------------------------------------------------
// init_temp() - initialize the temperature device
//
// since there's only one script for the device, we'll go ahead
// and set it all up here

void init_temp(void) {
  slm.sptr = scr75;     // points to the text of the script
  slm.slr  = 0x91;      // read command for LM75
  slm.slw  = 0x90;      // write command for LM75
  slm.fixed = 0;        // register select (0 = temperature)
  slm.dptr = lm75;      // where the results go
  slm.dcnt = 2;         // number of bytes to read
}

// -------------------------------------------------------------
// read_temp() - read the temperature device
//
// try to start the script to read the temperature device (LM75).
// when twi_sts is equal to TS_OK, the script has completed and
// the raw results are in lm75[0] and lm75[1].
//
// returns: 0 if script is started; 1 if the twi hardware is busy

char read_temp(void) {

  return run_twi(&slm);

}

// -------------------------------------------------------------
// celcius(rd) - convert temp in lm75[] to celcius
//
// rd - if nonzero, read the temp first (and wait for results)
//
// returns: temp in 0.5 degrees (i.e., 250 = 125 degrees)
//          999 for twi error (if read specified)

int celcius(char m) {
  int v;

  if (m) {
    if (run_twi(&slm)) return 999;      // can't run script
    while (twi_sts == TWI_RUN) ;        // wait
    if (twi_sts != TWI_OK) return 999;  // huh?
  }

  v = lm75[0] << 1;
  if (lm75[1] & 0x80) v++;

  if (v & 0x100) v |= 0xfe00;           // extend sign bit
  return v;
}
