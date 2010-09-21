// ad_EEPROM.c
//
// source file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//
// the EEPROM chip uses TWI and has 512 bytes
//
// these routines are interrupt driven for the TWI communications.
// the user starts a script to perform the desired task, then checks
// a status return to wait for success or failure.
//
// certain parameters must be preloaded (depends on specific script)
// in a structure prior to starting the script.

#include "ad_EEPROM.h"
struct twi_struct seep;  // script to read/write EEPROM

// ---------------------
// script for sequential read (set address and read up to 512 bytes)
// needs: CSW with addr bit, CSR, EEA -> fixed, dptr, dcnt

volatile __flash unsigned int escript_sr[] = {
  STS_START | W_FIRST | (TS_SNDW << 8),                // send the write command
  STS_RS | W_LAST     | (TS_SNDW << 8),                // send the write command
  STS_SLW | W_FIRST   | (TS_SNDF << 8),                // send the EEA as fixed data
  STS_SLWN | W_LAST   | (TS_RERUN << 8),               // restart the script if busy
  STS_DATA            | (TS_RS << 8),                  // send the restart command
  STS_RS              | (TS_SNDR << 8),                // send the read command
  STS_SLR             | (TS_RCV_A << 8),               // start receiving, advance script
  STS_ACK | W_FIRST   | ((MS_STORE | TS_RCV_H) << 8),  // store data, receive, hold
  STS_NAK | W_LAST    | ((MS_STORE | TS_STOP_R) << 8)  // store data, stop script
};

// ---------------------
// script to perform page write (set address and write up to 16 bytes)
// needs: CSW with addr bit, EEA -> fixed, dptr, dcnt

volatile __flash unsigned int escript_pw[] = {
  STS_START | W_FIRST | (TS_SNDW << 8),              // send the write command
  STS_RS | W_LAST     | (TS_SNDW << 8),              // send the write command
  STS_SLW | W_FIRST   | (TS_SNDF << 8),              // send the EEA as fixed data
  STS_SLWN | W_LAST   | (TS_RERUN << 8),             // restart the script if busy
  STS_DATA            | (TS_SNDI_H << 8),            // write data & hold
  STS_DATA            | ((TS_NOP | TS_STOP_R) << 8)  // stop when data runs out
};


// the following three scripts aren't really needed, but are included
// for completeness. on each read/write the EEPROM is checked for ready,
// and the address is loaded on each use of the two scripts above.
//
// ---------------------
// script to test for EEPROM ready
// needs: CSR

volatile __flash unsigned int escript_rdy[] = {
  STS_START         | (TS_SNDR << 8),    // send the read command
  STS_SLR | W_FIRST | (TS_STOP_R << 8),  // if ACK, we're OK
  STS_SLRN | W_LAST | (TS_STOP_N << 8)   // if NAK, we're busy
};

// ---------------------
// script to read next (using current EEPROM address)
// needs: CSR, dcnt, dptr

volatile __flash unsigned int escript_rn[] = {
  STS_START | W_FIRST | (TS_SNDR << 8),                // send the read command
  STS_RS | W_LAST     | (TS_SNDR << 8),                // send the read command
  STS_SLR | W_FIRST   | (TS_RCV_A << 8),               // start reading & advance
  STS_SLRN | W_LAST   | (TS_RERUN << 8),               // restart the script if busy
  STS_ACK | W_FIRST   | ((MS_STORE | TS_RCV_H) << 8),  // store, receive, hold
  STS_NAK | W_LAST    | ((MS_STORE | TS_STOP_R) << 8)  // store and stop
};

// ---------------------
// script to set EEPROM address
// needs: CSW with addr bit, EEA -> fixed

volatile __flash unsigned int escript_sa[] = {
  STS_START | W_FIRST | (TS_SNDW << 8),   // send the write command
  STS_RS | W_LAST     | (TS_SNDW << 8),   // send the write command
  STS_SLW | W_FIRST   | (TS_SNDF << 8),   // send the EEA as fixed data
  STS_SLWN | W_LAST   | (TS_RERUN << 8),  // restart the script if busy
  STS_DATA            | (TS_STOP_R << 8)  // stop
};

// -------------------------------------------------------------
// init_eeprom() - initialize the external eeprom

void init_eeprom(void) {
  seep.dcnt = 0;          // no data
  seep.slr = EE_R_ADDR;   // read command
  seep.slw = EE_W_ADDR;   // write cmd, plus b1 = A8
}

// -------------------------------------------------------------
// ee_rdy() - is the EEPROM chip ready?
//
// twi_sts will be set to status at some point later
//    = TWI_OK   - EEPROM is ready
//    = TWI_NRDY - EEPROM is busy
//    = TWI_ERR  - other TWI error
//
// returns: 0 - script started
//          1 - script not started
//
// note: it is not necessary to call this routine. its function
//       is included in the other routines. however, the other
//       routines will not release the twi bus while they are
//       waiting for the EEPROM to become ready

char ee_rdy(void) {
  seep.sptr = escript_rdy;    // the text of the script
  return (run_twi(&seep));
}

// -------------------------------------------------------------
// eewr_page(addr, &v, n) - write bytes to page in EEPROM chip
//
// addr - starting address (0 - 511)
// &v   - address of first byte to write
// n    - number of bytes to write (1 - 16)
//
// twi_sts will be set to status at some point later
//    = TWI_OK   - EEPROM write successful
//    = TWI_ERR  - other TWI error
//
// note: do not change values (*v) until status returns
//
// returns: 0 - script started
//          1 - script not started
//
// all bytes will be written to the same page (32 pages of 16 bytes)

char eewr_page(unsigned int addr, char *v, char n) {

  if (addr > 511 || n < 1 || n > 16) return (1);

  seep.slw = (addr & 0x100) ? (EE_W_ADDR | 0x02) : EE_W_ADDR;  // A8 --> bit 1
  seep.fixed = addr & 0xff;     // address 0-7 is EEA
  seep.sptr = escript_pw;       // page write script
  seep.dptr = v;                // where the values are
  seep.dcnt = n;                // number of bytes to write

  return (run_twi(&seep));
}

// -------------------------------------------------------------
// eewr_byte(addr, &vptr) - write single byte to EEPROM chip
//
// addr - address to write (0 - 511)
// vptr - pointer to value to write
//
// twi_sts will be set to status at some point later
//    = TWI_OK   - EEPROM write successful
//    = TWI_ERR  - other TWI error
//
// note: do not change value (*vptr) until status returns
//
// returns: 0 - script started
//          1 - script not started

char eewr_byte(unsigned int addr, char *v) {
  return (eewr_page(addr, v, 1));
}

// -------------------------------------------------------------
// eerd_seq(addr, &v, n) - read sequential from EEPROM chip
//
// addr - address to read (0 - 511)
// &v   - address of where to store bytes
// n    - number of bytes to read (1 - 512)
//
// twi_sts will be set to status at some point later
//    = TWI_OK   - EEPROM read successful
//    = TWI_ERR  - other TWI error
//
// note: do not use values (*v) until status returns
//
// returns: 0 - script started
//          1 - script not started

char eerd_seq(unsigned int addr, char *v, unsigned int n) {
  if (addr > 511 || n < 1 || n > 512) return (1);

  seep.slw = (addr & 0x100) ? (EE_W_ADDR | 0x02) : EE_W_ADDR;  // A8 --> bit 1
  seep.fixed = addr & 0xff;     // address 0-7 is EEA
  seep.sptr = escript_sr;       // sequential read script
  seep.dptr = v;                // where the results go
  seep.dcnt = n;                // number of bytes to read

  return (run_twi(&seep));
}

// -------------------------------------------------------------
// eerd_byte(addr, &v) - read a byte from the EEPROM chip
//
// addr - address to read (0 - 511)
// &v   - address of where to store value
//
// twi_sts will be set to status at some point later
//    = TWI_OK   - EEPROM read successful
//    = TWI_ERR  - other TWI error
//
// note: do not use value (*v) until status returns
//
// returns: 0 - script started
//          1 - script not started

char eerd_byte(unsigned int addr, char *v) {
  return (eerd_seq(addr, v, 1));
}

// -------------------------------------------------------------
// ee_addr(addr) - set EEPROM address
//
// addr - address to read (0 - 511)
//
// twi_sts will be set to status at some point later
//    = TWI_OK   - EEPROM read successful
//    = TWI_ERR  - other TWI error
//
// returns: 0 - script started
//          1 - script not started

char ee_addr(unsigned int addr) {
  if (addr > 511) return (1);

  seep.slw = (addr & 0x100) ? (EE_W_ADDR | 0x02) : EE_W_ADDR;  // A8 --> bit 1
  seep.fixed = addr & 0xff;     // address 0-7 is EEA
  seep.sptr = escript_sa;       // set address script
  return (run_twi(&seep));
}

// -------------------------------------------------------------
// eerd_next(&v, n) - read next bytes from EEPROM chip
//
// &v   - address of where to store bytes
// n    - number of bytes to read (1 - 512)
//
// twi_sts will be set to status at some point later
//    = TWI_OK   - EEPROM read successful
//    = TWI_ERR  - other TWI error
//
// note: do not use values (*v) until status returns
//
// returns: 0 - script started
//          1 - script not started
//
// this uses the current EEPROM address

char eerd_next(char *v, unsigned int n) {
  if (n < 1 || n > 512) return (1);

  seep.sptr = escript_rn;       // read next script
  seep.dptr = v;                // where the results go
  seep.dcnt = n;                // number of bytes to read

  return (run_twi(&seep));
}

/*

// -------------------------------------------------------------
// eewr_seq(addr, &v, n) - write sequential bytes to EEPROM chip
//
// addr - starting address (0 - 511)
// &v   - address of first byte to write
// n    - number of bytes to write (1 - 512)
//
// returns: number of bytes written (may be 0), -1 for TWI error
//
// this writes across page boundaries, but will not wrap back to
// the beginning of the EEPROM (address 0x000)

int eewr_seq(unsigned int addr, char *v, unsigned int n) {
  return (-1);
}

*/
