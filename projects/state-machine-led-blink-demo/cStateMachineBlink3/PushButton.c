//------------------------------------------------------------------------------
// PushButton.c in project StateMachineBlink3
//
// This module uses state machines to debounce three pushbuttons.  The state
// machines use the same machinery as the module Blink.c.  Events are sent to
// the state machine dispatch function by either the timer tick generator or
// the port ISR.  The port ISR is used to detect the initial push.  The timer
// is used for both debouncing and the accelerated mode interval.
//
// The function of each pushbutton is controlled by the client application
// Blink.c, which upon initialization "registers" with each pushbutton the
// address of a function to execute when a push is confirmed.  This function
// is called an "event sink."  This method is based on the approach found in
// "MSP430 State Machine Programming" by Tom Baugh.
//
// Each pushbutton can have an alternate event sink registered which executes
// instead if the button is held down for one second.  Or if an accelerated
// mode is specified, after one second the normal event sink will be executed
// repeatedly until pushbutton release.
//
// The pushbutton state machine has four states:
//
//   - IsIdle:        Wait for a push detected by port pin ISR, then go to
//                    the PushDetected state unless the selected mode is off.
//
//   - PushDetected:  Wait for the debounce delay, check pin status, go back
//                    to IsIdle if button released before delay complete.  If
//                    no release after debounce delay, go to PushConfirmed
//                    state.
//
//   - PushConfirmed: Wait for button release or extended push timeout.  
//                    If released before the extended push timeout, execute
//                    the normal registered event sink and return to IsIdle.
//                    If the extended push timeout is met, go to either 
//                    the state PushExtended or Accelerated, depending on
//                    the selected mode of operation chosen by the client.
//
//   - PushExtended:  Wait for button release.  When released execute the
//                    normal event sink if in normal mode, and if in alternate
//                    function mode execute the alternate event sink.  Then
//                    go to the state IsIdle.
//
//   - Accelerated:   Execute normal event sink at a specified interval until
//                    release, then go to IsIdle.
//
// The client module selects the pushbutton operating mode by setting the
// variable pb_mode variable to one of the following values:
//
//     PB_OFF:     Set both normal and alternate event sinks to a do nothing 
//                 event sink and set the operating mode to PB_NORMAL.
//
//     PB_NORMAL:  No extended push operation, execute normal event sink
//                 once per push and release.
//
//     PB_ACCEL:   Execute normal event sink once for a short push and release.
//                 If an extended push is detected, execute the normal event
//                 sink at regular intervals until release.
//
//     PB_ALT_FN:  Execute normal event sink once for a short push and release.
//                 If an extended push is detected, execute the alternative
//                 event sink once upon release.
//
// See Blink.c for the pushbutton functions in this demo.
//
// Andrew Palm
// 2011.10.09
//------------------------------------------------------------------------------
//
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "System.h"
#include "StateMachine.h"
#include "PushButton.h"

#define  PB_CHECK_LOW_TICKS   2  // Number of system ticks in debounce delay
#define  PB_CHECK_HIGH_TICKS  2  // Number of system ticks in PB release delay

// Number of PB_CHECK_HIGH_TICKS delays until PB extended push
// (Set to 1 second)
#define  PB_EXT_PUSH_COUNT  (TICKS_PER_SEC / PB_CHECK_HIGH_TICKS)

// Number of system ticks between event sinks in PB accelerated mode
#define  PB_ACCEL_TICKS  (TICKS_PER_SEC / 4)

//------------------------------------------------------------------------------
// PushButton finite state machine
enum PushButtonSignals {               // Signals for the PushButton FSM 
    TIMEOUT_SIG = SM_USER_SIG,         // Timer timeout signal
    PUSH_SIG                           // Pushbutton push detected
};

typedef struct PBEvtTag {
    Event super;       // Derived from Event structure 
} PBEvt;               // For this application, no additional user parameters

typedef struct PushButtonTag {         // The PushButton FSM 
    StateMachine super;                // Derived from StateTable structure
    uint8_t  timer_counter;            // Number of delay ticks remaining
    uint8_t  ext_push_counter;         // Number of accel counts remaining
    uint8_t  timer_active;             // Flag that timer is active 
    uint8_t  pb_mode;                  // Pushbutton mode
    PBSINK   pb_normal_sink;           // Pointer to push button pushed sink
    PBSINK   pb_alt_sink;              // Pointer to alternative event sink
    volatile uint8_t *pb_port;         // Pointer to pushbutton (input) port
    volatile uint8_t *pb_ie;           // Pointer to pushbutton int enable reg
    uint8_t  pb_bitmask;               // Bit mask for pushbutton pin
} PushButton;

static PushButton PB_1;                // PushButton FSM for PSHBTN1
static PushButton PB_2;                // PushButton FSM for PSHBTN2
static PushButton PB_3;                // PushButton FSM for PSHBTN3

//------------------------------------------------------------------------------
static void PushButton_DoNothingEventSink(void);

// The "constructor" for PushButton state machines
void PushButton_Ctor(PushButton *me, uint8_t  pb_bitmask, 
                      volatile uint8_t *pb_port, volatile uint8_t *pb_ie); 

static Status PushButton_Initial(PushButton *me, Event const *e);

static Status PushButton_IsIdle(PushButton *me, Event const *e); 
static Status PushButton_PushDetected(PushButton *me, Event const *e); 
static Status PushButton_PushConfirmed(PushButton *me, Event const *e); 
static Status PushButton_PushExtended(PushButton *me, Event const *e);
static Status PushButton_Accelerated(PushButton *me, Event const *e);

//------------------------------------------------------------------------------
// Pushbutton event sink registration functions that allow client modules
// to register event sinks with this module, i.e., set which client function
// is executed on each button push.  Also allow client to set whether or not
// the pushbutton has the acceleration feature.  Set pb_mode as follows:
// 
//     PB_OFF:     Reset event sinks to a default event sink, stay idle
//     PB_NORMAL:  No extended push operation, execute normal event sink
//                 once per push.
//     PB_ACCEL:   Execute normal event sink once for a short push.  If an
//                 extended push is detected, continue executing the normal
//                 event sink at regular intervals.
//     PB_ALT_FN:  Execute normal event sink once for a short push.  If an
//                 extended push is detected, execute the alternative event
//                 sink once.
//
void PushButton_RegisterEventSinkPB1(PBSINK NormalEventSink, 
                                     PBSINK AltEventSink,
                                     uint8_t pb_mode) {
    PB_1.pb_mode = pb_mode;
    switch (pb_mode) {
        case PB_NORMAL:                // Deliberate fall-through
        case PB_ACCEL: {
            PB_1.pb_normal_sink = NormalEventSink;
            PB_1.pb_alt_sink = PushButton_DoNothingEventSink;
            break;
        }
        case PB_ALT_FN: {
            PB_1.pb_normal_sink = NormalEventSink;
            PB_1.pb_alt_sink = AltEventSink;
            break;
        }
        default: {                     // PB_OFF or error value
            PB_1.pb_normal_sink = PushButton_DoNothingEventSink;
            PB_1.pb_alt_sink = PushButton_DoNothingEventSink;
            PB_1.pb_mode = PB_NORMAL;
        }
    }
}

void PushButton_RegisterEventSinkPB2(PBSINK NormalEventSink, 
                                     PBSINK AltEventSink,
                                     uint8_t pb_mode) {
    PB_2.pb_mode = pb_mode;
    switch (pb_mode) {
        case PB_NORMAL:                // Deliberate fall-through
        case PB_ACCEL: {
            PB_2.pb_normal_sink = NormalEventSink;
            PB_2.pb_alt_sink = PushButton_DoNothingEventSink;
            break;
        }
        case PB_ALT_FN: {
            PB_2.pb_normal_sink = NormalEventSink;
            PB_2.pb_alt_sink = AltEventSink;
            break;
        }
        default: {                     // PB_OFF or error value
            PB_2.pb_normal_sink = PushButton_DoNothingEventSink;
            PB_2.pb_alt_sink = PushButton_DoNothingEventSink;
            PB_2.pb_mode = PB_NORMAL;
        }
    }
}

void PushButton_RegisterEventSinkPB3(PBSINK NormalEventSink, 
                                     PBSINK AltEventSink,
                                     uint8_t pb_mode) {
    PB_3.pb_mode = pb_mode;
    switch (pb_mode) {
        case PB_NORMAL:                // Deliberate fall-through
        case PB_ACCEL: {
            PB_3.pb_normal_sink = NormalEventSink;
            PB_3.pb_alt_sink = PushButton_DoNothingEventSink;
            break;
        }
        case PB_ALT_FN: {
            PB_3.pb_normal_sink = NormalEventSink;
            PB_3.pb_alt_sink = AltEventSink;
            break;
        }
        default: {                     // PB_OFF or error value
            PB_3.pb_normal_sink = PushButton_DoNothingEventSink;
            PB_3.pb_alt_sink = PushButton_DoNothingEventSink;
            PB_3.pb_mode = PB_NORMAL;
        }
    }
}

void PushButton_DoNothingEventSink(void) {
}

//------------------------------------------------------------------------------
// Constructor function for pushbutton state machine

void PushButton_Ctor(PushButton *me, uint8_t  pb_bitmask, 
                     volatile uint8_t *pb_port, volatile uint8_t *pb_ie)
{
    StateMachine_Ctor(&me->super, (SHF)&PushButton_Initial); // Superclass
    // Set user parameters
    me->pb_bitmask = pb_bitmask;
    me->pb_port = pb_port;
    me->pb_ie = pb_ie;
}

//------------------------------------------------------------------------------
// Pushbutton state machine initialization
Status PushButton_Initial(PushButton *me, Event const *e) {
    (void)e;                             // Avoid unused parameter warning
    me->timer_active = FALSE;            // Be sure delay timer is off
    return SM_TRAN(&PushButton_IsIdle);  // Transition to initial state
}

//------------------------------------------------------------------------------
// Pushbutton state machine state handler functions

Status PushButton_IsIdle(PushButton *me, Event const *e) {
    switch (e->sig) {
        case SM_ENTRY_SIG: {
            *(me->pb_ie) |= me->pb_bitmask;  // Enable pin interrupt
            return SM_HANDLED();
        }
        case PUSH_SIG: {               // Interrupt on pushbutton pin
            return SM_TRAN(PushButton_PushDetected);
        }
        case SM_EXIT_SIG: {
            *(me->pb_ie) &= ~(me->pb_bitmask);  // Disable pin interrupt
            return SM_HANDLED();    
        }
    }
    return SM_IGNORED();
}

Status PushButton_PushDetected(PushButton *me, Event const *e) {
    switch (e->sig) {
        case SM_ENTRY_SIG: {               // Start debounce delay timer
            me->timer_counter = PB_CHECK_LOW_TICKS;
            me->timer_active = TRUE;
            return SM_HANDLED();
        }
        case TIMEOUT_SIG: {                  // Delay over, check PB port pin
            if(PB_IN & (me->pb_bitmask)) {  // Push not sustained thru delay
                return SM_TRAN(PushButton_IsIdle); // Back to idle state
            }
            // Button still down after delay
            return SM_TRAN(PushButton_PushConfirmed);  // Next state
        }
        case SM_EXIT_SIG: {
            me->timer_active = FALSE;      // Turn off timer
            return SM_HANDLED();
        }
    }
    return SM_IGNORED();
}

Status PushButton_PushConfirmed(PushButton *me, Event const *e) {
    switch (e->sig) {
        case SM_ENTRY_SIG: {
            me->ext_push_counter = PB_EXT_PUSH_COUNT;    // Set up for ext push
            me->timer_counter = PB_CHECK_HIGH_TICKS;  // Set up ticks counter
            me->timer_active = TRUE;
            return SM_HANDLED();
        }
        case TIMEOUT_SIG: {
            if(PB_IN & (me->pb_bitmask)) {     // Button released
                (me->pb_normal_sink)();        // so do normal event sink
                return SM_TRAN(PushButton_IsIdle); // and return to idle
            }
            me->ext_push_counter--;            // Decr interval counter
            if(0 == me->ext_push_counter) {    // Extended push detected
                if(PB_ACCEL == me->pb_mode) {  // Go to accelerated mode
                    return SM_TRAN(PushButton_Accelerated);
                }
                return SM_TRAN(PushButton_PushExtended);
            }                
            me->timer_counter = PB_CHECK_HIGH_TICKS;  // Set up for next ivl
            return SM_HANDLED();
        }
        case SM_EXIT_SIG: {
            me->timer_active = FALSE;      // Turn off timer
            return SM_HANDLED();
        }
    }
    return SM_IGNORED();
}

Status PushButton_PushExtended(PushButton *me, Event const *e) {
    switch (e->sig) {
        case SM_ENTRY_SIG: {
            me->timer_counter = PB_CHECK_HIGH_TICKS;  // Set up ticks counter
            me->timer_active = TRUE;
            return SM_HANDLED();
        }
        case TIMEOUT_SIG: {
            if(PB_IN & (me->pb_bitmask)) {     // Button released
                if(PB_NORMAL == me->pb_mode) { // In normal mode so
                    (me->pb_normal_sink)();    // execute normal event sink
                }
                else {                         // In alternate fn mode, so
                    (me->pb_alt_sink)();       // execute alternate event sink
                }
                return SM_TRAN(PushButton_IsIdle); // and return to idle
            }
            me->timer_counter = PB_CHECK_HIGH_TICKS;  // Set up for next ivl
            return SM_HANDLED();
        }
        case SM_EXIT_SIG: {
            me->timer_active = FALSE;      // Turn off timer
            return SM_HANDLED();
        }
    }
    return SM_IGNORED();
}

Status PushButton_Accelerated(PushButton *me, Event const *e) {
    switch (e->sig) {
        case SM_ENTRY_SIG: { 
            (me->pb_normal_sink)();    // Execute normal registered event sink
            me->timer_counter = PB_ACCEL_TICKS;  // Set up delay btwn events
            me->timer_active = TRUE;
            return SM_HANDLED();
        }
        case TIMEOUT_SIG: {
            if(PB_IN & (me->pb_bitmask)) { // Button released, return to idle
                return SM_TRAN(PushButton_IsIdle);
            }
            (me->pb_normal_sink)();    // Execute normal registered event sink
            me->timer_counter = PB_ACCEL_TICKS;  // Reload timer
            return SM_HANDLED();
        }
        case SM_EXIT_SIG: {
            me->timer_active = FALSE;      // Turn off timer
            return SM_HANDLED();
        }
    }
    return SM_IGNORED();
}

//------------------------------------------------------------------------------
// Initialization functions called by main on startup

void PushButton_InitializeHW(void) {
// Pullups and set 0 pin interrupts enabled in System.c
}

// Construct pushbutton state machines and do initial transition
void PushButton_InitializeApp(void) {
    PushButton_Ctor(&PB_1, PSHBTN1, &PB_IN, &PB_PIE); 
    StateMachine_Init((StateMachine *)&PB_1, (Event *)0); 

    PushButton_Ctor(&PB_2, PSHBTN2, &PB_IN, &PB_PIE); 
    StateMachine_Init((StateMachine *)&PB_2, (Event *)0);

    PushButton_Ctor(&PB_3, PSHBTN3, &PB_IN, &PB_PIE); 
    StateMachine_Init((StateMachine *)&PB_3, (Event *)0);
}

//------------------------------------------------------------------------------
// Pushbutton timer event generator function, called from main on system tick.
// These timers are activated by the state handler functions, as needed.
// They are used for debounce delays and the accerated mode intervales.

void PushButton_ProcessTimerEvents(void) {   
    static PBEvt to_evt = {{ TIMEOUT_SIG }};   // Timer timeout event

    if(PB_1.timer_active) {
        PB_1.timer_counter--;
        if(0 == PB_1.timer_counter) {
            StateMachine_Dispatch((StateMachine *)&PB_1, (Event *)&to_evt);
        }
    }

    if(PB_2.timer_active) {
        PB_2.timer_counter--;
        if(0 == PB_2.timer_counter) {
            StateMachine_Dispatch((StateMachine *)&PB_2, (Event *)&to_evt);        
        }
    }

    if(PB_3.timer_active) {
        PB_3.timer_counter--;
        if(0 == PB_3.timer_counter) {
            StateMachine_Dispatch((StateMachine *)&PB_3, (Event *)&to_evt);        
        }
    }
}

//------------------------------------------------------------------------------
// Shared interrupt vector for set 0 pin interrupts
// Sends push detected event to pushbutton state machine if interrupt occurs
// on pushbutton port pin.  Individual pin interrupts are enabled or disabled
// by the corresponding IsIdle state handler function.

ISR(PCINT0_vect) {
    static PBEvt push_evt = {{ PUSH_SIG }};   // Timer timeout event

    if(PB_PIE & PSHBTN1) {                    // PSHBTN1 interrupt enabled?
        if((~PB_IN) & PSHBTN1) {              // Interrupt on PSHBTN1?
            StateMachine_Dispatch((StateMachine *)&PB_1, (Event *)&push_evt);
            return;
        }
    }
    if(PB_PIE & PSHBTN2) {                    // PSHBTN2 interrupt enabled?
        if((~PB_IN) & PSHBTN2) {              // Interrupt on PSHBTN2?
            StateMachine_Dispatch((StateMachine *)&PB_2, (Event *)&push_evt);
            return;
        }
    }
    if(PB_PIE & PSHBTN3) {                    // PSHBTN3 interrupt enabled?
        if((~PB_IN) & PSHBTN3) {              // Interrupt on PSHBTN3?
            StateMachine_Dispatch((StateMachine *)&PB_3, (Event *)&push_evt);    
            return;
        }
    }
}
