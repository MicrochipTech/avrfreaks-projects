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
#include "StateMachine.h"

// Array of reserved events for invoking state entry, exit, and initialization
// functions.  
Event SMReservedEvt[] = {
    { 0 },
    { SM_ENTRY_SIG },
    { SM_EXIT_SIG },
    { SM_INIT_SIG }
};

//------------------------------------------------------------------------------
void StateMachine_Init(StateMachine *me, Event const *e) {
    (*me->state)(me, e);               // Execute top-most initial transition
	(void)(*me->state)(me, &SMReservedEvt[SM_ENTRY_SIG]);  // Enter target
}

//------------------------------------------------------------------------------
void StateMachine_Dispatch(StateMachine *me, Event const *e) {
    SHF s = me->state;                 // Save current state
    Status r = (*s)(me, e);            // Execute state handler function
    // If state transition, execute associated source state exit functions
    // and target state entry functions
	if(r == SM_RET_TRAN) {
	    (void)(*s)(me, &SMReservedEvt[SM_EXIT_SIG]); // Exit fns
		(void)(*me->state)(me, &SMReservedEvt[SM_ENTRY_SIG]);  // Entry fns
	}
}
