/*****************************************************************************
Title  :   Input Event Library
Author :   SA Development
Version:   1.00
*****************************************************************************/

#include "util/atomic.h"
#include "string.h"
#include "avr/eeprom.h"
#include "util/crc16.h"

#include "inputevent.h"

//volatile variables begin with a v_ as a reminder

//Read this to determine the current debounced state of all inputs
volatile INPUT_EVENT_BUFFER_DATATYPE v_InputState;

//Read this to determine if an input event is available
//Never change it
volatile INPUT_EVENT_BUFFER_INDEX_DATATYPE v_InputEventCount;

//These are set to the defaults specified in your inputevent_settings.h.
//You can change these, but do it in an atomic block because they are 2 bytes
#if (INPUT_ENABLE_HOLD_FEATURES)
volatile unsigned short v_InputHoldCycles=INPUT_HOLD_CYCLES_DEFAULT,v_InputHoldRepeatCycles=INPUT_HOLDREPEAT_CYCLES_DEFAULT;
#endif

//These variables are not accessed outside the library 
volatile INPUT_EVENT_BUFFER_DATATYPE v_InputBuffer[INPUT_EVENT_BUFFER_SIZE];
volatile INPUT_EVENT_BUFFER_INDEX_DATATYPE v_InputBufferIn,v_InputBufferOut;

//Returns an event from the event buffer.  If there are no events avaiable, it will wait for one.
//To avoid waiting, never call this function without checking v_InputEventCount to see if an event
//is available
INPUT_EVENT_BUFFER_DATATYPE InputGetEvent()
  {
    INPUT_EVENT_BUFFER_DATATYPE c1;

    while (!v_InputEventCount)
      ;

    c1=v_InputBuffer[v_InputBufferOut];

    v_InputBufferOut++;
    if (v_InputBufferOut==INPUT_EVENT_BUFFER_SIZE)
      v_InputBufferOut=0;

    ATOMIC_BLOCK(ATOMIC_FORCEON)
      {
        v_InputEventCount--;
      }

    return c1;
  }

//Function will wait until no buttons are pressed (v_InputState==0)
void WaitNoInput()
  {
    while(v_InputState)
      ;
  }

//Clear the event buffer  
void ClearEventBuffer()
  {
    ATOMIC_BLOCK(ATOMIC_FORCEON)
      {
        v_InputEventCount=0;
        v_InputBufferOut=v_InputBufferIn;
      }
  }

//Returns an event from the event buffer.  If there are no events avaiable, it will wait for one.  To
//avoid waiting, never call this function without checking v_InputEventCount to see if an event
//is available
void InputProcess(INPUT_EVENT_BUFFER_DATATYPE AInput)
  {
    static unsigned char InputDebounceCount;
    static INPUT_EVENT_BUFFER_DATATYPE InputLast,InputPressed;
    #if (INPUT_ENABLE_HOLD_FEATURES)
    static unsigned char InputSkipRelease;
    static unsigned int v_InputStateAge;
    #endif

    void InputPutEvent(INPUT_EVENT_BUFFER_DATATYPE AItem)
      {
        if (v_InputEventCount<INPUT_EVENT_BUFFER_SIZE)
          {
            v_InputBuffer[v_InputBufferIn]=AItem;
            v_InputBufferIn++;
            if (v_InputBufferIn==INPUT_EVENT_BUFFER_SIZE)
              v_InputBufferIn=0;
            v_InputEventCount++;
          }
      }

    //Debounce all button inputs
    if (InputLast==AInput)
      InputDebounceCount++;
    else
      {
        InputLast=AInput;
        InputDebounceCount=1;
      }

    //Button state has changed
    if (v_InputState!=InputLast && InputDebounceCount>=INPUT_DEBOUNCE_CYCLES)
      {
        v_InputState=InputLast;

        if (v_InputState)
          {
            #if (INPUT_ENABLE_HOLD_FEATURES)
            if (!InputPressed || !InputSkipRelease)
              {
            #endif
                InputPressed|=v_InputState;
            #if (INPUT_ENABLE_HOLD_FEATURES)
                v_InputStateAge=0;
              }
            #endif
          }
        else
          {
            if (InputPressed)
              {
                #if (INPUT_ENABLE_HOLD_FEATURES)
                if (!InputSkipRelease)
                #endif
                  InputPutEvent(InputPressed);
                #if (INPUT_ENABLE_HOLD_FEATURES)
                InputSkipRelease=0;
                #endif
                InputPressed=0;
              }
          }
      }

    #if (INPUT_ENABLE_HOLD_FEATURES)
    //Repeat
    if (InputPressed && ((!InputSkipRelease && v_InputHoldCycles && v_InputStateAge>=v_InputHoldCycles) ||
       (InputSkipRelease && v_InputHoldRepeatCycles && v_InputStateAge>=v_InputHoldRepeatCycles)))
      {
        InputSkipRelease=1;
        v_InputStateAge=0;
        InputPutEvent(INPUT_HOLD | InputPressed);
      }

    //Increment counter
    v_InputStateAge++;
    #endif
  }

