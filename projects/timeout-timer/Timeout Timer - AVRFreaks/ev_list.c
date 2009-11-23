//      ev_list.c
//
//      support for event list

#include <iom169v.h>
#include "ev_list.h"

char locked;            // whether event list is locked (0 = no)
char nevents;           // number of active events (0 to EMAX+1)
char etime;             // interrupt counter for event updates
char bypass;            // whether we were locked when interrupt happened

volatile struct s_events elist[EMAX];    // event list


// -------------------------------------------------------
//      check_events() - see if an event is ready

void check_events(void) {
  if (nevents) {                // only check if one or more active events
    if (!elist[0].tics) {       // if next event's count == 0, time to boogie
      void (*fun)(void);        // declare a pointer to a function

      fun = elist[0].efun;      // save our function pointer
      collapse_list();          // remove the top event
      (*fun)();                 // execute the event function
    }
  }
}

// -------------------------------------------------------
//      collapse_list() - remove the top event

void collapse_list(void) {
  locked = 1;                   // keep interrupt handler out of this
  nevents--;                    // one fewer events now
  if (nevents) {                // if there are any left, move them up in list
    char i;
    for (i = 0; i < nevents; i++)
      elist[i] = elist[i + 1];
  }
  locked = 0;                   // set interrupt handler loose
}

// -------------------------------------------------------
//      clear_events() - throw out all events

void clear_events(void) {
  locked = 1;                   // keep the interrupt handler away
  nevents = 0;                  // no active events
  etime = 0;                    // start int counter over
  bypass = 0;                   // not in the middle of a bypass
  locked = 0;                   // open the interrupt handler's cage
}

// -------------------------------------------------------
//      add_event(function, count) - add an event to the event list
//
// call the function when 'count' time (1/10 sec units) has elapsed

void add_event(void (*fun)(), unsigned int cnt) {
  if ((nevents <= EMAX) && cnt) {
    locked = 1;                         // keep the interrupt handler at bay
    elist[nevents].efun = fun;          // address of function for event
    elist[nevents++].tics = cnt;        // nonzero tic count
    sort_events();                      // keep them in ascending order
    locked = 0;                         // release interrupt handler
  }
}

// -------------------------------------------------------
//      sort_events() - sort event list ascending on remaining timer tics

void sort_events(void) {
  if (nevents > 1) {            // one or fewer events is already sorted
    char i, swap;
    struct s_events hold;

    swap = 1;                   // force first pass
    while (swap) {              // repeat as long as something was out of order
      swap = 0;                 // assume no swapping will be done
      for (i = 1; i < nevents; i++)
        if (elist[i - 1].tics > elist[i].tics) {
          swap = 1;                 // flag we made an exchange
          hold = elist[i];          // exchange order of events
          elist[i] = elist[i - 1];
          elist[i - 1] = hold;
        }
    }
  }
}

// -------------------------------------------------------
//      update_events() - decrement timers for events
//
// called by interrupt handler every 1/10 second unless events are locked, in which
// case it's called as soon thereafter as possible

void update_events(void) {
  bypass = 0;                   // cancel any bypassing
  if (nevents) {                // update timers on any active events
    char i;
    for (i = 0; i < nevents; i++)
      if (elist[i].tics) elist[i].tics--;    // zero is the minimum
  }
}

