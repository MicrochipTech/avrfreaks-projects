//      ev_list.h
//
//      header file for the event list

struct s_events {
  unsigned int tics;    // time until event (1/10 seconds)
  void (*efun)(void);   // pointer to function to handle event
};

extern void update_events(void);
extern void add_event(void (*)(void), unsigned int);
extern void sort_events(void);
extern void check_events(void);
extern void clear_events(void);
extern void collapse_list(void);

extern char locked;              // whether event list is locked (0 = no)
extern char nevents;             // number of active events (0 to EMAX+1)
extern char etime;               // interrupt counter for event updates
extern volatile struct s_events elist[];  // event list
extern char bypass;              // whether we were locked when interrupt happened

#define EMAX 7          // maximum number of events - 1
