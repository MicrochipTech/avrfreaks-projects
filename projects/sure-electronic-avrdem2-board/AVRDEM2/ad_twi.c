// ad_twi.c
//
// source file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//
// LM75 (temp device) - read address  = 0x91
//                    - write address = 0x90
// EEPROM             - read address  = 0xa1
//                    - write address = 0xa0 + [A8] * 2
//
// for the EEPROM, the high bit of the 9 bit address gets
// embedded in the SLR and SLW commands (bit position 1).
//
// twi_sts shows the status of the twi hardware and script:
//    TWI_DIS    twi hardware disabled, no script
//    TWI_OK     hardware enabled, no script running
//    TWI_RUN    script running
//    TWI_NRDY   script not running, hardware enabled,
//               EEPROM last returned busy status
//    TWI_ERR    script error; hardware not enabled (user must set
//              twi_sts = TWI_DIS when the error is recognized)
//
// a script is made up of a sequence of int entries. the
// low byte is the status expected from the twi hardware (with
// a couple of other bits), while the high byte specifies the
// action(s) to be taken when that status is received. as the status
// is received and the action taken, the script optionally advances
// to the next entry.
//
// there may be multiple status returns for a particular situation,
// and for them multiple values are put into the script. these
// multiple status returns are specified by ORing bits onto the
// low (status) byte in the script. for single entries, leave as-is
// (low 2 bits clear). for multiple entries, OR each with one of the following:
//    W_FIRST    first of multiple status entries  (must be present)
//    W_MIDDLE   middle of multiple status entries (optional)
//    W_LAST     last of multiple status entries   (must be present)
//
// on each TWI interrupt the TWI status is set by the TWI hardware.
// the various status returns recognized are:
//    STS_START  0x08      start sent
//    STS_RS     0x10      repeat start sent
//    STS_DATA   0x28      data sent, ACK received
//    STS_SLW    0x18      SLW sent, ACK received
//    STS_SLWN   0x20      SLW sent, NAK received
//    STS_SLR    0x40      SLR sent, ACK received
//    STS_SLRN   0x48      SLR sent, NAK received
//    STS_ACK    0x50      data received, ACK sent
//    STS_NAK    0x58      data received, NAK sent
//
// also possible (but can't be used in scripts) are 0x00 for a bus
// error, and 0xf8 for no info available. either will be treated as
// an unexpected and unrecognized status and will give an error return.
//
// the actions that the script may take are:
//    TS_NOP        nop
//    TS_RERUN      restart the script from beginning
//    TS_RCV_A      start receiver & advance script
//    TS_RCV_H      start rcvr & hold script
//    TS_RS         send restart to device
//    TS_SNDF       send fixed data
//    TS_SNDW       send write command
//    TS_SNDR       send read command
//    TS_SNDI_A     send indirect data, advance script
//    TS_SNDI_H     send indirect data, hold script
//
// in addition, the following flags may be ORed with the action:
//    MS_STORE      store indirect (received) data
//    TS_STOP_R     stop & set ready status     (TWI_OK)
//    TS_STOP_N     stop & set not ready status (TWI_NRDY)
//
// the script runs until it sends a stop or receives a status
// it doesn't recognize. at successful completion twi_sts is
// set to TWI_OK or TWI_NRDY. in the case of error the twi
// hardware is disabled and twi_sts is set to TWI_ERR. the user
// must set twi_sts = TWI_DIS before another script can be run.

#include "ad_twi.h"

volatile char twi_sts;                // twi script status
volatile struct twi_struct use;       // active script data
volatile __flash unsigned int *snow;  // pointer to current script entry

// -------------------------------------------------------------
// run_twi(spntr) - run a twi script
//
// spntr - pointer to user's twi_struct containing script info
//
// caller will have filled in the appropriate parts of the
// structure. its contents are not changed during execution,
// although the data pointed within it may be.
//
//   .sptr     pointer to the script (required)
//   .slr      read command. this will have bit 0 set 
//   .slw      write command (dev addr + ??), bit 0 cleared
//   .fixed    fixed data (device and command dependent)
//   .dptr     pointer to variable data to read/write
//   .dcnt     count of bytes to read/write using .dptr
//
// the read and/or write command(s) will be the device address with
// any other data (depending on the device) required. the read
// command must have bit 0 set. at least one of them is required.
//
// returns: 0 if script is started; 1 if the twi hardware is busy

char run_twi(struct twi_struct *twi) {
  if (twi_sts == TWI_RUN || twi_sts == TWI_ERR) return 1;

  use = *twi;           // the active version of the data
  snow = use.sptr;      // pointer to the script
  twi_sts = TWI_RUN;    // tell everyone we're running

  // start twi hardware, send start, twi enabled, ints enabled
  TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWIE);

  return 0;
}

// -------------------------------------------------------------
// init_twi() - initialize the twi hardware

void init_twi(void) {
  twi_sts = TWI_DIS;    // disabled, no twi script running

  TWCR = 0x00;          // disable twi
  TWBR = 0x12;          // set bit rate
  TWSR = 0x01;          // set prescale
  TWAR = 0x00;          // set slave address
}
