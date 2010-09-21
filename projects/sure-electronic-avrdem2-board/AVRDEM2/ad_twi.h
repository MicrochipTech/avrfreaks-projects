// ad_twi.h
//
// header file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//
// since the twi code is used by both the LM75 (temperature) and EEPROM
// devices, the mainline may try to load this header file twice. not
// a Good Idea [tm].

#ifndef OK_TWI                  // only load this file once

#include <iom16v.h>
#include <macros.h>
#include <stdio.h>
#define OK_TWI

struct twi_struct {
  volatile __flash unsigned int *sptr;   // pointer to the script in flash
  char slr;                     // read command (device address + ??)
  char slw;                     // write command (dev addr + ??) + 0x01
  unsigned int fixed;           // fixed data
  volatile char *dptr;          // pointer to variable data
  volatile unsigned int dcnt;   // count of variable data bytes
};

// ----------- function prototypes
void init_twi(void);                 // initialize the twi hardware
char run_twi(struct twi_struct *);   // run twi script

// ----------- TWI status returns
#define STS_START  0x08         // start sent
#define STS_RS     0x10         // repeat start sent
#define STS_DATA   0x28         // data sent, ACK received

#define STS_SLW    0x18         // SLW sent, ACK received
#define STS_SLWN   0x20         // SLW sent, NAK received

#define STS_SLR    0x40         // SLR sent, ACK received
#define STS_SLRN   0x48         // SLR sent, NAK received

#define STS_ACK    0x50         // data received, ACK sent
#define STS_NAK    0x58         // data received, NAK sent

// ----------- TWI status bit flags
// a script consists of a series of two byte entries executed in
// order. the first byte is a combination of the TWI status
// that is expected and an indicator of whether there are more entries
// related to this one. each entry will use either W_ONLY (the default)
// or one of W_FIRST, W_MIDDLE, or W_LAST.
//
// bit 0 = 0 means that this is the last status entry
// bit 1 = 0 means that this is the first status entry
#define MSK_1      0x02         // mask for first in list (bit 1)
#define MSK_N      0x01         // mask for last in list (bit 0)

#define W_ONLY     0x00             // only status (dflt, first and last)
#define W_FIRST    MSK_N            // first of many (0 means true)
#define W_MIDDLE   (MSK_1 | MSK_N)  // middle of multiple status entries
#define W_LAST     MSK_1            // last of many (0 means true)

// ----------- TWI script actions
// "hold" means to leave the script on the same entry
// "advance" means to advance to the next entry

#define MS_STOP    0x40         // send stop bit
#define MS_STORE   0x20         // store indirect (receive) bit
#define MS_XTRA    0x10         // extra data (stop, recv, and send indirect)
#define MS_SEND    0x08         // send commands have this bit set

#define TS_NOP     0x00         // nop
#define TS_RERUN   0x01         // restart the script from beginning
#define TS_RCV_A   0x02         // start receiver & advance script
#define TS_RCV_H   (TS_RCV_A | MS_XTRA)  // start rcvr & hold script
#define TS_RS      0x03         // send restart to device

#define TS_SNDF    (MS_SEND | 0x00)  // send fixed
#define TS_SNDW    (MS_SEND | 0x01)  // send write command
#define TS_SNDR    (MS_SEND | 0x02)  // send read command
#define TS_SNDI_A  (MS_SEND | 0x03)  // send indirect, advance script
#define TS_SNDI_H  (TS_SNDI_A | MS_XTRA)  // send indirect, hold script

#define TS_STOP_R  MS_STOP                // stop & set ready status
#define TS_STOP_N  (TS_STOP_R | MS_XTRA)  // stop & set not ready status

// ----------- TWI status as seen by user
#define TWI_DIS     0x00        // twi hardware disabled, no script
#define TWI_OK      0x01        // hardware enabled, no script running
#define TWI_NRDY    0x02        // like TWI_OK, but EEPROM returned a busy status
#define TWI_RUN     0x04        // script running
#define TWI_ERR     0x08        // script error; hardware not enabled (user must set
                                // twi_sts = TWI_DIS when error is recognized)

// ----------- globals
extern volatile char twi_sts;                // twi script status
extern volatile struct twi_struct use;       // active script data
extern volatile __flash unsigned int *snow;  // pointer to current script entry

#endif
