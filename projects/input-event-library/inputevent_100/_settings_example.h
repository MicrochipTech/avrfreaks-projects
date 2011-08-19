#ifndef INPUTEVENT_SETTINGS_H
#define INPUTEVENT_SETTINGS_H

#define INPUT_ENABLE_HOLD_FEATURES       1     //Disabling this will disable all hold and hold repeat features and save
                                               //some flash and memory.  Set it to 1 for enabled or 0 for disabled.

#define INPUT_EVENT_BUFFER_SIZE          8     //The maximum number of events the input event buffer can hold.

#define INPUT_NO_OF_INPUTS               7     //The maximum number of inputs.  This is used to determine the datatype
                                               //used for various structures and variables.  Smaller values allow for
                                               //smaller code and memory usage.
                                               //If hold features are enabled: 1-7=unsigned char, 8-15=unsigned int,
                                               //and 16-31=unsigned long. (highest bit reserved for INPUT_HOLD flag)
                                               //If hold features are disabled: 1-8=unsigned char, 9-16=unsigned int,
                                               //and 17-32=unsigned long.

#define INPUT_DEBOUNCE_CYCLES            30    //Debounce time in ISR cycles (1-255).  It is important to understand this
                                               //is not referencing a specific amount of time, but rather a specific
                                               //number of cycles.  If your ISR is executed every 5 milliseconds this
                                               //would be 5*30 or 150 milliseconds for a debounce time which would be
                                               //very large.  In my case I also use the ISR to keep track of time and
                                               //it executes every 1 millisecond.  1ms*30 equals a 30 millisecond
                                               //debounce time.

#define INPUT_HOLD_CYCLES_DEFAULT        3000  //This sets the default value for the InputBeginRepeat variable in ISR cycles.
                                               //This variable can be changed in code using an atomic block at any time
                                               //to suit different needs.  It is the number of cycles before a held button
                                               //or buttons change from a normal press to a repeated press.  Setting this
                                               //to 0 will disable repeat.

#define INPUT_HOLDREPEAT_CYCLES_DEFAULT  0     //This sets the default value for the InputAgainRepeat variable in ISR cycles.
                                               //This variable can be changed in code using an atomic block at any time
                                               //to suit different needs.  It is the number of cycles between repeated events
                                               //after the begin repeat event.  Setting this to 0 will disable auto repeat.

#endif

