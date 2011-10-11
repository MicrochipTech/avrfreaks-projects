//------------------------------------------------------------------------------
// Finite state machine implementation based on the QEP FSM processor found 
// in Chapter 3 of "Practical UML Statecharts in C/C++" by Miro Samek.
//
// Copyright (C) 2002-2007 Miro Samek. All rights reserved.
// This code may be distributed only under the terms and conditions of the
// GNU Public License Version 2.  Contact information:  miro@quantum-leaps.com
//
// Modified by Andrew Palm
// 2011.09.30
//------------------------------------------------------------------------------
//
#ifndef STATEMACH_H
#define STATEMACH_H

#include <stdint.h>

typedef struct EventTag {
    uint16_t sig;                      // Signal of the event
    // Add event parameters by derivation from this Event structure
} Event;

typedef uint8_t Status;                // Return type of state handler functions

struct StateMachineTag;                // Forward declaration

// State handler function
typedef Status (*SHF)(void *me, Event const *e);

typedef struct StateMachineTag {       // State machine structure
    SHF state;                         // Current active state
    // Add parameters by derivation from this StateMachine structure
} StateMachine;

#define StateMachine_Ctor(me_, initial_) ((me_)->state = (initial_))
void StateMachine_Init(StateMachine *me, Event const *e);
void StateMachine_Dispatch(StateMachine *me, Event const *e);

// Return values of state handler functions
#define SM_RET_HANDLED    ((Status)0)
#define SM_RET_IGNORED    ((Status)1)
#define SM_RET_TRAN       ((Status)2)

// Mock functions used in state handler function return statements
#define SM_HANDLED()      (SM_RET_HANDLED)
#define SM_IGNORED()      (SM_RET_IGNORED)
// Macro changes current state for transition and returns SM_RET_TRAN
// for state handler function return statement
#define SM_TRAN(target_) \
        (((StateMachine *)me)->state = (SHF)(target_), SM_RET_TRAN) 

// Reserved signals for invoking state entry, exit, and initialization
// functions.  Used in array SMReservedEvt[]. 
enum SMReservedSigs {
    SM_ENTRY_SIG = 1,
	SM_EXIT_SIG,
	SM_INIT_SIG,
	SM_USER_SIG                // First value allowed for user signals
};

#endif
