//======================================================
//   (c)2008-11 Matthew Ford
//   Forward Computing and Control Pty. Ltd.
//   www.forward.com.au
//   All rights reserved
//
//   You may freely copy and distribute this program for 
//           commercial and non-commercial use provided:-
//   i) this copyright notice remains attached
//   ii) you clearly mark any changes you make to the code
//======================================================
//
// PODLedDriver.asm
//
// Main:  ATtiny84
//      mode: ISP mode
//
// Program:  ATtiny84
//    Flash - Input HEX File: yourdir/PODLedDriver.hex
//    EEPROM - not used
//
// Fuses:
//    SPIEN    ticked
//    BODLEVEL  Brown Out detection at VCC = 1.8V
//    CKDIV8   ticked
//    SUT_CKSEL  Int RC Osc. 8Mhz; .... 6CK/14CK +64mS
//    Auto Read  ticked
//    Smart Warnings  ticked
//    Verify After programming  ticked
//  then Progam the fuses
//
// LockBits:  
//    LB  no lock features enabled
// 
// Advanced:
//   Calibrate of frequency 8.0Mhz
//  Do Not write any value to the Cal byte.  startup handles this 
//
// HW Setting
//   VTarget  5.1 
//   VREF 5.1 (not used)
//   Clock Generator 3.686Mhz (max)
//
//  Auto:
//  Erase Device
//  Check Signature
//  Program FLASH
//  Verify FLASH
//---------------------------------------------------------
//
// The constants N and C determine the data rate. C selects clock frequency
// the following expression yields the data rate:
//
//            Clk_freq
//     BAUD =  ------     x 1/N 
//             C        
//  
//
// Absolute minimum value for N*C is about 10 (which causes the 
// timer to interrupt to be set again before the interrupt is finished).
//  Absolute maximum is 170.
// (Caused by the 1.5bit-lenght that is necessary to receive bits correctly.)
//
// The RS232 uses SERIAL_OUT_PIN as the transmit pin 
// The receive pin is  SERIAL_IN_PIN 
// You need to adjust the other .equ to match the pins you choose.
//
//
// Since this RS232 module is half duplex, it can either send or recieve data.
// It can't do both simoutaneausly.
//
// To use first call RS232_INIT. (with interrupts disabled)
// Then either call RS232_SET_TO_RECEIVE to wait for a character to arrive
// or RS232_TRANSMIT to transmit the bytes to have previously stored in the
// RS232_BUFFER_?? To transmit you also need to set the RS232_SEND_COUNT
// which is the number of bytes to be send from 1 to RS232_MAX_SEND_BUFFER
//
//
//*************************************************************************

// 1 --  Copy from BluetoothLedDriverDecimal
// 2 --  Added 5sec timer for ADC reading and changed reading to %
// 3 --  Added BRANCH_IF_TRANSMITTING macro and BRANCH_IF_RS232_BUSY
// 4 --  Added error msg and send Amps also but amps not working 
// 5 --  Fixed error msg and send Amps 
// 6 --  Removed IamAlive and added i,I for amps 
// 7 --  Added macros for NEW_LINE and LOAD_CHARS_FROM
// 8 --  Used macros for all msg loads
// 9 --  Added Level setpoint lookup. Still need to do 0,1,2,3 settings
// 10 -- Added 0,1,2,3 settings
// 11 -- Adjusted levels for percived brightness changes
// 12 -- Minor formatting
// 13 -- Corrections to Comments
// 14 -- Added menus for POD support
// 15 -- Added On/Off toggle cmd
// 19 -- Added fade on/off menu item
// 20 -- Added fade timer
// 21 -- fade on/off works but up/down does not
// 22 -- fixed up/down and menu update
// 23 -- 40 level fade
// 24 -- added up/down steps by 4 levels
// 25 -- replaced LOAD_CHARS_FROM macro with LOAD_CHARS routine reduces size by 172 bytes
// 26 -- simplified sending streaming cmd
// 27 -- added BRANCH_IF_TORCH_OFF_OR_FADING_DOWN macro
// 28 -- added Navigation menu and sub_menu

.include "tn84def.inc"

// start interrupt routine
.macro SAVE_SREG
   push Temp
   in Temp, SREG    // save the SREG
   push Temp
.endmacro

// end interrupt routine
.macro RESTORE_SREG
   pop Temp
   out SREG, Temp    // restore the SREG
   pop Temp
.endmacro

//--------------------------------------------------------
// NEW_LINE
//  --------THIS IS NOT USED IN THIS CODE---------------
// insert a new line in the Tx buffer
// either Cr or CrLf or just Lf
// edit this macro to suit
// Assumes XH:XL has been set to point to the RS232 Buffer 
// Decrements XH:XL before storing each char
//--------------------------------------------------------
.macro NEW_LINE
    ldi Temp, 13 // Cr
    st -X, Temp  // store leading Cr
 //   ldi Temp, 10 // Lf
 //   st -X, Temp  // store leading Lf
.endmacro

//------------------------
// BRANCH_IF_TRANSMITTING
// usage
// BRANCH_IF_TRANSMITTING labelToBranchToIfTransmitting
// else just continue to next statement after this macro
// uses Temp register
// -----------------------
.macro BRANCH_IF_TRANSMITTING // one argument
    lds Temp, RS232_STATUS
    // select just the busy and send bits
    andi Temp, (1<<RS232_STATUS_BUSY) | (1<<RS232_STATUS_SEND)
    // check if busy ==1 and send ==1
    cpi Temp, (1<<RS232_STATUS_BUSY) | (1<<RS232_STATUS_SEND)
    breq (@0) // skip to this label if busy and sending
.endmacro

//------------------------
// BRANCH_IF_RS232_BUSY
// usage
// BRANCH_IF_RS232_BUSY labelToBranchToIfTransmitting
// else just continue to next statement after this macro
// uses Temp register
// -----------------------
.macro BRANCH_IF_RS232_BUSY // one argument
    lds Temp, RS232_STATUS
    // select just the busy and send bits
    sbrc Temp, RS232_STATUS_BUSY
    // check if busy ==1 
    rjmp (@0) // skip to this label if busy 
    // else skip to here if busy == 0
.endmacro

//------------------------
// BRANCH_IF_TORCH_OFF_OR_FADING_DOWN
// usage
// BRANCH_IF_TORCH_OFF_OR_FADING_DOWN labelToBranchToIfOfforFadingDown
// else just continue to next statement after this macro
// no registers changed
// -----------------------
.macro BRANCH_IF_TORCH_OFF_OR_FADING_DOWN
    tst Torch_State
    breq (@0) // off
    // else test if fading off
    cpi Torch_State, Torch_State_FADE_DOWN
    breq (@0) // fading to off
    // else not off or fading down
    // drop through
.endmacro

//------------------------
// BRANCH_IF_TORCH_NOT_MAX
// usage
// BRANCH_IF_TORCH_NOT_MAX labelToBranchToIfNOTatMax
// else just continue to next statement after this macro
// Temp used
// -----------------------
.macro BRANCH_IF_TORCH_NOT_MAX
    tst Torch_State
    breq (@0) // off
    // else test if fading off
    cpi Torch_State, Torch_State_FADE_DOWN
    breq (@0) // fading to off
    // else not off or fading down
    // check current level
    mov Temp, Level
    cpi Temp, Level_Max 
    brne (@0)
    // drop through
.endmacro

// set the uC clock calibration
// I have allowed for two steps incase you need to move the default more the 0x20
// my default was 0x9E so only one step was needed
// in this case just set step 2 to equal step 1
.equ OSC_CAL_STEP_1 = 0xA5  // first adjustment from the default
.equ OSC_CAL_STEP_2 = 0xA5  // second adjustment from the default.

// 9600 at 8MHz 
.equ    N=103    // == ((8000000/9600) / C) - 1   
.equ    C=8      //    counter prescaler == Divisor
                 // ((0<<CS02)|(1<<CS01)|(0<<CS00))  for divide by 8
                 // the -1 in the above equation is because the counter
                 // counts from 0 to N inclusive (i.e. N+1 counts)

// *********** SERIAL OUTPUT DEFINES
.equ SERIAL_OUT_PIN = PA7 // define name for rs232 output
 // set these to match SERIAL_OUT_PIN
.equ SERIAL_OUT_PORT = PORTA  
.equ SERIAL_OUT_DDR = DDRA

// ************ SERIAL INPUT DEFINES
.equ SERIAL_IN_PIN = PB2  // define name for rs232 input
// set these to match SERIAL_IN_PIN
.equ SERIAL_IN_PORT = PORTB 
.equ SERIAL_IN_PORTIN = PINB
.equ SERIAL_IN_DDR = DDRB
.equ SERIAL_IN_GROUP_INT = PCIE1 // Group enable interrupt for SERIAL_IN_PIN (PCINT10)
.equ SERIAL_IN_PIN_INT = PCINT10 // pin interrupt to PB2
.equ SERIAL_IN_INTERRUPT_MASK = PCMSK1 // where serial_in_pin_int is set
.equ SERIAL_IN_GROUP_INT_FLAG = PCIF1 
// flag set if any of enabled pin in PCINT8-11 changes level 
// PIN_CHANGE_INT has to be set appropiate interrupt addres either PCI1addr or PCI0addr
// in this case it is PCI1addr see //  INTERRUPT VECTORS below

.equ SW_B = PB1  // define name for switch input pin
.equ uC_OUTPUT_B = PB0  // define name for uC output pin

.equ CURRENT_SP_MAX = 1000 // MAX CURRENT SP

.equ Level_Max = 40 // max number of levels
// min level is 0

// Level settings for low, med, high
.equ LEVEL_LOW = 16
.equ LEVEL_MED = 28
.equ LEVEL_HIGH = Level_Max // the code depends on this being == Level_Max

// sets the maximum number of bytes that can sent at one time
.equ RS232_MAX_SEND_BUFFER = 70


.equ RS232_STATUS = RAMEND-0 // the STATUS of the RS232
//**** RS232_STATUS bits
   .equ RS232_STATUS_NEXT_BIT = 0 // the next bit to send or the bit just received
   .equ RS232_STATUS_TX_COMPLETE = 3 // trigger, set if have completed send, cleared by RS232_TRANSMIT and RS232_INIT and RS232_SET_TO_RECEIVE
   .equ RS232_STATUS_SET_TO_RECEIVE = 4 // set if SET_TO_RECEIVE has been called and have not started to receive a byte, cleared by call to RS232_TRANSMIT and RS232_INIT
   .equ RS232_STATUS_DATA_RECEIVED=5    // trigger, set when data byte received, cleared by call to RS232_SET_TO_RECEIVE or RS232_INIT
   .equ RS232_STATUS_SEND=6    // Sending data ==1, receiving == 0  only valid when RS232_STATUS_BUSY == 1
   .equ RS232_STATUS_BUSY=7    // 1== busy sending or receiving see RS232_STATUS_SEND to find out which
//**** END RS232_STATUS bits

.equ RS232_BIT_COUNT = RAMEND-1 // the number of bits sent/recieved on the RS232
.equ RS232_RECEIVE_BYTE = RAMEND-2 // byte being received
.equ RS232_SEND_COUNT = RAMEND-3 // the number of bytes to send from RS232_BUFFER_
.equ RS232_SEND_BYTE = RAMEND-4 // the byte being sent
.equ RS232_BUFFER_PTR_HIGH = RAMEND-5  // holds the pointer to the send buffer high
.equ RS232_BUFFER_PTR_LOW  = RAMEND-6  // holds the pointer to the send buffer low

.equ RS232_BUFFER_0 = RS232_BUFFER_PTR_LOW
// send buffer runs from RS232_SEND_BYTE -1 to RS232_SEND_BYTE - RS232_MAX_SEND_BUFFER 
.equ RS232_BUFFER_1 = RS232_BUFFER_0-1 // the first to send
//....................
.equ RS232_BUFFER_END = RS232_BUFFER_0-RS232_MAX_SEND_BUFFER // last RS232 byte storage

.equ ADC_HIGH = RS232_BUFFER_END-1 // storage for adc measurements
.equ ADC_LOW = RS232_BUFFER_END-2

.equ FADE_COUNTER = RS232_BUFFER_END-3 // counter for fading
.equ Fade_Count_Up = 6 // 6 x 2mS x 40 levels = 0.48sec to fade up to Max from off
.equ Fade_Count_Down = 12 // 16 x 2mS x 40 levels = 0.96sec to fade down from Max

.equ LAST_GLOBAL = FADE_COUNTER - 1 // end of globals

// number of globals available
// this MUST be greater then the number of globals defined below
// otherwise the stack will be overridded by the globals
.equ NO_GLOBAL_VARS = RAMEND - LAST_GLOBAL + 2 

.def Level = r14 // the current Led level for lookup of setpoint
.equ Level_Shift = 2 // step in levels for up/down

.def Temp2 = r15 // temp reg
.def Temp = r16// Temporary register
  
//------------------------------------------------
// TRIGGER_Flags  Flags set for triggers
// This register is updated by both interrupts and main code
//------------------------------------------------
.def TRIGGER_Flags = r17  // initially 0 no triggers set
    .equ TRIGGER_SwitchChanged = 0 // bit0, 1 if switch just changed else 0
    .equ TRIGGER_Timer1 = 1  // set by 2mS Timer1 
    .equ TRIGGER_FADE = 2 // set by each 0.05 sec 20Hz
    .equ TRIGGER_Light = 3 // send light level
    .equ TRIGGER_Amps = 4 // send amps
    .equ TRIGGER_Level = 5 // send level

// Debounce Counter Counts allows up to 0.5sec debounce = 250 count
.def DEBOUNCE_Counter = r18 //  count debounce timeout
    .equ Debounce_Count_Low = 5 // about 0.01 sec  = 5 x 2mS
    .equ Debounce_Count_High = 5 // about 0.01 sec  = 5 x 2mS
// increase the appropiate count if you switch needs a longer debouce in one direction

//------------------------------------------------
//SW_Flags,  Holds the current debounced switch state and last state and change trigger
//------------------------------------------------
.def SW_Flags = r19  // 0b00000000 initially i.e. Switch up and last switch up
    .equ SW_SWDown = 0   // bit0, 1 if button pressed, 0 if button released.
    .equ SW_SWLastInput = 1  // bit1,  1 if last read SW pressed (low) not debounced
//------------------------------------------------

//------------------------------------------------
//New_Torch_State,  Holds the new state of the torch
// updated from current Torch_State by triggers
//------------------------------------------------
.def New_Torch_State = r20 

//------------------------------------------------
//Torch_State,  Holds the current state of the torch
//------------------------------------------------
.def Torch_State = r21   // initially 0 == off
.equ Torch_State_OFF = 0  // the torch state constants
.equ Torch_State_LOW = Torch_State_OFF+1 // 1
.equ Torch_State_MED = Torch_State_LOW+1 // 2
.equ Torch_State_HIGH = Torch_State_MED+1 // 3
.equ Torch_State_EXCEEDED = Torch_State_HIGH+1 // 4 first invalid state when switching levels
.equ Torch_State_VARIABLE = Torch_State_EXCEEDED  //  variable levels, up / down 
.equ Torch_State_FADE = Torch_State_VARIABLE+1  //  start of FADE  levels
.equ Torch_State_FADE_UP = Torch_State_FADE  // FADE variable levels
.equ Torch_State_FADE_DOWN = Torch_State_FADE_UP+1  // FADE variable levels
.equ Torch_State_INVALID = Torch_State_FADE_DOWN+1  // first invalid state



// registers to hold current setpoint
.def SP_Low = r22  // holds the low byte of the setpoint current
.def SP_High = r23 // holds the high byte of the setpoint current

.def CMDS = r24
// These flags are set when we want to send a response to the PODapp
.equ CMDS_MainMenu = 0
.equ CMDS_EmptyCmd = 1
.equ CMDS_SetMenu = 2
.equ CMDS_DataMenu = 3
.equ CMDS_Streaming = 4
.equ CMDS_OnOffUpdate = 5
.equ CMDS_SubMenu = 6

.def commandCharacter = r25 // the last cmd received

/*********
.def    XH    = r27
.def    XL    = r26
.def    YH    = r29
.def    YL    = r28
.def    ZH    = r31
.def    ZL    = r30
***********/

//*************************************************************************
//
//    PROGRAM START - EXECUTION STARTS HERE
//
//*************************************************************************
.cseg
//  INTERRUPT VECTORS 
.org 0x0000
   rjmp RESET
.org  PCI1addr  // pin change interrupt 1 for PCINT10 
   rjmp  PIN_CHANGE_INT
.org OC1Aaddr    // = 0x0006  TIMER1 COMPARE A MATCH
  rjmp TIMER1_COMPARE_A_INT
.org  OC0Aaddr   // counter time 0 Compare A match
   rjmp TIMER0_CMP_A_INT



//----------------------------------------------------------------------
//----------------------------------------------------------------------
//
//  This section is the RS232 support
//
//

//**************************************************************************
// PIN_CHANGE_INT 
//
// Called when level changes on SERIAL_IN_PIN, can be a change up or down
// so need to check at the top of this call.
// Looking for start bit ==> low -> high on pin input (ie 1 -> 0) idle is 1
// So check for port input == 1 at top and just return if not.
// Otherwise start the timer to sample in 1.5 bits
// Disable this pin interrupt and clear the interrupt flag
//
// RS232_STATUS_SET_TO_RECEIVE is cleared by this routine
// RS232_STATUS_BUSY is set by this routine
// RS232_STATUS_SEND is cleared by this routine
// 
// When receive completes RS232_STATUS_DATA_RECEIVED will be set
// Clear this flag when you have handled the data
//
//*************************************************************************
PIN_CHANGE_INT:
    SAVE_SREG  // save Temp and SREG

    // see if pin low or high
    sbic SERIAL_IN_PORTIN, SERIAL_IN_PIN   // if SERIAL_IN_PIN low == 1 (set)  just return 
    rjmp END_PIN_CHANGE_INT
    // else want to get the timer started as close as possible
    // to the leading edge on the input

    // start it  with divide by 8
    // this assumes that OCR0A is set to some resonably high value
    // and that timer0 is cleared, RS232_INIT and RS232_STOP should do this
    in Temp, TCCR0B
    sbr Temp, (1<<CS01)  // CS02==0,CS01=1,CS00=0
    out TCCR0B , Temp   // clock select /8 == 1mhZ
    // OK now counting
   
    // set the upper
    ldi Temp,((N+N/2)-(33/C))
    //Set timer reload-value to 1.5 time the bit length
    // and subtract the clocks used so far divided by 
    // the time clock divisor
    // the 1.5 is so that the fisrt sample is 
    // in the middle of the first bit after the start bit
    out OCR0A , TEMP  // set top counter

    in Temp, TIMSK0
    sbr Temp, (1<<OCIE0A)
    out TIMSK0, Temp      // enable COMPARE A overflow interrupt
    in  Temp, TIFR0
    sbr Temp, (1<<OCIE0A)
    out    TIFR0,Temp     // to clear COMPARE A overflow flag

    // the following checks could be added
    // note if already busy here then still receiving/sending !! error should not happen
    // if already receiving here then have not read previous byte received PROCESSING TOO SLOW
    ldi Temp, (1<<RS232_STATUS_BUSY) // set the busy flag and clear the rest
    // clears RS232_STATUS_SET_TO_RECEIVE as have disabled pin interrupt in this interrupt
    sts RS232_STATUS, Temp  // save the status byte

    clr Temp
    sts RS232_BIT_COUNT, Temp  // 0 is first bit which is start bit which is skipped
    // so have already processed it here

    in Temp, SERIAL_IN_INTERRUPT_MASK 
    cbr Temp, (1<<SERIAL_IN_PIN_INT) // pb2 disable pin chage interrupt
    out SERIAL_IN_INTERRUPT_MASK, temp 

    in Temp, GIFR
    sbr Temp,(1<<SERIAL_IN_GROUP_INT_FLAG) 
    out GIFR,Temp ; clear pending pin chage 1 int. 


END_PIN_CHANGE_INT:
    RESTORE_SREG  // restore Temp and SREG and reti
reti              // return from interrupt



//*************************************************************************
//
// TIMER0_CMP_A_INT
// 
// This interrupt is called when the timer reaches its Compare A value
// The time then restarts from 0 automatically while these statements are
// being executed so it stays in time.
//
// If sending i.e. RS232_STATUS_SEND set
// set the output pin based on the RS232_STATUS_NEXT_BIT flag
// if RS232BitCount is 9 this is the 9th bit output the stop bit
// if RS232BitCount is 10 then next byte to send or stop if no more
// otherwise set the RS232_STATUS_NEXT_BIT based on bit 0 of the RS232_SEND_BYTE
// and shift the bit out.
// When transmit complete set RS232_STATUS_TX_COMPLETE
// and clear RS232_STATUS_SET_TO_RECEIVE, RS232_STATUS_BUSY and RS232_STATUS_SEND
//
// If receiving 
// sample the pin level and set the carry flag and save the SREG
// because the next comparison will update the carry flag.
// if this is the 9th bit, stop
// otherwise restore SREG and the carry and rotate into the RS232_RECEIVE_BYTE
// When 9th bit (the stop bit is sampled)
// set RS232_STATUS_DATA_RECEIVED
// and clear RS232_STATUS_SET_TO_RECEIVE, RS232_STATUS_BUSY and RS232_STATUS_SEND 
// 
// 
//*************************************************************************
// These two registers are used locally in this interrupt
// They are saved and restored
// Temp r16 also used and restored
.def RS232StatusReg = r17  // used in interrupt saved and restored
.def RS232BitCount = r18   // used in interrupt saved and restored

TIMER0_CMP_A_INT:
    SAVE_SREG  // save Temp and SREG

    push RS232StatusReg
    push RS232BitCount

    lds RS232StatusReg, RS232_STATUS
    lds RS232BitCount, RS232_BIT_COUNT
    inc RS232BitCount  // sending/receiving next bit 

    // are we sending or receiving
    sbrs RS232StatusReg, RS232_STATUS_SEND
    rjmp TIMER0_CMP_A_RECEIVE  // jump if RS232_STATUS_SEND clear
    // else Sending

TIMER0_CMP_A_SEND:
    // start bit was set in transmit call
    // so here can send 
    // set next output bit from status RS232_STATUS_NEXT_BIT 
    sbrc RS232StatusReg, RS232_STATUS_NEXT_BIT    // if RS232_STATUS_NEXT_BIT is 1
    sbi  SERIAL_OUT_PORT,SERIAL_OUT_PIN        //   Set transmit to 1
    sbrs RS232StatusReg, RS232_STATUS_NEXT_BIT    // if RS232_STATUS_NEXT_BIT is 0
    cbi  SERIAL_OUT_PORT,SERIAL_OUT_PIN        //  Set transmit to 0

    // counter == number of bits sent
    // if counter == 9 then send stop bit
    // if counter > 9 stop
    // else set up to send next bit
    cpi RS232BitCount, 9
    breq TIMER0_CMP_A_STOP_BIT // == 9 send stop bit
    brsh TIMER0_CMP_A_STOP_SEND  // > 9 stop

    // else set next bit to next bit in byte
    // the code below takes the same time for either 0 or 1 next bit
    lds Temp, RS232_SEND_BYTE
    sbrc Temp,0
    sbr RS232StatusReg,(1<<RS232_STATUS_NEXT_BIT)
    sbrs Temp,0
    cbr RS232StatusReg,(1<<RS232_STATUS_NEXT_BIT)
    lsr Temp  // shift this bit out of byte
    sts RS232_SEND_BYTE, Temp
    rjmp END_TIMER0_CMP_A    //  jump to exit

TIMER0_CMP_A_STOP_BIT:
    // set next bit to 1
    sbr RS232StatusReg,(1<<RS232_STATUS_NEXT_BIT)
    rjmp END_TIMER0_CMP_A    //  jump to exit


TIMER0_CMP_A_STOP_SEND:
    // check if there is another byte to send
    lds Temp, RS232_SEND_COUNT
    dec Temp;
    sts RS232_SEND_COUNT, Temp

    breq TIMER0_CMP_A_SEND_END // zero so finished
    // else next byte      
    // clear next bit start bit
    andi RS232StatusReg, ~(1<<RS232_STATUS_NEXT_BIT)
    clr RS232BitCount

    // set up the byte to be sent
    // using ZH, ZL locally here so save and restore
    push ZH  // save register values
    push ZL
    lds ZH, RS232_BUFFER_PTR_HIGH
    lds ZL, RS232_BUFFER_PTR_LOW
    ld Temp, -Z // decrement before loading
    sts RS232_SEND_BYTE,Temp
    sts RS232_BUFFER_PTR_LOW, ZL
    sts RS232_BUFFER_PTR_HIGH, ZH
    pop ZL  // restore them
    pop ZH
    rjmp END_TIMER0_CMP_A    //  jump to exit

TIMER0_CMP_A_RECEIVE:
    sec            // Set carry
    sbis SERIAL_IN_PORTIN,SERIAL_IN_PIN    // <=== SAMPLE HERE
    clc        //  clear carry
    in Temp, SREG  // save this bit

    cpi RS232BitCount,9    //if RS232BitCount < 9 (must sample stop-bit)
    brlo TIMER0_CMP_A_RECEIVE_SAVE_BIT    // more bits to come

    // else finished do not save stop bit just set data received flag
    sbr RS232StatusReg,(1<<RS232_STATUS_DATA_RECEIVED)  // data receieved
    // leave receiving flag set until byte buffer read
    rjmp TIMER0_CMP_A_STOP  // stop timer and clear interrupts

TIMER0_CMP_A_RECEIVE_SAVE_BIT:
    out SREG,Temp
    lds Temp, RS232_RECEIVE_BYTE
    ror Temp    // rotate carry into data, right most bit goes to carry
    sts RS232_RECEIVE_BYTE, Temp
    rjmp END_TIMER0_CMP_A

TIMER0_CMP_A_SEND_END:
    sbr  RS232StatusReg, (1<<RS232_STATUS_TX_COMPLETE) // set trigger
    // rjmp TIMER0_CMP_A_STOP drop through

TIMER0_CMP_A_STOP:
    rcall RS232_STOP 
    cbr RS232StatusReg,(1<<RS232_STATUS_SET_TO_RECEIVE)|(1<<RS232_STATUS_BUSY)|(1<<RS232_STATUS_SEND)
    // not busy or sending or set to recieve
    // rjmp END_TIMER0_CMP_A drop through

END_TIMER0_CMP_A:
    ldi Temp, N
    out OCR0A, Temp  // set top compare for one byte time

    sts RS232_BIT_COUNT, RS232BitCount
    sts RS232_STATUS, RS232StatusReg  // save the current status

    pop RS232BitCount
    pop RS232StatusReg
    RESTORE_SREG  // restore Temp and SREG and reti
reti              // return from interrupt

//*****************************************************************
//
// RS232_SET_TO_RECEIVE
//
// If RS232_STATUS_BUSY or already RS232_SET_TO_RECEIVE then
// just return as nothing to do, either already receiving or sending
// and send has precedence, or have already set to receive
// Otherwise enable the pin change interrupt and clear the flag
// Set RS232_STATUS_SET_TO_RECEIVE, this will be cleared when start bit detected
// or when a transmit is started.
// Clear RS232_STATUS_TX_COMPLETE and RS232_STATUS_BUSY and RS232_STATUS_DATA_RECEIVED
// 
// Only Temp is used
// It is saved and restored here
//
//*****************************************************************
RS232_SET_TO_RECEIVE:
    SAVE_SREG  // save Temp and SREG
    cli // prevent interrupt here
        
    lds Temp, RS232_STATUS
    sbrc Temp, RS232_STATUS_BUSY 
    rjmp END_RS232_SET_TO_RECEIVE // if still busy skip this
    // either still sending or receiving a char
    sbrc Temp, RS232_STATUS_SET_TO_RECEIVE // have already called this so just return
    // prevents clearing of pending pin change below
    rjmp END_RS232_SET_TO_RECEIVE // if already called skip this

    rcall RS232_STOP

    in Temp, SERIAL_IN_INTERRUPT_MASK // pin change mask reg 
    sbr Temp, 1<<SERIAL_IN_PIN_INT // pb2 enble pin chage interrupt
    out SERIAL_IN_INTERRUPT_MASK, Temp 

    in Temp, GIFR
    sbr Temp,1<<SERIAL_IN_GROUP_INT_FLAG 
    out GIFR,Temp ; clear pending pin change int. 

    // clear trigger and set to receive
    // set to receive clears all other status bits
    // including RS232_STATUS_TX_COMPLETE  trigger if set
    // and RS232_STATUS_DATA_RECEIVED and RS232_STATUS_BUSY
    ldi Temp, (1<<RS232_STATUS_SET_TO_RECEIVE)
    sts RS232_STATUS, Temp // clear RS232Status
END_RS232_SET_TO_RECEIVE:
    RESTORE_SREG  // restore Temp and SREG and global interrupt flag
ret

//*********************************************************
//  RS232_STOP 
// Should only be called inside interrupt disabled block
// This is a utility routine to disable the interrupts and
// stop the TIMER0 clock
//
// Only Temp is used
// It is assumed the caller has saved Temp
// prior to this call
//**********************************************************
RS232_STOP:
    sbi SERIAL_OUT_PORT,SERIAL_OUT_PIN        //   Set transmit to 1

    in Temp, SERIAL_IN_INTERRUPT_MASK 
    cbr Temp, (1<<SERIAL_IN_PIN_INT) // pb2 disable pin chage interrupt
    out SERIAL_IN_INTERRUPT_MASK, temp 

    in Temp, GIFR
    sbr Temp,(1<<SERIAL_IN_GROUP_INT_FLAG) 
    out GIFR,Temp ; clear pending pin chage 1 int. 

    // stop the clock
    in Temp, TCCR0B
    cbr Temp, (1<<CS02)|(1<<CS01)|(1<<CS00)
    out TCCR0B , Temp   // stop the clock

    // set CTC mode in the counter
    in Temp,TCCR0A
    sbr Temp, (1<<WGM01)
    cbr Temp, (1<<WGM00)
    out TCCR0A, Temp

    ldi Temp, N  // set the compare to N
    out OCR0A, Temp

    // clear the timer
    clr Temp 
    out TCNT0,Temp    

    in  Temp, TIMSK0
    cbr Temp, (1<<OCIE0A) // disable Compare A interrupt on timer 0
    out TIMSK0,Temp           
    
    in Temp, TIFR0
    sbr Temp, (1<<OCF0A) // clear Compare A interrupt flag on timer 0
    out TIFR0, Temp
ret

//*************************************************************************
// RS232_INIT 
// Initializes the RS232
// Should be called with interrupts disabled.
//
// Sets up input and output pin and timer and pin change interrupt group
// Need to call either 
// RS232_SET_TO_RECEIVE or 
// RS232_TRANSMIT
// to actually send or receive
//
// Saves and restores Temp register and SREG
//*************************************************************************
RS232_INIT:    
    SAVE_SREG
    // setup the output pin
    // leave other pins unchanged
    in Temp, SERIAL_OUT_PORT
    sbr Temp, (1<<SERIAL_OUT_PIN) // set pin high
    out SERIAL_OUT_PORT, Temp        
    in Temp, SERIAL_OUT_DDR
    sbr Temp, (1<<SERIAL_OUT_PIN) // set as output
    out SERIAL_OUT_DDR, Temp 

    // set the input pin
    // leave other pins unchanged
    in Temp, SERIAL_IN_PORT
    sbr Temp, (1<<SERIAL_IN_PIN) // set pull up  
    out SERIAL_IN_PORT,Temp
    in Temp, SERIAL_IN_DDR
    cbr Temp, (1<<SERIAL_IN_PIN) // clear to set as input 
    out SERIAL_IN_DDR,Temp

    rcall RS232_STOP  // setup the RS232 for next send or receive
    clr Temp // clear all flags and triggers 
    sts RS232_STATUS, Temp

    in Temp, GIMSK
    sbr Temp, (1<<SERIAL_IN_GROUP_INT)
    out GIMSK, Temp   // enable pin change interrup for this group if not already enabled

    in Temp, GIFR
    sbr Temp,1<<SERIAL_IN_GROUP_INT_FLAG 
    out GIFR,Temp ; clear pending pin change int. 

    clr Temp
    sts RS232_STATUS, Temp // clear RS232Status
    RESTORE_SREG
ret

//*************************************************************************
// RS232_TRANSMIT 
//  
// Assumes RS232_BUFFER_1 to n globals have already set up.
// and that RS232_SEND_COUNT contains the number of bytes to send.
// Load the first byte into RS232_SEND_BYTE
// Set up the buffer pointers to point to the first byte
// Clear RS232_STATUS_NEXT_BIT for the first start bit
// Clear RS232_STATUS_SET_TO_RECEIVE and RS232_STATUS_TX_COMPLETE
// Set RS232_STATUS_SEND and RS232_STATUS_BUSY
// Load half a bit time into the timer to clear the last half bit of any
// previous receive (for the other ends benifit)
// and enable the timer interrupt and start the timer.
//
// Transmit takes precedence over receive
// if receiving now then receiving will be abandoned and transmit will start
//
// Always send at least one byte and at most RS232_MAX_SEND_BUFFER bytes
//
// All registers (including Temp) preserved by this call
//
//*************************************************************************
RS232_TRANSMIT:
    // Temp holds byte to transmit
    SAVE_SREG
    cli // prevent interrupt here 

    lds Temp, RS232_SEND_COUNT
    // make sure tx count is within range of 1 to max send buffer
    cpi Temp, 1
    brsh RS232_TX_ONE
    ldi Temp, 1 // send one 
RS232_TX_ONE:
    cpi Temp, RS232_MAX_SEND_BUFFER+1
    brlo RS232_TX_MAX
    ldi Temp, RS232_MAX_SEND_BUFFER

RS232_TX_MAX:
    sts RS232_SEND_COUNT, Temp

    rcall RS232_STOP  // stop receive if runnning

    clr Temp
    sts RS232_BIT_COUNT, Temp

    lds Temp, RS232_BUFFER_1 // load first byte to send
    sts RS232_SEND_BYTE, Temp

    // set up the buffer pointers 
    ldi Temp, low(RS232_BUFFER_1)
    sts RS232_BUFFER_PTR_LOW, Temp
    ldi Temp, high(RS232_BUFFER_1)
    sts RS232_BUFFER_PTR_HIGH, Temp

    // if transmitting then ignore receives
    lds Temp, RS232_STATUS
    cbr Temp, ( (1<<RS232_STATUS_SET_TO_RECEIVE)|(1<<RS232_STATUS_NEXT_BIT)|(1<<RS232_STATUS_TX_COMPLETE))
    // clear trigger 
    sbr Temp, (1<<RS232_STATUS_SEND)|(1<<RS232_STATUS_BUSY)
    // leave  RS232_STATUS_DATA_RECEIVED as it is if have received char
    // it has been saved in RS232_RECEIVE_BYTE
    sts RS232_STATUS, Temp

    ldi    Temp,N/2  //Set timer delay before transmitting start bit
    // set half bit delay here assumes sender is half-duplex also
    // and it needs to finish sending its stop bit.
    out OCR0A , TEMP  // set top counter

    in Temp, TIMSK0
    sbr Temp, (1<<OCIE0A)
    out TIMSK0, Temp           // enable COMPARE A overflow interrupt
    in  Temp, TIFR0
    sbr Temp, (1<<OCIE0A)
    out    TIFR0,Temp        // to clear COMPARE A overflow flag
        
    // start the clock
    // start it  with divide by 8
    // this assumes that OCR0A is set to some resonably high value
    // and that timer0 is cleared
    in Temp, TCCR0B
    sbr Temp, (1<<CS01)  // CS02==0,CS01=1,CS00=0
    out TCCR0B , Temp   // clock select /8 == 1mhZ
    // OK now counting

    RESTORE_SREG // restores interrupt setting as well
ret

//
// END of RS232 support routines
//------------------------------------------------------------------------
//------------------------------------------------------------------------


//------------------------------------------------------------------------
//------------------------------------------------------------------------
// 
//  NUMBER format routines used to send back level and current reading
//
//
//-----------------------------------------------------
// CONVERT_13BITS_PERCENT
// Converts the, possiably signed, 13bit number in ZH:ZL 
// (actually -9999 to +9999)
// and save acsii char in global SRAM whose start is pointed
// to by XH:XL,
// XH:XL decremented BEFORE each store of a char
// If ZH:ZL is negative a leading '-' is stored
// 
// Format of number is determined by uncommenting
// one of the 3 #defines
// #define CONVERT_13BITS_PERCENT_LEFT
//  left justifies the result  eg |-21 and |2
// #define CONVERT_13BITS_PERCENT_RIGHT
//  right justifies the result eg |  -21 and |    2
// #define CONVERT_13BITS_PERCENT_ZEROS
//  right justifies with leading zeros eg |-0021 and | 0002
// if you uncomment more then one you will get a compile error (duplicate label)
//
// Numbers >9999 are output as 9999
// Numbers <-9999 are output as -9999
// 
// On return ZH:ZL is zero and XH:XL is updated to point to
// the last char stored
//
// all used registers pushed and popped (saved and restored) 
//-----------------------------------------------------
#define CONVERT_13BITS_PERCENT_LEFT
//#define CONVERT_13BITS_PERCENT_RIGHT
//#define CONVERT_13BITS_PERCENT_ZEROS

// register defines for CONVERT_13BITS_PERCENT
.def LeadingDigit = r17 
.def Negative = r18
 
CONVERT_13BITS_PERCENT:
    push Temp
    push LeadingDigit 
    push Negative

    clr Negative // no neg
    clr LeadingDigit // no leading digit yet

    // check if number to be converted is neg
    // if so prepend - and negate
    clr Temp
    cp ZL, Temp
    cpc ZH, Temp
    brpl CONVERT_13BITS_PERCENT_PAD_NEG_SIGN

    // else negate the input
    // Temp zero from above
    //clr Temp
    sub Temp, ZL
    mov ZL, Temp
    clr Temp
    sbc Temp, ZH
    mov ZH, Temp
    ldi Temp, 45 // '-'
    st -X, Temp // store -
    ldi Negative, 1 // set negative flag
    rjmp CONVERT_13BITS_PERCENT_CHECK_MAX_NUMBER

CONVERT_13BITS_PERCENT_PAD_NEG_SIGN:
   #ifndef CONVERT_13BITS_PERCENT_LEFT
    // store a space if you want to padd the '-' position of +ve number
    ldi Temp, 32
    st -X, Temp // store space
   #endif
CONVERT_13BITS_PERCENT_CHECK_MAX_NUMBER:
    // check against max value
    push XH
    push XL
    mov XH, ZH
    mov XL, ZL
    subi XL,  low(9999)
    sbci XH, high(9999)
    pop XL
    pop XH
    brsh CONVERT_13BITS_PERCENT_STORE_9999
    rjmp CONVERT_13BITS_PERCENT_START_CONVERSION
CONVERT_13BITS_PERCENT_STORE_9999:
    ldi Temp, 48 + 9 // '0' + 9
    st -X, Temp
    st -X, Temp
    st -X, Temp
    st -X, Temp
    rjmp END_CONVERT_13BITS_PERCENT

CONVERT_13BITS_PERCENT_START_CONVERSION:
    clr Temp
CONVERT_13BITS_PERCENT_LOOP_1000:
    subi ZL, low(1000)
    sbci  ZH, high(1000)
    brmi CONVERT_13BITS_PERCENT_ADD_BACK_1000
    inc Temp // else inc temp
    rjmp CONVERT_13BITS_PERCENT_LOOP_1000 // try again
CONVERT_13BITS_PERCENT_ADD_BACK_1000:
    subi ZL, low(-1000)
    sbci  ZH, high(-1000)
CONVERT_13BITS_PERCENT_STORE_1000:
    rcall CONVERT_13BITS_PERCENT_STORE_DIGIT

    clr Temp
CONVERT_13BITS_PERCENT_LOOP_100:       
    subi ZL, low(100)
    sbci  ZH, high(100)
    brmi CONVERT_13BITS_PERCENT_ADD_BACK_100
    inc Temp // else inc temp
    rjmp CONVERT_13BITS_PERCENT_LOOP_100  // try again
CONVERT_13BITS_PERCENT_ADD_BACK_100:
    subi ZL, low(-100)
    sbci  ZH, high(-100)
CONVERT_13BITS_PERCENT_STORE_100:
    rcall CONVERT_13BITS_PERCENT_STORE_DIGIT

    ldi LeadingDigit, 1 // force last two digits even if 0

    clr Temp
CONVERT_13BITS_PERCENT_LOOP_10:
    sbiw Z, 10 // use word subtraction for < 64       
    brmi CONVERT_13BITS_PERCENT_ADD_BACK_10
    inc Temp // else inc temp
    rjmp CONVERT_13BITS_PERCENT_LOOP_10  // try again
CONVERT_13BITS_PERCENT_ADD_BACK_10:
    adiw Z, 10 // use word addition for < 64
CONVERT_13BITS_PERCENT_STORE_10:
    rcall CONVERT_13BITS_PERCENT_STORE_DIGIT

    ldi Temp, 46 // '.' 
    st -X, Temp // store decimal point

    clr Temp
CONVERT_13BITS_PERCENT_LOOP_1:       
    sbiw Z, 1 // use word subtraction for < 64       
    brmi CONVERT_13BITS_PERCENT_ADD_BACK_1
    inc Temp // else inc temp
    rjmp CONVERT_13BITS_PERCENT_LOOP_1  // try again
CONVERT_13BITS_PERCENT_ADD_BACK_1:
    adiw Z, 1 // use word addition for < 64       
CONVERT_13BITS_PERCENT_STORE_1:
     // always store last digit even if 0
    subi Temp, -48 //  temp - (-48) 
    st -X, Temp // store 0 or 1's

    ldi Temp, 37 // '%'
    st -X, Temp // store the % 

END_CONVERT_13BITS_PERCENT:
    pop Negative
    pop LeadingDigit
    pop Temp
ret

//--------------------------
// CONVERT_13BITS_PERCENT_STORE_DIGIT
// Checks Temp against 0 and if not 0
// add 48 '0' and store in location 
// pointed to by XH:XL  (decremented first)
// see code comments for how to store
// leading zeros or padding blanks
//--------------------------
CONVERT_13BITS_PERCENT_STORE_DIGIT:
    //------------------------------
#ifdef CONVERT_13BITS_PERCENT_ZEROS
    // this section stores leading zeros
CONVERT_13BITS_PERCENT_STORE_NEXT_DIGIT:
    subi Temp, -48 //  temp - (-48) 
    st -X, Temp  // store
    rjmp END_CONVERT_13BITS_PERCENT_STORE_DIGIT
#endif //CONVERT_13BITS_PERCENT_ZEROS
    //------------------------------

    //------------------------------
#ifdef CONVERT_13BITS_PERCENT_LEFT
   // this section has no padding
   tst LeadingDigit // zero if no leading digit stored yet
   brne CONVERT_13BITS_PERCENT_STORE_NEXT_DIGIT // always store if have leading digit
   tst Temp
   breq END_CONVERT_13BITS_PERCENT_STORE_DIGIT // skip zero

CONVERT_13BITS_PERCENT_STORE_NEXT_DIGIT:
   ldi LeadingDigit, 1 // have leading digit now
   subi Temp, -48 //  temp - (-48) 
   st -X, Temp  // store
   rjmp END_CONVERT_13BITS_PERCENT_STORE_DIGIT
#endif //CONVERT_13BITS_PERCENT_LEFT
   //------------------------------

   //------------------------------
#ifdef CONVERT_13BITS_PERCENT_RIGHT
   // this section padds with leading spaces
   tst LeadingDigit // zero if no leading digit stored yet
   brne CONVERT_13BITS_PERCENT_STORE_NEXT_DIGIT // always store if have leading digit
   tst Temp
   breq CONVERT_13BITS_PERCENT_STORE_ZERO_DIGIT // handle zero

CONVERT_13BITS_PERCENT_STORE_NEXT_DIGIT:
   ldi LeadingDigit, 1 // have leading digit now
   subi Temp, -48 //  temp - (-48) 
   st -X, Temp  // store
   rjmp END_CONVERT_13BITS_PERCENT_STORE_DIGIT

CONVERT_13BITS_PERCENT_STORE_ZERO_DIGIT:
   // if negative move the - to the right
   tst Negative
   brne CONVERT_13BITS_PERCENT_STORE_PADDING // negative store over neg
   // else no negative dec X before store
   sbiw X, 1 
CONVERT_13BITS_PERCENT_STORE_PADDING:
   ldi Temp, 32 // ' '
   st X, Temp
   // restore neg if there is one
   tst Negative
   breq END_CONVERT_13BITS_PERCENT_STORE_DIGIT // not negative nothing to do
   // store neg in next location
   ldi Temp, 45 // '-'
   st -X, Temp // store -
   //rjmp END_CONVERT_13BITS_PERCENT_STORE_DIGIT drop throught
#endif // CONVERT_13BITS_PERCENT_RIGHT
   //------------------------------

END_CONVERT_13BITS_PERCENT_STORE_DIGIT:
ret

.undef LeadingDigit
.undef Negative
//=======================================================================

//-----------------------------------------------------
// CONVERT_13BITS
// Converts the, possiably signed, 13bit number in ZH:ZL 
// (actually -9999 to +9999)
// and save acsii char in global SRAM whose start is pointed
// to by XH:XL,
// XH:XL decremented BEFORE each store of a char
// If ZH:ZL is negative a leading '-' is stored
// 
// Format of number is determined by uncommenting
// one of the 3 #defines
// #define CONVERT_13BITS_LEFT
//  left justifies the result  eg |-21 and |2
// #define CONVERT_13BITS_RIGHT
//  right justifies the result eg |  -21 and |    2
// #define CONVERT_13BITS_ZEROS
//  right justifies with leading zeros eg |-0021 and | 0002
// if you uncomment more then one you will get a compile error (duplicate label)
//
// Numbers >9999 are output as 9999
// Numbers <-9999 are output as -9999
// 
// On return ZH:ZL is zero and XH:XL is updated to point to
// the last char stored
//
// all used registers pushed and popped (saved and restored) 
//-----------------------------------------------------
#define CONVERT_13BITS_LEFT
//#define CONVERT_13BITS_RIGHT
//#define CONVERT_13BITS_ZEROS

// register defines for CONVERT_13BITS
.def LeadingDigit = r17 
.def Negative = r18
 
CONVERT_13BITS:
    push Temp
    push LeadingDigit 
    push Negative

    clr Negative // no neg
    clr LeadingDigit // no leading digit yet

    // check if number to be converted is neg
    // if so prepend - and negate
    clr Temp
    cp ZL, Temp
    cpc ZH, Temp
    brpl CONVERT_13BITS_PAD_NEG_SIGN

    // else negate the input
    // Temp zero from above
    //clr Temp
    sub Temp, ZL
    mov ZL, Temp
    clr Temp
    sbc Temp, ZH
    mov ZH, Temp
    ldi Temp, 45 // '-'
    st -X, Temp // store -
    ldi Negative, 1 // set negative flag
    rjmp CONVERT_13BITS_CHECK_MAX_NUMBER

CONVERT_13BITS_PAD_NEG_SIGN:
   #ifndef CONVERT_13BITS_LEFT
    // store a space if you want to padd the '-' position of +ve number
    ldi Temp, 32
    st -X, Temp // store space
   #endif
CONVERT_13BITS_CHECK_MAX_NUMBER:
    // check against max value
    push XH
    push XL
    mov XH, ZH
    mov XL, ZL
    subi XL,  low(9999)
    sbci XH, high(9999)
    pop XL
    pop XH
    brsh CONVERT_13BITS_STORE_9999
    rjmp CONVERT_13BITS_START_CONVERSION
CONVERT_13BITS_STORE_9999:
    ldi Temp, 48 + 9 // '0' + 9
    st -X, Temp
    st -X, Temp
    st -X, Temp
    st -X, Temp
    rjmp END_CONVERT_13BITS

CONVERT_13BITS_START_CONVERSION:
    clr Temp
CONVERT_13BITS_LOOP_1000:
    subi ZL, low(1000)
    sbci  ZH, high(1000)
    brmi CONVERT_13BITS_ADD_BACK_1000
    inc Temp // else inc temp
    rjmp CONVERT_13BITS_LOOP_1000 // try again
CONVERT_13BITS_ADD_BACK_1000:
    subi ZL, low(-1000)
    sbci  ZH, high(-1000)
CONVERT_13BITS_STORE_1000:
    rcall CONVERT_13BITS_STORE_DIGIT

    clr Temp
CONVERT_13BITS_LOOP_100:       
    subi ZL, low(100)
    sbci  ZH, high(100)
    brmi CONVERT_13BITS_ADD_BACK_100
    inc Temp // else inc temp
    rjmp CONVERT_13BITS_LOOP_100  // try again
CONVERT_13BITS_ADD_BACK_100:
    subi ZL, low(-100)
    sbci  ZH, high(-100)
CONVERT_13BITS_STORE_100:
    rcall CONVERT_13BITS_STORE_DIGIT

    clr Temp
CONVERT_13BITS_LOOP_10:
    sbiw Z, 10 // use word subtraction for < 64       
    brmi CONVERT_13BITS_ADD_BACK_10
    inc Temp // else inc temp
    rjmp CONVERT_13BITS_LOOP_10  // try again
CONVERT_13BITS_ADD_BACK_10:
    adiw Z, 10 // use word addition for < 64
CONVERT_13BITS_STORE_10:
    rcall CONVERT_13BITS_STORE_DIGIT

    clr Temp
CONVERT_13BITS_LOOP_1:       
    sbiw Z, 1 // use word subtraction for < 64       
    brmi CONVERT_13BITS_ADD_BACK_1
    inc Temp // else inc temp
    rjmp CONVERT_13BITS_LOOP_1  // try again
CONVERT_13BITS_ADD_BACK_1:
    adiw Z, 1 // use word addition for < 64       
CONVERT_13BITS_STORE_1:
     // always store last digit even if 0
    subi Temp, -48 //  temp - (-48) 
    st -X, Temp // store 0 or 1's

END_CONVERT_13BITS:
    pop Negative
    pop LeadingDigit
    pop Temp
ret

//--------------------------
// CONVERT_13BITS_STORE_DIGIT
// Checks Temp against 0 and if not 0
// add 48 '0' and store in location 
// pointed to by XH:XL  (decremented first)
// see code comments for how to store
// leading zeros or padding blanks
//--------------------------
CONVERT_13BITS_STORE_DIGIT:
   //------------------------------
#ifdef CONVERT_13BITS_ZEROS
   // this section stores leading zeros
CONVERT_13BITS_STORE_NEXT_DIGIT:
   subi Temp, -48 //  temp - (-48) 
   st -X, Temp  // store
   rjmp END_CONVERT_13BITS_STORE_DIGIT
#endif //CONVERT_13BITS_ZEROS
   //------------------------------

   //------------------------------
#ifdef CONVERT_13BITS_LEFT
   // this section has no padding
   tst LeadingDigit // zero if no leading digit stored yet
   brne CONVERT_13BITS_STORE_NEXT_DIGIT // always store if have leading digit
   tst Temp
   breq END_CONVERT_13BITS_STORE_DIGIT // skip zero

CONVERT_13BITS_STORE_NEXT_DIGIT:
   ldi LeadingDigit, 1 // have leading digit now
   subi Temp, -48 //  temp - (-48) 
   st -X, Temp  // store
   rjmp END_CONVERT_13BITS_STORE_DIGIT
#endif //CONVERT_13BITS_LEFT
   //------------------------------

   //------------------------------
#ifdef CONVERT_13BITS_RIGHT
   // this section padds with leading spaces
   tst LeadingDigit // zero if no leading digit stored yet
   brne CONVERT_13BITS_STORE_NEXT_DIGIT // always store if have leading digit
   tst Temp
   breq CONVERT_13BITS_STORE_ZERO_DIGIT // handle zero

CONVERT_13BITS_STORE_NEXT_DIGIT:
   ldi LeadingDigit, 1 // have leading digit now
   subi Temp, -48 //  temp - (-48) 
   st -X, Temp  // store
   rjmp END_CONVERT_13BITS_STORE_DIGIT

CONVERT_13BITS_STORE_ZERO_DIGIT:
   // if negative move the - to the right
   tst Negative
   brne CONVERT_13BITS_STORE_PADDING // negative store over neg
   // else no negative dec X before store
   sbiw X, 1 
CONVERT_13BITS_STORE_PADDING:
   ldi Temp, 32 // ' '
   st X, Temp
   // restore neg if there is one
   tst Negative
   breq END_CONVERT_13BITS_STORE_DIGIT // not negative nothing to do
   // store neg in next location
   ldi Temp, 45 // '-'
   st -X, Temp // store -
   //rjmp END_CONVERT_13BITS_STORE_DIGIT drop throught
#endif // CONVERT_13BITS_RIGHT
   //------------------------------

END_CONVERT_13BITS_STORE_DIGIT:
ret

.undef LeadingDigit
.undef Negative
//=======================================================================


//
//  END of number format routines
//
//
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------


//*************************************************************************
//
// TIMER1_CMP_A_INT
// called every 2mS
// 
//*************************************************************************
TIMER1_COMPARE_A_INT:
    SAVE_SREG
    // Temp not used here but leave extra push in as standard interrupt preamble 
    sbr TRIGGER_Flags, (1<<TRIGGER_Timer1)  // modifies SREG
    RESTORE_SREG
reti


//-----------------------------------------
//  ADC interrupt, result available
// removed this interrupt 
// just check if conversion start still set.
//-----------------------------------------

#define ADCLow r24
#define ADCHigh r25
//-------------------------------------------
// LED_CONTROL
// update the led control if adc trigger set
//-----------------------------------------
LED_CONTROL:
    push ADCLow
    push ADCHigh
    sbic ADCSRA, ADSC
    rjmp END_LED_CONTROL // ADSC still set no reading yet
    // else have reading
    // get it
    // clear the interrupt flag
    sbi ADCSRA, ADIF
    in ADCLow,ADCL
    in ADCHigh,ADCH

    sts ADC_HIGH,ADCHigh
    sts ADC_LOW,ADCLow

    // start another one now
    sbi ADCSRA, ADSC

    cpi Torch_State, Torch_State_OFF
    breq CURRENT_HIGH  // if led off set output low
    // else led should be on
    sub ADCLow, SP_low 
    sbc ADCHigh, SP_High
    brge CURRENT_HIGH      // branch if ADC >= setpoint  (signed comparison)
    // else ADC < setpoint

CURRENT_LOW:
    // ADC < setpoint  ==> make output high
    sbi  PORTB, uC_OUTPUT_B   // set output high
    rjmp END_LED_CONTROL

CURRENT_HIGH:
    // ADC > setpoint  ==> make output low
    cbi  PORTB, uC_OUTPUT_B   // set output low
    rjmp END_LED_CONTROL

// all paths jmp to here
END_LED_CONTROL:
    pop ADCHigh
    pop ADCLow
ret
//-------------------------------
#undef ADCLow
#undef ADCHigh

//--------------------------
// TIMER1_2mS
// called each main loop
// but only call Switch_Debounce if trigger
// has been set by timer interrupt
//-------------------------
TIMER1_2mS:
   sbrs TRIGGER_Flags, TRIGGER_Timer1
   rjmp END_TIMER1_2mS
   // else changed clear trigger now
   cbr Trigger_Flags, (1<<TRIGGER_Timer1)
   rcall SWITCH_DEBOUNCE
   // call any other timer counters from here eg 
   rcall TIMER_FADE
END_TIMER1_2mS:
ret
 
//--------------------------
// TIMER_FADE:
// Called from TIMER1_2mS each 2 mSec
// counts up to 6 
// counts up to 12 
// 40 steps => 0.5 sec up and 1sec fade down
//-------------------------
TIMER_FADE:
    lds Temp, FADE_COUNTER
    dec Temp
    brpl END_TIMER_FADE
    // else == 0
    ldi Temp, 1 // for next dec unless updated below
    cpi Torch_State, Torch_State_FADE
    brlo END_TIMER_FADE // not fade just load 1 in counter

    // else set trigger
    sbr TRIGGER_Flags, (1<<TRIGGER_FADE)
    ldi Temp,Fade_Count_Up  // reload 
    cpi Torch_State, Torch_State_FADE_UP
    breq END_TIMER_FADE  // load up timer

    // else down
    ldi Temp,Fade_Count_Down
    //rjmp  END_TIMER_FADE // drop through

END_TIMER_FADE:
    sts FADE_COUNTER, Temp
ret
//-------------------------------------

// ------------------------
// SWITCH_DEBOUNCE
//  this is called each 1mS
//  checks for debounce and updates 
//  SW_SWDown and SW_SWLastInput and SW_SwitchChanged
//  SW_SwitchChanged set to 1 each time switch changes state
//  SW_SWDown set to 1 when switch is pressed (debouced) else 0 when up
//  SW_SWLastInput is set to 1 when last switch read was low/on else set 0 for high/off
// ------------------------
SWITCH_DEBOUNCE:
    cpi DEBOUNCE_Counter, 0xfe    // inc counter but limit to 253
    brsh DEBOUNCE_Counter_LIMITED  // skip increment if at or above limit
    inc DEBOUNCE_Counter ; count up 1KHz counter for debouce

DEBOUNCE_Counter_LIMITED:
    sbic PINB, SW_B   // read SW if SW high jump to released
    rjmp SW_HIGH_RELEASED
    // else drop through to SW_LOW_PRESSED

SW_LOW_PRESSED: // read SW low/pressed
    sbrc SW_Flags, SW_SWLastInput //check if last flag high/on, rjmp if was high 
    rjmp SW_low_check_debounced   // if last input low and this one low check for debounce low
     
    // else this sw low and last high, set last input to high and reset debounce counter
    ori SW_Flags,(1<<SW_SWLastInput)  // 1 for last pressed
    clr DEBOUNCE_Counter;  load counter restart counter for next debounce
    rjmp SWITCH_DEBOUNCE_RETURN // no change yet

SW_low_check_debounced:
    cpi DEBOUNCE_Counter, Debounce_Count_Low 
    brne SWITCH_DEBOUNCE_RETURN  // only do switch change on DEBOUNCE count
    // else update output and return
    sbr SW_Flags,(1<<SW_SWDown) // debounced
    sbr TRIGGER_Flags, (1<<TRIGGER_SwitchChanged) // debouced switch changed
    rjmp SWITCH_DEBOUNCE_RETURN

SW_HIGH_RELEASED: // read SW high/released
    sbrs SW_Flags, SW_SWLastInput  // check if last flag low/off, rjmp if was low
    rjmp SW_high_check_debounced  // if last input high and this one high check for debounce high
     
    // else this sw high and last low, clear last input and reset debounce counter
    andi SW_Flags,~(1<<SW_SWLastInput)
    clr DEBOUNCE_Counter;  load counter restart counter for next debounce
    rjmp SWITCH_DEBOUNCE_RETURN  // no change yet

SW_high_check_debounced:
    cpi DEBOUNCE_Counter, Debounce_Count_High 
    brne SWITCH_DEBOUNCE_RETURN  // only do switch change on DEBOUNCE count
    // update output and return
    cbr SW_Flags, (1<<SW_SWDown) // debounced
    sbr TRIGGER_Flags, (1<<TRIGGER_SwitchChanged) // debouced switch changed
    rjmp SWITCH_DEBOUNCE_RETURN

SWITCH_DEBOUNCE_RETURN:
ret
//-------------------------------------

//-------------------------------------
//PROCESS_SWITCH_STATE_TRIGGER
// handles switch trigger
// and fade trigger
//-------------------------------------
PROCESS_SWITCH_STATE_TRIGGER:
   cpi TRIGGER_Flags, 0  // are any trigger flags set
   breq END_PROCESS_SWITCH_STATE_TRIGGER   // all zero so just return
   sbrs TRIGGER_Flags, TRIGGER_SwitchChanged
   rjmp Finished_SwitchChanged_TRIGGER_PROCESSING
   // else changed, clear trigger now
   cbr Trigger_Flags, (1<<TRIGGER_SwitchChanged)
   // check switch state
   sbrc SW_Flags, SW_SWDown // skip rcall if switch is up
   rcall PROCESS_SWDown_TRIGGER  // set level accordingly
Finished_SwitchChanged_TRIGGER_PROCESSING:

   sbrs TRIGGER_Flags, TRIGGER_FADE
   rjmp Finished_TRIGGER_FADE
   // else clear trigger now
   cbr Trigger_Flags, (1<<TRIGGER_FADE) // have processed this trigger now
   rcall FADE
Finished_TRIGGER_FADE:

   // check other triggers here
END_PROCESS_SWITCH_STATE_TRIGGER:
ret
//-------------------------------------

//-------------------------------------
//PROCESS_SWDown_TRIGGER
//  Press to cycle through levels
//-------------------------------------
PROCESS_SWDown_TRIGGER:
    // else switch pressed down, cycle through states.
   cpi TORCH_State, Torch_State_OFF
   breq PROCESS_SWDown_TRIGGER_TORCH_OFF
   // else
   cpi TORCH_State, TORCH_State_LOW
   breq PROCESS_SWDown_TRIGGER_TORCH_LOW
   // else
   cpi TORCH_State, TORCH_State_MED
   breq PROCESS_SWDown_TRIGGER_TORCH_MED
   // else
   cpi TORCH_State, TORCH_State_HIGH
   breq PROCESS_SWDown_TRIGGER_TORCH_HIGH
   // else invalid state drop through to set to off

PROCESS_SWDown_TRIGGER_TORCH_STATE_INVALID: // invalid -> off
PROCESS_SWDown_TRIGGER_TORCH_HIGH: // high -> off,  variable -> off
   ldi New_TORCH_State, Torch_State_OFF
   rjmp END_PROCESS_SWDown_TRIGGER

PROCESS_SWDown_TRIGGER_TORCH_OFF: // off -> low
   ldi New_TORCH_State, Torch_State_LOW
   rjmp END_PROCESS_SWDown_TRIGGER
    
PROCESS_SWDown_TRIGGER_TORCH_LOW: // low -> med
   ldi New_TORCH_State, Torch_State_MED
   rjmp END_PROCESS_SWDown_TRIGGER

PROCESS_SWDown_TRIGGER_TORCH_MED:  // med -> high
   ldi New_TORCH_State, Torch_State_HIGH
   // rjmp END_PROCESS_SWDown_TRIGGER drop through

END_PROCESS_SWDown_TRIGGER:
ret
//-------------------------------------

//-------------------------------------
//UPDATE_TORCH_STATE
// Set the new torch state and load current setpoint
//-------------------------------------
UPDATE_TORCH_STATE:
   mov TORCH_State, New_TORCH_State // update state to new state
   rcall LOAD_CURRENT_SETPOINT // load new setpoint 
ret
//-------------------------------------

//-------------------------------------
//LOAD_CURRENT_SETPOINT
//  Loads the current setpoint base on the torch state
//  Also set SW_LedOn depending on TORCH_State
//-------------------------------------
LOAD_CURRENT_SETPOINT:
   push Temp
   cpi TORCH_State, Torch_State_OFF
   breq SET_TORCH_OFF
   // else
   cpi TORCH_State, TORCH_State_LOW
   breq SET_TORCH_LOW
   // else
   cpi TORCH_State, TORCH_State_MED
   breq SET_TORCH_MED
   // else
   cpi TORCH_State, TORCH_State_HIGH
   breq SET_TORCH_HIGH

   cpi Torch_State, Torch_State_INVALID
   brmi END_LOAD_CURRENT_SETPOINT // some other valid level just load current level

INVALID_STATE:  // else invalid turn off
   ldi TORCH_State, TORCH_State_OFF // set valid state
   // drop through to set off
SET_TORCH_OFF:
   ldi Temp, 0;
   mov Level, Temp;
   rjmp END_LOAD_CURRENT_SETPOINT
    
SET_TORCH_LOW:
   ldi Temp, LEVEL_LOW;
   mov Level, Temp;
   rjmp END_LOAD_CURRENT_SETPOINT

SET_TORCH_MED:
   ldi Temp, LEVEL_MED;
   mov Level, Temp;
   rjmp END_LOAD_CURRENT_SETPOINT

SET_TORCH_HIGH:
   ldi Temp, LEVEL_HIGH;
   mov Level, Temp;
   rjmp END_LOAD_CURRENT_SETPOINT

END_LOAD_CURRENT_SETPOINT:
   rcall LOAD_SETPOINT_FROM_LEVEL
   pop Temp
ret
//-------------------------------------

//-------------------------------------
//PROCESS_RECEIVED_CHAR
// { char }
// save first char after {
// process it on receiving }
//
// char in Temp
//-------------------------------------
PROCESS_RECEIVED_CHAR:

   cpi Temp, '{' // 
   breq PROCESS_RECEIVED_START_CMD 

   cpi Temp, '}' // 
   breq PROCESS_RECEIVED_END_CMD 

   // else save last char
   tst commandCharacter 
   brne RET_PROCESS_RECEIVED_CHAR
   // else save the first
   mov commandCharacter,Temp;
   rjmp RET_PROCESS_RECEIVED_CHAR

PROCESS_RECEIVED_START_CMD:
   clr commandCharacter // clear on msg start
   rjmp RET_PROCESS_RECEIVED_CHAR

PROCESS_RECEIVED_END_CMD:
   mov Temp, commandCharacter
   rcall PROCESS_CMD_CHAR  // process the cmd
   ldi commandCharacter, -1 // finished processing


RET_PROCESS_RECEIVED_CHAR:
 ret
//-------------------------------------

//-------------------------------------
// PROCESS_CMD_CHAR
// once the } is received
//
PROCESS_CMD_CHAR:
   cpi Temp, 'o' //  turn on/off 
   breq PROCESS_RECEIVED_ON_OFF

   cpi Temp, 'f' //  fade on/off 
   breq PROCESS_RECEIVED_FADE_ON_OFF

   cpi Temp, 'a' //  show data menu 
   breq PROCESS_RECEIVED_DATA_MENU
   // else
   cpi Temp, 'l' // 
   breq PROCESS_RECEIVED_SET_MENU
   // else
   cpi Temp, '.' //  
   breq PROCESS_RECEIVED_MAIN_MENU
   // else
   cpi Temp, 'm' //  
   breq PROCESS_RECEIVED_SUB_MAIN
   // else
   cpi Temp, '0' // 
   breq PROCESS_RECEIVED_CHAR_TORCH_OFF
   // else
   cpi Temp, '1' // 
   breq PROCESS_RECEIVED_CHAR_TORCH_LOW
   // else
   cpi Temp, '2' // 
   breq PROCESS_RECEIVED_CHAR_TORCH_MED
   // else
   cpi Temp, '3' // 
   breq PROCESS_RECEIVED_CHAR_TORCH_HIGH
   // else
   cpi Temp, 'u' // 
   breq PROCESS_RECEIVED_CHAR_TORCH_UP

   cpi Temp, 'd' //  
   breq PROCESS_RECEIVED_CHAR_TORCH_DOWN

   cpi Temp, 'i' // 
   breq PROCESS_RECEIVED_CHAR_SEND_ADC_READING

   cpi Temp, 'v' // 
   breq PROCESS_RECEIVED_CHAR_SEND_LIGHT_LEVEL

   // else invalid char so just ignore
   // send back {}
   rjmp PROCESS_RECEIVED_CHAR_SEND_EMPTY_CMD

PROCESS_RECEIVED_FADE_ON_OFF:
   // if on or fade on fade off on turn off else turn on
   BRANCH_IF_TORCH_OFF_OR_FADING_DOWN PROCESS_RECEIVED_FADE_TO_ON // EQ so was FADE_DOWN => fade up
   rjmp PROCESS_RECEIVED_FADE_TO_OFF // not off or fade down so => fade down

PROCESS_RECEIVED_FADE_TO_ON:
   ldi New_Torch_State, Torch_State_FADE_UP
   inc Level // increase level now so state not cleared to Off
   rjmp PROCESS_RECEIVED_FADE_UPDATE

PROCESS_RECEIVED_FADE_TO_OFF:
   ldi New_Torch_State, Torch_State_FADE_DOWN
   dec Level // decrease level now so state not cleared to MAX
   //rjmp PROCESS_RECEIVED_FADE_UPDATE  // drop through

PROCESS_RECEIVED_FADE_UPDATE:
   sbr CMDS, (1<<CMDS_OnOffUpdate) //set update after updating state
   rjmp END_PROCESS_CMD_CHAR

PROCESS_RECEIVED_ON_OFF:
   // if on turn off else turn on
   BRANCH_IF_TORCH_OFF_OR_FADING_DOWN PROCESS_RECEIVED_TURN_ON // was off or FADE_DOWN => turn on
   // else
   // drop through
   // rjmp PROCESS_RECEIVED_TURN_OFF

PROCESS_RECEIVED_TURN_OFF:
   ldi New_TORCH_State,Torch_State_OFF 
   sbr CMDS, (1<<CMDS_OnOffUpdate)
   rjmp END_PROCESS_CMD_CHAR

PROCESS_RECEIVED_TURN_ON:
   // else turn on
   ldi New_TORCH_State, Torch_State_LOW
   sbr CMDS, (1<<CMDS_OnOffUpdate)
   rjmp END_PROCESS_CMD_CHAR

PROCESS_RECEIVED_DATA_MENU:
   // set flag to send menu
    sbr CMDS, (1<<CMDS_DataMenu)
   rjmp END_PROCESS_CMD_CHAR

PROCESS_RECEIVED_SET_MENU:
   // set flag to send menu
    sbr CMDS, (1<<CMDS_SetMenu)
   rjmp END_PROCESS_CMD_CHAR

PROCESS_RECEIVED_MAIN_MENU:
   // set flag to send menu
    sbr CMDS, (1<<CMDS_MainMenu)
   rjmp END_PROCESS_CMD_CHAR

PROCESS_RECEIVED_SUB_MAIN:
   // set flag to send menu
   sbr CMDS, (1<<CMDS_SubMenu)
   rjmp END_PROCESS_CMD_CHAR

PROCESS_RECEIVED_CHAR_TORCH_OFF: // 0 == off
   ldi New_TORCH_State, Torch_State_OFF
   rjmp PROCESS_RECEIVED_CHAR_UPDATE
    
PROCESS_RECEIVED_CHAR_TORCH_LOW: // 1 == low
   ldi New_TORCH_State, Torch_State_LOW
   rjmp PROCESS_RECEIVED_CHAR_UPDATE

PROCESS_RECEIVED_CHAR_TORCH_MED:  // 2 == med
   ldi New_TORCH_State, Torch_State_MED
   rjmp PROCESS_RECEIVED_CHAR_UPDATE

PROCESS_RECEIVED_CHAR_TORCH_HIGH: // 3 == high
   ldi New_TORCH_State,Torch_State_HIGH 
   rjmp PROCESS_RECEIVED_CHAR_UPDATE

PROCESS_RECEIVED_CHAR_UPDATE:
   rjmp PROCESS_RECEIVED_CHAR_SEND_SP

PROCESS_RECEIVED_CHAR_TORCH_UP:
   ldi New_TORCH_State, Torch_State_VARIABLE
   // clear levels
   rcall SHIFT_DOWN_LEVEL
   inc Level
   rcall SHIFT_UP_LEVEL
   sbr CMDS, (1<<CMDS_OnOffUpdate)
   rjmp END_PROCESS_CMD_CHAR

PROCESS_RECEIVED_CHAR_TORCH_DOWN:
   ldi New_TORCH_State, Torch_State_VARIABLE
   rcall SHIFT_DOWN_LEVEL
   dec Level
   rcall SHIFT_UP_LEVEL
   sbr CMDS, (1<<CMDS_OnOffUpdate)
   rjmp END_PROCESS_CMD_CHAR

PROCESS_RECEIVED_CHAR_SEND_SP:
   sbr TRIGGER_Flags, (1<<TRIGGER_Level)|(1<<TRIGGER_Light)
   // drop through
PROCESS_RECEIVED_CHAR_SEND_EMPTY_CMD:
   sbr CMDS, (1<<CMDS_EmptyCmd)
   rjmp END_PROCESS_CMD_CHAR

PROCESS_RECEIVED_CHAR_SEND_LIGHT_LEVEL:
   sbr TRIGGER_Flags,  (1<<TRIGGER_Level)  | (1<<TRIGGER_Light)  
   sbr CMDS, (1<<CMDS_Streaming)
   rjmp END_PROCESS_CMD_CHAR

PROCESS_RECEIVED_CHAR_SEND_ADC_READING:
   sbr TRIGGER_Flags, (1<<TRIGGER_Amps)  
   sbr CMDS, (1<<CMDS_Streaming)
   rjmp END_PROCESS_CMD_CHAR

   // drop through
END_PROCESS_CMD_CHAR:
ret

//-------------------------------------
// Shift down level
// When get u or d cmd want to move level by 4
// so there is only 10 steps between off and MaxLevel 40
// this routine divides by 4 before the level is 
// inc or dec
// -----------------------------------
SHIFT_DOWN_LEVEL:
    push Temp
    ldi Temp, Level_Shift
    tst Temp
    breq END_SHIFT_DOWN_LEVEL // nothing to do
SHIFT_DOWN_LEVEL_LOOP:
    lsr Level
    dec Temp
    tst Temp
    brne SHIFT_DOWN_LEVEL_LOOP
END_SHIFT_DOWN_LEVEL:
    pop Temp
ret


//-------------------------------------
// Shift up level
// When get u or d cmd want to move level by 4
// so there is only 10 steps between off and MaxLevel 40
// this routine multiplies by 4 after the level has
// be inc or dec 
// -----------------------------------
SHIFT_UP_LEVEL:
    push Temp
    ldi Temp, Level_Shift
    tst Temp
    breq END_SHIFT_UP_LEVEL // nothing to do
SHIFT_UP_LEVEL_LOOP:
    lsl Level
    dec Temp
    tst Temp
    brne SHIFT_UP_LEVEL_LOOP
END_SHIFT_UP_LEVEL:
    pop Temp
ret


//-------------------------------------
// LOAD_SETPOINT_FROM_LEVEL
// Level is set before entry
// this will set Torch state to off if level is zero
// and set torch state to MAx if level is 40
// so need to increase level if fading up before calling this 
// so need to decrease level from max if fade down before calling this 
//-------------------------------------
LOAD_SETPOINT_FROM_LEVEL:
  // limit level to Level_Max
  push Temp
  ldi Temp, 1
  cp Level, Temp // cmp to level 1
  brpl LOAD_SETPOINT_FROM_LEVEL_TEST_MAX
  clr Level // set to zero if negative
  ldi Torch_State, Torch_State_Off
LOAD_SETPOINT_FROM_LEVEL_TEST_MAX:
  ldi Temp, Level_Max
  cp Level, Temp
  brmi LOAD_SETPOINT_FROM_LEVEL_LOAD // Level < Level_Max
  mov Level, Temp //set to Level_Max if too large
  ldi Torch_State, Torch_State_HIGH

LOAD_SETPOINT_FROM_LEVEL_LOAD:
  push ZH
  push ZL
  ldi ZH, high(2*Log_SetPoint)
  ldi ZL, low(2*Log_SetPoint)
  // add level offset
  mov Temp, Level
  lsl Temp // 2 times, this handles levels < 63
  add ZL, Temp // add Level to address to get word to load
  clr Temp
  adc ZH, Temp  // add 0 with carry to set the high byte
 
  lpm Temp, Z+
  mov SP_Low, Temp
  lpm Temp, Z+
  mov SP_High, Temp
  pop Temp
  pop ZL
  pop ZH
END_LOAD_SETPOINT_FROM_LEVEL:
ret

//*************************************************************************
//
// PODapp / RS232 controlled Led Driver Program
// As well as allowing the led level to be controlled by the push button
// this program changes the level based on the cmds sent via RS232.
// 0 => off
// 1 => low, 2 => med, 3 => high
// f => fade on / fade off
// o => on / off 
// u increase level
// d decrease level
// i show current reading
// v show level and light setting
// 
//*************************************************************************
RESET:
    cli  // disable interrupts
    // set clock to 8Mhz i.e. remove div8 that is set by programmer
    ldi Temp, (1<<CLKPCE)
    out CLKPR, Temp  // enable clock change for next 4 cycles
    clr Temp
    out CLKPR,Temp  // set 8Mhz clock

    // ******** Stack Pointer Setup Code ********
    ldi Temp,high(RAMEND-(NO_GLOBAL_VARS+1)) ; Stack Pointer Setup
    out SPH,Temp ; Stack Pointer High Byte
    ldi Temp,low(RAMEND-(NO_GLOBAL_VARS+1)) ; Stack Pointer Setup
    out SPL,Temp ; Stack Pointer Low Byte

    // adjust the clock frequency   
    // for this chip
    //  default cal is 9E for 7.77Mhz, 0xA5 gives 8.04Mhz at about 26deg
    ldi Temp, OSC_CAL_STEP_1 //0xA5
    out OSCCAL, Temp
    ldi Temp, OSC_CAL_STEP_2 //0xA5 also in this case
    out OSCCAL, Temp

    // default port settings are all zeros i.e. input tri-state
    // start by clearing all pin interrupt masks
    clr Temp
    out PCMSK0,Temp
    out SERIAL_IN_INTERRUPT_MASK,Temp

    // uC_OUTPUT_B (PB0) is an output for driving the led  DDB0=1, PB0=0 low OFF to start
    // SW_B (PB1) is the switch input  DDB1=0, PB1=1 with pullup
    ldi Temp,(1<<SW_B)
    ldi r17, (1<<uC_OUTPUT_B)
    out PORTB,Temp
    out DDRB,r17

    // Enable ADC
    ldi Temp, (1<<ADEN)  | (1<<ADPS2) | (1<<ADPS1) // ADPS1 was ADPS0 for 4Mhz 
    // (1<<ADEN) enable ADC   // remove | (1<<ADIE) so interrupt not called
    //  ADSC zero do not start conversion yet
    //  ADATE zero do not trigger conversion
    //  (1<<ADIE) enable ADC interrupt need I-bit in SREG set also
     // Now 8Mhz uC clock so use (1<<ADPS2) (1<<ADPS1) == 64
    //  now 8Mhz/64 = 125Khz 
    out ADCSRA, Temp       // set the  ADCSR

    // set Vref 1.1V (1,0) and the ADC mux inputs and gain (101101) PA2+ve, PA1-ve, x20
    ldi Temp, (1<<REFS1) | (1<<MUX5) | (1<<MUX3) | (1<<MUX2) | (1<<MUX0)
    out ADMUX, Temp

    // set up timer1
    ldi Temp, (0<WGM10) | (0<<WGM11)
    out TCCR1A, Temp
    ldi Temp , (0<<WGM13) | (1<<WGM12) 
    // CTC MODE using OCR1A  and clock of 1/64
    out TCCR1B, Temp
    // load couter with 250 -1 = 0x00, 0xF9 for 0.002 sec timer from 8Mhz
    // load high first then low
    ldi Temp,  0x00
    out OCR1AH, Temp
    ldi Temp, 0xF9 
    out OCR1AL, Temp
    // clear the counter 
    clr Temp
    out TCNT1H, Temp // must write high byte first
    out TCNT1L, Temp

    // set the interrupt
    ldi Temp, (1<<OCIE1A)
    out TIMSK1, Temp  // compare A match interrupt
    // clear the interrupt flag
    ldi Temp, (1<<OCF1A)
    out TIFR1, Temp 

    // start timer1
    in Temp, TCCR1B
    ori Temp, (1<<CS11) | (1<<CS10)  // 1/64
    out TCCR1B, Temp  // timer started

    clr Temp
    mov SW_Flags, Temp
    mov TRIGGER_Flags, Temp
    mov Torch_State, Temp
    mov Level, Temp 
    sts FADE_COUNTER, Temp


    rcall RS232_INIT    // Initialize RS232 interrupts still disabled
    sei  // Enable interrupts

    sbi ADCSRA, ADSC  // start first conversion
    rcall RS232_SET_TO_RECEIVE // sets to recieve and enables interrupts

MAIN_LOOP:
    mov New_TORCH_State, TORCH_State // set initial state
    rcall PROCESS_RS232  // process char if there is one
    // and process cmd if } received
    rcall LED_CONTROL_PROCESSING // each time ADC completes and save value
    rcall UPDATE_TORCH_STATE  // transfer new state to TORCH_State and update setpoint etc.
    //
    // now call all the message routines 
    // they will just return if nothing set to send
    rcall SEND_EMPTY_CMD  // send this first if set
    rcall SEND_MAIN_MENU
    rcall SEND_ONOFF_UPDATE
    rcall SEND_SET_MENU
    rcall SEND_DATA_MENU
    rcall SEND_SUB_MENU
    rcall SEND_STREAMING_CMD
    rcall SEND_LEVEL // send level
    rcall SEND_LIGHT // then % light
    rcall SEND_ADC_READING // send Amps if needed
    rjmp MAIN_LOOP   

//---------------------------------------------------
// LED_CONTROL_PROCESSING:
//
// The LED_CONTROL checks if ADC conversion has finished and if so
// updates the led control output
// The Timer check if the 2mS Timer1 counter has triggers and if so
// runs the pushbutton debouce code and calls the 0.5sec counter
// The Process State Triggers handles any push button triggers generated
// by the debouce code.
//---------------------------------------------------
LED_CONTROL_PROCESSING:
  rcall LED_CONTROL
  rcall TIMER1_2mS  // also updates 0.5sec counter and debounces pushbutton
  // also set Fade_Trigger if necessary
  rcall PROCESS_SWITCH_STATE_TRIGGER // handle pushbutton presses
ret

//----------------------------------------------
// PROCESS_RS232
// 
// This check is RS232 has finished sending and can be set back to receive.
// Also checks is a new byte has been received. If so it calles
// PROCESS_RECEIVE_CHAR to change the LED setpoint based on the char received
//----------------------------------------------
PROCESS_RS232:
    lds Temp, RS232_STATUS // get current status

    sbrc Temp, RS232_STATUS_TX_COMPLETE // check if just completed TX
    rjmp PROCESS_RS232_SET_RECEIVE  // if just completed transmit then set to receive

    // else has a new data byte been received
    sbrs Temp,RS232_STATUS_DATA_RECEIVED
    rjmp END_PROCESS_RS232  // no new char so have just return

    // else have received new char
    // get the char to send back
    lds Temp2, RS232_RECEIVE_BYTE

    // now clear RS232_STATUS_DATA_RECEIVED
    // disable interrupts the to prevent loosing some other update of RS232_STATUS
    cli
    lds Temp, RS232_STATUS
    cbr Temp, (1<<RS232_STATUS_DATA_RECEIVED) 
    sts RS232_STATUS, Temp
    sei

    mov Temp, Temp2
    rcall PROCESS_RECEIVED_CHAR // process the char received

PROCESS_RS232_SET_RECEIVE:
    rcall RS232_SET_TO_RECEIVE  
    // rjmp END_PROCESS_RS232  drop through

END_PROCESS_RS232:
ret


//--------------------------------------
// FADE
// called every time FADE_TRIGGER is set
// if Torch_State_FADE_UP increase level
// if Torch_State_FADE_DOWN decrease level
// if not FADE do nothing
//
//--------------------------------------
FADE:
   push Temp
   cpi Torch_State, Torch_State_FADE
   brmi END_FADE // not fading so return

   cpi Torch_State, Torch_State_FADE_UP
   breq FADE_FADE_UP

   // else fade down
FADE_FADE_DOWN:
   dec Level
   rcall LOAD_SETPOINT_FROM_LEVEL
   rjmp END_FADE 

FADE_FADE_UP:
   inc Level
   rcall LOAD_SETPOINT_FROM_LEVEL
  // rjmp END_FADE fall through
    
END_FADE:
   pop Temp
ret



//-----------------------------------------------------
// SEND_ADC_READING:
// 
// Send back the ADC reading as a % of full scale (1000) counts
// if the TRIGGER_Amps flag is set
// This flag is set by the 'i' command key
//-----------------------------------------------------
SEND_ADC_READING:
    sbrs TRIGGER_Flags, TRIGGER_Amps
    rjmp END_SEND_ADC_READING

    tst CMDS 
    brne END_SEND_ADC_READING // there is a cmd to send 

    // check if still busy transmitting last buffer
    // or busy receiving byte
    // do not clear trigger yet
    BRANCH_IF_RS232_BUSY END_SEND_ADC_READING  // busy sending so keep trigger and try later
    // else not busy sending so send this ADC reading 

    // clear this trigger now as now sending ADC reading
    cbr TRIGGER_Flags, (1<<TRIGGER_Amps)
    // do not send for now
    rcall CONVERT_AND_SEND_ADC_READING

END_SEND_ADC_READING:
ret

//-------------------------------------------------------
// CONVERT_AND_SEND_ADC_READING
// send back suppressing leading zeros
// ADC nnnn    
//-------------------------------------------------------
CONVERT_AND_SEND_ADC_READING:
    push Temp
    push XH
    push XL
    // add 1 due to -X in  ST -X,Temp below
    ldi XH, high(RS232_BUFFER_1+1) 
    ldi XL, low(RS232_BUFFER_1+1)

    push ZH
    push ZL
    ldi ZH, high(2*Amps_Msg)
    ldi ZL, low(2*Amps_Msg)     // Set pointer to RAM data
    rcall LOAD_CHARS
//    pop ZL
//    pop ZH    
//    push ZH  // save temp registers
//    push ZL
    // Note ADC_High and ADC_Low only undate by LED_CONTROL in main loop
    // so no chance of an interrupt modifying the value between then next
    // to statements
    lds ZH, ADC_High // load the 10bit number to send
    lds ZL, ADC_LOW
    rcall CONVERT_13BITS_PERCENT // convert  it
    pop ZL
    pop ZH  // save temp registers

    // calculate number of chars to send
    ldi Temp, low(RS232_BUFFER_1+1)
    sub Temp, XL
    // only need to do the low byte as buffer size < 255
    // and sub result above will wrap round to the correct difference
    sts RS232_SEND_COUNT, Temp // set number of chars to send
    // restore registers

    pop XL
    pop XH
    pop Temp
    rcall RS232_TRANSMIT
ret
//---------------------------------------------------------

//-------------------------------------------------------
// SEND_MAIN_MENU
// send back main menu 
//     
//-------------------------------------------------------
SEND_MAIN_MENU:
    sbrs CMDS, CMDS_MainMenu
    rjmp END_SEND_MAIN_MENU
     
    // check if still busy transmitting last buffer
    // or busy receiving byte
    // do not clear trigger yet
    BRANCH_IF_RS232_BUSY END_SEND_MAIN_MENU  // busy sending so keep trigger and try later
    // else not busy sending so send this light % reading 

    // clear this trigger now as now sending main menu
    cbr CMDS, (1<<CMDS_MainMenu)

    push Temp
    push XH
    push XL
    // add 1 due to -X in  ST -X,Temp below
    ldi XH, high(RS232_BUFFER_1+1) 
    ldi XL, low(RS232_BUFFER_1+1)

    BRANCH_IF_TORCH_OFF_OR_FADING_DOWN SEND_MAIN_MENU_OFF  // off or fade down so => send on
    //else  on send off menu
    //rjmp SEND_MAIN_MENU_ON  drop through

    BRANCH_IF_TORCH_NOT_MAX SEND_MAIN_MENU_ON


SEND_MAIN_MENU_MAX:
    push ZH
    push ZL
    ldi ZH, high(2*Main_Menu_Max)
    ldi ZL, low(2*Main_Menu_Max)     // Set pointer to RAM data
    rcall LOAD_CHARS
    pop ZL
    pop ZH
    rjmp SEND_ONOFF_UPDATE_FINISHED

SEND_MAIN_MENU_ON:
    push ZH
    push ZL
    ldi ZH, high(2*Main_Menu_On)
    ldi ZL, low(2*Main_Menu_On)     // Set pointer to RAM data
    rcall LOAD_CHARS
    pop ZL
    pop ZH
    rjmp SEND_MAIN_MENU_FINISH 

SEND_MAIN_MENU_OFF:
    // else on load off msg
    push ZH
    push ZL
    ldi ZH, high(2*Main_Menu_Off)
    ldi ZL, low(2*Main_Menu_Off)     // Set pointer to RAM data
    rcall LOAD_CHARS
    pop ZL
    pop ZH
//    rjmp SEND_MAIN_MENU_FINISH  drop through


SEND_MAIN_MENU_FINISH:
    // calculate number of chars to send
    ldi Temp, low(RS232_BUFFER_1+1)
    sub Temp, XL
    // only need to do the low byte as buffer size < 255
    // and sub result above will wrap round to the correct difference
    sts RS232_SEND_COUNT, Temp // set number of chars to send
    // restore registers

    pop XL
    pop XH
    pop Temp
    rcall RS232_TRANSMIT
END_SEND_MAIN_MENU:
ret
//---------------------------------------------------------

//-------------------------------------------------------
// SEND_ONOFF_UPDATE
// send back menu update
//     
//-------------------------------------------------------
SEND_ONOFF_UPDATE:
    sbrs CMDS, CMDS_OnOffUpdate
    rjmp END_SEND_ONOFF_UPDATE
     
    // check if still busy transmitting last buffer
    // or busy receiving byte
    // do not clear trigger yet
    BRANCH_IF_RS232_BUSY END_SEND_ONOFF_UPDATE  // busy sending so keep trigger and try later
    // else not busy sending so send this light % reading 

    // clear this trigger now as now sending main menu
    cbr CMDS, (1<<CMDS_OnOffUpdate)

    push Temp
    push XH
    push XL
    // add 1 due to -X in  ST -X,Temp below
    ldi XH, high(RS232_BUFFER_1+1) 
    ldi XL, low(RS232_BUFFER_1+1)

    // if on update to off // else leave as is
    BRANCH_IF_TORCH_OFF_OR_FADING_DOWN SEND_ONOFF_UPDATE_OFF  // off or fade down so => send on
    // else on => send off
    //rjmp SEND_ONOFF_UPDATE_OFF drop through

    BRANCH_IF_TORCH_NOT_MAX SEND_ONOFF_UPDATE_ON


SEND_ONOFF_UPDATE_MAX:
    push ZH
    push ZL
    ldi ZH, high(2*Main_Menu_UpdateMsg_Max)
    ldi ZL, low(2*Main_Menu_UpdateMsg_Max)     // Set pointer to RAM data
    rcall LOAD_CHARS
    pop ZL
    pop ZH
    rjmp SEND_ONOFF_UPDATE_FINISHED


SEND_ONOFF_UPDATE_OFF:
    push ZH
    push ZL
    ldi ZH, high(2*Main_Menu_UpdateMsg_Off)
    ldi ZL, low(2*Main_Menu_UpdateMsg_Off)     // Set pointer to RAM data
    rcall LOAD_CHARS
    pop ZL
    pop ZH
    rjmp SEND_ONOFF_UPDATE_FINISHED

SEND_ONOFF_UPDATE_ON:
    push ZH
    push ZL
    ldi ZH, high(2*Main_Menu_UpdateMsg_On)
    ldi ZL, low(2*Main_Menu_UpdateMsg_On)     // Set pointer to RAM data
    rcall LOAD_CHARS
    pop ZL
    pop ZH
    rjmp SEND_ONOFF_UPDATE_FINISHED

SEND_ONOFF_UPDATE_FINISHED:
    // calculate number of chars to send
    ldi Temp, low(RS232_BUFFER_1+1)
    sub Temp, XL
    // only need to do the low byte as buffer size < 255
    // and sub result above will wrap round to the correct difference
    sts RS232_SEND_COUNT, Temp // set number of chars to send
    // restore registers

    pop XL
    pop XH
    pop Temp
    rcall RS232_TRANSMIT
END_SEND_ONOFF_UPDATE:
ret
//---------------------------------------------------------

//-------------------------------------------------------
// SEND_SET_MENU
// send set menu
//     
//-------------------------------------------------------
SEND_SET_MENU:
    sbrs CMDS, CMDS_SetMenu
    rjmp END_SEND_SET_MENU
     
    // check if still busy transmitting last buffer
    // or busy receiving byte
    // do not clear trigger yet
    BRANCH_IF_RS232_BUSY END_SEND_SET_MENU  // busy sending so keep trigger and try later
    // else not busy sending so send this light % reading 

    // clear this trigger now as now sending main menu
    cbr CMDS, (1<<CMDS_SetMenu)

    push Temp
    push XH
    push XL
    // add 1 due to -X in  ST -X,Temp below
    ldi XH, high(RS232_BUFFER_1+1) 
    ldi XL, low(RS232_BUFFER_1+1)

    push ZH
    push ZL
    ldi ZH, high(2*Set_Menu_Msg)
    ldi ZL, low(2*Set_Menu_Msg)     // Set pointer to RAM data
    rcall LOAD_CHARS
    pop ZL
    pop ZH

    // calculate number of chars to send
    ldi Temp, low(RS232_BUFFER_1+1)
    sub Temp, XL
    // only need to do the low byte as buffer size < 255
    // and sub result above will wrap round to the correct difference
    sts RS232_SEND_COUNT, Temp // set number of chars to send
    // restore registers

    pop XL
    pop XH
    pop Temp
    rcall RS232_TRANSMIT
END_SEND_SET_MENU:
ret
//---------------------------------------------------------



//-------------------------------------------------------
// SEND_DATA_MENU
// send data menu
//     
//-------------------------------------------------------
SEND_DATA_MENU:
    sbrs CMDS, CMDS_DataMenu
    rjmp END_SEND_DATA_MENU
     
    // check if still busy transmitting last buffer
    // or busy receiving byte
    // do not clear trigger yet
    BRANCH_IF_RS232_BUSY END_SEND_DATA_MENU  // busy sending so keep trigger and try later
    // else not busy sending so send this light % reading 

    // clear this trigger now as now sending main menu
    cbr CMDS, (1<<CMDS_DataMenu)

    push Temp
    push XH
    push XL
    // add 1 due to -X in  ST -X,Temp below
    ldi XH, high(RS232_BUFFER_1+1) 
    ldi XL, low(RS232_BUFFER_1+1)

    push ZH
    push ZL
    ldi ZH, high(2*Data_Menu_Msg)
    ldi ZL, low(2*Data_Menu_Msg)     // Set pointer to RAM data
    rcall LOAD_CHARS
    pop ZL
    pop ZH

    // calculate number of chars to send
    ldi Temp, low(RS232_BUFFER_1+1)
    sub Temp, XL
    // only need to do the low byte as buffer size < 255
    // and sub result above will wrap round to the correct difference
    sts RS232_SEND_COUNT, Temp // set number of chars to send
    // restore registers

    pop XL
    pop XH
    pop Temp
    rcall RS232_TRANSMIT
END_SEND_DATA_MENU:
ret
//---------------------------------------------------------

//-------------------------------------------------------
// SEND_SUB_MENU
// send sub menu
//     
//-------------------------------------------------------
SEND_SUB_MENU:
    sbrs CMDS, CMDS_SubMenu
    rjmp END_SEND_SUB_MENU
     
    // check if still busy transmitting last buffer
    // or busy receiving byte
    // do not clear trigger yet
    BRANCH_IF_RS232_BUSY END_SEND_SUB_MENU  // busy sending so keep trigger and try later
    // else not busy sending so send this light % reading 

    // clear this trigger now as now sending main menu
    cbr CMDS, (1<<CMDS_SubMenu)

    push Temp
    push XH
    push XL
    // add 1 due to -X in  ST -X,Temp below
    ldi XH, high(RS232_BUFFER_1+1) 
    ldi XL, low(RS232_BUFFER_1+1)

    push ZH
    push ZL
    ldi ZH, high(2*Sub_Menu_Msg)
    ldi ZL, low(2*Sub_Menu_Msg)     // Set pointer to RAM data
    rcall LOAD_CHARS
    pop ZL
    pop ZH

    // calculate number of chars to send
    ldi Temp, low(RS232_BUFFER_1+1)
    sub Temp, XL
    // only need to do the low byte as buffer size < 255
    // and sub result above will wrap round to the correct difference
    sts RS232_SEND_COUNT, Temp // set number of chars to send
    // restore registers

    pop XL
    pop XH
    pop Temp
    rcall RS232_TRANSMIT
END_SEND_SUB_MENU:
ret
//---------------------------------------------------------




//-------------------------------------------------------
// SEND_EMPTY_CMD
// send back empty cmd
//     
//-------------------------------------------------------
SEND_EMPTY_CMD:
    sbrs CMDS, CMDS_EmptyCmd
    rjmp END_SEND_EMPTY_CMD
     
    // check if still busy transmitting last buffer
    // or busy receiving byte
    // do not clear trigger yet
    BRANCH_IF_RS232_BUSY END_SEND_EMPTY_CMD  // busy sending so keep trigger and try later
    // else not busy sending so send this light % reading 

    // clear this trigger now as now sending light % reading
    cbr CMDS, (1<<CMDS_EmptyCmd)

    push Temp
    push XH
    push XL
    // add 1 due to -X in  ST -X,Temp below
    ldi XH, high(RS232_BUFFER_1+1) 
    ldi XL, low(RS232_BUFFER_1+1)

    push ZH
    push ZL
    ldi ZH, high(2*Empty_Cmd_Msg)
    ldi ZL, low(2*Empty_Cmd_Msg)     // Set pointer to RAM data
    rcall LOAD_CHARS
    pop ZL
    pop ZH

    // calculate number of chars to send
    ldi Temp, low(RS232_BUFFER_1+1)
    sub Temp, XL
    // only need to do the low byte as buffer size < 255
    // and sub result above will wrap round to the correct difference
    sts RS232_SEND_COUNT, Temp // set number of chars to send
    // restore registers

    pop XL
    pop XH
    pop Temp
    rcall RS232_TRANSMIT
END_SEND_EMPTY_CMD:
ret
//---------------------------------------------------------

//-------------------------------------------------------
// SEND_STREAMING_CURRENT
// send streaming current cmd
//     
//-------------------------------------------------------
SEND_STREAMING_CMD:
    sbrs CMDS, CMDS_Streaming
    rjmp END_SEND_STREAMING_CMD
     
    // check if still busy transmitting last buffer
    // or busy receiving byte
    // do not clear trigger yet
    BRANCH_IF_RS232_BUSY END_SEND_STREAMING_CMD  // busy sending so keep trigger and try later
    // else not busy sending so send this light % reading 

    // clear this trigger now as now sending light % reading
    cbr CMDS, (1<<CMDS_Streaming)

    push Temp
    push XH
    push XL
    // add 1 due to -X in  ST -X,Temp below
    ldi XH, high(RS232_BUFFER_1+1) 
    ldi XL, low(RS232_BUFFER_1+1)

    push ZH
    push ZL
    ldi ZH, high(2*Streaming_Msg)
    ldi ZL, low(2*Streaming_Msg)     // Set pointer to RAM data
    rcall LOAD_CHARS
    pop ZL
    pop ZH

    // calculate number of chars to send
    ldi Temp, low(RS232_BUFFER_1+1)
    sub Temp, XL
    // only need to do the low byte as buffer size < 255
    // and sub result above will wrap round to the correct difference
    sts RS232_SEND_COUNT, Temp // set number of chars to send
    // restore registers

    pop XL
    pop XH
    pop Temp
    rcall RS232_TRANSMIT
END_SEND_STREAMING_CMD:
ret
//---------------------------------------------------------


//-------------------------------------------------------
// SEND_LIGHT
// send back Light level
//     
//-------------------------------------------------------
SEND_LIGHT:
    sbrs TRIGGER_Flags, TRIGGER_Light
    rjmp END_SEND_LIGHT

    sbrc TRIGGER_Flags, TRIGGER_Level // send level first
    rjmp END_SEND_LIGHT
     
    tst CMDS 
    brne END_SEND_LIGHT // there is a cmd to send

    // check if still busy transmitting last buffer
    // or busy receiving byte
    // do not clear trigger yet
    BRANCH_IF_RS232_BUSY END_SEND_LIGHT  // busy sending so keep trigger and try later
    // else not busy sending so send this light % reading 

    // clear this trigger now as now sending light % reading
    cbr TRIGGER_Flags, (1<<TRIGGER_Light)

    push Temp
    push XH
    push XL
    // add 1 due to -X in  ST -X,Temp below
    ldi XH, high(RS232_BUFFER_1+1) 
    ldi XL, low(RS232_BUFFER_1+1)

    push ZH
    push ZL
    ldi ZH, high(2*Light_Msg)
    ldi ZL, low(2*Light_Msg)     // Set pointer to RAM data
    rcall LOAD_CHARS
//    pop ZL
//    pop ZH
//    push ZH  // save temp registers
//    push ZL
    // Note ADC_High and ADC_Low only undate by LED_CONTROL in main loop
    // so no chance of an interrupt modifying the value between then next
    // to statements
    mov ZH, SP_High // load the 10bit number to send
    mov ZL, SP_LOW
    rcall CONVERT_13BITS_PERCENT // convert  it
    pop ZL
    pop ZH  // save temp registers

    //NEW_LINE
    // calculate number of chars to send
    ldi Temp, low(RS232_BUFFER_1+1)
    sub Temp, XL
    // only need to do the low byte as buffer size < 255
    // and sub result above will wrap round to the correct difference
    sts RS232_SEND_COUNT, Temp // set number of chars to send
    // restore registers

    pop XL
    pop XH
    pop Temp
    rcall RS232_TRANSMIT
END_SEND_LIGHT:
ret
//---------------------------------------------------------

//-------------------------------------------------------
// SEND_LEVEL
// send level number
//     
//-------------------------------------------------------
SEND_LEVEL:
    sbrs TRIGGER_Flags, TRIGGER_Level
    rjmp END_SEND_LEVEL

    tst CMDS 
    brne END_SEND_LIGHT // there is a cmd to send
 
    // check if still busy transmitting last buffer
    // or busy receiving byte
    // do not clear trigger yet
    BRANCH_IF_RS232_BUSY END_SEND_LEVEL  // busy sending so keep trigger and try later
    // else not busy sending so send this level 

    // clear this trigger now as now sending level
    cbr TRIGGER_Flags, (1<<TRIGGER_Level)

    push Temp
    push XH
    push XL
    // add 1 due to -X in  ST -X,Temp below
    ldi XH, high(RS232_BUFFER_1+1) 
    ldi XL, low(RS232_BUFFER_1+1)

    push ZH
    push ZL
    ldi ZH, high(2*Level_Msg)
    ldi ZL, low(2*Level_Msg)     // Set pointer to RAM data
    rcall LOAD_CHARS
//    pop ZL
//    pop ZH
//    push ZH  // save temp registers
//    push ZL
    ldi ZH, 0 // load the 10bit number to send
    mov ZL, Level
    rcall CONVERT_13BITS // convert  it
    pop ZL
    pop ZH  // save temp registers

    // calculate number of chars to send
    ldi Temp, low(RS232_BUFFER_1+1)
    sub Temp, XL
    // only need to do the low byte as buffer size < 255
    // and sub result above will wrap round to the correct difference
    sts RS232_SEND_COUNT, Temp // set number of chars to send
    // restore registers

    pop XL
    pop XH
    pop Temp
    rcall RS232_TRANSMIT
END_SEND_LEVEL:
ret
//---------------------------------------------------------



#define CharCount r15
// --------------------------
// LOAD_CHARS
// routine version of LOAD_CHARS_FROM macro
// first byte holds the number of chars to load
// chars follow.
// Assumes ZH:ZL has been set to point to the start of the msg in RAM data
// Assumes XH:XL has been set to point to the RS232 Buffer 
// Decrements XH:XL before storing each char
//       push ZH
//       push ZL
//       ldi ZH, high(2*Error_Msg_Line1)
//       ldi ZL, low(2*Error_Msg_Line1)     // Set pointer to RAM data
//       rcall LOAD_CHARS
//       pop ZL
//       pop ZH
//
// Note: the .db must have an even number of bytes
// including the leading count byte
//
// All registers used here are saved and restored
LOAD_CHARS:
    push CharCount
    push Temp
    // push ZH // set by caller
    // push ZL // set by caller
   lpm CharCount, Z+    // this is the number of chars to load
LOAD_CHARS_LOOP:
   lpm Temp, Z+  //   Load program memory and postincrement
    st -X, Temp  //   Store indirect and predecrement
    dec CharCount   //   Decrement
    brne LOAD_CHARS_LOOP  //   Branch if not equal
END_LOAD_CHARS:
    pop Temp
    pop CharCount
ret
#undef CharCount
//------------------------------------------------------------


Log_SetPoint:
// this maps levels into current setpoints between 0 and 1000 (max ADC count is 1024)
     // 0 == 0, 1==16    2==24   3 == 40
     // 0  1  2  3  4  5  6   7   8   9   10  11  12  13  14  15  16  17  18  19  20 
.dw     0, 1, 1, 2, 2, 3, 3,  5,  6,  7,  8,  10, 12, 15, 18, 21, 26, 32, 38, 47, 57
    //  21   22   23   24   25   26  27   28   29   30   31   32   33   34   35   36  37   38    39  40   
.dw     70,  86, 105, 129, 156, 188, 226, 273, 319, 373, 436, 509, 569, 636, 710,794, 841, 891, 944,1000 // words 


// make sure the RS232_MAX_SEND_BUFFER size is larger enough to hold the largest msg below + any additional data                        

//======= CMDS =============
// 
Main_Menu_On:
     // .db line must have an even number of chars
     // first value is the number of chars to load
.db    53, "{^Led Control||m~menu|u~Brighter|d~Darker|f~Fade Off}" 

Main_Menu_Off:
     // .db line must have an even number of chars
     // first value is the number of chars to load
.db    45, "{^Led Control||m~menu|u~Brighter|d|f~Fade On}" 

Main_Menu_Max:
     // .db line must have an even number of chars
     // first value is the number of chars to load
.db    45, "{^Led Control||m~menu|u|d~Darker|f~Fade Off} " 

Main_Menu_UpdateMsg_Off:
     // .db line must have an even number of chars
     // first value is the number of chars to load
.db    27,"{:|u~Brighter|d|f~Fade On} "

Main_Menu_UpdateMsg_On:
     // .db line must have an even number of chars
     // first value is the number of chars to load
.db    35,"{:|u~Brighter|d~Darker|f~Fade Off} " 

Main_Menu_UpdateMsg_Max: // reached max level
     // .db line must have an even number of chars
     // first value is the number of chars to load
.db    25,"{:|u|d~Darker|f~Fade Off}" 

Sub_Menu_Msg:
     // .db line must have an even number of chars
     // first value is the number of chars to load
.db    39, "{.Settings Menu|l~Set Level->|a~Data->}"

Empty_Cmd_Msg:
     // .db line must have an even number of chars
     // first value is the number of chars to load
.db    3, "{} "

Streaming_Msg:
// this msg is used for both streaming current and level
     // .db line must have an even number of chars
     // first value is the number of chars to load
.db    3, "{=}"

Set_Menu_Msg:
     // .db line must have an even number of chars
     // first value is the number of chars to load
.db    31, "{.|1~Low|2~Medium|3~High|0~Off}"


Data_Menu_Msg:
     // .db line must have an even number of chars
     // first value is the number of chars to load
.db    31, "{.|v~Light Level|i~Led Current}"


// ========= Streaming data messages ==========
Level_Msg:
     // .db line must have an even number of chars
     // first value is the number of chars to load
.db      7,  13, "Level "

Light_Msg:
     // .db line must have an even number of chars
     // first value is the number of chars to load
.db      7,  13,"Light " 

Amps_Msg:
     // .db line must have an even number of chars
     // first value is the number of chars to load
.db      7, 13," Amps " 
