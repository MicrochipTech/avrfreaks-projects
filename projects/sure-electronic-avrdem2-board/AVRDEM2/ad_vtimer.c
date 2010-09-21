// ad_vtimer.c - virtual timer routines
//
// source file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//
#include <iom16v.h>
#include <macros.h>
#include "ad_vtimer.h"

// timer resolution is ~0.01 second. timers serviced every 10 interrupts,
// but not on the same interrupt, so they need to be considered asynchronous
// in relation to each other.
//
// the virtual timers are numbered 1 to NVTIMER. each virtual timer has
// an entry in atime[], with timer #1 being at atime[0]. once a timer is
// set it is considered available, even though it may be inactive.

volatile struct vt_struct atime[NVTIMER];   // the virtual timers

// ------------------------------------------------------------- 
// set_vtime(n, t, mode, &flag) - set virtual timer
//
// n    = timer number (1 - NVTIMER)
// t    = time in 1/100 seconds (1 - 60000, about 10 minutes)
//
// this sets the timer, but may also start it. when the timer is started
// (with this call or separately), it will start on either the next (10x)
// interrupt or will effectively ignore the next (10x) interrupt.
//
// mode = action
//    V_NOW    - start the timer immediately (use n directly)
//    V_NEXT   - start the timer on the next interrupt (use n + 1)
//
// (the following may be ORed to one of the above)
//    V_GO     - combine with above: start timer when set
//    V_HOLD   - combine with above: do not start when set (default)
//    V_ONCE   - combine with above: stop after first run out
//    V_REPEAT - combine with above: keep running after run out (default)
// &flag - pointer to flag that will be set when time elapses
//
// note that times will be off by up to almost a full 1/100 second (ten
// interrupt periods). a V_NOW with t = 1 could run out almost immediately
// (0 to 0.01 sec), while V_NEXT, t = 1 will run out in 0.01 to 0.02 secs.
//
// if mode does not include V_GO, then the timer will not be started.
// the flag is cleared when the timer is set, and set when it runs out.
// if mode does not include V_ONCE, timer will continue to run (caller must
// clear flag after each run out on a free running timer).

void set_vtime(char n, unsigned int t, char mode, char *pflag) {
  unsigned int stime;

  if (n < 1 || n > NVTIMER || t < 1 || t > 60000) return;

  n--;                  // change from timer number to subscript
  stime = t;            // the time we'll use for future set operations
  switch (mode & V_MASK) {
    case V_NOW:         // start immediate (next interrupt decrements)
      break;
    case V_NEXT:        // defer start (next interrupt ignored)
      t++;              // bump initial count by 1
      break;
    default:
      return;
  }
  
  *pflag = 0;                     // clear caller's runout flag

  CLI();                          // can't have the interrupt handler messing us up
  atime[n].max = stime;           // count used for next reset
  atime[n].count = t;             // current count
  atime[n].mode = mode & V_ONCE;  // free running (=0) or one shot (=V_ONCE)
  atime[n].active = mode & V_GO;  // whether we're running (=V_GO) or stopped (=0)
  atime[n].flag = pflag;          // save pointer to their flag variable
  SEI();                          // all done - let 'er rip
}

// -------------------------------------------------------------
// run_vtime(n, action) - manipulate a virtual timer
//
// n = timer number (1 - NVTIMER, 0 for all)
// action:
//    V_KILL   - release the timer's resources
//    V_START  - start timer using set time value
//    V_RESUME - start timer using current time value
//    V_STOP   - stop timer
//    V_RUNOUT - force immediate timer runout
//    V_NOP    - no action (can used to set the options below
//               without affecting the timer)
// (the following may be ORed to one of the above)
//    V_ONCE   - stop timer next time it runs out
//    V_REPEAT - keep timer running next time it runs out
//
// if the timer has run out and stopped, V_RESUME will start it
// using the set time value. resuming a running timer is ignored.
// when a timer(s) is started or resumed, its flag will be cleared.

void run_vtime(char n, char action) {
  char j1, j2, i, newact;
  unsigned int newcnt;

  if (n > NVTIMER) return;
  if (n) {
    j1 = j2 = n - 1;    // do an individual timer
  } else {
    j1 = 0;             // manipulate all timers - first subscript
    j2 = NVTIMER - 1;   // last subscript
  }

  CLI();                // put the interrupt handler to sleep

  for (i = j1; i <= j2; i++) {          // do all the timers in range
    newact = atime[i].active;           // current active status
    if (newact != V_UNDEF) {            // only update set timers
      if (action & (V_ONCE | V_REPEAT)) atime[i].mode = action | V_ONCE;
    
      switch (action & V_MASK) {
        case V_STOP:            // stop the timer
          newact = 0;
          break;
        case V_START:           // start the timer using set time
          atime[i].count = atime[i].max;
          newact = V_GO;
          break;
        case V_RESUME:          // start the timer using current time
          if (atime[i].count < 1) atime[i].count = atime[i].max;
          newact = V_GO;
          break;
        case V_KILL:            // release timer
          newact = V_UNDEF;
          break;
        case V_RUNOUT:          // force immediate runout
          *atime[i].flag = 1;   // set the user flag
          if (atime[i].mode) {  // is this one single shot?
            newact = 0;         // yes - stop the timer
            newcnt = 0;         // counter ran out
          } else {
            newact = V_GO;          // free running
            newcnt = atime[i].max;  // set up for next run
          }
          atime[i].count = newcnt;  // current counter
          break;
        default:          // V_NOP or unknown
          break;
      }
      atime[i].active = newact;
    }
  }
  SEI();                // release the cracken
}

// -------------------------------------------------------------
// init_vtimer() - initialize virtual timers
//
// start out with all timers undefined.

void init_vtimers(void) {
  char i;

  for (i = 0; i < NVTIMER; i++) {
    atime[i].active = V_UNDEF;      // timer is undefined
  }
}
