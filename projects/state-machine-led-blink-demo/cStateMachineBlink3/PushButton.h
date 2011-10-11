#ifndef PUSHBUTTONS_H
#define PUSHBUTTONS_H

typedef void( * PBSINK )(void);

void PushButton_InitializeHW(void);
void PushButton_InitializeApp(void);
void PushButton_ProcessTimerEvents(void);

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
enum PushButtonModes {
    PB_OFF,
    PB_NORMAL,
    PB_ACCEL,
    PB_ALT_FN
};

void PushButton_RegisterEventSinkPB1(PBSINK NormalEventSink, 
                                     PBSINK AltEventSink,
                                     uint8_t pb_mode);
void PushButton_RegisterEventSinkPB2(PBSINK NormalEventSink, 
                                     PBSINK AltEventSink, 
                                     uint8_t pb_mode);
void PushButton_RegisterEventSinkPB3(PBSINK NormalEventSink,
                                     PBSINK AltEventSink,
                                     uint8_t pb_mode);

#endif
