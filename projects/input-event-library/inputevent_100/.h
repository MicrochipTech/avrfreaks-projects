/*****************************************************************************
Title  :   Input Event Library
Author :   SA Development
Version:   1.00
*****************************************************************************/

#ifndef INPUTEVENT_H
#define INPUTEVENT_H

#include "inputevent_settings.h"

//The following code validates settings in inputevent_settings.h and selects the appropriate datatypes
#if (INPUT_EVENT_BUFFER_SIZE<=255)
  #define INPUT_EVENT_BUFFER_INDEX_DATATYPE unsigned char
#elif (INPUT_EVENT_BUFFER_SIZE<=65535)
  #define INPUT_EVENT_BUFFER_INDEX_DATATYPE unsigned short
#else
  #error INPUT_EVENT_BUFFER_SIZE is too large
#endif

#if (INPUT_ENABLE_HOLD_FEATURES)
  #if (INPUT_NO_OF_INPUTS<=7)
    #define INPUT_EVENT_BUFFER_DATATYPE  unsigned char
    #define INPUT_HOLD                   _BV(7)
  #elif (INPUT_NO_OF_INPUTS<=15)
    #define INPUT_EVENT_BUFFER_DATATYPE  unsigned int
    #define INPUT_HOLD                   _BV(15)
  #elif (INPUT_NO_OF_INPUTS<=31)
    #define INPUT_EVENT_BUFFER_DATATYPE  unsigned long
    #define INPUT_HOLD                   _BV(31)
  #else
    #error INPUT_NO_OF_INPUTS is too large
  #endif
#else
  #if (INPUT_NO_OF_INPUTS<=8)
    #define INPUT_EVENT_BUFFER_DATATYPE  unsigned char
  #elif (INPUT_NO_OF_INPUTS<=16)
    #define INPUT_EVENT_BUFFER_DATATYPE  unsigned int
  #elif (INPUT_NO_OF_INPUTS<=32)
    #define INPUT_EVENT_BUFFER_DATATYPE  unsigned long
  #else
    #error INPUT_NO_OF_INPUTS is too large
  #endif
#endif

#if (INPUT_DEBOUNCE_CYCLES<1 || INPUT_DEBOUNCE_CYCLES>255)
  #error INPUT_DEBOUNCE_CYCLES should be 1-255
#endif

#if (INPUT_HOLD_CYCLES_DEFAULT>65535)
  #error INPUT_HOLD_CYCLES_DEFAULT should be 0-65535
#endif

#if (INPUT_HOLDREPEAT_CYCLES_DEFAULT>65535)
  #error INPUT_HOLDREPEAT_CYCLES_DEFAULT should be 0-65535
#endif

extern volatile INPUT_EVENT_BUFFER_INDEX_DATATYPE v_InputEventCount;
extern volatile INPUT_EVENT_BUFFER_DATATYPE v_InputState;
#if (INPUT_ENABLE_HOLD_FEATURES)
extern volatile unsigned short v_InputHoldCycles,v_InputHoldRepeatCycles;
#endif

void WaitNoInput();
void ClearEventBuffer();
INPUT_EVENT_BUFFER_DATATYPE InputGetEvent();
void InputProcess(INPUT_EVENT_BUFFER_DATATYPE AInput);

#endif

