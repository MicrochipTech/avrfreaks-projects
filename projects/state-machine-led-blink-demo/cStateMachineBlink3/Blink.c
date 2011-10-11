//------------------------------------------------------------------------------
// Blink.c in project StateMachineBlink3
//
// This module blinks three LEDs at different duty cycles using state machines.
//
// Three pushbuttons are set up with the following functions:
//    PB1:  Increases duty cycle of the active LED, normal pushbutton operation.
//    PB2:  Decreases duty cycle of the active LED, with acceleration feature.
//    PB3:  Selects the active LED by rotating through the three LEDs in the
//          order 1, 2, 3, 1, 2, 3,... with a normal push and release. On an
//          extended push and release the active LED is reset to a 50% duty
//          cycle.
//
// See Pushbutton.c and Pushbutton.h for more information on setting up
// the pushbuttons.
//
// To simplify this demo, all three LEDs have the same period of one
// second--only the duty cycles can be changed by the pushbuttons.  And there
// is no indication of which LED is currently active based on PB3 pushes.
//
// There is one "state handler function" (SHF) for each state. Each SHF
// contains a switch statement with cases based on the event signals received.
// There are special cases for state entry and state exit functions, which are
// automatically invoked by the state machine dispatch function when a state
// transition occurs.  
//
// There are two states in this LED blinking state machine, LED_IsOn and
// LED_IsOff.  
//
// There is only one type of user event in this state machine, a delay timer
// timeout event.  Blink_ProcessEvents is called by main upon wake-up
// from a timer interrupt, decrements the timer counters, and issues timeout
// events at the end of the delays.  
//
// The state handler functions in this state machine set up these timers
// which determine how long a state is held. On entry the time counter is
// set for the delay interval.  When the counter times out there is a 
// transition to the new state.  For this application, the timers run
// continuously.
//
// Andrew Palm
// 2011.10.09
//------------------------------------------------------------------------------
#include <avr/io.h>
#include <stdint.h>
#include "System.h"
#include "StateMachine.h"
#include "Blink.h"
#include "PushButton.h"

// Each tick approximately 15.6 ms with ACLK at 32768 Hz (64 ticks/sec)
// Initially all LEDs are at 50% duty cycle
#define LED_PERIOD_TICKS  TICKS_PER_SEC
#define LED1_IS_ON_TICKS  (LED_PERIOD_TICKS / 2)   // 1 Hz, 50% duty cycle
#define LED2_IS_ON_TICKS  (LED_PERIOD_TICKS / 2)   // 1 Hz, 50% duty cycle
#define LED3_IS_ON_TICKS  (LED_PERIOD_TICKS / 2)   // 1 Hz, 50% duty cycle

//------------------------------------------------------------------------------
// Blink LED finite state machine
enum BlinkSignals {                    // Signals for the Blink FSM 
    TIMEOUT_SIG = SM_USER_SIG
};

typedef struct BlinkEvtTag {
    Event super;       // Derived from Event structure 
} BlinkEvt;            // For this application, no additional user parameters

typedef struct BlinkTag {              // The Blink FSM 
    StateMachine super;                // Derived from StateTable structure 
    uint16_t timer_counter;            // Number of delay ticks remaining
    uint16_t led_is_on_ticks;          // Number of delay ticks for on state
    uint16_t led_period_ticks;         // Number of ticks for flash period
    volatile uint8_t *led_port;        // Pointer to LED port
    uint8_t  led_bitmask;              // Bit mask for LED pin
} Blink;

static Blink Blink_1;                  // Blink FSM for LED1
static Blink Blink_2;                  // Blink FSM for LED2
static Blink Blink_3;                  // Blink FSM for LED3

//------------------------------------------------------------------------------
void Blink_Incr_DutyCycle(void);
void Blink_Decr_DutyCycle(void);
void Blink_Select_LED(void);
void Blink_Reset_LED(void);

// The "constructor" for Blink state machines
void Blink_Ctor(Blink *me, 
                uint16_t led_is_on_ticks, uint16_t led_period_ticks,
                uint8_t  led_bitmask, volatile uint8_t *led_port); 

static Status Blink_Initial(Blink *me, Event const *e);    // Initial trans 
static Status Blink_LED_IsOff(Blink *me, Event const *e);  // State handler fn 
static Status Blink_LED_IsOn(Blink *me, Event const *e);   // State handler fn 

//------------------------------------------------------------------------------
// Note:  These demonstration event sink fns depend on LED_PERIOD_TICKS and
//        LED_IS_ON_TICKS being each a power of 2.
static Blink *Current_LED[] = {
    &Blink_1,
    &Blink_2,
    &Blink_3
};
static uint8_t LED_index = 0; 

void Blink_Decr_DutyCycle(void) {
    Blink *p = Current_LED[LED_index];
    if(p->led_is_on_ticks > 1) {
        p->led_is_on_ticks >>= 1;
    }
}

void Blink_Incr_DutyCycle(void) {
    Blink *p = Current_LED[LED_index];
    if(p->led_is_on_ticks < (p->led_period_ticks / 2)) {
        p->led_is_on_ticks <<= 1;
    }
}

void Blink_Select_LED(void) {
    LED_index = (LED_index+1) % 3; 
}

void Blink_Reset_LED(void) {
    Blink *p = Current_LED[LED_index];
    p->led_is_on_ticks = LED_PERIOD_TICKS / 2;
}

//------------------------------------------------------------------------------
void Blink_Ctor(Blink *me, 
                uint16_t led_is_on_ticks, uint16_t led_period_ticks,
                uint8_t led_bitmask, volatile uint8_t *led_port)
{
    StateMachine_Ctor(&me->super, (SHF)&Blink_Initial);// Construct superclass
    // Set user parameters
    me->led_is_on_ticks = led_is_on_ticks;
    me->led_period_ticks = led_period_ticks;
    me->led_bitmask = led_bitmask;
    me->led_port = led_port;
}

//------------------------------------------------------------------------------
// State machine initialization function
Status Blink_Initial(Blink *me, Event const *e) {
    (void)e;                               // Avoid unused parameter warning
    // Insert any additional initialization actions here that are not state
    // entry actions or initializations done in the constructor.
	  return SM_TRAN(&Blink_LED_IsOn);       // Transition to initial state
}

//------------------------------------------------------------------------------
// State handler functions

Status Blink_LED_IsOff(Blink *me, Event const *e) {
    switch (e->sig) {
        case SM_ENTRY_SIG: {
            *(me->led_port) |= (me->led_bitmask);    // Turn off LED
            // Reload delay timer
            me->timer_counter = me->led_period_ticks - me->led_is_on_ticks;
            return SM_HANDLED(); 
        }
        case TIMEOUT_SIG: {
            return SM_TRAN(Blink_LED_IsOn);  // Transistion to new state	
        }
        // Insert any other user signal type cases here
        case SM_EXIT_SIG: {
            return SM_HANDLED();
        }
    }
    return SM_IGNORED();
}

Status Blink_LED_IsOn(Blink *me, Event const *e) {
    switch (e->sig) {
        case SM_ENTRY_SIG: {
            *(me->led_port) &= ~(me->led_bitmask);  // Turn on LED
            me->timer_counter = me->led_is_on_ticks; // Reload delay timer
            return SM_HANDLED();
        }
        case TIMEOUT_SIG: {
            return SM_TRAN(Blink_LED_IsOff); // Transistion to new state	
        }
        // Insert any other user signal type cases here
        case SM_EXIT_SIG: {
            return SM_HANDLED();
        }
    }
    return SM_IGNORED();
}

//------------------------------------------------------------------------------
// Initialiation functions called by main on startup

void Blink_InitializeHW(void) {
}

void Blink_InitializeApp(void) {
    // Register event sink functions with pushbutton module
    PushButton_RegisterEventSinkPB1(Blink_Incr_DutyCycle, NULL, PB_NORMAL);
    PushButton_RegisterEventSinkPB2(Blink_Decr_DutyCycle, NULL, PB_ACCEL);
    PushButton_RegisterEventSinkPB3(Blink_Select_LED, Blink_Reset_LED, PB_ALT_FN);
    
    // Construct LED blink state machines and do initial transitions
    Blink_Ctor(&Blink_1, LED1_IS_ON_TICKS, LED_PERIOD_TICKS, LED1, &LED_OUT); 
    StateMachine_Init((StateMachine *)&Blink_1, (Event *)0); 

    Blink_Ctor(&Blink_2, LED2_IS_ON_TICKS, LED_PERIOD_TICKS, LED2, &LED_OUT); 
    StateMachine_Init((StateMachine *)&Blink_2, (Event *)0);

    Blink_Ctor(&Blink_3, LED3_IS_ON_TICKS, LED_PERIOD_TICKS, LED3, &LED_OUT); 
    StateMachine_Init((StateMachine *)&Blink_3, (Event *)0);
}

//------------------------------------------------------------------------------
// Timer event generator function, called from main after on system tick
// Note: Timers are always active for this application
void Blink_ProcessTimerEvents(void) {  
    static BlinkEvt to_evt = {{ TIMEOUT_SIG }};   // Blink timer timeout event

    Blink_1.timer_counter--;
    if(0 == Blink_1.timer_counter) {
        StateMachine_Dispatch((StateMachine *)&Blink_1, (Event *)&to_evt);
    }

    Blink_2.timer_counter--;
    if(0 == Blink_2.timer_counter) {
        StateMachine_Dispatch((StateMachine *)&Blink_2, (Event *)&to_evt);        
    }
    
    Blink_3.timer_counter--;
    if(0 == Blink_3.timer_counter) {
        StateMachine_Dispatch((StateMachine *)&Blink_3, (Event *)&to_evt);        
    }
}
