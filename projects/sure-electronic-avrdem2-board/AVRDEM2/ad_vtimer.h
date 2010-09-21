// ad_vtimer.h - virtual timer header file
//
// header file for Sure Electronics AVRDEM2 board test routines
// written by Chuck Baird (avrfreaks.net, user zbaird)
// you may freely use this code for any purpose; attribution is appreciated
//
// the virtual timers are approximate, with a resolution of about 0.01 sec.
// NVTIMER is the number of virtual timers possible, maximum of 10
// (to the user the timers are numbered 1 to NVTIMER)
#define NVTIMER 5

// virtual timer goodies
#define V_MASK   0x07   // mask off the optional parts
#define V_UNDEF  0xff   // undefined timer (.active value)
#define V_NOW    0      // set the timer immediately
#define V_NEXT   1      // set the timer on the next (10x) interrupt

#define V_NOP    0      // no action (can used to set the options below
#define V_STOP   1      // stop timer
#define V_START  2      // start timer using set time value
#define V_RESUME 3      // start timer using current time value
#define V_KILL   4      // release the timer's resources
#define V_RUNOUT 5      // force immediate timer runout

#define V_ONCE   0x10   // stop timer next time it runs out
#define V_REPEAT 0x20   // keep timer running next time it runs out (default)
#define V_GO     0x40   // start timer when set
#define V_HOLD   0x80   // do not start when set (default)

struct vt_struct {
  char *flag;           // pointer to flag variable
  unsigned int max;     // maximum count
  unsigned int count;   // current count
  char mode;            // mode (0 = free running or V_ONCE = single shot)
  char active;          // V_UNDEF if unset, 0 if stopped, V_GO if running
};

extern volatile struct vt_struct atime[];        // the virtual timers


// ----------- function prototypes
void set_vtime(char, unsigned int, char, char *);
void run_vtime(char, char);
void init_vtimers(void);                // initialization routine
